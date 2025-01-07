#pragma once
#include <vulkan/vulkan.h>

namespace origin {

struct PhysicalDevice
{
    VkPhysicalDevice Device;
    VkPhysicalDeviceProperties Properties;
    std::vector<VkQueueFamilyProperties> QueueFamilyProperties;
    std::vector<VkBool32> QueueSupportPresent;
    std::vector<VkSurfaceFormatKHR> SurfaceFormats;
    std::vector<VkPresentModeKHR> PresentModes;
    VkSurfaceCapabilitiesKHR SurfaceCapabilities;
    VkPhysicalDeviceMemoryProperties MemoryProperties;
    VkPhysicalDeviceFeatures Features;
};

using VkSurfaceFormats = std::vector<VkSurfaceFormatKHR>;
using VkPresentModes = std::vector<VkPresentModeKHR>;

class VulkanPhysicalDevice
{
public:
    VulkanPhysicalDevice() = default;
    VulkanPhysicalDevice(VkInstance instance, const VkSurfaceKHR &surface);
    u32 SelectDevice(VkQueueFlags requiredQueueFlags, bool supportPresent);
    const PhysicalDevice &GetSelectedDevice() const;
private:
    static VkSurfaceFormats GetSurfaceFormat(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
    static VkPresentModes GetSurfacePresentModes(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);

    std::vector<PhysicalDevice> m_Devices;
    i32 m_DeviceIndex = -1;
}; 
}

