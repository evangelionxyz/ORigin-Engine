#include "Gizmos.h"

#include "Origin\Scene\Components.h"
#include "Origin\Renderer\Renderer2D.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/compatibility.hpp>
#include "..\EditorLayer.h"

namespace origin {

#define BOUNDARY2D_ID -2

	void Gizmos::Draw2DVerticalGrid(const EditorCamera &camera)
	{
		OGN_PROFILER_RENDERING();

		if (camera.GetProjectionType() != ProjectionType::Orthographic)
			return;

		Renderer2D::Begin(camera);
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

		glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 0.15f);

		for (float x = nx; x <= maxX; x += lineSpacing)
			Renderer2D::DrawLine(glm::vec3(x + offset, minY, 0.0f), glm::vec3(x + offset, maxY, 0.0f), color);

		for (float y = ny; y <= maxY; y += lineSpacing)
			Renderer2D::DrawLine(glm::vec3(minX, y + offset, 0.0f), glm::vec3(maxX, y + offset, 0.0f), color);

		Renderer2D::End();
	}

	void Gizmos::Draw2DOverlay()
	{
		OGN_PROFILER_RENDERING();

		auto &editor = EditorLayer::Get();

		if (editor.m_VisualizeCollider)
		{
			auto &scene = EditorLayer::Get().m_ActiveScene;

			const auto &quad = scene->GetAllEntitiesWith<TransformComponent, BoxCollider2DComponent>();
			for (auto entity : quad)
			{
				const auto &[tc, bc2d] = quad.get<TransformComponent, BoxCollider2DComponent>(entity);

				glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(glm::vec2(tc.WorldTranslation) + bc2d.Offset, tc.WorldTranslation.z))
					* glm::rotate(glm::mat4(1.0f), tc.WorldRotation.z, glm::vec3(0.0f, 0.0f, 1.0f))
					* glm::scale(glm::mat4(1.0f), glm::vec3(glm::vec2(tc.WorldScale) * bc2d.Size * 2.0f, 1.0f));

				Renderer2D::DrawRect(transform, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), (int)entity);
			}

			const auto &circle = scene->GetAllEntitiesWith<TransformComponent, CircleCollider2DComponent>();
			for (auto entity : circle)
			{
				const auto &[tc, cc2d] = circle.get<TransformComponent, CircleCollider2DComponent>(entity);

				glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(glm::vec2(tc.WorldTranslation) + cc2d.Offset, tc.WorldTranslation.z))
					* glm::scale(glm::mat4(1.0f), glm::vec3(glm::vec2(tc.WorldScale * cc2d.Radius * 2.0f), 1.0f));

				Renderer2D::DrawCircle(transform, glm::vec4(0, 1, 0, 1), 0.05f, 0.0f, (int)entity);
			}
		}

		if (Entity selectedEntity = editor.m_SceneHierarchy.GetSelectedEntity())
		{
			const auto &tc = selectedEntity.GetComponent<TransformComponent>();
			glm::mat4 rotation = glm::toMat4(glm::quat(tc.WorldRotation));

			if (selectedEntity.HasComponent<SpriteRenderer2DComponent>())
			{
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(tc.WorldTranslation.x, tc.WorldTranslation.y, tc.WorldTranslation.z))
					* rotation * glm::scale(glm::mat4(1.0f), tc.WorldScale);
				Renderer2D::DrawRect(transform, glm::vec4(1.0f, 0.5f, 0.0f, 1.0f));
			}

			if (selectedEntity.HasComponent<CircleRendererComponent>())
			{
				glm::vec3 translation = tc.WorldTranslation + glm::vec3(0.0f, 0.0f, 0.5f);
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation)
					* rotation * glm::scale(glm::mat4(1.0f), tc.WorldScale);

				Renderer2D::DrawCircle(transform, glm::vec4(1.0f, 0.5f, 0.0f, 1.0f), 0.05f);
			}
		}
		Renderer2D::End();
	}

	void Gizmos::Draw3DOverlay(const EditorCamera &camera)
	{
		OGN_PROFILER_RENDERING();

		if (camera.GetProjectionType() == ProjectionType::Orthographic && !EditorLayer::Get().m_VisualizeCollider)
			return;

		glEnable(GL_DEPTH_TEST);
		Renderer3D::Begin(camera);
		auto &scene = EditorLayer::Get().m_ActiveScene;

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		const auto &box = scene->GetAllEntitiesWith<TransformComponent, BoxColliderComponent>();
		for (auto entity : box)
		{
			const auto &[tc, bc] = box.get<TransformComponent, BoxColliderComponent>(entity);

			glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(tc.WorldTranslation + bc.Offset))
				* glm::toMat4(glm::quat(tc.WorldRotation))
				* glm::scale(glm::mat4(1.0f), tc.WorldScale * glm::vec3(bc.Size * 2.0f) * 2.0f);

			Renderer3D::DrawCube(transform, glm::vec4(1.0f, 0.0f, 1.0f, 1.0f), (int)entity);
		}

		const auto &sphere = scene->GetAllEntitiesWith<TransformComponent, SphereColliderComponent>();
		for (auto entity : sphere)
		{
			const auto &[tc, sc] = sphere.get<TransformComponent, SphereColliderComponent>(entity);

			glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(tc.WorldTranslation + sc.Offset))
				* glm::toMat4(glm::quat(tc.WorldRotation))
				* glm::scale(glm::mat4(1.0f), tc.WorldScale);

			Renderer3D::DrawSphere(transform,
														 glm::vec4(1.0f, 0.0f, 1.0f, 1.0f),
														 (sc.Radius + 0.1f) * 2.0f,
														 (int)entity);
		}

		const auto &capsule = scene->GetAllEntitiesWith<TransformComponent, CapsuleColliderComponent>();
		for (auto entity : capsule)
		{
			const auto &[tc, cc] = capsule.get<TransformComponent, CapsuleColliderComponent>(entity);
			glm::vec3 rot = tc.WorldRotation + glm::vec3(0.0f, 0.0f, glm::radians(90.0f));

			glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(tc.WorldTranslation + cc.Offset))
				* glm::toMat4(glm::quat(rot))
				* glm::scale(glm::mat4(1.0f), tc.WorldScale);

			Renderer3D::DrawCapsule(transform, glm::vec4(1.0f, 0.0f, 1.0f, 1.0f), cc.Radius, cc.Height * 2.0f, (int)entity);
		}

		Renderer3D::End();

		Renderer2D::Begin(camera);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		const auto &revoluteJoint2DView = scene->m_Registry.view<TransformComponent, RevoluteJoint2DComponent>();
		for (auto e : revoluteJoint2DView)
		{
			auto &[tc, rjc] = revoluteJoint2DView.get<TransformComponent, RevoluteJoint2DComponent>(e);
			glm::vec2 anchorPoint = glm::vec2(tc.WorldTranslation.x + rjc.AnchorPoint.x, tc.WorldTranslation.y + rjc.AnchorPoint.y);
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(anchorPoint,
																					 tc.WorldTranslation.z + 0.0f))
				* glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

			Renderer2D::DrawCircle(transform, glm::vec4(0.4f, 1.0f, 0.4f, 1.0f), 100.0f);
		}
		Renderer2D::End();

		glDisable(GL_DEPTH_TEST);
	}

	void Gizmos::DrawIcons(const EditorCamera &camera)
	{
		OGN_PROFILER_RENDERING();

		auto &textures = EditorLayer::Get().m_UITextures;
		auto &reg = EditorLayer::Get().m_ActiveScene->m_Registry;

		auto drawIcon = [&](TransformComponent tc, const std::shared_ptr<Texture2D> &texture, int entity)
			{
				glm::mat4 transform = glm::mat4(1.0f);
				float scale = glm::clamp(glm::length(camera.GetPosition() - tc.WorldTranslation) * 0.05f, 1.0f, 10.0f);

				switch (camera.GetProjectionType())
				{
					case ProjectionType::Perspective:
						transform = glm::translate(glm::mat4(1.0f), tc.WorldTranslation)
							* glm::rotate(glm::mat4(1.0f), -camera.GetYaw(), glm::vec3(0, 1, 0))
							* glm::rotate(glm::mat4(1.0f), -camera.GetPitch(), glm::vec3(1, 0, 0))
							* glm::scale(glm::mat4(1.0f), glm::vec3(scale));
						break;
					case ProjectionType::Orthographic:
						transform = translate(glm::mat4(1.0f), tc.WorldTranslation);
						break;
				}

				Renderer2D::DrawQuad(transform, texture, (int)entity, glm::vec2(1.0f), glm::vec4(1.0f));
			};

		auto &cam = reg.view<TransformComponent, CameraComponent>();
		for (auto &entity : cam)
		{
			auto &[tc, cc] = cam.get<TransformComponent, CameraComponent>(entity);
			auto &sceneCam = cc.Camera;

			if (camera.GetOrthoSize() > 10.0f || camera.GetProjectionType() == ProjectionType::Perspective)
			{
				drawIcon(tc, textures.at("camera"), (int)entity);

				glm::mat4 transform = glm::mat4(1.0f);
				glm::vec3 rotation = camera.GetProjectionType() == ProjectionType::Perspective ? glm::vec3(tc.WorldRotation) : glm::vec3(0.0f, 0.0f, tc.WorldRotation.z);
				float sizeX = sceneCam.GetOrthographicSize() * sceneCam.GetAspectRatioSize();
				float sizeY = 0.0f;

				switch (cc.Camera.GetAspectRatioType())
				{
					case SceneCamera::AspectRatioType::SixteenByNine:
					{
						sizeY = sizeX / 16.0f * 9.0f;
						transform = glm::translate(glm::mat4(1.0f), tc.WorldTranslation) * glm::toMat4(glm::qua(rotation))
							* glm::scale(glm::mat4(1.0f), glm::vec3(sizeX, sizeY, 1.0f));
						break;
					}
					case SceneCamera::AspectRatioType::FourByThree:
					{
						sizeY = sizeX / 4.0f * 3.0f;
						transform = glm::translate(glm::mat4(1.0f), tc.WorldTranslation) * glm::toMat4(glm::qua(rotation))
							* glm::scale(glm::mat4(1.0f), glm::vec3(sizeX, sizeY, 1.0f));
						break;
					}
					case SceneCamera::AspectRatioType::SixteenByTen:
					{
						sizeY = sizeX / 16.0f * 10.0f;
						transform = glm::translate(glm::mat4(1.0f), tc.WorldTranslation) * glm::toMat4(glm::qua(rotation))
							* glm::scale(glm::mat4(1.0f), glm::vec3(sizeX, sizeY, 1.0f));
						break;
					}
					case SceneCamera::AspectRatioType::TwentyOneByNine:
					{
						sizeY = sizeX / 21.0f * 9.0f;
						transform = glm::translate(glm::mat4(1.0f), tc.WorldTranslation) * glm::toMat4(glm::qua(rotation))
							* glm::scale(glm::mat4(1.0f), glm::vec3(sizeX, sizeY, 1.0f));
						break;
					}
				}

				if(cc.Camera.GetAspectRatioType() != SceneCamera::AspectRatioType::Free)
					Renderer2D::DrawRect(transform, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
			}
		}

		auto &audio = reg.view<TransformComponent, AudioComponent>();
		for (auto &entity : audio)
		{
			auto &tc = audio.get<TransformComponent>(entity);
			drawIcon(tc, textures.at("audio"), (int)entity);
		}

		auto &lighting = reg.view<TransformComponent, LightComponent>();
		for (auto &entity : lighting)
		{
			auto &tc = lighting.get<TransformComponent>(entity);
			drawIcon(tc, textures.at("lighting"), (int)entity);
		}

		Renderer2D::End();
	}

	void Gizmos::OnRender(const EditorCamera &camera)
	{
		glDisable(GL_DEPTH_TEST);

		OGN_PROFILER_RENDERING();

		////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////     2D     //////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////
		Renderer2D::Begin(camera);

		if (Entity selectedEntity = EditorLayer::Get().m_SceneHierarchy.GetSelectedEntity())
		{
			if (m_Type == GizmoType::BOUNDARY2D)
			{
				CalculateBoundary2DSizing(camera);

				float size = camera.GetOrthoSize() * 0.03f;
				auto &tc = selectedEntity.GetComponent<TransformComponent>();

				// bottom left corner
				glm::vec4 red = glm::vec4(0.8f, 0.1f, 0.1f, 1.0f);
				glm::vec4 green = glm::vec4(0.1f, 0.8f, 0.1f, 1.0f);
				glm::vec4 col = m_Boundary2DCorner == Boundary2DCorner::BOTTOM_LEFT ? green : red;

				std::vector<glm::vec3> cornerOffsets = {
						{ -tc.WorldScale.x / 2.0f, -tc.WorldScale.y / 2.0f, 1.0f }, // Bottom left
						{ -tc.WorldScale.x / 2.0f,  tc.WorldScale.y / 2.0f, 1.0f }, // Top left
						{  tc.WorldScale.x / 2.0f, -tc.WorldScale.y / 2.0f, 1.0f }, // Bottom right
						{  tc.WorldScale.x / 2.0f,  tc.WorldScale.y / 2.0f, 1.0f }  // Top right
				};

				for (int i = 0; i < 4; ++i)
				{
					glm::vec4 col = (m_Boundary2DCorner == static_cast<Boundary2DCorner>(i)) ? green : red;
					glm::quat rotationQuat = glm::quat(tc.WorldRotation);
					glm::mat4 tf = glm::translate(glm::mat4(1.0f), tc.WorldTranslation + glm::vec3(rotationQuat * glm::vec4(cornerOffsets[i], 0.0f))) *
						glm::toMat4(rotationQuat) * glm::scale(glm::mat4(1.0f), glm::vec3(size));

					Renderer2D::DrawQuad(tf, col, BOUNDARY2D_ID - (i + 1));
				}
			}
			Renderer2D::End();
		}

		Draw2DOverlay();
		DrawIcons(camera);


		////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////     3D     //////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////
		Draw3DOverlay(camera);

		// Grid
		Draw2DVerticalGrid(camera);
	}

	void Gizmos::OnEvent(Event &e)
	{
		OGN_PROFILER_INPUT();

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseButtonPressedEvent>(OGN_BIND_EVENT_FN(Gizmos::OnMouseButtonPressed));
	}

	bool Gizmos::OnMouseButtonPressed(MouseButtonPressedEvent &e)
	{
		OGN_PROFILER_INPUT();

		if (e.GetMouseButton() == Mouse::ButtonLeft)
		{
			switch (m_Hovered)
			{
				// bottom left
				case BOUNDARY2D_ID - 1:
					m_Boundary2DCorner = Boundary2DCorner::BOTTOM_LEFT;
					break;
					// top left
				case BOUNDARY2D_ID - 2:
					m_Boundary2DCorner = Boundary2DCorner::TOP_LEFT;
					break;
					// bottom right
				case BOUNDARY2D_ID - 3:
					m_Boundary2DCorner = Boundary2DCorner::BOTTOM_RIGHT;
					break;
					// top right
				case BOUNDARY2D_ID - 4:
					m_Boundary2DCorner = Boundary2DCorner::TOP_RIGHT;
					break;
				default:
					m_Boundary2DCorner = Boundary2DCorner::NONE;
					break;
			}
		}

		return false;
	}

	void Gizmos::SetType(GizmoType type)
	{
		OGN_PROFILER_FUNCTION();

		if (m_Hovered >= -1)
			m_Type = type;
	}

	void Gizmos::CalculateBoundary2DSizing(const EditorCamera &camera)
	{
		OGN_PROFILER_LOGIC();

		static glm::vec2 initialPosition = { 0.0f, 0.0f };
		const glm::vec2 mouse { Input::GetMouseX(), Input::GetMouseY() };
		const glm::vec2 delta { mouse - initialPosition };
		initialPosition = mouse;

		float orthoScale = camera.GetOrthoSize() / camera.GetHeight();
		glm::vec3 translation = glm::vec3(delta, 0.0f);

		if (Entity entity = EditorLayer::Get().m_SceneHierarchy.GetSelectedEntity())
		{
			if (Input::IsMouseButtonPressed(Mouse::ButtonLeft) && EditorLayer::Get().m_SceneViewportHovered)
			{
				auto &tc = entity.GetComponent<TransformComponent>();
				auto &idc = entity.GetComponent<IDComponent>();

				if (entity.HasParent())
				{
					Entity parent = EditorLayer::Get().m_ActiveScene->GetEntityWithUUID(idc.Parent);
					auto &parentTC = parent.GetComponent<TransformComponent>();
					translation = glm::inverse(glm::quat(parentTC.WorldRotation))
						//* glm::quat(tc.WorldRotation) 
						* glm::vec3(delta.x * orthoScale, delta.y * orthoScale, 0.0f);
				}
				else
				{
					translation = glm::quat(tc.WorldRotation) 
						* glm::vec4(delta.x * orthoScale, delta.y * orthoScale, 0.0f, 1.0f);
				}

				switch (m_Boundary2DCorner)
				{
					case Boundary2DCorner::TOP_RIGHT:
						tc.Scale.x += translation.x;
						tc.Scale.y -= translation.y;
						tc.Translation.x -= translation.x / 2.0f;
						tc.Translation.y += translation.y / 2.0f;
						break;
					case Boundary2DCorner::BOTTOM_RIGHT:
						tc.Scale.x += translation.x;
						tc.Scale.y += translation.y;
						tc.Translation.x -= translation.x / 2.0f;
						tc.Translation.y += translation.y / 2.0f;
						break;
					case Boundary2DCorner::TOP_LEFT:
						tc.Scale.x -= translation.x;
						tc.Scale.y -= translation.y;
						tc.Translation.x -= translation.x / 2.0f;
						tc.Translation.y += translation.y / 2.0f;
						break;
					case Boundary2DCorner::BOTTOM_LEFT:
						tc.Scale.x -= translation.x;
						tc.Scale.y += translation.y;
						tc.Translation.x -= translation.x / 2.0f;
						tc.Translation.y += translation.y / 2.0f;
						break;
				}
			}
		}
	}

}
