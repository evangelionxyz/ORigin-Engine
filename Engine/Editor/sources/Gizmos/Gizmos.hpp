#pragma once

#include "Origin/Core/Base.h"
#include "Origin/Core/Event.h"
#include "Origin/Scene/Entity.h"
#include "Origin/Scene/EditorCamera.h"
#include <glm/glm.hpp>

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

        void DrawCollider(const Camera &camera, Scene *scene);
        void OnEvent(Event &e);
        bool OnMouseButtonPressed(MouseButtonPressedEvent &e);
        void SetSnapValue(float value) { m_SnapValue = value; }
        void SetType(GizmoType type);
        GizmoType GetType() const { return m_Type; }

        void Draw2DGrid(const Camera &camera);
        void Draw3DGrid(const Camera &camera, bool horizontal, bool vertical, int size);
        void DrawIcons(const Camera &camera, Scene *scene);
        void DrawBoundingBox(const Camera &camera, Scene *scene);
        void DrawFrustum(const Camera &camera, Scene *scene);
        void CalculateBoundary2DSizing(const Camera &camera);

    private:
        Boundary2DCorner m_Boundary2DCorner = Boundary2DCorner::NONE;
        GizmoType m_Type = GizmoType::NONE;
        float m_SnapValue = 0.0f;
    };

}

