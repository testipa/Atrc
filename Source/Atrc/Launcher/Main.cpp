#include <iostream>

#include <Atrc/Lib/Core/Film.h>
#include <Atrc/Lib/Core/Renderer.h>
#include <Atrc/Lib/Core/Scene.h>

#include <Atrc/Mgr/BuiltinCreatorRegister.h>
#include <Atrc/Mgr/Parser.h>
#include <Atrc/Mgr/Context.h>

using namespace Atrc;

int Run()
{
    AGZ::Config config;
    if(!config.LoadFromFile("./Build/scene.txt"))
        throw Mgr::MgrErr("Failed to load configuration file");
    auto &root = config.Root();

    Mgr::Context context(root);
    Mgr::RegisterBuiltinCreators(context);

    auto camera   = context.Create<Camera>(root["camera"]);
    auto renderer = context.Create<Renderer>(root["renderer"]);
    auto sampler  = context.Create<Sampler>(root["sampler"]);
    auto filter   = context.Create<FilmFilter>(root["film.filter"]);

    auto outputFilename = root["outputFilename"].AsValue();

    std::vector<Entity*> entities;
    std::vector<Light*> lights;

    auto &entArr = root["entities"].AsArray();
    for(size_t i = 0; i < entArr.Size(); ++i)
        entities.push_back(context.Create<Entity>(entArr[i]));
    
    auto &lghtArr = root["lights"].AsArray();
    for(size_t i = 0; i < lghtArr.Size(); ++i)
        lights.push_back(context.Create<Light>(lghtArr[i]));

    auto filmSize = Mgr::Parser::ParseVec2i(root["film.size"]);
    if(filmSize.x <= 0 || filmSize.y <= 0)
        throw Mgr::MgrErr("Invalid film size value");

    Film film(filmSize, *filter);

    std::vector<const Entity*> cEntities;
    std::vector<const Light*> cLights;

    cEntities.reserve(entities.size());
    for(auto ent : entities)
        cEntities.push_back(ent);

    cLights.reserve(lights.size());
    for(auto lht : lights)
        cLights.push_back(lht);
    
    Scene scene(cEntities.data(), cEntities.size(), cLights.data(), cLights.size(), camera);

    for(auto lht : lights)
        lht->PreprocessScene(scene);
    
    renderer->Render(scene, sampler, &film);

    AGZ::TextureFile::WriteRGBToPNG(outputFilename, film.GetImage().Map(
    [](const Spectrum &c)
    {
        return c.Map([](Real s)
            { return uint8_t(Clamp<Real>(s * 256, 0, 255)); });
    }));

    return 0;
}

int main()
{
    try
    {
        return Run();
    }
    catch(const Mgr::MgrErr &err)
    {
        const Mgr::MgrErr *pErr = &err;
        while(pErr)
        {
            std::cout << pErr->GetMsg().ToStdString() << std::endl;
            if(pErr->TryGetLeaf())
            {
                std::cout << pErr->TryGetLeaf()->what() << std::endl;
                break;
            }
            pErr = pErr->TryGetInterior();
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
