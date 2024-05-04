// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Serializer.h"
#include "MaterialSerializer.h"

#include <fstream>
#include <yaml-cpp\yaml.h>

namespace origin
{
	
	bool MaterialSerializer::Serialize(const std::filesystem::path &filepath, const std::shared_ptr<Material> &mat)
	{
		OGN_PROFILER_FUNCTION();

		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Material";
		{
			out << YAML::BeginMap;
			out << YAML::Key << "AlbedoMap" << YAML::Value << mat->AlbedoMap;
			out << YAML::Key << "Color" << YAML::Value << mat->BufferData.Color;
			out << YAML::Key << "Emission" << YAML::Value << mat->BufferData.Emission;
			out << YAML::Key << "Metallic" << YAML::Value << mat->BufferData.MetallicValue;
			out << YAML::Key << "Roughness" << YAML::Value << mat->BufferData.RoughnessValue;
			out << YAML::Key << "UseNormalMaps" << YAML::Value << mat->BufferData.UseNormalMaps;
			out << YAML::EndMap;
		}
		out << YAML::EndMap;

		std::ofstream outFile(filepath.string());
		outFile << out.c_str();
		return true;
	}

	bool MaterialSerializer::Deserialize(const std::filesystem::path &filepath, std::shared_ptr<Material> &material)
	{
		OGN_PROFILER_FUNCTION();

		std::ifstream stream(filepath);
		std::stringstream strStream;
		strStream << stream.rdbuf();

		YAML::Node mat = YAML::Load(strStream.str());
		if (!mat["Material"])
			return false;

		if (YAML::Node data = mat["Material"])
		{
			material->AlbedoMap = data["AlbedoMap"].as<uint64_t>();
			material->BufferData.Color = data["Color"].as<glm::vec4>();
			material->BufferData.Emission = data["Emission"].as<float>();
			material->BufferData.MetallicValue = data["Metallic"].as<float>();
			material->BufferData.RoughnessValue = data["Roughness"].as<float>();
			material->BufferData.UseNormalMaps = data["UseNormalMaps"].as<bool>();
		}

		return true;
	}

}