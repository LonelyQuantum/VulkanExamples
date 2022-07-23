#include "vulkan_util.h"
#include "vulkan_reflection_util.h"

#include <iostream>
#include <iomanip>

namespace PVulkanExamples
{
    std::map<std::string, int> VulkanUtil::valuePrintColNum{
           { "VkPhysicalDeviceFeatures2", 4 }, { "VkPhysicalDeviceVulkan11Features", 4 }, { "VkPhysicalDeviceVulkan12Features", 3 },
           { "VkPhysicalDeviceAccelerationStructureFeaturesKHR", 3 }
    };

    std::map<std::string, int> VulkanUtil::valuePrintAlignment{
        { "VkPhysicalDeviceFeatures2", 45 }, { "VkPhysicalDeviceVulkan11Features", 45 }, { "VkPhysicalDeviceVulkan12Features", 60 },
        { "VkPhysicalDeviceAccelerationStructureFeaturesKHR", 45 }
    };


    /*
    * Helper function to print VkPhysicalDeviceFeatures struct
    */
    void VulkanUtil::printVkPhysicalDeviceFeatures(VkPhysicalDeviceFeatures features, int colNum) {
        std::vector<VkBool32>  featureValues = VulkanReflectionUtil::getVkBool32StructValues(features);
        std::cout << "\n=============================Print VkPhysicalDeviceFeatures=============================";
        for (int i = 0; i < featureValues.size(); i++) {
            if (i % colNum == 0) std::cout << "\n";
            std::cout << std::setw(45) << std::left << std::string(VulkanReflectionUtil::physicalDeviceFeatures2Vector[i]) + ": " + std::to_string(featureValues[i]);
        }
        std::cout << std::endl;
    }

    /*
    * Helper function to print VkPhysicalDeviceFeatures struct
    */
    void VulkanUtil::printVkPhysicalDeviceFeatures(void* features, int colNumInput) {
        std::string featureStructName = VulkanReflectionUtil::getVkBool32StructName(features);
        std::vector<const char*> featureStructMemberNames = VulkanReflectionUtil::getVkBool32StructVector(features);
        if (!featureStructMemberNames.empty()) {
            std::vector<VkBool32>  featureValues = VulkanReflectionUtil::getVkBool32StructValues(features);
            std::cout << "\n=============================Print " + featureStructName + "============================ = ";
            int colNum = colNumInput;
            if (valuePrintColNum.find(featureStructName) != valuePrintColNum.end()) {
                colNum = valuePrintColNum[featureStructName];
            }
            int alignment = alignmentDefault;
            if (valuePrintAlignment.find(featureStructName) != valuePrintAlignment.end()) {
                alignment = valuePrintAlignment[featureStructName];
            }
            for (int i = 0; i < featureValues.size(); i++) {
                if (i % colNum == 0) std::cout << "\n";
                std::cout << std::setw(alignment) << std::left << std::string(featureStructMemberNames[i]) + ": " + std::to_string(featureValues[i]);
            }
            std::cout << std::endl;
        }
        else std::cout << "\nPrint not support for this feature structure: " << featureStructName << std::endl;
    }

    /*
    * Helper function to print VkPhysicalDeviceFeatures struct
    */
    void VulkanUtil::printVkPhysicalDeviceFeatureStructChain(void* structFeatures, int colNum) {
        VulkanStructCommon* features = reinterpret_cast<VulkanStructCommon*>(structFeatures);
        while (features != nullptr) {
            printVkPhysicalDeviceFeatures(features);
            features = (VulkanStructCommon*)features->pNext;
        }
    }



    /*
    * Populate debug messenger according to whether it is debug mode
    */
    void VulkanUtil::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo,
        PFN_vkDebugUtilsMessengerCallbackEXT debugCallback,
        bool debug)
    {
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        if (debug)
        {
            createInfo.messageSeverity =
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
            createInfo.messageType =
                VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        }
        else
        {
            createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
            createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        }
        createInfo.pfnUserCallback = debugCallback;
    }

    /*
    * Create debug messenger for the Vulkan instance
    */
    VkResult VulkanUtil::createDebugUtilsMessengerEXT(VkInstance instance,
        const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkDebugUtilsMessengerEXT* pDebugMessenger)
    {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        }
        else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    /*
    * Destroy the debug messenger for Vulkan instance
    */
    void VulkanUtil::destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
    {
        auto func =
            (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr)
        {
            func(instance, debugMessenger, pAllocator);
        }
    }

    /*
    * Plain debug callback that only outputs the message
    */
    VKAPI_ATTR VkBool32 VKAPI_CALL VulkanUtil::plainDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT instance,
        VkDebugUtilsMessageTypeFlagsEXT pCreateInfo,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData)
    {
        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
        return VK_FALSE;
    }

    uint32_t VulkanUtil::findMemoryType(VkPhysicalDevice      physical_device,
        uint32_t              type_filter,
        VkMemoryPropertyFlags properties_flag)
    {
        VkPhysicalDeviceMemoryProperties physical_device_memory_properties;
        vkGetPhysicalDeviceMemoryProperties(physical_device, &physical_device_memory_properties);
        for (uint32_t i = 0; i < physical_device_memory_properties.memoryTypeCount; i++)
        {
            if (type_filter & (1 << i) && (physical_device_memory_properties.memoryTypes[i].propertyFlags & properties_flag) == properties_flag)
            {
                return i;
            }
        }
        throw std::runtime_error("Failed to find memory type");
    }
} // namespace PVulkanExamples