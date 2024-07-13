// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once

#include "Origin/Asset/Asset.h"
#include "Origin/Core/UUID.h"

namespace origin
{
    class Scene;
    class ModelComponent;

    struct ModelLoaderPanel
    {
        static void Show(UUID entityID, ModelComponent *component, AssetHandle handle, Scene *scene);
        static void OnUIRender();
    };
}