#pragma once
#include <Origin.h>
#include <glm\glm.hpp>

#include <unordered_map>
#include <string>

using namespace Origin;

class Draw
{
public:
	Draw() = default;

	void Init();
	void Begin();
	void RenderCube();
	void RenderLight();

	std::shared_ptr<Shader>& GetShader(std::string name);
	std::unordered_map<std::string, std::shared_ptr<Shader>> shaderMap;
};

