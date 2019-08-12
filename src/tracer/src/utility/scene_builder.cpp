#include <agz/tracer/core/camera.h>
#include <agz/tracer/core/entity.h>
#include <agz/tracer/core/light.h>
#include <agz/tracer/utility/logger.h>
#include <agz/tracer/utility/scene_builder.h>

AGZ_TRACER_BEGIN

Scene *SceneBuilder::build(const Config &params, obj::ObjectInitContext &init_ctx)
{
    AGZ_HIERARCHY_TRY

    auto scene = SceneFactory.create(params, init_ctx);
    std::vector<Light*> lights;

    if(auto ent_arr = params.find_child_array("entities"))
    {
        if(ent_arr->size() == 1)
            AGZ_LOG1("creating 1 entity");
        else if(ent_arr->size() > 1)
            AGZ_LOG1("creating ", ent_arr->size(), " entities");

        for(size_t i = 0; i < ent_arr->size(); ++i)
        {
            auto group = ent_arr->at_group(i);
            if(stdstr::ends_with(group.child_str("type"), "//"))
                continue;

            auto entity = EntityFactory.create(group, init_ctx);
            scene->add_entity(entity);
            if(auto light = entity->as_light())
                lights.push_back(light);
        }
    }

    if(auto ent_grp = params.find_child_group("named_entities"))
    {
        if(ent_grp->size() == 1)
            AGZ_LOG1("creating 1 named entity");
        else if(ent_grp->size() > 1)
            AGZ_LOG1("creating ", ent_grp->size(), " named entities");

        for(auto &grp : *ent_grp)
        {
            if(stdstr::ends_with(grp.first, "//"))
                continue;

            auto &group = grp.second->as_group();
            if(stdstr::ends_with(group.child_str("type"), "//"))
                continue;

            auto entity = EntityFactory.create(group, init_ctx);
            scene->add_entity(entity);
            if(auto light = entity->as_light())
                lights.push_back(light);
        }
    }

    if(auto group = params.find_child_group("env"))
    {
        AGZ_LOG1("creating environment light");

        auto env = EntityFactory.create(*group, init_ctx);
        if(auto light = env->as_light())
        {
            lights.push_back(light);
            scene->set_env_light(env);
        }
        else
            throw ObjectConstructionException("'env' must be an light source");
    }

    AGZ_LOG1("creating camera");
    auto camera = CameraFactory.create(params.child_group("camera"), init_ctx);
    scene->set_camera(camera);

    AGZ_LOG1("preprocessing light sources");
    for(auto light : lights)
        light->preprocess(*scene);

    return scene;

    AGZ_HIERARCHY_WRAP("in building scene")
}

AGZ_TRACER_END
