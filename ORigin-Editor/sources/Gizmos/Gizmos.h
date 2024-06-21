#pragma once

#include "Origin\Core\Base.h"
#include "Origin\Core\Event.h"
#include "Origin\Scene\Entity.h"
#include "Origin\Scene\EditorCamera.h"
#include <glm\glm.hpp>

namespace origin {

	enum class Boundary2DCorner
	{
		NONE = -1,
		BOTTOM_LEFT = 0,
		TOP_LEFT = 1,
		BOTTOM_RIGHT = 2,
		TOP_RIGHT = 3
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

		void OnRender(const EditorCamera &camera);
		void OnEvent(Event &e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent &e);
		void SetSnapValue(float value) { m_SnapValue = value; }
		void SetHovered(int hovered) { m_Hovered = hovered; }
		void SetType(GizmoType type);
		GizmoType GetType() const { return m_Type; }

		void Draw2DVerticalGrid(const EditorCamera &camera);
		void Draw3DGrid(const EditorCamera &camera, bool horizontal, bool vertical, int size);

		void Draw2DOverlay();
		void Draw3DOverlay(const EditorCamera &camera);
		void DrawIcons(const EditorCamera &camera);
		void CalculateBoundary2DSizing(const EditorCamera &camera);

	private:
		Boundary2DCorner m_Boundary2DCorner = Boundary2DCorner::NONE;
		GizmoType m_Type = GizmoType::NONE;

		int m_Hovered = -1;
		float m_SnapValue = 0.0f;
	};

}

