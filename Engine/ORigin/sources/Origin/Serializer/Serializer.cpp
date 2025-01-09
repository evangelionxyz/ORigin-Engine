// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Serializer.hpp"

namespace origin {

Serializer::Serializer(const std::filesystem::path &filepath)
	: m_Filepath(filepath)
{
}

void Serializer::Serialize() const
{
	std::ofstream output_file(m_Filepath);
	output_file << m_Emitter.c_str();
	output_file.close();
}

void Serializer::Serialize(const std::filesystem::path& filepath)
{
	m_Filepath = filepath;
	std::ofstream output_file(m_Filepath);
	output_file << m_Emitter.c_str();
	output_file.close();
}

void Serializer::BeginMap()
{
	m_Emitter << YAML::BeginMap;
}

void Serializer::BeginMap(const std::string& map_name)
{
	m_Emitter << YAML::Key << map_name << YAML::Value << YAML::BeginMap;
}

void Serializer::EndMap()
{
	m_Emitter << YAML::EndMap;
}

void Serializer::BeginSequence()
{
	m_Emitter << YAML::BeginSeq;
}

void Serializer::BeginSequence(const std::string& sequence_name)
{
	m_Emitter << YAML::Key << sequence_name << YAML::BeginSeq;
}

void Serializer::EndSequence()
{
	m_Emitter << YAML::EndSeq;
}

YAML::Node Serializer::Deserialize(const std::filesystem::path& filepath)
{
	std::ifstream input_file(filepath);
	std::stringstream buffer;
	buffer << input_file.rdbuf();

	input_file.close();

	return YAML::Load(buffer.str());
}

}
