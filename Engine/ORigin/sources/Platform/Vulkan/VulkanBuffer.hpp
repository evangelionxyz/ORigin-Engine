// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once
#include <vulkan/vulkan.h>
#include "VulkanWrapper.hpp"

namespace origin {

static u32 FindMemoryType(VkPhysicalDevice physical_device, u32 type_filter,
    VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties mem_prop;
    vkGetPhysicalDeviceMemoryProperties(physical_device, &mem_prop);

    for (u32 i = 0; i < mem_prop.memoryHeapCount; ++i)
    {
        if ((type_filter & (1 << i)) &&
            (mem_prop.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }
    throw std::runtime_error("[Vulkan] Failed to find suitable memory type!");
}

static void CopyDataToBuffer(VkDevice device, VkDeviceMemory buffer_memory,
    void *data, VkDeviceSize size)
{
    void *mapped_data;
    vkMapMemory(device, buffer_memory, 0, size, 0, &mapped_data);
    memcpy(mapped_data, data, (size_t)size);
    vkUnmapMemory(device, buffer_memory);
}

class VulkanBuffer
{
public:
    VulkanBuffer() = default;
    VulkanBuffer(VkBufferUsageFlags usage, VkDeviceSize buffer_size);
    ~VulkanBuffer();

    VkDeviceMemory GetBufferMemory() const { return m_buffer_memory; }
    VkBuffer GetBuffer() const { return m_buffer; }

    void Destroy();

private:
    VkBuffer m_buffer = VK_NULL_HANDLE;
    VkDeviceSize m_buffer_size = 0;
    VkDeviceMemory m_buffer_memory = VK_NULL_HANDLE;
};

}