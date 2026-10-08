// Copyright 2023-2024 DreamWorks Animation LLC
// SPDX-License-Identifier: Apache-2.0

#include "ArrasRenderer.h"
#include <hydramoonray/NullRenderer.h>
#include <hydramoonray/renderDelegate.h>
#include <pxr/imaging/hd/rendererPlugin.h>
#include <pxr/imaging/hd/rendererPluginRegistry.h>

#include <iostream>

PXR_NAMESPACE_OPEN_SCOPE // this does not work unless inside the pxr namespace

class HdMoonrayRendererPlugin final : public pxr::HdRendererPlugin {
public:
    HdMoonrayRendererPlugin() {}

    pxr::HdRenderDelegate *CreateRenderDelegate() override {
        return new hdMoonray::HdMoonray_RenderDelegate(new hdMoonray::ArrasRenderer());
    }

    pxr::HdRenderDelegate *CreateRenderDelegate(pxr::HdRenderSettingsMap const& settings) override {

        if (hdMoonray::RenderSettings::staticDisableRender(settings)) {
            std::cout << "HdMoonray: rendering is DISABLED" << std::endl;
            auto rd = new hdMoonray::HdMoonray_RenderDelegate(new hdMoonray::NullRenderer(),settings);
            rd->options().setDisableRender(true);
            return rd;
        }

        return new hdMoonray::HdMoonray_RenderDelegate(new hdMoonray::ArrasRenderer(),settings);
    }

    void DeleteRenderDelegate(pxr::HdRenderDelegate *renderDelegate) override {
        delete renderDelegate;
    }

#if HD_API_VERSION >= 97
// USD 26.08 removed the container-handle overload from HdRendererPlugin
// (its pure virtual became the HdRendererCreateArgsSchema one, which the
// HdRendererCreateArgs override below still satisfies); keeping this
// declaration there would break the `override` on 26.08 while 26.03/26.05
// need it as an available (non-pure) virtual.
#if PXR_VERSION < 2608
    bool IsSupported(
        HdContainerDataSourceHandle const &rendererCreateArgs,
        std::string *reasonWhyNot = nullptr) const override {
        return true;
    }
#endif

    bool IsSupported(
        HdRendererCreateArgs const &rendererCreateArgs,
        std::string *reasonWhyNot = nullptr) const override {
        return true;
    }
#elif HD_API_VERSION < 83
    bool IsSupported(bool gpuEnabled = true) const override {
        return true;
    }
#else
    bool IsSupported(
        HdRendererCreateArgs const &rendererCreateArgs,
        std::string * reasonWhyNot = nullptr) const override {
            return true;
        }
#endif

private:
    // uncopyable
    HdMoonrayRendererPlugin(const HdMoonrayRendererPlugin&)             = delete;
    HdMoonrayRendererPlugin &operator =(const HdMoonrayRendererPlugin&) = delete;
};

TF_REGISTRY_FUNCTION(TfType)
{
    pxr::HdRendererPluginRegistry::Define<HdMoonrayRendererPlugin>();
}

PXR_NAMESPACE_CLOSE_SCOPE
