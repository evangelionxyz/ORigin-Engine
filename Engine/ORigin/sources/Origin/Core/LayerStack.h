// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#ifndef LAYER_STACK_H
#define LAYER_STACK_H

#include "Layer.h"

namespace origin {

    class LayerStack
    {
    private:
        std::vector<Layer*> m_Layers;

    public:
        LayerStack();

        void PushLayer(Layer* layer);
        void PushOverlay(Layer* overlay);
        void PopLayer(Layer* layer);
        void PopOverlay(Layer* overlay);

        void Shutdown();

        std::vector<Layer*>::const_iterator begin() { return m_Layers.begin(); }
        std::vector<Layer*>::const_iterator end() { return m_Layers.end(); }

        std::vector<Layer*>::const_reverse_iterator rbegin() { return m_Layers.rbegin(); }
        std::vector<Layer*>::const_reverse_iterator rend() { return m_Layers.rend(); }

    };
}

#endif