// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"

#include "ProjectSerializer.h"
#include "Origin/Asset/AssetManager.h"
#include "Origin/Core/Assert.h"
#include "Origin/Profiler/Profiler.h"

#include <stdint.h>
#include <fstream>
#include <yaml-cpp/yaml.h>

namespace origin
{
	ProjectSerializer::ProjectSerializer(std::shared_ptr<Project> project)
		: m_Project(project)
	{
	}

	bool ProjectSerializer::Serialize(const std::filesystem::path& filepath)
	{
		OGN_PROFILER_FUNCTION();

		const auto& config = m_Project->GetConfig();
		YAML::Emitter out;
		out << YAML::BeginMap;
		{
			out << YAML::Key << "Project" << YAML::Value;
			out << YAML::BeginMap;
			{
				out << YAML::Key << "Name" << YAML::Value << config.Name;

				AssetHandle handle = config.StartScene;
				if (handle != 0)
				{
					if (!AssetManager::IsAssetHandleValid(handle))
						handle = 0;
				}

				out << YAML::Key << "StartScene" << YAML::Value << handle;
				out << YAML::Key << "AssetDirectory" << YAML::Value << config.AssetDirectory.string();
				out << YAML::Key << "AssetRegistry" << YAML::Value << config.AssetRegistry.string();
				out << YAML::Key << "ScriptModulePath" << YAML::Value << config.ScriptModulePath.string();
				out << YAML::EndMap;
			}
			out << YAML::EndMap;
		}

		std::ofstream fout(filepath.string());
		fout << out.c_str();

		// Serialized the updated AssetRegistry
		if(m_Project->GetAssetManager())
		{
			m_Project->GetEditorAssetManager()->SerializeAssetRegistry();
		}

		OGN_CORE_INFO("[Project Serialzer] Project serialized in {0}", filepath.string());
		return true;
	}

	bool ProjectSerializer::Deserialize(const std::filesystem::path& filepath)
	{
		OGN_PROFILER_FUNCTION();

		if (!std::filesystem::exists(filepath))
		{
			return false;
		}

		auto& config = m_Project->GetConfig();

		YAML::Node data = YAML::LoadFile(filepath.string());

		YAML::Node projectNode = data["Project"];
		if (!projectNode)
			return false;

		config.Name = projectNode["Name"].as<std::string>();
		config.StartScene = projectNode["StartScene"].as<uint64_t>();
		config.AssetDirectory = projectNode["AssetDirectory"].as<std::string>();
		config.AssetRegistry = projectNode["AssetRegistry"].as<std::string>();
		config.ScriptModulePath = projectNode["ScriptModulePath"].as<std::string>();

		OGN_CORE_INFO("ProjectSerializer::Deserialize {0}", filepath.string());
		OGN_CORE_INFO("	Name: {0}", config.Name);
		OGN_CORE_INFO("	Start Scene: {0}", config.StartScene);
		OGN_CORE_INFO("	Asset Reg: {0}", config.AssetRegistry.string());

		return true;
	}

}