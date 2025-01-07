// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "VulkanFramebuffer.hpp"

namespace origin {

VulkanFramebuffer::VulkanFramebuffer(const FramebufferSpecification &specification)
    : m_Specification(specification)
{
}

VulkanFramebuffer::~VulkanFramebuffer()
{

}

void VulkanFramebuffer::Resize(u32 width, u32 height)
{

}

int VulkanFramebuffer::ReadPixel(u32 attachmentIndex, int x, int y)
{
    return 0;
}

void VulkanFramebuffer::ClearAttachment(u32 attachmentIndex, int value)
{

}

void VulkanFramebuffer::Bind()
{

}

void VulkanFramebuffer::Unbind()
{

}

}