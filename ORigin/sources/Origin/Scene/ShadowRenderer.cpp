// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "ShadowRenderer.h"
#include "Origin/Renderer/Renderer.h"

#include "Lighting.h"

#include <glm/glm.hpp>

namespace origin {

    void ShadowRenderer::OnCreate(LightingType type)
	{
        // depth map shader
        m_DepthMapShader = Renderer::GetShader("DepthMap");
        glGenFramebuffers(1, &FrameBufferID);
        glGenTextures(1, &DepthMapID);
        glBindTexture(GL_TEXTURE_2D, DepthMapID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
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
        glViewport(0, 0, 1024, 1024);
        glBindFramebuffer(GL_FRAMEBUFFER, FrameBufferID);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void ShadowRenderer::UnbindFBO()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, NULL);
    }
}