// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef FRUSTUM_H
#define FRUSTUM_H

#include <glm/glm.hpp>
#include <vector>
#include <array>

namespace origin
{
    class Frustum
    {
    public:
        enum Plane
        {
            Plane_Left = 0,
            Plane_Right,
            Plane_Bottom,
            Plane_Top,
            Plane_Near,
            Plane_Far,
            Plane_Count
        };

        Frustum() = default;
        Frustum(const glm::mat4 &view_projection);

        void Update(const glm::mat4 &view_projection);
        bool IsPointVisible(const glm::vec3 &point) const;
        bool IsSphereVisible(const glm::vec3 &center, float radius) const;
        bool IsAABBVisible(const glm::vec3 &min, const glm::vec3 &max) const;
        const std::array<glm::vec3, 8> &GetCorners() const { return m_Corners; }
        const std::array<glm::vec4, Plane_Count> &GetPlanes() const { return m_Planes; }
        std::vector<std::pair<glm::vec3, glm::vec3>> GetEdges() const;

    private:
        std::array<glm::vec4, Plane_Count> m_Planes;
        std::array<glm::vec3, 8> m_Corners;
        glm::mat4 m_ViewProjectionInverse;
    };
}

#endif