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
        auto it = std::find(m_Layers.begin(), m_Layers.end(), layer);
        if (it != m_Layers.end())
            m_Layers.erase(it);
    }

    void LayerStack::PopOverlay(Layer* overlay)
    {
        auto it = std::find(m_Layers.begin(), m_Layers.end(), overlay);
        if (it != m_Layers.end())
            m_Layers.erase(it);
    }

    void LayerStack::Shutdown()
    {
        for (Layer* layer : m_Layers) 
        {
            layer->OnDetach();
            delete layer;
        }
            
    }
}