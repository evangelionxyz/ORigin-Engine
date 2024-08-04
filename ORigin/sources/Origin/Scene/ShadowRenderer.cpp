// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "ShadowRenderer.h"
#include "Origin/Renderer/Renderer.h"

#include "Lighting.h"

#include <glm/glm.hpp>

namespace origin {

    void ShadowRenderer::OnCreate(LightingType type)
	{
		OGN_PROFILER_RENDERING();
        // depth map shader
        m_DepthMapShader = Shader::Create("Resources/Shaders/SPIR-V/DepthMap.glsl", false);

        glGenFramebuffers(1, &FrameBufferID);
        glGenTextures(1, &DepthMapID);
        glBindTexture(GL_TEXTURE_2D, DepthMapID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 2048, 2048, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        glBindFramebuffer(GL_FRAMEBUFFER, FrameBufferID);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, DepthMapID, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}

    // bind for each lighting
    void ShadowRenderer::BindFBO()
    {
        glViewport(0, 0, 2048, 2048);
        glBindFramebuffer(GL_FRAMEBUFFER, FrameBufferID);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void ShadowRenderer::UnbindFBO()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, NULL);
    }

	// begin for each object
    void ShadowRenderer::CalculateViewProjection(const TransformComponent &tc, const glm::mat4 &camViewProjection)
	{
#if 1
        glm::vec3 lightDir = -tc.GetForward();// use negative forward as light direction

        std::array<glm::vec3, 8> frustumCorners;
        CalculateFrustumCorners(camViewProjection, frustumCorners);

        // find the bounding box of the frustum in light space
        glm::vec3 minCorner(std::numeric_limits<float>::max());
        glm::vec3 maxCorner(std::numeric_limits<float>::lowest());

        glm::mat4 lightView = glm::lookAt(glm::vec3(0.0f) - lightDir, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        for (const auto &corner : frustumCorners)
        {
            glm::vec3 lightSpaceCorner = glm::vec3(lightView * glm::vec4(corner, 1.0f));
            minCorner = glm::min(minCorner, lightSpaceCorner);
            maxCorner = glm::max(maxCorner, lightSpaceCorner);
        }

        // calculate orthographic projection parameters
        float l = minCorner.x, r = maxCorner.x;
        float b = minCorner.y, t = maxCorner.y;
        float n = -maxCorner.z, f = minCorner.z; // flip for GL's negative z direction

        // add some padding to avoid clipping errors
        float padding = 10.0f;
        glm::mat4 lightProjection = glm::ortho(l - padding, r + padding, b - padding, t + padding, n - padding, f + padding);
        ViewProjection = lightProjection * lightView;
#else
        //glm::mat4 lightView = glm::lookAt(-tc.GetUp(), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        //glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, 20.0f, 20.0f, 20.0f, 20.0f);
        //ViewProjection = lightProjection * lightView;
#endif
	}

    void ShadowRenderer::CalculateFrustumCorners(const glm::mat4 &camViewProjection, std::array<glm::vec3, 8> &frustumCorners)
    {
        glm::mat4 invCamVP = glm::inverse(camViewProjection);

        std::array<glm::vec4, 8> ndcCorners = {
            glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f),
            glm::vec4(1.0f, -1.0f, -1.0f, 1.0f),
            glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f),
            glm::vec4(1.0f, 1.0f, -1.0f, 1.0f),
            glm::vec4(-1.0f, -1.0f, 1.0f, 1.0f),
            glm::vec4(1.0f, -1.0f, 1.0f, 1.0f),
            glm::vec4(-1.0f, 1.0f, 1.0f, 1.0f),
            glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)
        };

        for (size_t i = 0; i < 8; ++i)
        {
            glm::vec4 invCorner = invCamVP * ndcCorners[i];
            frustumCorners[i] = invCorner / invCorner.w;
        }
    }

}