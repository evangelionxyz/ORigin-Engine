// Copyright (c) Evangelion Manuhutu | ORigin Engine
#include "pch.h"
#include "VulkanBuffer.hpp"
#include "VulkanContext.hpp"

namespace origin {

VulkanBuffer::VulkanBuffer(VkBufferUsageFlags usage, VkDeviceSize buffer_size)
{
    VulkanContext *vk = VulkanContext::GetInstance();

    VkBufferCreateInfo create_info = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
    create_info.size = buffer_size;
    create_info.usage = usage;
    create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkResult result = vkCreateBuffer(vk->GetVkDevice(), &create_info, nullptr, &m_buffer);
    VK_ERROR_CHECK(result, "[Vulkan] Failed to create buffer");

    // memory requirements
    VkMemoryRequirements mem_requirements;
    vkGetBufferMemoryRequirements(vk->GetVkDevice(), m_buffer, &mem_requirements);

    // allocate memory
    VkMemoryAllocateInfo alloc_info = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
    alloc_info.allocationSize = mem_requirements.size;
    alloc_info.memoryTypeIndex = FindMemoryType(
        vk->GetVkPhysicalDevice(),
        mem_requirements.memoryTypeBits,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );

    result = vkAllocateMemory(vk->GetVkDevice(), &alloc_info, nullptr, &m_buffer_memory);
    vkBindBufferMemory(vk->GetVkDevice(), m_buffer, m_buffer_memory, 0);

}

VulkanBuffer::~VulkanBuffer()
{
}

void VulkanBuffer::Bind(VkCommandBuffer cmd)
{
    VkBuffer buffers[] = { m_buffer };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(cmd, 0, 1, buffers, offsets);
}

void VulkanBuffer::Destroy()
{
    VulkanContext *vk = VulkanContext::GetInstance();
    vkDestroyBuffer(vk->GetVkDevice(), m_buffer, nullptr);
    vkFreeMemory(vk->GetVkDevice(), m_buffer_memory, nullptr);
}

}