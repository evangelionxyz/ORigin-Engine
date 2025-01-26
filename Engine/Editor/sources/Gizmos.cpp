// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "Gizmos.hpp"
#include "EditorLayer.hpp"
#include "Origin/Scene/Components/Components.h"
#include "Origin/Renderer/Renderer2D.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include <glad/glad.h>

namespace origin {

void Gizmos::Draw2DGrid(const Camera &camera)
{
    OGN_PROFILER_RENDERING();

    if (camera.GetProjectionType() != ProjectionType::Orthographic)
    {
        return;
    }

    Renderer2D::Begin();
    f32 orthoSize = camera.GetOrthoScale();
    glm::vec2 cameraPosition = { camera.GetPosition().x, camera.GetPosition().y };

    f32 lineSpacing = 1.0f;
    if (orthoSize >= 20.0f)
    {
        lineSpacing = pow(5.0f, floor(log10(orthoSize)));
    }

    f32 offset = -0.5f * lineSpacing;

    glm::vec2 viewportSize = camera.GetViewportSize();
    f32 minX = cameraPosition.x - orthoSize - viewportSize.x / 10.0f;
    f32 maxX = cameraPosition.x + orthoSize + viewportSize.x / 10.0f;
    f32 minY = cameraPosition.y - orthoSize - viewportSize.y / 10.0f;
    f32 maxY = cameraPosition.y + orthoSize + viewportSize.y / 10.0f;

    glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 0.15f);
    auto n = floor(minX / lineSpacing) * lineSpacing;
    for (f32 i = n; i <= maxX; i += lineSpacing)
    {
        Renderer2D::DrawLine({ i + offset, minY, 0.0f }, { i + offset, maxY, 0.0f }, color);
    }

    n = floor(minY / lineSpacing) * lineSpacing;
    for (f32 i = n; i <= maxY; i += lineSpacing)
    {
        Renderer2D::DrawLine({ minX, i + offset, 0.0f }, { maxX, i + offset, 0.0f }, color);
    }

    Renderer2D::End();
}

void Gizmos::DrawFrustum(const Camera &camera, Scene *scene)
{
    glEnable(GL_DEPTH_TEST);
    Renderer2D::Begin();

    // Camera
    for (auto [e, tc, cc] : scene->m_Registry.view<TransformComponent, CameraComponent>().each())
    {
        Frustum frustum(cc.Camera.GetProjectionMatrix() * cc.Camera.GetViewMatrix() * glm::inverse(tc.GetTransform()));
        for (const auto &edge : frustum.GetEdges())
        {
            Renderer2D::DrawLine(edge.first, edge.second, { 1.0f, 0.0f, 0.0f, 1.0f });
        }
    }

    Renderer2D::End();
}

void Gizmos::DrawIcons(const Camera &camera, Scene *scene)
{
    OGN_PROFILER_RENDERING();
    auto drawIcon = [&](TransformComponent tc, const std::shared_ptr<Texture2D> &texture)
    {
        glm::mat4 transform = glm::mat4(1.0f);
        switch (camera.GetProjectionType())
        {
        case ProjectionType::Perspective:
            transform = glm::translate(glm::mat4(1.0f), tc.WorldTranslation)
                * glm::rotate(glm::mat4(1.0f), -camera.GetYaw(), glm::vec3(0, 1, 0))
                * glm::rotate(glm::mat4(1.0f), -camera.GetPitch(), glm::vec3(1, 0, 0))
                * glm::scale(glm::mat4(1.0f), tc.WorldScale);
            break;
        case ProjectionType::Orthographic:
            transform = translate(glm::mat4(1.0f), tc.WorldTranslation);
            break;
        }

        Renderer2D::DrawQuad(transform, texture, glm::vec2(1.0f), glm::vec4(1.0f));
    };

    // Draw ICONS
    {
        auto &textures = EditorLayer::Get().m_UITextures;
        glEnable(GL_DEPTH_TEST);
        Renderer2D::Begin();
        for (auto [e, tc] : scene->GetAllEntitiesWith<TransformComponent>().each())
        {
            Entity entity = { e, scene };
            if (entity.HasComponent<CameraComponent>())
            {
                CameraComponent &cc = entity.GetComponent<CameraComponent>();
                if (camera.IsPerspective())
                {
                    drawIcon(tc, textures.at("camera"));
                }
                else if (!camera.IsPerspective() && camera.GetOrthoScale() > 15.0f)
                {
                    drawIcon(tc, textures.at("camera"));
                }
            }
            else if (entity.HasComponent<AudioComponent>())
            {
                drawIcon(tc, textures.at("audio"));
            }
            else if (entity.HasComponent<LightComponent>())
            {
                drawIcon(tc, textures.at("lighting"));
            }
        }
        Renderer2D::End();
    }
}

