#include <iostream>

#include <Atrc/Lib/Core/Film.h>
#include <Atrc/Lib/Core/PostProcessor.h>
#include <Atrc/Lib/Core/Renderer.h>
#include <Atrc/Lib/Core/Scene.h>

#include <Atrc/Mgr/BuiltinCreatorRegister.h>
#include <Atrc/Mgr/Context.h>
#include <Atrc/Mgr/Parser.h>
#include <Atrc/Mgr/SceneBuilder.h>

using namespace Atrc;

int Run()
{
    AGZ::Config config;
    if(!config.LoadFromFile("./Build/scene.txt"))
        throw Mgr::MgrErr("Failed to load configuration file");
    auto &root = config.Root();

    Mgr::Context context(root);
    Mgr::RegisterBuiltinCreators(context);

    auto renderer = context.Create<Renderer>  (root["renderer"]);
    auto sampler  = context.Create<Sampler>   (root["sampler"]);
    auto filter   = context.Create<FilmFilter>(root["film.filter"]);
    auto reporter = context.Create<Reporter>  (root["reporter"]);

    auto scene = Mgr::SceneBuilder::Build(root, context);

    auto outputFilename = root["outputFilename"].AsValue();

    Vec2i filmSize;
    ATRC_MGR_TRY
    {
        filmSize = Mgr::Parser::ParseVec2i(root["film.size"]);
        if(filmSize.x <= 0 || filmSize.y <= 0)
            throw Mgr::MgrErr("Invalid film size value");
    }
    ATRC_MGR_CATCH_AND_RETHROW("In creating film")

    PostProcessorChain postProcessChain;
    ATRC_MGR_TRY
    {
        if(auto ppNode = root.Find("postProcessors"))
        {
            auto &arr = ppNode->AsArray();
            for(size_t i = 0; i < arr.Size(); ++i)
                postProcessChain.AddBack(context.Create<PostProcessor>(arr[i]));
        }
    }
    ATRC_MGR_CATCH_AND_RETHROW("In creating post processors")

    Film film(filmSize, *filter);
    renderer->Render(scene, sampler, &film, reporter);

    auto image = film.GetImage();
    postProcessChain.Process(&image);

    AGZ::TextureFile::WriteRGBToPNG(outputFilename, image.Map(
    [](const Spectrum &c)
    {
        return c.Map([](Real s)
            { return uint8_t(Clamp<Real>(s * 256, 0, 255)); });
    }));

    return 0;
}

#if defined(_WIN32) && defined(_DEBUG)
#include <crtdbg.h>
#endif

int main()
{
#if defined(_WIN32) && defined(_DEBUG)
    _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG)
        | _CRTDBG_LEAK_CHECK_DF);
#endif

    try
    {
        return Run();
    }
    catch(const Mgr::MgrErr &err)
    {
        for(auto pErr = &err; pErr; pErr = pErr->TryGetInterior())
        {
            std::cout << pErr->GetMsg().ToStdString() << std::endl;
            if(pErr->TryGetLeaf())
            {
                std::cout << pErr->TryGetLeaf()->what() << std::endl;
                break;
            }
        }
    }
    catch(const std::exception &err)
    {
        std::cout << err.what() << std::endl;
    }
    catch(...)
    {
        std::cout << "An unknown error occurred..." << std::endl;
    }
}