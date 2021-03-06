#pragma once

#include <agz/tracer/core/geometry.h>
#include <agz/tracer/core/light.h>

AGZ_TRACER_BEGIN

class GeometryToDiffuseLight : public AreaLight
{
    const Geometry *geometry_;

    Spectrum radiance_;

    real user_specified_power_;

public:

    GeometryToDiffuseLight(
        const Geometry *geometry, const Spectrum &radiance,
        real user_specified_power);

    LightSampleResult sample(
        const Vec3 &ref, const Sample5 &sam) const noexcept override;

    LightEmitResult sample_emit(
        const Sample5 &sam) const noexcept override;

    LightEmitPDFResult emit_pdf(
        const Vec3 &pos, const Vec3 &dir, const Vec3 &nor) const noexcept override;

    Spectrum power() const noexcept override;

    Spectrum radiance(
        const Vec3 &pos, const Vec3 &nor, const Vec2 &uv,
        const Vec3 &light_to_out) const noexcept override;

    real pdf(
        const Vec3 &ref,
        const Vec3 &pos, const Vec3 &nor) const noexcept override;
};

AGZ_TRACER_END
