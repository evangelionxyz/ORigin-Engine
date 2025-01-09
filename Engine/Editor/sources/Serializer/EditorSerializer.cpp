// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "EditorSerializer.hpp"
#include "../EditorLayer.hpp"
#include "../Themes.hpp"

#include "Origin/Serializer/Serializer.hpp"
#include <fstream>
#include <yaml-cpp/yaml.h>

namespace origin
{
    bool EditorSerializer::Serialize(EditorLayer *editor, const std::filesystem::path &filepath)
    {
        Serializer sr(filepath);

        // begin root node
        sr.BeginMap();
        sr.AddKeyValue("Editor", filepath.stem().string());

        // editor camera
        sr.BeginMap("Camera");
        sr.AddKeyValue("ProjectionType", static_cast<int>(editor->m_EditorCamera.GetProjectionType()));
        sr.AddKeyValue("Style", static_cast<int>(editor->m_EditorCamera.GetStyle()));
        sr.AddKeyValue("Position", editor->m_EditorCamera.GetPosition());
        sr.AddKeyValue("Distance", editor->m_EditorCamera.GetDistance());
        sr.AddKeyValue("Yaw", editor->m_EditorCamera.GetYaw());
        sr.AddKeyValue("Pitch", editor->m_EditorCamera.GetPitch());
        sr.AddKeyValue("FOV", editor->m_EditorCamera.GetFOV());
        sr.AddKeyValue("AspectRatio", editor->m_EditorCamera.GetAspectRatio());
        sr.AddKeyValue("NearPlane", editor->m_EditorCamera.GetNear());
        sr.AddKeyValue("FarPlane", editor->m_EditorCamera.GetFar());
        sr.AddKeyValue("OrthoNearPlane", editor->m_EditorCamera.GetOrthoNear());
        sr.AddKeyValue("OrthoFarPlane", editor->m_EditorCamera.GetOrthoFar());
        sr.AddKeyValue("OrthoScale", editor->m_EditorCamera.GetOrthoScale());
        sr.EndMap();

        // windows
        sr.BeginMap("Windows");
        sr.AddKeyValue("MenuFullScreen", editor->GuiMenuFullscreen);
        sr.AddKeyValue("MenuStyleWindow", editor->GuiMenuStyleWindow);
        sr.AddKeyValue("RenderSettingsWindow", editor->GuiRenderSettingsWindow);
        sr.AddKeyValue("ImGuiDemoWindow", editor->GuiImGuiDemoWindow);
        sr.AddKeyValue("ConsoleWindow", editor->GuiConsoleWindow);
        sr.AddKeyValue("Theme", Themes::GetCurrentTheme());
        sr.EndMap();

        // Settings
        sr.BeginMap("Settings");
        sr.AddKeyValue("Draw3DGrid", editor->m_Draw3DGrid);
        sr.AddKeyValue("Draw2DGrid", editor->m_Draw2DGrid);
        sr.AddKeyValue("VisualizeCollider", editor->m_VisualizeCollider);
        sr.AddKeyValue("VisualizeBoundingBox", editor->m_VisualizeBoundingBox);
        sr.AddKeyValue("ClearColor", editor->m_ClearColor);
        sr.AddKeyValue("DrawLineMode", editor->m_DrawLineModeActive);
        sr.EndMap();

        // end root node
        sr.EndMap();

        sr.Serialize();
        
        return true;
    }

    bool EditorSerializer::Deserialize(EditorLayer *editor, const std::filesystem::path &filepath)
    {
        if (!exists(filepath))
        {
            PUSH_CONSOLE_WARNING("Could not find editor config");
            return false;
        }

        YAML::Node editor_node = Serializer::Deserialize(filepath);
        if (!editor_node["Editor"])
            return false;

        // Deserialize camera
        if (YAML::Node camera_node = editor_node["Camera"])
        {
            editor->m_EditorCamera.SetPosition(camera_node["Position"].as<glm::vec3>());
            editor->m_EditorCamera.SetDistance(camera_node["Distance"].as<float>());
            editor->m_EditorCamera.SetYaw(camera_node["Yaw"].as<float>());
            editor->m_EditorCamera.SetPitch(camera_node["Pitch"].as<float>());
            editor->m_EditorCamera.SetNear(camera_node["NearPlane"].as<float>());
            editor->m_EditorCamera.SetFar(camera_node["FarPlane"].as<float>());
            editor->m_EditorCamera.SetOrthoNear(camera_node["OrthoNearPlane"].as<float>());
            editor->m_EditorCamera.SetOrthoFar(camera_node["OrthoFarPlane"].as<float>());
            editor->m_EditorCamera.SetOrthoScale(camera_node["OrthoScale"].as<float>());

            const float fov = camera_node["FOV"].as<float>();
            const float aspect_ratio = camera_node["AspectRatio"].as<float>();
            editor->m_EditorCamera.InitPerspective(fov, aspect_ratio, camera_node["NearPlane"].as<float>(), camera_node["FarPlane"].as<float>());
            editor->m_EditorCamera.SetProjectionType(static_cast<ProjectionType>(camera_node["ProjectionType"].as<int>()));
            editor->m_EditorCamera.SetStyle(static_cast<CameraStyle>(camera_node["Style"].as<int>()));
        }

        // Deserialize windows
        if (YAML::Node windows_node = editor_node["Windows"])
        {
            editor->GuiMenuFullscreen = windows_node["MenuFullScreen"].as<bool>();
            editor->GuiMenuStyleWindow = windows_node["MenuStyleWindow"].as<bool>();
            editor->GuiRenderSettingsWindow = windows_node["RenderSettingsWindow"].as<bool>();
            editor->GuiImGuiDemoWindow = windows_node["ImGuiDemoWindow"].as<bool>();
            editor->GuiConsoleWindow = windows_node["ConsoleWindow"].as<bool>();
            Themes::ApplyTheme(windows_node["Theme"].as<std::string>());
        }

        // Deserialize settings
        if (YAML::Node settings_node = editor_node["Settings"])
        {
            editor->m_Draw3DGrid = settings_node["Draw3DGrid"].as<bool>();
            editor->m_Draw2DGrid = settings_node["Draw2DGrid"].as<bool>();
            editor->m_VisualizeCollider = settings_node["VisualizeCollider"].as<bool>();
            editor->m_VisualizeBoundingBox = settings_node["VisualizeBoundingBox"].as<bool>();
            editor->m_ClearColor = settings_node["ClearColor"].as<glm::vec4>();
            editor->m_DrawLineModeActive = settings_node["DrawLineMode"].as<bool>();
        }

        return true;
    }
}