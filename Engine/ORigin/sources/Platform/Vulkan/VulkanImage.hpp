// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once

#include "Origin/Renderer/Texture.h"
#include "VulkanBuffer.hpp"
#include "VulkanWrapper.hpp"

#include <vulkan/vulkan.h>
#include <filesystem>

namespace origin {

class VulkanImage final : public Texture2D
{
public:
    VulkanImage() = default;
    VulkanImage(const std::filesystem::path &filepath, const TextureSpecification &spec = TextureSpecification());

    VkImage GetImage() const { return m_image; }
    VkDeviceMemory GetMemory() const { return m_memory; }
    VkImageView GetImageView() const { return m_image_view; }
    VkSampler GetSampler() const { return m_sampler; }

    void Destroy() override;

    bool operator==(const Texture &other) const override
    {
        return m_image == ((VulkanImage &)other).m_image;
    }

private:
    void CreateImageView(VkFormat format);
    void CreateSampler(VkFilter filter, VkSamplerAddressMode address_mode);
    void CreateImage(VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);
    void CopyBufferToImage(VkFormat format);
    void TransitionImageLayout(VkFormat format, VkImageLayout old_layout, VkImageLayout new_layout);

    VkImage m_image;
    VkImageView m_image_view;
    VkSampler m_sampler;

    VkDeviceMemory m_memory;
    VulkanBuffer m_buffer;
};

}
