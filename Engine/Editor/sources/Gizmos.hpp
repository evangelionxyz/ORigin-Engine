// Copyright (c) Evangelion Manuhutu | Origin Engine

#ifndef GIZMOS_HPP
#define GIZMOS_HPP

#include "Origin/Core/Base.h"
#include "Origin/Core/Event.h"
#include "Origin/Scene/Entity.h"
#include "Origin/Scene/Camera/EditorCamera.h"
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
        Gizmos();

        void DrawCollider(const Camera &camera, Scene *scene);
        void OnEvent(Event &e);
        bool OnMouseButtonPressed(MouseButtonPressedEvent &e);
        void SetSnapValue(float value) { m_SnapValue = value; }
        void SetType(GizmoType type);
        GizmoType GetType() const { return m_Type; }

        void Draw2DGrid(const Camera &camera);
        void DrawGrid();

        void DrawIcons(const Camera &camera, Scene *scene);
        void DrawBoundingBox(const Camera &camera, Scene *scene);
        void DrawFrustum(const Camera &camera, Scene *scene);
        void CalculateBoundary2DSizing(const Camera &camera);

        static void DrawBoxLine(const glm::vec3 &position, const glm::vec3 &rotation, const glm::vec3 &size, const glm::vec4 &color);
        static void DrawCapsuleLine(const glm::vec3 &position, const glm::vec3 &rotation, f32 half_height, f32 radius, const glm::vec4 &color);
        static void DrawSphereLine(const glm::vec3 &position, f32 radius, const glm::vec4 &color);

    private:
        Boundary2DCorner m_Boundary2DCorner = Boundary2DCorner::NONE;
        GizmoType m_Type = GizmoType::NONE;
        float m_SnapValue = 0.0f;

        struct GridData
        {
            u32 vao;
            u32 vbo;
            Shader *shader;

        } m_grid_data;
    };

}

#endif