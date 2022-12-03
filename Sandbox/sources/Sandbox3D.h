// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

//// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine
//#pragma once
//#include <Origin.h>
//
//namespace Origin
//{
//  class Sandbox3D : public Layer
//  {
//  public:
//    Sandbox3D() : Layer("Sandbox3D")
//    {
//    }
//
//    void OnAttach() override;
//    void OnUpdate(Timestep ts) override;
//
//    void OnEvent(Event& event) override
//    {
//
//    }
//
//  private:
//    std::shared_ptr<VertexArray> va;
//    std::shared_ptr<VertexBuffer> vb;
//    std::shared_ptr<Shader> shader;
//
//    glm::mat4 transform;
//    glm::mat4 projection;
//    glm::vec3 position = glm::vec3(0.0f);
//    float rotation = 0.0f;
//    float increment = 50.0f;
//    glm::vec3 scale = glm::vec3(1.0f);
//  };
//}