void Gizmos::DrawBoundingBox(const Camera &camera, Scene *scene)
{
    glEnable(GL_DEPTH_TEST);
    Renderer2D::Begin();
#if 0
    // AABB
    for (auto [e, tc] : scene->m_Registry.view<TransformComponent>().each())
    {
        AABB aabb = AABB(tc.WorldTranslation, tc.WorldScale);
        const glm::vec3 &min = aabb.Min;
        const glm::vec3 &max = aabb.Max;
        glm::vec4 color = { 1.0f, 0.7f, 0.0f, 1.0f }; // Orange color

        // Define the 8 corners of the AABB
        glm::vec3 corners[8] = {
            {min.x, min.y, min.z}, {max.x, min.y, min.z},
            {min.x, max.y, min.z}, {max.x, max.y, min.z},
            {min.x, min.y, max.z}, {max.x, min.y, max.z},
            {min.x, max.y, max.z}, {max.x, max.y, max.z}
        };

        // Draw 12 lines connecting the corners
        // Bottom face
        Renderer2D::DrawLine(corners[0], corners[1], color);
        Renderer2D::DrawLine(corners[1], corners[3], color);
        Renderer2D::DrawLine(corners[3], corners[2], color);
        Renderer2D::DrawLine(corners[2], corners[0], color);

        // Top face
        Renderer2D::DrawLine(corners[4], corners[5], color);
        Renderer2D::DrawLine(corners[5], corners[7], color);
        Renderer2D::DrawLine(corners[7], corners[6], color);
        Renderer2D::DrawLine(corners[6], corners[4], color);

        // Connecting edges
        Renderer2D::DrawLine(corners[0], corners[4], color);
        Renderer2D::DrawLine(corners[1], corners[5], color);
        Renderer2D::DrawLine(corners[2], corners[6], color);
        Renderer2D::DrawLine(corners[3], corners[7], color);
    }
#endif
    // OBB
    for (auto [e, tc] : scene->m_Registry.view<TransformComponent>().each())
    {
        if (!tc.Clickable)
            continue;

        OBB obb = OBB(tc.WorldTranslation, tc.WorldScale, tc.WorldRotation);
        if (scene->m_Registry.any_of<TextComponent>(e))
        {
            auto text = scene->m_Registry.get<TextComponent>(e);
            glm::vec3 scale = tc.WorldScale * glm::vec3(text.Size.x, -text.Size.y, 1.0f);
            obb = OBB(tc.WorldTranslation, scale, tc.WorldRotation);
        }

        const auto &vertices = obb.GetVertices();
        static constexpr glm::vec4 color = { 0.0f, 1.0f, 0.0f, 1.0f };

        // Bottom face
        Renderer2D::DrawLine(vertices[0], vertices[1], color);
        Renderer2D::DrawLine(vertices[1], vertices[3], color);
        Renderer2D::DrawLine(vertices[3], vertices[2], color);
        Renderer2D::DrawLine(vertices[2], vertices[0], color);

        // Top face
        Renderer2D::DrawLine(vertices[4], vertices[5], color);
        Renderer2D::DrawLine(vertices[5], vertices[7], color);
        Renderer2D::DrawLine(vertices[7], vertices[6], color);
        Renderer2D::DrawLine(vertices[6], vertices[4], color);

        // Vertical edges connecting top and bottom faces
        Renderer2D::DrawLine(vertices[0], vertices[4], color);
        Renderer2D::DrawLine(vertices[1], vertices[5], color);
        Renderer2D::DrawLine(vertices[2], vertices[6], color);
        Renderer2D::DrawLine(vertices[3], vertices[7], color);
    }
    Renderer2D::End();
}

