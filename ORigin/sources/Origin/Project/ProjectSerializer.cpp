#include "pch.h"
#include "ProjectSerializer.h"

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
		const auto& config = m_Project->GetConfig();

		YAML::Emitter out;
		out << YAML::BeginMap;
		{
			out << YAML::Key << "Project" << YAML::Value;
			out << YAML::BeginMap;
			{
				out << YAML::Key << "Name" << YAML::Value << config.Name;
				out << YAML::Key << "StartScene" << YAML::Value << (uint64_t)config.StartScene;
				out << YAML::Key << "AssetDirectory" << YAML::Value << config.AssetDirectory.string();
				out << YAML::Key << "AssetRegistry" << YAML::Value << config.AssetRegistry.string();
				out << YAML::Key << "ScriptModulePath" << YAML::Value << config.ScriptModulePath.string();
				out << YAML::EndMap;
			}
			out << YAML::EndMap;
		}

		std::ofstream fout(filepath.string());
		fout << out.c_str();

		return true;
	}

	bool ProjectSerializer::Deserialize(const std::filesystem::path& filepath)
	{
		auto& config = m_Project->GetConfig();

		YAML::Node data;
		try
		{
			data = YAML::LoadFile(filepath.string());
		}
		catch (const std::exception& e)
		{
			OGN_CORE_ERROR("Failed to load project file '{0}'\n			{1}", filepath, e.what());
			return false;
		}

		YAML::Node projectNode = data["Project"];
		if (!projectNode)
			return false;

		config.Name = projectNode["Name"].as<std::string>();
		config.StartScene = projectNode["StartScene"].as<uint64_t>();
		config.AssetDirectory = projectNode["AssetDirectory"].as<std::string>();
		config.AssetRegistry = projectNode["AssetRegistry"].as<std::string>();
		config.ScriptModulePath = projectNode["ScriptModulePath"].as<std::string>();

		OGN_CORE_INFO("ProjectSerializer::Deserializing {}", filepath.string());
		OGN_CORE_INFO("\n	Name: {0}\n	Start Scene: {1}\n	AssetDirectory: {2}\n	Asset Registry: {3}\n	ScriptModulePath: {4}",
			config.Name,
			config.AssetDirectory.string(),
			config.AssetRegistry,
			config.ScriptModulePath.string());

		return true;
	}

}