#pragma once
#include "Origin.h"

using namespace origin;

class TestLight
{
public:
	TestLight(const std::shared_ptr<Model>& model);
	void OnRender(const glm::mat4& viewProjection);

	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 color = glm::vec3(1.0f);
private:
	std::shared_ptr<Model> m_Model;
	std::shared_ptr<Shader> m_Shader;
};