void Gizmos::DrawCollider(const Camera &camera, Scene *scene)
{
    OGN_PROFILER_RENDERING();

    glEnable(GL_DEPTH_TEST);
    Renderer2D::Begin();

    Entity selectedEntity = SceneHierarchyPanel::GetInstance()->GetSelectedEntity();

    const auto view = scene->GetAllEntitiesWith<TransformComponent>();
    for (auto [e, tc] : view.each())
    {
        Entity entity = { e, scene };

        // Selected entity
        if (entity == selectedEntity && selectedEntity.IsValid())
        {
            if (m_Type == GizmoType::BOUNDARY2D && !camera.IsPerspective())
            {
                CalculateBoundary2DSizing(camera);
                f32 size = camera.GetOrthoScale() * 0.03f;

                // bottom left corner
                glm::vec4 red = glm::vec4(0.8f, 0.1f, 0.1f, 1.0f);
                glm::vec4 green = glm::vec4(0.1f, 0.8f, 0.1f, 1.0f);
                glm::vec4 col = m_Boundary2DCorner == Boundary2DCorner::BOTTOM_LEFT ? green : red;
                glm::vec3 cornerOffsets[4] = {
                        { -tc.WorldScale.x / 2.0f, -tc.WorldScale.y / 2.0f, 1.0f }, // Bottom left
                        { -tc.WorldScale.x / 2.0f,  tc.WorldScale.y / 2.0f, 1.0f }, // Top left
                        {  tc.WorldScale.x / 2.0f, -tc.WorldScale.y / 2.0f, 1.0f }, // Bottom right
                        {  tc.WorldScale.x / 2.0f,  tc.WorldScale.y / 2.0f, 1.0f }  // Top right
                };

                for (int i = 0; i < 4; ++i)
                {
                    glm::vec4 col = (m_Boundary2DCorner == static_cast<Boundary2DCorner>(i)) ? green : red;
                    glm::vec3 localTranslation = tc.WorldRotation * cornerOffsets[i];
                    glm::mat4 tf = glm::translate(glm::mat4(1.0f), tc.WorldTranslation + localTranslation) *
                        glm::toMat4(tc.WorldRotation) * glm::scale(glm::mat4(1.0f), glm::vec3(size));

                    Renderer2D::DrawQuad(tf, col);
                }
            }

            if (entity.HasComponent<RevoluteJoint2DComponent>())
            {
                auto &rjc = entity.GetComponent<RevoluteJoint2DComponent>();
                glm::vec2 anchorPoint = {
                    tc.WorldTranslation.x + rjc.AnchorPoint.x,
                    tc.WorldTranslation.y + rjc.AnchorPoint.y };

                glm::mat4 transform = glm::translate(glm::mat4(1.0f), { anchorPoint, tc.WorldTranslation.z + 0.0f })
                    * glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

                Renderer2D::DrawCircle(transform, glm::vec4(0.4f, 1.0f, 0.4f, 1.0f), 100.0f);
            }
        }

        // 2D Collider
        if (entity.HasComponent<BoxCollider2DComponent>())
        {
            BoxCollider2DComponent &cc = entity.GetComponent<BoxCollider2DComponent>();

            glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(glm::vec2(tc.WorldTranslation) + cc.Offset, tc.WorldTranslation.z))
                * glm::toMat4(tc.WorldRotation)
                * glm::scale(glm::mat4(1.0f), glm::vec3(glm::vec2(tc.WorldScale) * cc.Size * 2.0f, 1.0f));

            Renderer2D::DrawRect(transform, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
        }

        if (entity.HasComponent<CircleCollider2DComponent>())
        {
            CircleCollider2DComponent &cc = entity.GetComponent<CircleCollider2DComponent>();

            glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(glm::vec2(tc.WorldTranslation) + cc.Offset, tc.WorldTranslation.z))
                * glm::scale(glm::mat4(1.0f), glm::vec3(glm::vec2(tc.WorldScale * cc.Radius * 2.0f), 1.0f));

            Renderer2D::DrawCircle(transform, glm::vec4(0, 1, 0, 1), 0.05f, 0.0f);
        }

        // 3D Collider
        if (entity.HasComponent<RigidbodyComponent>())
        {
            RigidbodyComponent &rb = entity.GetComponent<RigidbodyComponent>();

            if (entity.HasComponent<BoxColliderComponent>())
            {
                BoxColliderComponent &bc = entity.GetComponent<BoxColliderComponent>();
                DrawBoxLine(tc.WorldTranslation + rb.Offset, glm::eulerAngles(tc.WorldRotation), tc.WorldScale * bc.Scale, {1.0f, 0.0f, 1.0f, 1.0f});
            }

            if (entity.HasComponent<CapsuleColliderComponent>())
            {
                CapsuleColliderComponent &cc = entity.GetComponent<CapsuleColliderComponent>();
                DrawCapsuleLine(tc.WorldTranslation + rb.Offset, glm::eulerAngles(tc.WorldRotation), tc.Scale.y * (cc.HalfHeight / 2.0f), cc.Radius, { 1.0f, 0.0f, 1.0f, 1.0f });
            }

            if (entity.HasComponent<SphereColliderComponent>())
            {
                SphereColliderComponent &sc = entity.GetComponent<SphereColliderComponent>();
                DrawSphereLine(tc.WorldTranslation + rb.Offset, tc.WorldScale.x * sc.Radius * 2.0f, { 1.0f, 0.0f, 1.0f, 1.0f });
            }
        }
    }

    Renderer2D::End();
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

    return false;
}

