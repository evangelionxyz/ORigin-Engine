// Copyright (c) 2023 Evangelion Manuhutu | ORigin Engine

#include "Editor.h"

namespace Origin
{
	bool Editor::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		bool control = Input::IsKeyPressed(Key::LeftControl);
		auto& selectedEntity = m_SceneHierarchy.GetSelectedEntity();

		if (e.GetMouseButton() == Mouse::ButtonLeft && m_SceneViewportHovered)
		{
			if (!ImGuizmo::IsOver((ImGuizmo::OPERATION)m_GizmosType) && !ImGuizmo::IsUsing() && !control)
			{
				if (m_HoveredEntity != selectedEntity)
				{
					m_SceneHierarchy.SetSelectedEntity(m_HoveredEntity);
				}
				else if (m_HoveredEntity == selectedEntity && !ImGuizmo::IsUsing())
				{
					m_GizmosType = -1;
					m_SceneHierarchy.SetSelectedEntity({});
				}
			}

			if (!ImGuizmo::IsOver((ImGuizmo::OPERATION)m_GizmosType) && m_HoveredEntity == selectedEntity && control)
			{
				if (m_GizmosMode == ImGuizmo::MODE::LOCAL)
				{
					m_GizmosType == -1 ? m_GizmosType = ImGuizmo::OPERATION::TRANSLATE : m_GizmosType == ImGuizmo::OPERATION::TRANSLATE ?
					m_GizmosType = ImGuizmo::OPERATION::ROTATE : m_GizmosType == ImGuizmo::OPERATION::ROTATE ? m_GizmosType = ImGuizmo::OPERATION::SCALE : m_GizmosType = -1;
				}
				else if (m_GizmosMode == ImGuizmo::MODE::WORLD)
				{
					m_GizmosType == -1 ? m_GizmosType = ImGuizmo::OPERATION::TRANSLATE : m_GizmosType == ImGuizmo::OPERATION::TRANSLATE ?
						m_GizmosType = ImGuizmo::OPERATION::ROTATE : m_GizmosType == ImGuizmo::OPERATION::ROTATE ? m_GizmosType = -1 : m_GizmosType = -1;
				}
			}
		}
		return false;
	}

	void Editor::InputProccedure(Timestep time)
	{
		auto& selectedEntity = m_SceneHierarchy.GetSelectedEntity();

		if (Input::IsMouseButtonPressed(Mouse::ButtonRight))
		{
			lastMouseX = lastMouseX;
			lastMouseY = lastMouseY;
			RMHoldTime += time.GetDeltaTime();

			if (lastMouseX == mouseX && lastMouseY == mouseY) VpMenuContextActive = true;
			else if (lastMouseX != mouseX && lastMouseY != mouseY) VpMenuContextActive = false;

			if (lastMouseX == mouseX && lastMouseY == mouseY)
			{
				if (m_HoveredEntity == selectedEntity && m_HoveredEntity)
					m_VpMenuContext = ViewportMenuContext::EntityProperties;
				if (m_HoveredEntity != selectedEntity || !m_HoveredEntity)
					m_VpMenuContext = ViewportMenuContext::CreateMenu;
			}
		}
		else
		{
			lastMouseX = mouseX;
			lastMouseY = mouseY;
			RMHoldTime = 0.0f;
		}
	}

	bool Editor::OnKeyPressed(KeyPressedEvent& e)
	{
		auto& app = Application::Get();
		bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
		bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);

		ImGuiIO& io = ImGui::GetIO();

		switch (e.GetKeyCode())
		{
			// File Operation
			case Key::S:
			{
				if (control)
				{
					if (shift)
						SaveSceneAs();
					else
						SaveScene();
				}
				break;
			}

			case Key::O:
			{
				if (control) OpenScene();
				break;
			}

			case Key::N:
			{
				if (control) NewScene();
				break;
			}

			case Key::T:
			{
				if (!ImGuizmo::IsUsing() && !io.WantTextInput)
					m_GizmosType = ImGuizmo::OPERATION::TRANSLATE;
				break;
			}

			case Key::D:
			{
				if (control)
					OnDuplicateEntity();
				break;
			}

			case Key::E:
			{
				if (!ImGuizmo::IsUsing() && !io.WantTextInput && m_GizmosMode == ImGuizmo::MODE::LOCAL)
					m_GizmosType = ImGuizmo::OPERATION::SCALE;
				break;
			}

			case Key::R:
			{
				if (!ImGuizmo::IsUsing() && !io.WantTextInput)
					m_GizmosType = ImGuizmo::OPERATION::ROTATE;
				break;
			}

			case Key::F11:
			{
				guiMenuFullscreen == false ? guiMenuFullscreen = true : guiMenuFullscreen = false;
				app.GetWindow().SetFullscreen(guiMenuFullscreen);
				break;
			}
		}

		return false;
	}

	bool Editor::OnWindowResize(WindowResizeEvent& e)
	{
		return false;
	}

	bool Editor::OnMouseMovedEvent(MouseMovedEvent& e)
	{
		return false;
	}

	bool Editor::OnMouseButtonEvent(MouseButtonEvent& e)
	{
		return false;
	}
}