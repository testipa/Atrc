﻿#pragma once

#include <agz/tracer/core/intersection.h>

AGZ_TRACER_BEGIN

class Scene;
class AreaLight;
class NonareaLight;

/**
 * @brief 根据参考点采样光源得到的结果
 */
struct LightSampleResult
{
    Vec3 ref;
    Vec3 pos;
    Spectrum radiance;
    real pdf = 0;
    bool is_delta = false;

    bool valid() const noexcept
    {
        return pdf != 0;
    }

    Vec3 ref_to_light() const noexcept
    {
        return (pos - ref).normalize();
    }
};

/**
 * @brief 采样实体光源失败时的返回值
 */
inline const LightSampleResult LIGHT_SAMPLE_RESULT_NULL = { { }, { }, { }, 0, false };

/**
 * @brief 光源接口
 * 
 * 全体光源被分为两类：实体光源和环境光源。
 * 实体光源在场景中有对应的Entity，环境光源则没有
 */
class Light
{
public:

    virtual ~Light() = default;

    /**
     * @brief 是否是有实体的光源
     */
    virtual bool is_area() const noexcept = 0;

    /**
     * @brief 返回其实体光源接口
     */
    virtual const AreaLight *as_area() const noexcept { return nullptr; }

    /**
     * @brief 返回其非实体光源接口
     */
    virtual const NonareaLight *as_nonarea() const noexcept { return nullptr; }

    /**
     * @brief 采样一条照射到ref的射线
     * @param ref 参考点
     * @param sam 采样数据
     */
    virtual LightSampleResult sample(const Vec3 &ref, const Sample5 &sam) const noexcept = 0;

    /**
     * @brief 发射的光通量
     */
    virtual Spectrum power() const noexcept = 0;

    /**
     * @brief 基于场景进行预处理，在渲染开始之前、场景准备完毕之后调用一次
     */
    virtual void preprocess(const Scene &scene) = 0;
};

/**
 * @brief 实体光源接口
 */
class AreaLight : public Light
{
public:

    bool is_area() const noexcept override final { return true; }

    const AreaLight *as_area() const noexcept override final { return this; }

    /**
     * @brief 光源表面某点朝指定方向的辐射亮度
     * 
     * @param spt 光源表面的点
     * @param light_to_out spt向外照射的方向
     * @return 沿该射线反方向的radiance
     */
    virtual Spectrum radiance(const SurfacePoint &spt, const Vec3 &light_to_out) const noexcept = 0;

    /**
     * @brief 采样到某条照射ref的射线的概率密度（w.r.t. solid angle）
     *
     * @param ref 参考点
     * @param spt 光源上的点
     */
    virtual real pdf(const Vec3 &ref, const SurfacePoint &spt) const noexcept = 0;
};

/**
 * @brief 非实体光源接口
 */
class NonareaLight : public Light
{
public:

    bool is_area() const noexcept override final { return false; }

    const NonareaLight* as_nonarea() const noexcept override final { return this; }

    /**
     * @brief 光源沿指定方向照射到空间中某点的辐射亮度
     * 
     * @param ref 被照射的点
     * @param ref_to_light 沿哪个方向照射到ref点
     * @param light_point 发射点，主要用于visibility test，可以为nullptr
     * @return 沿ref_to_light向ref点发射的辐射亮度
     */
    virtual Spectrum radiance(const Vec3 &ref, const Vec3 &ref_to_light, Vec3 *light_point = nullptr) const noexcept = 0;

    /**
     * @brief 以ref点为参考点时采样入射方向采样到ref_to_light的概率密度（w.r.t. solid angle）
     */
    virtual real pdf(const Vec3 &ref, const Vec3 &ref_to_light) const noexcept = 0;
};

class EnvirLight : public NonareaLight
{
protected:

    real world_radius_ = 1;
    Vec3 world_centre_;

public:

    void preprocess(const Scene &scene) override final;
};

AGZ_TRACER_END
