#include "Gizmos.h"

#include "Origin\Renderer\Renderer2D.h"
#include "..\EditorLayer.h"

namespace origin {

	void Gizmos::DrawIcon(const EditorCamera& camera)
	{
		auto textures = EditorLayer::Get().m_UITextures;
		auto& reg = EditorLayer::Get().m_ActiveScene->m_Registry;

		auto draw = [&](TransformComponent tc, std::shared_ptr<Texture2D> texture, int entity)
			{
				glm::mat4 transform = glm::mat4(1.0f);
				switch (camera.GetProjectionType())
				{
				case ProjectionType::Perspective:
					transform = glm::translate(glm::mat4(1.0f), tc.Translation)
						* glm::rotate(glm::mat4(1.0f), -camera.GetYaw(), glm::vec3(0, 1, 0))
						* glm::rotate(glm::mat4(1.0f), -camera.GetPitch(), glm::vec3(1, 0, 0))
						* glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
					break;
				case ProjectionType::Orthographic:
					transform = translate(glm::mat4(1.0f), glm::vec3(tc.Translation.x, tc.Translation.y, 0.5f));
					break;
				}
				Renderer2D::DrawQuad(transform, texture, glm::vec2(1.0f), glm::vec4(1.0f), (int)entity);
			};

		auto& cam = reg.view<TransformComponent, CameraComponent>();
		for (auto& entity : cam)
		{
			auto& tc = cam.get<TransformComponent>(entity);
			if (camera.GetOrthoSize() > 10.0f || camera.GetProjectionType() == ProjectionType::Perspective)
				draw(tc, textures.at("camera"), (int)entity);
		}

		auto& audio = reg.view<TransformComponent, AudioComponent>();
		for (auto& entity : audio)
		{
			auto& tc = audio.get<TransformComponent>(entity);
			draw(tc, textures.at("audio"), (int)entity);
		}

		auto& lights = reg.view<TransformComponent, AudioComponent>();
		for (auto& entity : lights)
		{
			auto& tc = lights.get<TransformComponent>(entity);
			draw(tc, textures.at("audio"), (int)entity);
		}
	}

	void Gizmos::DrawVerticalGrid(EditorCamera& camera, glm::vec4 color)
	{
		float orthoSize = camera.GetOrthoSize();
		glm::vec2 cameraPosition = glm::vec2(camera.GetPosition());

		float lineSpacing = 1.0f;
		if (orthoSize >= 20.0f)
			lineSpacing = pow(5.0f, floor(log10(orthoSize)));

		float offset = -0.5f * lineSpacing;

		float minX = cameraPosition.x - orthoSize - camera.GetWidth() / 10.0f;
		float maxX = cameraPosition.x + orthoSize + camera.GetWidth() / 10.0f;
		float minY = cameraPosition.y - orthoSize - camera.GetHeight() / 10.0f;
		float maxY = cameraPosition.y + orthoSize + camera.GetHeight() / 10.0f;

		auto nx = floor(minX / lineSpacing) * lineSpacing;
		auto ny = floor(minY / lineSpacing) * lineSpacing;

		for (float x = nx; x <= maxX; x += lineSpacing)
			Renderer2D::DrawLine(glm::vec3(x + offset, minY, 1.0f), glm::vec3(x + offset, maxY, 1.0f), color);

		for (float y = ny; y <= maxY; y += lineSpacing)
			Renderer2D::DrawLine(glm::vec3(minX, y + offset, 1.0f), glm::vec3(maxX, y + offset, 1.0f), color);
	}

}