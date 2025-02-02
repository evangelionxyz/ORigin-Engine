// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Origin/Core/Buffer.h"
#include "VulkanImage.hpp"
#include "VulkanContext.hpp"

#include <stb_image.h>

namespace origin {

VulkanImage::VulkanImage(const std::filesystem::path &filepath, const TextureSpecification &spec)
{
    i32 width, height, channels;
    Buffer buffer;

    buffer.Data = stbi_load(filepath.generic_string().c_str(), &width, &height, &channels, 0);
    if (!buffer.Data)
    {
        buffer.Release();
        OGN_CORE_ASSERT(false, "[Vulkan] Failed to load texture");
    }

    buffer.Size = width * height * channels;

    m_width = static_cast<u32>(width);
    m_height = static_cast<u32>(height);
    m_channels = static_cast<u32>(channels);

    VkFormat format = channels == 4 ? VK_FORMAT_R8G8B8A8_SRGB : VK_FORMAT_R8G8B8_SRGB;
    VkFilter filter = VK_FILTER_LINEAR;
    VkSamplerAddressMode address_mode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;

    // CREATE IMAGE
    CreateImage(format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    // CREATE BUFFER
    m_buffer = VulkanBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, buffer.Size);
    m_buffer.SetData(buffer.Data, buffer.Size);

    // COPY BUFFER TO IMAGE
    CopyBufferToImage(format);

    CreateImageView(format);

    CreateSampler(filter, address_mode);

    buffer.Release();
}

void VulkanImage::Destroy()
{
    m_buffer.Destroy();

    vkFreeMemory(VulkanContext::GetInstance()->GetVkDevice(), m_memory, nullptr);

    vkDestroySampler(VulkanContext::GetInstance()->GetVkDevice(), m_sampler, nullptr);
    vkDestroyImageView(VulkanContext::GetInstance()->GetVkDevice(), m_image_view, nullptr);
    vkDestroyImage(VulkanContext::GetInstance()->GetVkDevice(), m_image, nullptr);
}

void VulkanImage::CreateImageView(VkFormat format)
{
    VkImageViewCreateInfo view_info{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
    view_info.image = m_image;
    view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    view_info.format = format;

    // device how the image should be interpreted
    view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    view_info.subresourceRange.baseMipLevel = 0;
    view_info.subresourceRange.levelCount = 1;
    view_info.subresourceRange.baseArrayLayer = 0;
    view_info.subresourceRange.layerCount = 1;

    vkCreateImageView(VulkanContext::GetInstance()->GetVkDevice(), &view_info, nullptr, &m_image_view);
}

void VulkanImage::CreateSampler(VkFilter filter, VkSamplerAddressMode address_mode)
{
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(VulkanContext::GetInstance()->GetVkPhysicalDevice(), &properties);

    VkSamplerCreateInfo sampler_info{ VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
    sampler_info.minFilter = filter;
    sampler_info.magFilter = filter;

    sampler_info.addressModeU = address_mode;
    sampler_info.addressModeV = address_mode;
    sampler_info.addressModeW = address_mode;

    sampler_info.anisotropyEnable = VK_FALSE;
    sampler_info.maxAnisotropy = properties.limits.maxSamplerAnisotropy;

    // user border color
    sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;

    // normalize UV coordinates (true for 0-1 range)
    sampler_info.unnormalizedCoordinates = VK_FALSE;

    sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    sampler_info.mipLodBias = 0.0f; // bias for mipmap level
    sampler_info.minLod = 0.0f; // mipmap level
    sampler_info.maxLod = 1.0f;

    VkResult result = vkCreateSampler(VulkanContext::GetInstance()->GetVkDevice(), &sampler_info, nullptr, &m_sampler);
    VK_ERROR_CHECK(result, "[Vulkan] Failed to create sampler");
}

void VulkanImage::CreateImage(VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties)
{
    VulkanContext *vk = VulkanContext::GetInstance();

    // create VkImage
    VkImageCreateInfo image_info = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
    image_info.imageType = VK_IMAGE_TYPE_2D;
    image_info.extent.width = m_width;
    image_info.extent.height = m_height;
    image_info.extent.depth = 1;
    image_info.mipLevels = 1;
    image_info.arrayLayers = 1;
    image_info.format = format;
    image_info.tiling = tiling;
    image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_info.usage = usage;
    image_info.samples = VK_SAMPLE_COUNT_1_BIT;
    image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkResult result = vkCreateImage(vk->GetVkDevice(), &image_info, nullptr, &m_image);
    VK_ERROR_CHECK(result, "[Vulkan] Failed to create image!");

    // allocate memory for image
    VkMemoryRequirements mem_requirements;
    vkGetImageMemoryRequirements(vk->GetVkDevice(), m_image, &mem_requirements);

    VkMemoryAllocateInfo alloc_info{ VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
    alloc_info.allocationSize = mem_requirements.size;
    alloc_info.memoryTypeIndex = VkFindMemoryType(vk->GetVkPhysicalDevice(), mem_requirements.memoryTypeBits, properties);

    result = vkAllocateMemory(vk->GetVkDevice(), &alloc_info, nullptr, &m_memory);
    VK_ERROR_CHECK(result, "[Vulkan] Failed to allocate image memory");

    vkBindImageMemory(vk->GetVkDevice(), m_image, m_memory, 0);
}

void VulkanImage::CopyBufferToImage(VkFormat format)
{
    VkCommandBuffer cmd = VkBeginSingleTimeCommands();

    // transition the image to a shader-readable layout
    TransitionImageLayout(format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    // define the region of the buffer to copy
    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset = { 0, 0, 0 };
    region.imageExtent = { m_width, m_height, 1 };

    // copy the buffer data into the image
    vkCmdCopyBufferToImage(cmd, m_buffer.GetBuffer(), m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    VkEndSingleTimeCommands(cmd);

    // transition the image to a shader-readable layout
    TransitionImageLayout(format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

void VulkanImage::TransitionImageLayout(VkFormat format, VkImageLayout old_layout, VkImageLayout new_layout)
{
    VkCommandBuffer cmd = VkBeginSingleTimeCommands();

    VkImageMemoryBarrier barrier{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
    barrier.oldLayout = old_layout;
    barrier.newLayout = new_layout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = m_image;

    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags source_stage;
    VkPipelineStageFlags destination_stage;

    if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destination_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        source_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destination_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else
    {
        throw std::invalid_argument("Unsupported layout transition!");
    }

    vkCmdPipelineBarrier(cmd,
        source_stage, destination_stage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier);

    VkEndSingleTimeCommands(cmd);
}
}
