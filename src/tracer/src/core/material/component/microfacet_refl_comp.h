#pragma once

#include "../utility/fresnel_point.h"
#include "./component.h"

AGZ_TRACER_BEGIN

class GGXMicrofacetReflectionComponent : public BSDFComponent
{
    const FresnelPoint *fresnel_;
    real ax_;
    real ay_;

public:

    GGXMicrofacetReflectionComponent(
        const FresnelPoint *fresnel,
        real roughness, real anisotropic) noexcept;

    Spectrum eval(
        const Vec3 &lwi, const Vec3 &lwo,
        TransMode mode) const noexcept override;

    SampleResult sample(
        const Vec3 &lwo, TransMode mode,
        const Sample2 &sam) const noexcept override;

    real pdf(
        const Vec3 &lwi, const Vec3 &lwo) const noexcept override;
};

AGZ_TRACER_END
