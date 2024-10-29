// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "EditorSerializer.h"
#include "../EditorLayer.h"

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
        out << YAML::Key << "ProjectionType" << YAML::Value << (int)editor->m_EditorCamera.GetProjectionType();
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
        out << YAML::Key << "Theme" << YAML::Value << editor->m_Themes.GetCurrentTheme();
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

        YAML::Node edtr = YAML::Load(strStream.str());
        if (!edtr["Editor"])
            return false;

        // Deserialize camera
        if (YAML::Node cameraNode = edtr["Camera"])
        {
            editor->m_EditorCamera.SetPosition(cameraNode["Position"].as<glm::vec3>());
            editor->m_EditorCamera.SetDistance(cameraNode["Distance"].as<float>());
            editor->m_EditorCamera.SetYaw(cameraNode["Yaw"].as<float>());
            editor->m_EditorCamera.SetPitch(cameraNode["Pitch"].as<float>());
            editor->m_EditorCamera.SetNear(cameraNode["NearPlane"].as<float>());
            editor->m_EditorCamera.SetFar(cameraNode["FarPlane"].as<float>());
            editor->m_EditorCamera.SetOrthoNear(cameraNode["OrthoNearPlane"].as<float>());
            editor->m_EditorCamera.SetOrthoFar(cameraNode["OrthoFarPlane"].as<float>());
            editor->m_EditorCamera.SetOrthoScale(cameraNode["OrthoScale"].as<float>());

            float fov = cameraNode["FOV"].as<float>();
            float aspectRatio = cameraNode["AspectRatio"].as<float>();
            editor->m_EditorCamera.InitPerspective(fov, aspectRatio, cameraNode["NearPlane"].as<float>(), cameraNode["FarPlane"].as<float>());
            editor->m_EditorCamera.SetProjectionType((ProjectionType)cameraNode["ProjectionType"].as<int>());
            editor->m_EditorCamera.SetStyle((CameraStyle)cameraNode["Style"].as<int>());
        }

        // Deserialize windows
        if (YAML::Node windowsNode = edtr["Windows"])
        {
            editor->GuiMenuFullscreen = windowsNode["MenuFullScreen"].as<bool>();
            editor->GuiMenuStyleWindow = windowsNode["MenuStyleWindow"].as<bool>();
            editor->GuiRenderSettingsWindow = windowsNode["RenderSettingsWindow"].as<bool>();
            editor->GuiImGuiDemoWindow = windowsNode["ImGuiDemoWindow"].as<bool>();
            editor->GuiConsoleWindow = windowsNode["ConsoleWindow"].as<bool>();
            editor->m_Themes.ApplyTheme(windowsNode["Theme"].as<std::string>());
        }

        // Deserialize settings
        if (YAML::Node settingsNode = edtr["Settings"])
        {
            editor->m_Draw3DGrid = settingsNode["Draw3DGrid"].as<bool>();
            editor->m_Draw2DGrid = settingsNode["Draw2DGrid"].as<bool>();
            editor->m_VisualizeCollider = settingsNode["VisualizeCollider"].as<bool>();
            editor->m_VisualizeBoundingBox = settingsNode["VisualizeBoundingBox"].as<bool>();
            editor->m_ClearColor = settingsNode["ClearColor"].as<glm::vec4>();
            editor->m_DrawLineModeActive = settingsNode["DrawLineMode"].as<bool>();
        }

        return true;
    }
}