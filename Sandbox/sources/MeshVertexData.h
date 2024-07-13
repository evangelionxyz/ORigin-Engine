#pragma once
#include <glm/glm.hpp>

struct MeshVertexData
{
	glm::vec3 Position;
	glm::vec3 Normals;
	glm::vec4 Color;
	glm::vec2 TexCoord;
	float TexIndex;
	int EntityID;
};