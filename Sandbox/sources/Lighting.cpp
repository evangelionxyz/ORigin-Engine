#include "Lighting.h"

TestLight::TestLight(const std::shared_ptr<Model>& model)
	: m_Model(model)
{
	m_Shader = Shader::Create("Resources/Sandbox/Light.glsl");
}

void TestLight::OnRender(const glm::mat4& viewProjection)
{
	m_Shader->Enable();
	glm::mat4 transform = glm::translate(glm::mat4(1.0f), position);
	m_Shader->SetMatrix("viewProjection", viewProjection);
	m_Shader->SetMatrix("modelTransform", transform);
	m_Shader->SetVector("color", color);

	m_Model->Draw();
	m_Shader->Disable();
}
