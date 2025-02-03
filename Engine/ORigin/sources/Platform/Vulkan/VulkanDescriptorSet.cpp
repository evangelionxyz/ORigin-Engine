// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "VulkanDescriptorSet.hpp"

#include "VulkanWrapper.hpp"

namespace origin {
VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(std::initializer_list<VulkanDescriptorSetLayoutBinding> elements)
    : m_elements(elements)
{
    CreateLayout();
}

void VulkanDescriptorSetLayout::CreateLayout()
{
    m_descriptor_sets.reserve(m_elements.size());

    for (const auto& element : m_elements)
    {
        VkDescriptorSetLayoutBinding layout;
        layout.binding = element.binding_point;
        layout.descriptorType = element.type;
        layout.stageFlags = element.stage;
        layout.descriptorCount = element.descriptor_count;
        layout.pImmutableSamplers = VK_NULL_HANDLE;
        m_descriptor_sets.push_back(layout);
    }

    VkDescriptorSetLayoutCreateInfo descriptor_layout_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
    descriptor_layout_info.bindingCount = static_cast<u32>(m_descriptor_sets.size());
    descriptor_layout_info.pBindings = m_descriptor_sets.data();
    descriptor_layout_info.pNext = VK_NULL_HANDLE;
    descriptor_layout_info.flags = 0;

    // Creating the layout
    VkResult result = vkCreateDescriptorSetLayout(VulkanContext::GetInstance()->GetVkDevice(),
                                                  &descriptor_layout_info,
                                                  nullptr,
                                                  &layout);

    VK_ERROR_CHECK(result, "[Vulkan] Failed to create descriptor set layout");
}

VulkanDescriptorSet::VulkanDescriptorSet(const VulkanDescriptorSetLayout &layout)
    : m_layout(layout)
{
    VkDescriptorSetAllocateInfo alloc_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
    alloc_info.descriptorPool = VulkanContext::GetInstance()->GetVkDescriptorPool();
    alloc_info.descriptorSetCount = 1;
    alloc_info.pSetLayouts = &m_layout.layout;

    VkResult result = vkAllocateDescriptorSets(VulkanContext::GetInstance()->GetVkDevice(), &alloc_info, &m_descriptor_set);
    VK_ERROR_CHECK(result, "[Vulkan] Failed to create descriptor");
}

void VulkanDescriptorSet::Bind(VkCommandBuffer cmd, VkPipelineLayout layout)
{
    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 0, 1, &m_descriptor_set, 0, nullptr);
}

void VulkanDescriptorSet::Update(const std::vector<VkWriteDescriptorSet>& writes)
{
    vkUpdateDescriptorSets(VulkanContext::GetInstance()->GetVkDevice(), 
                           static_cast<u32>(writes.size()), writes.data(), 0, nullptr);

}

void VulkanDescriptorSet::Destroy()
{
    vkDestroyDescriptorSetLayout(VulkanContext::GetInstance()->GetVkDevice(), m_layout.layout, nullptr);
}

}
