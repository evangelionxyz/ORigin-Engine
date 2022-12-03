// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine
#include "Origin\EntryPoint.h"
#include "Sandbox.h"

#include <glm\gtc\type_ptr.hpp>

namespace Origin {

  void Sandbox::OnAttach()
  {
    m_Texture = Texture2D::Create("assets/textures/checkerboard.jpg");
    m_Sprites = Texture2D::Create("assets/textures/Spritesheet.png");
    m_Logo = Texture2D::Create("assets/textures/icon_editor.png");

    m_StairsTexture = SubTexture2D::CreateFromCoords(m_Sprites, glm::vec2(7.0f, 6.0f), glm::vec2(128, 128));
    m_BarrelTexture = SubTexture2D::CreateFromCoords(m_Sprites, glm::vec2(8.0f, 2.0f), glm::vec2(128, 128));
    m_TreeTexture = SubTexture2D::CreateFromCoords(m_Sprites, glm::vec2(2.0f, 1.0f), glm::vec2(128, 128), glm::vec2(1, 2));
  }

  void Sandbox::OnUpdate(Timestep ts)
  {
     RenderCommand::ClearColor(color);
     RenderCommand::Clear();

     m_CameraController.OnUpdate(ts);
     Renderer2D::ResetStats();

    {
      Renderer2D::BeginScene(m_CameraController.GetCamera());

      Renderer2D::DrawQuad(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.5f), glm::vec4(1.0f, 0.2f, 0.3f, 1.0f));
      Renderer2D::DrawQuad(glm::vec3(0.0f, 0.0f, -0.1f), glm::vec2(20.0f), m_Texture, 8.0f);

      Renderer2D::DrawRotatedQuad(glm::vec3(2.0f, -1.0f, 0.0f), rotation * 2, glm::vec2(0.2f, 0.4f), glm::vec4(1.0f, 0.5f, 0.0f, 1.0f));
      Renderer2D::DrawRotatedQuad(glm::vec3(-0.5f, 0.0f, 0.1f), -rotation, glm::vec2(1.0f, 0.2f), glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));
      Renderer2D::DrawRotatedQuad(glm::vec3(1.0f, 0.0f, 0.0f), rotation, glm::vec2(1.0f), m_Texture, 5.0f);

      rotation += incremental * ts;

      for (float y = -5.0f; y < 5.0f; y += 0.5f)
      {
        for (float x = -5.0f; x < 5.0f; x += 0.5f)
        {
          glm::vec4 color = glm::vec4((x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.8f);
          Renderer2D::DrawQuad(glm::vec3(x, y, 0.0f), glm::vec2(0.45f), color);
        }
      }
      Renderer2D::EndScene();
    }

    {
      Renderer2D::BeginScene(m_CameraController.GetCamera());
      Renderer2D::DrawQuad(glm::vec3(-3.0f, -2.0f, 0.2f), glm::vec2(1.0f), m_StairsTexture);
      Renderer2D::DrawQuad(glm::vec3(-2.0f, -2.0f, 0.2f), glm::vec2(1.0f), m_BarrelTexture);
      Renderer2D::DrawQuad(glm::vec3(-1.0f, -2.0f, 0.2f), glm::vec2(1.0f, 2.0f), m_TreeTexture);
      Renderer2D::EndScene();
    }
  }

  void Sandbox::OnEvent(Event& e)
  {
    m_CameraController.OnEvent(e);

    if (Input::IsKeyPressed(OGN_KEY_ESCAPE))
      Application::Get().Close();
  }

}

