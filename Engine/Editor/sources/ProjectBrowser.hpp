// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Origin/Core/Layer.h"

namespace origin {

class EditorApp;

class ProjectBrowser : public Layer
{
public:
    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate(Timestep ts) override;
    void OnGuiRender() override;
    void OnEvent(Event& e) override;

private:
    
};

}
