// Copyright (c) Evangelion Manuhutu | ORigin Engine

#ifndef PROJECT_BROWSER_HPP
#define PROJECT_BROWSER_HPP

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

#endif