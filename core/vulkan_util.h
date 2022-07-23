#pragma once

#include <vulkan/vulkan_core.h>

#include <string>
#include <map>

namespace PVulkanExamples
{
	class VulkanUtil
	{
	public:
        static void printVkPhysicalDeviceFeatures(VkPhysicalDeviceFeatures features, int colNum = 4);
        static void printVkPhysicalDeviceFeatures(void* features, int colNumInput = 3);
        static void printVkPhysicalDeviceFeatureStructChain(void* structFeatures, int colNum = 3);

        static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo,
            PFN_vkDebugUtilsMessengerCallbackEXT debugCallback,
            bool debug = true);
        static VkResult createDebugUtilsMessengerEXT(VkInstance instance,
            const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
            const VkAllocationCallbacks* pAllocator,
            VkDebugUtilsMessengerEXT* pDebugMessenger);
        static void destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

        static VKAPI_ATTR VkBool32 VKAPI_CALL plainDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT instance,
            VkDebugUtilsMessageTypeFlagsEXT pCreateInfo,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData);

        uint32_t VulkanUtil::findMemoryType(VkPhysicalDevice      physical_device,
            uint32_t              type_filter,
            VkMemoryPropertyFlags properties_flag);
    private:
        static const int alignmentDefault = 60;
        static std::map<std::string, int> valuePrintColNum;
        static std::map<std::string, int> valuePrintAlignment;
	};
} // namespace PVulkanExmaples