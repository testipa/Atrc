#pragma once

#include <agz/editor/renderer/renderer_widget.h>

namespace Ui
{
    class PathTracer;
}

AGZ_EDITOR_BEGIN

class PathTracerWidget : public RendererWidget
{
public:

    explicit PathTracerWidget(QWidget *parent);

    ~PathTracerWidget();

    Box<Renderer> create_renderer(
        RC<tracer::Scene> scene, const Vec2i &framebuffer_size,
        bool enable_preview) const override;

    void save_asset(AssetSaver &saver) const override;

    void load_asset(AssetLoader &loader) override;

private:

    int min_depth_  = 5;
    int max_depth_  = 10;
    real cont_prob_ = real(0.9);

    int specular_depth_ = 20;

    Ui::PathTracer *ui_;
};

class PathTracerWidgetCreator : public RendererWidgetCreator
{
public:

    std::string name() const override { return "Path Tracer"; }

    RendererWidget *create_widget(QWidget *parent) const override;
};

AGZ_EDITOR_END
