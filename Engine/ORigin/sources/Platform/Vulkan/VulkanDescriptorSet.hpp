// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once

#include "Origin/Core/Types.h"

#include <vulkan/vulkan.h>
#include <initializer_list>
#include <vector>
#include <string>

namespace origin {

struct VulkanDescriptorSetLayoutBinding {
    std::string name;
    VkDescriptorType type;
    VkShaderStageFlags stage;
    u32 binding_point = 0;
    u32 descriptor_count = 1;
};

class VulkanDescriptorSetLayout {
public:
    VulkanDescriptorSetLayout() = default;
    VulkanDescriptorSetLayout(std::initializer_list<VulkanDescriptorSetLayoutBinding> elements);
    const std::vector<VkDescriptorSetLayoutBinding>& GetDescriptorSets() const { return m_descriptor_sets; }

    VkDescriptorSetLayout layout;

private:
    void CreateLayout();

    std::vector<VkDescriptorSetLayoutBinding> m_descriptor_sets;
    std::vector<VulkanDescriptorSetLayoutBinding> m_elements;
    friend class VulkanDescriptorSet;
};

class VulkanDescriptorSet
{
public:
    VulkanDescriptorSet(const VulkanDescriptorSetLayout& layout);

    void Bind(VkCommandBuffer cmd, VkPipelineLayout layout);
    void Update(const std::vector<VkWriteDescriptorSet> &writes);
    void Destroy();

    const VkDescriptorSet *GetDescriptorSet() const { return &m_descriptor_set; }
    const VkDescriptorSetLayout *GetLayout() const { return &m_layout.layout; }

private:
    VkDescriptorSet m_descriptor_set = VK_NULL_HANDLE;
    VulkanDescriptorSetLayout m_layout;

};

}