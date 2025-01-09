// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "EditorSerializer.hpp"
#include "../EditorLayer.hpp"
#include "../Themes.hpp"

#include "Origin/Serializer/Serializer.h"
#include <fstream>
#include <yaml-cpp/yaml.h>

namespace origin
{
    bool EditorSerializer::Serialize(EditorLayer *editor, const std::filesystem::path &filepath)
    {
        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "Editor" << YAML::Value << filepath.stem().string();

        // Editor camera
        out << YAML::Key << "Camera" << YAML::Value << YAML::BeginMap;
        out << YAML::Key << "ProjectionType" << YAML::Value << static_cast<int>(editor->m_EditorCamera.GetProjectionType());
        out << YAML::Key << "Style" << YAML::Value << (int)editor->m_EditorCamera.GetStyle();
        out << YAML::Key << "Position" << YAML::Value << editor->m_EditorCamera.GetPosition();
        out << YAML::Key << "Distance" << YAML::Value << editor->m_EditorCamera.GetDistance();
        out << YAML::Key << "Yaw" << YAML::Value << editor->m_EditorCamera.GetYaw();
        out << YAML::Key << "Pitch" << YAML::Value << editor->m_EditorCamera.GetPitch();
        out << YAML::Key << "FOV" << YAML::Value << editor->m_EditorCamera.GetFOV();
        out << YAML::Key << "AspectRatio" << YAML::Value << editor->m_EditorCamera.GetAspectRatio();
        out << YAML::Key << "NearPlane" << YAML::Value << editor->m_EditorCamera.GetNear();
        out << YAML::Key << "FarPlane" << YAML::Value << editor->m_EditorCamera.GetFar();
        out << YAML::Key << "OrthoNearPlane" << YAML::Value << editor->m_EditorCamera.GetOrthoNear();
        out << YAML::Key << "OrthoFarPlane" << YAML::Value << editor->m_EditorCamera.GetOrthoFar();
        out << YAML::Key << "OrthoScale" << YAML::Value << editor->m_EditorCamera.GetOrthoScale();
        out << YAML::EndMap;

        // Windows
        out << YAML::Key << "Windows" << YAML::Value << YAML::BeginMap;
        out << YAML::Key << "MenuFullScreen" << YAML::Value << editor->GuiMenuFullscreen;
        out << YAML::Key << "MenuStyleWindow" << YAML::Value << editor->GuiMenuStyleWindow;
        out << YAML::Key << "RenderSettingsWindow" << YAML::Value << editor->GuiRenderSettingsWindow;
        out << YAML::Key << "ImGuiDemoWindow" << YAML::Value << editor->GuiImGuiDemoWindow;
        out << YAML::Key << "ConsoleWindow" << YAML::Value << editor->GuiConsoleWindow;
        out << YAML::Key << "Theme" << YAML::Value << Themes::GetCurrentTheme();
        out << YAML::EndMap;

        // Settings
        out << YAML::Key << "Settings" << YAML::Value << YAML::BeginMap;
        out << YAML::Key << "Draw3DGrid" << YAML::Value << editor->m_Draw3DGrid;
        out << YAML::Key << "Draw2DGrid" << YAML::Value << editor->m_Draw2DGrid;
        out << YAML::Key << "VisualizeCollider" << YAML::Value << editor->m_VisualizeCollider;
        out << YAML::Key << "VisualizeBoundingBox" << YAML::Value << editor->m_VisualizeBoundingBox;
        out << YAML::Key << "ClearColor" << YAML::Value << editor->m_ClearColor;
        out << YAML::Key << "DrawLineMode" << YAML::Value << editor->m_DrawLineModeActive;
        out << YAML::EndMap;

        out << YAML::EndMap;

        std::ofstream outFile(filepath);
        outFile << out.c_str();
        outFile.close();
        return true;
    }

    bool EditorSerializer::Deserialize(EditorLayer *editor, const std::filesystem::path &filepath)
    {
        if (!std::filesystem::exists(filepath))
        {
            PUSH_CONSOLE_WARNING("Could not find editor config");
            return false;
        }

        std::ifstream stream(filepath);
        std::stringstream strStream;
        strStream << stream.rdbuf();
        stream.close();

        YAML::Node editor_node = YAML::Load(strStream.str());
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

            float fov = camera_node["FOV"].as<float>();
            float aspectRatio = camera_node["AspectRatio"].as<float>();
            editor->m_EditorCamera.InitPerspective(fov, aspectRatio, camera_node["NearPlane"].as<float>(), camera_node["FarPlane"].as<float>());
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