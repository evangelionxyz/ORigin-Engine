// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Origin/Core/Event.h"
#include "Origin/Core/MouseEvent.h"
#include "Origin/Audio/AudioListener.h"
#include "Origin/Core/Time.h"
#include "Camera.h"

#include <glm/glm.hpp>

namespace origin {

	class Entity;
	class EditorCamera : public Camera
	{
	public:
		EditorCamera() = default;
		void InitPerspective(float fovy, float aspectRatio, float nearClip, float farClip) override;
		void InitOrthographic(float size, float nearClip, float farClip) override;
		void OnUpdate(Timestep ts) override;
		void OnEvent(Event& e) override;
		void SetViewportSize(float width, float height) override;
		void SetPosition(const glm::vec3 &position) override;
        bool OnMouseScroll(MouseScrolledEvent &e);
        void MousePan(const glm::vec2 &delta);
        void MouseRotate(const glm::vec2 &delta);
        void MouseZoom(const float dela);
        float RotationSpeed() const;
        std::pair<float, float> PanSpeed() const;
		void UpdateAudioListener(Timestep ts) override;
		float GetZoomSpeed() const;
		const glm::mat4 &GetProjection() const override;
		const glm::mat4 &GetViewMatrix() const override;
	private:
        void UpdateProjection() override;
        void UpdateView() override;
	};
}