void Gizmos::SetType(GizmoType type)
{
    OGN_PROFILER_FUNCTION();
    m_Type = type;
}

void Gizmos::CalculateBoundary2DSizing(const Camera &camera)
{
    OGN_PROFILER_LOGIC();

    const glm::vec2 delta = Input::GetMouseClickDragDelta();

    f32 viewportHeight = camera.GetViewportSize().y;
    f32 orthoScale = camera.GetOrthoScale() / viewportHeight;
    glm::vec3 translation = glm::vec3(delta, 0.0f);

    Entity selectedEntity = SceneHierarchyPanel::GetInstance()->GetSelectedEntity();

    if (selectedEntity.IsValid())
    {
        if (Input::IsMouseButtonPressed(Mouse::ButtonLeft) && EditorLayer::Get().IsViewportHovered)
        {
            auto &tc = selectedEntity.GetComponent<TransformComponent>();
            auto &idc = selectedEntity.GetComponent<IDComponent>();

            if (selectedEntity.HasParent())
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

void Gizmos::DrawBoxLine(const glm::vec3 &position, const glm::vec3 &rotation, const glm::vec3 &size, const glm::vec4 &color)
{
    glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
        * glm::rotate(rotation.x, glm::vec3{1.0f, 0.0f, 0.0f})
        * glm::rotate(rotation.y, glm::vec3{0.0f, 1.0f, 0.0f}) 
        * glm::rotate(rotation.z, glm::vec3{0.0f, 0.0f, 1.0f}) 
        * glm::scale(glm::mat4(1.0f), size * 0.5f);

    std::array<glm::vec3, 8> vertices = {
        glm::vec3(-1, -1, -1), glm::vec3(1, -1, -1),
        glm::vec3(1, 1, -1), glm::vec3(-1, 1, -1),
        glm::vec3(-1, -1, 1), glm::vec3(1, -1, 1),
        glm::vec3(1, 1, 1), glm::vec3(-1, 1, 1)
    };

    for (auto &v : vertices)
    {
        v = glm::vec3(transform * glm::vec4(v, 1.0f));
    }

    // Bottom face
    Renderer2D::DrawLine(vertices[0], vertices[1], color);
    Renderer2D::DrawLine(vertices[1], vertices[2], color);
    Renderer2D::DrawLine(vertices[2], vertices[3], color);
    Renderer2D::DrawLine(vertices[3], vertices[0], color);

    // Top face
    Renderer2D::DrawLine(vertices[4], vertices[5], color);
    Renderer2D::DrawLine(vertices[5], vertices[6], color);
    Renderer2D::DrawLine(vertices[6], vertices[7], color);
    Renderer2D::DrawLine(vertices[7], vertices[4], color);

    // Vertical edges
    Renderer2D::DrawLine(vertices[0], vertices[4], color);
    Renderer2D::DrawLine(vertices[1], vertices[5], color);
    Renderer2D::DrawLine(vertices[2], vertices[6], color);
    Renderer2D::DrawLine(vertices[3], vertices[7], color);
}

void Gizmos::DrawCapsuleLine(const glm::vec3 &position, const glm::vec3 &rotation, f32 half_height, f32 radius, const glm::vec4 &color)
{
    const int segments = 8;
    const int stacks = 4;

    // Calculate rotated axes
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(rotation.x), glm::vec3(1, 0, 0))
        * glm::rotate(glm::mat4(1.0f), glm::radians(rotation.y), glm::vec3(0, 1, 0))
        * glm::rotate(glm::mat4(1.0f), glm::radians(rotation.z), glm::vec3(0, 0, 1));

    glm::vec3 up = glm::vec3(rotationMatrix * glm::vec4(0, 1, 0, 0));
    glm::vec3 right = glm::normalize(glm::cross(up, glm::vec3(1, 0, 0)));
    glm::vec3 forward = glm::normalize(glm::cross(up, right));

    // Vertical line
    glm::vec3 top = position + up * half_height;
    glm::vec3 bottom = position - up * half_height;
    Renderer2D::DrawLine(bottom, top, color);

    // Additional vertical lines
    glm::vec3 rightPoint = right * radius;
    glm::vec3 forwardPoint = forward * radius;

    Renderer2D::DrawLine(bottom + rightPoint, top + rightPoint, color);
    Renderer2D::DrawLine(bottom - rightPoint, top - rightPoint, color);
    Renderer2D::DrawLine(bottom + forwardPoint, top + forwardPoint, color);
    Renderer2D::DrawLine(bottom - forwardPoint, top - forwardPoint, color);

    // Draw hemispheres
    for (int hemisphere = 0; hemisphere < 2; ++hemisphere)
    {
        glm::vec3 hemisphereCenter = hemisphere == 0 ? top : bottom;
        f32 sign = hemisphere == 0 ? 1.0f : -1.0f;

        for (int stack = 0; stack <= stacks; ++stack)
        {
            f32 stackAngle = glm::pi<f32>() * 0.5f * stack / stacks * sign;
            f32 stackRadius = radius * std::cos(std::abs(stackAngle));
            f32 stackHeight = radius * std::sin(std::abs(stackAngle));

            for (int segment = 0; segment < segments; ++segment)
            {
                f32 angle1 = 2.0f * glm::pi<f32>() * segment / segments;
                f32 angle2 = 2.0f * glm::pi<f32>() * (segment + 1) / segments;

                glm::vec3 p1 = hemisphereCenter +
                    right * (stackRadius * std::cos(angle1)) +
                    forward * (stackRadius * std::sin(angle1)) +
                    up * stackHeight * sign;

                glm::vec3 p2 = hemisphereCenter +
                    right * (stackRadius * std::cos(angle2)) +
                    forward * (stackRadius * std::sin(angle2)) +
                    up * stackHeight * sign;

                Renderer2D::DrawLine(p1, p2, color);
            }
        }
    }
}


void Gizmos::DrawSphereLine(const glm::vec3 &position, f32 radius, const glm::vec4 &color)
{
    const int segments = 16;

    // Draw latitude rings
    for (int i = 0; i < segments; ++i)
    {
        f32 theta1 = glm::pi<f32>() * i / segments;
        f32 theta2 = glm::pi<f32>() * (i + 1) / segments;

        for (int j = 0; j < segments; ++j)
        {
            f32 phi1 = 2.0f * glm::pi<f32>() * j / segments;
            f32 phi2 = 2.0f * glm::pi<f32>() * (j + 1) / segments;

            glm::vec3 p1 = position + radius * glm::vec3(sin(theta1) * cos(phi1), cos(theta1), sin(theta1) * sin(phi1));
            glm::vec3 p2 = position + radius * glm::vec3(sin(theta2) * cos(phi1), cos(theta2), sin(theta2) * sin(phi1));
            glm::vec3 p3 = position + radius * glm::vec3(sin(theta1) * cos(phi2), cos(theta1), sin(theta1) * sin(phi2));

            Renderer2D::DrawLine(p1, p2, color);
            Renderer2D::DrawLine(p1, p3, color);
        }
    }
}

}
