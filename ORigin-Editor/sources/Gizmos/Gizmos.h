#pragma once

#include "Origin\Core\Base.h"
#include "Origin\IO\Events\Event.h"
#include "Origin\Scene\Entity.h"
#include "Origin\Scene\EditorCamera.h"
#include <glm\glm.hpp>

namespace origin {

	enum class Boundary2DCorner
	{
		NONE = -1,
		TOP_LEFT,
		TOP_RIGHT,
		BOTTOM_LEFT,
		BOTTOM_RIGHT,
		LEFT,
		RIGHT
	};

	enum class GizmoType
	{
		NONE = -1,
		TRANSLATE,
		ROTATE,
		SCALE,
		BOUNDARY,
		BOUNDARY2D
	};

	enum class GizmoMode
	{
		LOCAL,
		GLOBAL
	};

	class Gizmos
	{
	public:
		Gizmos() = default;
		void SetSelectedEntity(Entity entity);
		void OnRender(const EditorCamera &camera);

		void OnEvent(Event &e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent &e);
		void SetSnapValue(float value) { m_SnapValue = value; }
		void SetHovered(int hovered) { m_Hovered = hovered; }
		void SetType(GizmoType type);
		GizmoType GetType() const { return m_Type; }

	private:
		void Draw2DVerticalGrid();
		void Draw2DOverlay();
		void DrawOverlay();
		void DrawIcons();
		void CalculateBoundary2DSizing();

	private:
		Boundary2DCorner m_Boundary2DCorner = Boundary2DCorner::NONE;
		Entity m_SelectedEntity;
		EditorCamera m_Camera;
		GizmoType m_Type = GizmoType::NONE;
		int m_Hovered = -1;
		float m_SnapValue = 0.0f;
	};

}

