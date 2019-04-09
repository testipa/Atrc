#pragma once

#include <filesystem>
#include <map>
#include <string>
#include <type_traits>

#include <AGZUtils/Utils/Exception.h>
#include <Atrc/Editor/GL.h>

namespace Atrc::Editor
{

class HasName
{
    std::string name_;

public:

    explicit HasName(std::string name) noexcept
        : name_(std::move(name))
    {

    }

    const std::string &GetName() const noexcept
    {
        return name_;
    }
};

class ExportAsConfigGroup
{
protected:

    static std::string Wrap(const std::string &content)
    {
        return "{" + content + "}";
    }
};

class IResource
{
    const HasName *creator_;

protected:

    static std::filesystem::path RelPath(const std::filesystem::path &dst, const std::filesystem::path &base)
    {
        return relative(absolute(dst), absolute(base));
    }

public:

    explicit IResource(const HasName *creator) noexcept
        : creator_(creator)
    {
        
    }

    virtual ~IResource() = default;

    const std::string &GetType() const noexcept
    {
        return creator_->GetName();
    }

    const HasName *GetCreator() const noexcept
    {
        return creator_;
    }

    virtual void DisplayOnDnD() const
    {
        ImGui::Text("type: %s", GetType().c_str());
    }
};

class IResourceCreator : public HasName
{
public:

    explicit IResourceCreator(std::string name) noexcept
        : HasName(std::move(name))
    {

    }

    virtual ~IResourceCreator() = default;
};

template<typename TResourceCreatorCategory>
class ResourceFactory
{
public:

    using Resource = typename TResourceCreatorCategory::Resource;
    using Creator = TResourceCreatorCategory;

    void AddCreator(const Creator *creator)
    {
        AGZ_HIERARCHY_TRY

        auto it = name2Creator_.find(creator->GetName());
        if(it != name2Creator_.end())
            throw std::runtime_error("repeated creator name: " + creator->GetName());
        name2Creator_[creator->GetName()] = creator;

        AGZ_HIERARCHY_WRAP("in registering resource creator")
    }

    const Creator &operator[](const std::string &name) const
    {
        AGZ_HIERARCHY_TRY

        auto it = name2Creator_.find(name);
        if(it == name2Creator_.end())
            throw std::runtime_error("unknown creator name: " + name);
        return *it->second;

        AGZ_HIERARCHY_WRAP("in getting creator from creator factory")
    }

    auto begin() const { return name2Creator_.begin(); }
    auto end()   const { return name2Creator_.end(); }

    auto begin() { return name2Creator_.begin(); }
    auto end()   { return name2Creator_.end(); }

private:

    static_assert(std::is_base_of_v<IResource, Resource>);
    static_assert(std::is_base_of_v<IResourceCreator, TResourceCreatorCategory>);

    std::map<std::string, const TResourceCreatorCategory*> name2Creator_;
};

#define DEFINE_DEFAULT_RESOURCE_CREATOR_INTERFACE(RESOURCE) \
    class RESOURCE##Creator : public IResourceCreator \
    { \
    public: \
        using Resource = RESOURCE; \
        using IResourceCreator::IResourceCreator; \
        virtual std::shared_ptr<RESOURCE> Create() const = 0; \
    }

#define DEFINE_DEFAULT_RESOURCE_CREATOR(CREATOR_BASE, RESOURCE, NAME) \
    class RESOURCE##Creator : public CREATOR_BASE \
    { \
    public: \
        RESOURCE##Creator() : CREATOR_BASE(NAME) { } \
        std::shared_ptr<CREATOR_BASE::Resource> Create() const override \
        { \
            return std::make_shared<RESOURCE>(this); \
        } \
    }

template<typename TResourceBase>
class ResourceCommon :
    public IResource,
    public ExportAsConfigGroup
{
public:

    using IResource::IResource;

    virtual std::shared_ptr<TResourceBase> Clone() const = 0;
};

template<typename TResourceBase, typename TResource>
class ResourceCommonImpl : public TResourceBase
{
public:

    using TResourceBase::TResourceBase;

    std::shared_ptr<TResourceBase> Clone() const override
    {
        auto pThis = dynamic_cast<const TResource*>(this);
        return std::make_shared<TResource>(*pThis);
    }
};

template<typename TResourceFactory>
class ResourceSlot;

class IFilmFilterCreator;
using FilmFilterFactory = ResourceFactory<IFilmFilterCreator>;
using FilmFilterSlot = ResourceSlot<FilmFilterFactory>;

class IFresnelCreator;
using FresnelFactory = ResourceFactory<IFresnelCreator>;
using FresnelSlot = ResourceSlot<FresnelFactory>;

class ILightCreator;
using LightFactory = ResourceFactory<ILightCreator>;
using LightSlot = ResourceSlot<LightFactory>;

class IMaterialCreator;
using MaterialFactory = ResourceFactory<IMaterialCreator>;
using MaterialSlot = ResourceSlot<MaterialFactory>;

class ISamplerCreator;
using SamplerFactory = ResourceFactory<ISamplerCreator>;
using SamplerSlot = ResourceSlot<SamplerFactory>;

class ITextureCreator;
using TextureFactory = ResourceFactory<ITextureCreator>;
using TextureSlot = ResourceSlot<TextureFactory>;

#define TRESOURCE_LIST IFilmFilter, IFresnel, ILight, IMaterial, ISampler, ITexture
#define TRESOURCE_FACTORY_LIST FilmFilterFactory, FresnelFactory, LightFactory, MaterialFactory, SamplerFactory, TextureFactory

}; // namespace Atrc::Editor
