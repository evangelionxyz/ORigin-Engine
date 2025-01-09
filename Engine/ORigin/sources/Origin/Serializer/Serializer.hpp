// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef SERIALIZER_H
#define SERIALIZER_H

#include "Origin/Core/UUID.h"
#include "Origin/Math/Math.h"

#include <yaml-cpp/yaml.h>
#include <glm/glm.hpp>
#include <string>

namespace YAML {
template <>
struct convert<origin::UUID>
{
	static Node encode(const origin::UUID uuid)
	{
		Node node;
		node.push_back(static_cast<uint64_t>(uuid));
		return node;
	}

	static bool decode(const Node &node, origin::UUID uuid)
	{
		uuid = node.as<uint64_t>();
		return true;
	}
};

template<>
struct convert<origin::Rect>
{
	static Node encode(const origin::Rect &rect)
	{
		Node node;
		node.push_back(rect.min.x);
		node.push_back(rect.min.y);
		node.push_back(rect.max.x);
		node.push_back(rect.max.y);
		return node;
	}

	static bool decode(const Node &node, origin::Rect &rect)
	{
		if (!node.IsSequence() || node.size() != 4)
			return false;

		rect.SetMin({ node[0].as<float>(), node[1].as<float>() });
		rect.SetMax({ node[2].as<float>(), node[3].as<float>() });
		return true;
	}
};

template <>
struct convert<glm::vec2>
{
	static Node encode(const glm::vec2 &rhs)
	{
		Node node;
		node.push_back(rhs.x);
		node.push_back(rhs.y);
		return node;
	}

	static bool decode(const Node &node, glm::vec2 &rhs)
	{
		if (!node.IsSequence() || node.size() != 2)
			return false;

		rhs.x = node[0].as<float>();
		rhs.y = node[1].as<float>();
		return true;
	}
};

template <>
struct convert<glm::vec3>
{
	static Node encode(const glm::vec3 &rhs)
	{
		Node node;
		node.push_back(rhs.x);
		node.push_back(rhs.y);
		node.push_back(rhs.z);
		return node;
	}

	static bool decode(const Node &node, glm::vec3 &rhs)
	{
		if (!node.IsSequence() || node.size() != 3)
			return false;

		rhs.x = node[0].as<float>();
		rhs.y = node[1].as<float>();
		rhs.z = node[2].as<float>();
		return true;
	}
};

template <>
struct convert<glm::vec4>
{
	static Node encode(const glm::vec4 &rhs)
	{
		Node node;
		node.push_back(rhs.x);
		node.push_back(rhs.y);
		node.push_back(rhs.z);
		node.push_back(rhs.w);
		return node;
	}

	static bool decode(const Node &node, glm::vec4 &rhs)
	{
		if (!node.IsSequence() || node.size() != 4)
			return false;

		rhs.x = node[0].as<float>();
		rhs.y = node[1].as<float>();
		rhs.z = node[2].as<float>();
		rhs.w = node[3].as<float>();
		return true;
	}
};

template <>
struct convert<glm::ivec2>
{
	static Node encode(const glm::ivec2 &rhs)
	{
		Node node;
		node.push_back(rhs.x);
		node.push_back(rhs.y);
		return node;
	}

	static bool decode(const Node &node, glm::ivec2 &rhs)
	{
		if (!node.IsSequence() || node.size() != 2)
			return false;

		rhs.x = node[0].as<int>();
		rhs.y = node[1].as<int>();
		return true;
	}
};

template <>
struct convert<glm::ivec3>
{
	static Node encode(const glm::ivec3 &rhs)
	{
		Node node;
		node.push_back(rhs.x);
		node.push_back(rhs.y);
		node.push_back(rhs.z);
		return node;
	}

	static bool decode(const Node &node, glm::ivec3 &rhs)
	{
		if (!node.IsSequence() || node.size() != 3)
			return false;

		rhs.x = node[0].as<int>();
		rhs.y = node[1].as<int>();
		rhs.z = node[2].as<int>();
		return true;
	}
};

template <>
struct convert<glm::ivec4>
{
	static Node encode(const glm::ivec4 &rhs)
	{
		Node node;
		node.push_back(rhs.x);
		node.push_back(rhs.y);
		node.push_back(rhs.z);
		node.push_back(rhs.w);
		return node;
	}

	static bool decode(const Node &node, glm::ivec4 &rhs)
	{
		if (!node.IsSequence() || node.size() != 4)
			return false;

		rhs.x = node[0].as<int>();
		rhs.y = node[1].as<int>();
		rhs.z = node[2].as<int>();
		rhs.w = node[3].as<int>();
		return true;
	}
};

template <>
struct convert<glm::quat>
{
	static Node encode(const glm::quat &rhs)
	{
		Node node;
		node.push_back(rhs.x);
		node.push_back(rhs.y);
		node.push_back(rhs.z);
		node.push_back(rhs.w);
		return node;
	}

	static bool decode(const Node &node, glm::quat &rhs)
	{
		if (!node.IsSequence() || node.size() != 4)
			return false;

		rhs.x = node[0].as<float>();
		rhs.y = node[1].as<float>();
		rhs.z = node[2].as<float>();
		rhs.w = node[3].as<float>();
		return true;
	}
};
}

namespace origin {
static YAML::Emitter &operator<<(YAML::Emitter &out, const Rect &rect)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << rect.min.x << rect.min.y << rect.max.x << rect.max.y << YAML::EndSeq;
	return out;
}

static YAML::Emitter &operator<<(YAML::Emitter &out, const glm::vec2 &v)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
	return out;
}

static YAML::Emitter &operator<<(YAML::Emitter &out, const glm::vec3 &v)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
	return out;
}

static YAML::Emitter &operator<<(YAML::Emitter &out, const glm::vec4 &v)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
	return out;
}

static YAML::Emitter &operator<<(YAML::Emitter &out, const glm::quat &q)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << q.x << q.y << q.z << q.w << YAML::EndSeq;
	return out;
}

}

namespace origin {
class Serializer
{
public:
	explicit Serializer(const std::filesystem::path &filepath);
	
	void Serialize() const;
	void Serialize(const std::filesystem::path &filepath);
	
	void BeginMap();
	void BeginMap(const std::string &map_name);
	void EndMap();

	void BeginSequence();
	void BeginSequence(const std::string &sequence_name);
	void EndSequence();
	
	template<typename T>
	void AddKeyValue(const char *key_name, T value)
	{
		m_Emitter << YAML::Key << key_name << YAML::Value << value;
	}

	static YAML::Node Deserialize(const std::filesystem::path &filepath);

	const std::filesystem::path &GetFilepath() const { return m_Filepath; }
	
private:
	YAML::Emitter m_Emitter;
	std::filesystem::path m_Filepath;
};
}

#endif