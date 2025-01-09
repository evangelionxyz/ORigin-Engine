// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "LayerStack.h"

#include <vector>

namespace origin
{
    LayerStack::LayerStack()
    {
    }

    void LayerStack::PushLayer(Layer* layer)
    {
        m_Layers.emplace(m_Layers.end(), layer);
    }

    void LayerStack::PushOverlay(Layer* overlay)
    {
        m_Layers.emplace_back(overlay);
    }

    void LayerStack::PopLayer(Layer* layer)
    {
        if (const auto it = std::ranges::find(m_Layers, layer); it != m_Layers.end())
        {
            m_Layers.erase(it);
            delete layer;
        }
    }

    void LayerStack::PopOverlay(Layer* overlay)
    {
        if (const auto it = std::ranges::find(m_Layers, overlay); it != m_Layers.end())
        {
            m_Layers.erase(it);
            delete overlay;
        }
    }

    void LayerStack::Shutdown() const
    {
        for (Layer* layer : m_Layers) 
        {
            layer->OnDetach();
            delete layer;
        }
            
    }
}