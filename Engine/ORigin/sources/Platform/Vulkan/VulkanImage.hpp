// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once

#include <vulkan/vulkan.h>
#include "VulkanBuffer.hpp"
#include "VulkanWrapper.hpp"

namespace origin {

class VulkanImage
{
public:

    VulkanImage() = default;
    VulkanImage(const std::string &filepath);

    VkImage GetImage() const { return m_image; }
    VkDeviceMemory GetMemory() const { return m_memory; }
    VkImageView GetImageView() const { return m_image_view; }
    VkSampler GetSampler() const { return m_sampler; }

    void Destroy();

private:
    void CreateImageView(VkFormat format);
    void CreateSampler(VkDevice device, VkPhysicalDevice physical_device);
    void CreateImage(u32 width, u32 height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);
    void CopyBufferToImage(u32 width, u32 height);
    void TransitionImageLayout(VkFormat format, VkImageLayout old_layout, VkImageLayout new_layout);

    VkImage m_image;
    VkImageView m_image_view;
    VkSampler m_sampler;

    VkDeviceMemory m_memory;
    VulkanBuffer m_buffer;
};

}
