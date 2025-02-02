// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once
#include <vulkan/vulkan.h>
#include "VulkanWrapper.hpp"

namespace origin {
class VulkanBuffer
{
public:
    VulkanBuffer() = default;
    VulkanBuffer(VkBufferUsageFlags usage, VkDeviceSize buffer_size);

    void SetData(void *data, u64 size, u64 offset = 0) const;

    VkDeviceMemory GetMemory() const { return m_memory; }
    const VkBuffer GetBuffer() const { return m_buffer; }

    void Destroy();

private:
    VkBuffer m_buffer = VK_NULL_HANDLE;
    VkDeviceMemory m_memory = VK_NULL_HANDLE;
    VkDeviceSize m_buffer_size = 0;
};

}