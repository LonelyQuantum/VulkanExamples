#include "vulkan_example_base.h"
#include "templates.h"
#include "vulkan_reflection_util.h"
#include "vulkan_util.h"

#include <set>
#include <algorithm>
#include <stdexcept>

namespace PVulkanExamples
{
	void ExampleBase::init()
	{
        //Window settings
        m_windowWidth = 800;
        m_windowHeight = 600;
        if (!glfwInit())
        {
            throw std::runtime_error("failed to initialize GLFW");
        }
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        m_window = glfwCreateWindow(m_windowWidth, m_windowHeight, m_title, nullptr, nullptr);

        //Specify vulkan api settings
        m_apiMajor = 1;
        m_apiMinor = 2;
        m_apiVersion = VK_MAKE_API_VERSION(0, m_apiMajor, m_apiMinor, 0);
        m_instanceLayers = { "VK_LAYER_KHRONOS_validation" };
        m_enableValidationLayers = !m_instanceLayers.empty();

        //Memory allocator
        m_defaultAllocator = nullptr;

        // Instance extensions
        m_instanceExtensions = {};
        // GLFW extensions
        uint32_t count{ 0 };
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&count);
        for (int extId = 0; extId < count; ++extId)
        {
            m_instanceExtensions.push_back(glfwExtensions[extId]);
        }

        // Debug extension
        if (m_enableValidationLayers)
        {
            m_instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        // Device extensions and physical device features
        addDeviceExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
        VkPhysicalDeviceAccelerationStructureFeaturesKHR accelFeature{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR };
        addDeviceExtension(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME, &accelFeature, { "accelerationStructure" });
        VkPhysicalDeviceRayTracingPipelineFeaturesKHR rtPipelineFeature{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR };
        addDeviceExtension(VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME, &rtPipelineFeature, { "rayTracingPipeline" });  // To use vkCmdTraceRaysKHR
        addDeviceExtension(VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME);  // Required by ray tracing pipeline
        addPhysicalDeviceFeatureRequirement(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, "samplerAnisotropy");
        addPhysicalDeviceFeatureRequirement(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, "fragmentStoresAndAtomics");  // support inefficient readback storage buffer
        addPhysicalDeviceFeatureRequirement(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, "independentBlend");          // support independent blending
        addPhysicalDeviceFeatureRequirement(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, "geometryShader");            // support geometry shader
        addPhysicalDeviceFeatureRequirement(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES, "multiviewGeometryShader"); // Test struct chain
        addPhysicalDeviceFeatureRequirement(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES, "imagelessFramebuffer");    // Test struct chain
        constructStructChain(); // Construct the struct chain for physical device features  

        // Command buffer setting
        m_maxFrameInFlight = 3;
        m_currentFrameIndex = 0;

        // Descriptor pool settings
        m_maxVertexBlendingMeshCount = 256;
        m_maxMaterialCount = 256;

		createInstance();
		createDebugMessenger();
		createSurface();
		createPhysicalDevice();
		createLogicalDevice();
		initializeCommandPools();
		initializeCommandBuffers();
		createDescriptorPools();
		createSyncObjects();
	}

    void ExampleBase::cleanup()
    {
        vkDestroyDevice(m_device, m_defaultAllocator);
        vkDestroySurfaceKHR(m_instance, m_surface, m_defaultAllocator);
        VulkanUtil::destroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger, m_defaultAllocator);
        vkDestroyInstance(m_instance, m_defaultAllocator);
    }

    void ExampleBase::setup()
    {
        //TODO:fix setup
    }

	void ExampleBase::createInstance()
	{
        // Check the availability for required validation layers
        if (m_enableValidationLayers && !checkValidationLayerSupport())
        {
            throw std::runtime_error("validation layers requested, but not available!");
        }

        // App info
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "VulkanExamples";
        appInfo.pEngineName = "No Engine";
        appInfo.apiVersion = m_apiVersion;

        // Instance create info
        VkInstanceCreateInfo instanceCreateInfo{};
        instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceCreateInfo.pApplicationInfo = &appInfo;
        instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(m_instanceExtensions.size());
        instanceCreateInfo.ppEnabledExtensionNames = m_instanceExtensions.data(); //Instance extensions

        // Instance layers
        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        if (m_enableValidationLayers)
        {
            instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(m_instanceLayers.size());
            instanceCreateInfo.ppEnabledLayerNames = m_instanceLayers.data();
            VulkanUtil::populateDebugMessengerCreateInfo(debugCreateInfo, VulkanUtil::plainDebugCallback);
            instanceCreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
        }
        else
        {
            instanceCreateInfo.enabledLayerCount = 0;
            instanceCreateInfo.pNext = nullptr;
        }

        // Create instance
        if (vkCreateInstance(&instanceCreateInfo, m_defaultAllocator, &m_instance) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create instance!");
        }
	}

	void ExampleBase::createDebugMessenger()
	{
        if (m_enableValidationLayers)
        {
            VkDebugUtilsMessengerCreateInfoEXT createInfo;
            VulkanUtil::populateDebugMessengerCreateInfo(createInfo, VulkanUtil::plainDebugCallback);
            if (VK_SUCCESS != VulkanUtil::createDebugUtilsMessengerEXT(m_instance, &createInfo, m_defaultAllocator, &m_debugMessenger))
            {
                throw std::runtime_error("failed to set up debug messenger!");
            }
        }
	}

	void ExampleBase::createSurface()
	{
        if (glfwCreateWindowSurface(m_instance, m_window, m_defaultAllocator, &m_surface) != VK_SUCCESS)
        {
            throw std::runtime_error("glfwCreateWindowSurface");
        }
	}

	void ExampleBase::createPhysicalDevice()
	{
        //Check if there are any physical device available
        uint32_t availableDeviceCount = 0;
        vkEnumeratePhysicalDevices(m_instance, &availableDeviceCount, nullptr);
        if (availableDeviceCount == 0)
        {
            throw std::runtime_error("Failed to find physical device with Vulkan support!");
        }

        std::vector<VkPhysicalDevice> compatibleDevices{};
        std::vector<VkPhysicalDevice> availableDevices(availableDeviceCount);
        vkEnumeratePhysicalDevices(m_instance, &availableDeviceCount, availableDevices.data());

        //Find all suitable devices	
        for (const VkPhysicalDevice device : availableDevices)
        {
            if (isDeviceSuitable(device))
            {
                compatibleDevices.push_back(device);
                break;
            }
        }

        if (compatibleDevices.size() > 0)
        {
            m_physicalDevice = compatibleDevices[0];
        }
        else
        {
            throw std::runtime_error("Cannot find any compatible physical device");
        }

        m_queueFamilyIndices = findQueueFamilies(m_physicalDevice);
	}

	void ExampleBase::createLogicalDevice()
	{
        // Initialize queue create infos
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = {
            m_queueFamilyIndices.graphicsFamily.value(), m_queueFamilyIndices.computeFamily.value(),
            m_queueFamilyIndices.transferFamily.value(), m_queueFamilyIndices.presentFamily.value()
        };

        float queuePriority = 1.0f; // TODO compute queue priority
        for (uint32_t queueFamily : uniqueQueueFamilies)
        {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        // Device create info
        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data(); // Add queue create infos
        createInfo.pEnabledFeatures = nullptr;
        createInfo.pNext = &m_physicalFeaturesStructChain; // When version >= vulkan1.1 we use pNext to add physical device features
        createInfo.enabledExtensionCount = static_cast<uint32_t>(m_deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = m_deviceExtensions.data(); // Add device extensions
        createInfo.enabledLayerCount = 0;

        // Create logical device
        VkResult res = vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_device);
        if (res != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create logical device!");
        }

        // Create efficient function pointers
        m_pfn_vkSetDebugUtilsObjectNameEXT = (PFN_vkSetDebugUtilsObjectNameEXT)vkGetInstanceProcAddr(m_instance, "vkSetDebugUtilsObjectNameEXT");

        // Create queues
        vkGetDeviceQueue(m_device, m_queueFamilyIndices.graphicsFamily.value(), 0, &m_graphicsQueue);
        setObjectName(m_graphicsQueue, "Graphics Queue");
        vkGetDeviceQueue(m_device, m_queueFamilyIndices.computeFamily.value(), 0, &m_computeQueue);
        setObjectName(m_computeQueue, "Compute Queue");
        vkGetDeviceQueue(m_device, m_queueFamilyIndices.transferFamily.value(), 0, &m_transferQueue);
        setObjectName(m_transferQueue, "Transfer Queue");
        vkGetDeviceQueue(m_device, m_queueFamilyIndices.presentFamily.value(), 0, &m_presentQueue);
        setObjectName(m_presentQueue, "Present Queue");
	}

	void ExampleBase::initializeCommandPools()
	{

	}

	void ExampleBase::initializeCommandBuffers()
	{

	}

	void ExampleBase::createDescriptorPools()
	{

	}

	void ExampleBase::createSyncObjects()
	{

	}



    /*
    * Add device extension requirement and corresponding physical device feature requirements
    */
    void ExampleBase::addDeviceExtension(const char* extension, void* pPhysicalDeviceFeatureStruct, std::vector<const char*> featureRequirements) {
        add_unique(m_deviceExtensions, extension);
        if (pPhysicalDeviceFeatureStruct != nullptr)
        {
            m_EXTPhysicalDeviceFeatureStructs.push_back(pPhysicalDeviceFeatureStruct);
            VkStructureType sType = reinterpret_cast<VulkanExtensionHeader*>(pPhysicalDeviceFeatureStruct)->sType;
            if (m_physicalDeviceFeatureRequirements.find(sType) != m_physicalDeviceFeatureRequirements.end())
            {
                m_physicalDeviceFeatureRequirements[sType].insert(m_physicalDeviceFeatureRequirements[sType].end(),
                    featureRequirements.begin(),
                    featureRequirements.end());
            }
            else if (featureRequirements.size() > 0)
            {
                m_physicalDeviceFeatureRequirements[sType] = featureRequirements;
            }

        }
    }

    void ExampleBase::addPhysicalDeviceFeatureRequirement(VkStructureType featureStructType, const char* feature) {
        if (m_physicalDeviceFeatureRequirements.find(featureStructType) != m_physicalDeviceFeatureRequirements.end())
            add_unique(m_physicalDeviceFeatureRequirements[featureStructType], feature);
        else
            m_physicalDeviceFeatureRequirements[featureStructType] = std::vector<const char*>{ feature };
    }

    /*
    * Check if the required validation layers are supported
    */
    bool ExampleBase::checkValidationLayerSupport() {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (const char* layerName : m_instanceLayers)
        {
            bool layerFound = false;

            for (const auto& layerProperties : availableLayers)
            {
                if (strcmp(layerName, layerProperties.layerName) == 0)
                {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound)
                return false;
        }

        return true;
    }

    /*
    * Construct the struct chain of physical device features
    */
    void ExampleBase::constructStructChain() {
        m_physicalFeaturesStructChain.features = m_features10;

        // Only support Vulkan 1.2 for now
        if (m_apiMajor == 1 && m_apiMinor >= 2)
        {
            m_physicalFeaturesStructChain.pNext = &m_features11;
            m_features11.pNext = &m_features12;
            m_features12.pNext = nullptr;
        }
        // use the physicalFeaturesStructChain to append extensions
        if (!m_EXTPhysicalDeviceFeatureStructs.empty())
        {
            // build up chain of all used extension features
            for (size_t i = 0; i < m_EXTPhysicalDeviceFeatureStructs.size(); i++)
            {
                auto* header = reinterpret_cast<VulkanExtensionHeader*>(m_EXTPhysicalDeviceFeatureStructs[i]);
                header->pNext = i < m_EXTPhysicalDeviceFeatureStructs.size() - 1 ? m_EXTPhysicalDeviceFeatureStructs[i + 1] : nullptr;
            }

            // append to the end of current feature2 struct
            VulkanExtensionHeader* lastCoreFeature = (VulkanExtensionHeader*)&m_physicalFeaturesStructChain;
            while (lastCoreFeature->pNext != nullptr)
            {
                lastCoreFeature = (VulkanExtensionHeader*)lastCoreFeature->pNext;
            }
            lastCoreFeature->pNext = m_EXTPhysicalDeviceFeatureStructs[0];
        }
    }

    /*
    * Set debug name of given object
    */
    void ExampleBase::setObjectName(const uint64_t object, const std::string& name, VkObjectType t)
    {
        VkDebugUtilsObjectNameInfoEXT s{ VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT, nullptr, t, object, name.c_str() };
        m_pfn_vkSetDebugUtilsObjectNameEXT(m_device, &s);
    }



    /*
    * Check whether the physical device has required features and support required extensions and queue families
    */
    bool ExampleBase::isDeviceSuitable(VkPhysicalDevice device)
    {
        m_queueFamilyIndices = findQueueFamilies(device);               //Initialize queue family indices and check queue family support
        bool extensionsSupported = checkDeviceExtensionSupport(device, m_deviceExtensions); //Check device extension support

        //Check presentation support if surface is assigned
        uint32_t formatCount;
        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, nullptr);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount, nullptr);
        bool presentSupported = formatCount > 0 && presentModeCount > 0;

        //Check physical device features support
        vkGetPhysicalDeviceFeatures2(device, &m_physicalFeaturesStructChain);
        bool requiredFeaturesSupported = checkDeviceFeaturesSupport(device, m_physicalDeviceFeatureRequirements);

        return extensionsSupported && requiredFeaturesSupported && m_queueFamilyIndices.isComplete() && presentSupported;
    }

    /*
    * Return the queue families supported by give physical device
    */
    QueueFamilyIndices  ExampleBase::findQueueFamilies(VkPhysicalDevice device) {
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        int i = 0;
        for (const auto& queueFamily : queueFamilies) {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                m_queueFamilyIndices.graphicsFamily = i;
            }

            if (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) {
                m_queueFamilyIndices.computeFamily = i;
            }

            if (queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT) {
                m_queueFamilyIndices.transferFamily = i;
            }

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_surface, &presentSupport);
            if (presentSupport) {
                m_queueFamilyIndices.presentFamily = i;
            }

            if (m_queueFamilyIndices.isComplete()) break;
            i++;
        }

        return m_queueFamilyIndices;
    }

    bool ExampleBase::checkDeviceExtensionSupport(VkPhysicalDevice device, std::vector<const char*> deviceExtensions) {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

        std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

        for (const auto& extension : availableExtensions)
            requiredExtensions.erase(extension.extensionName);


        return requiredExtensions.empty();
    }

    bool ExampleBase::checkDeviceFeaturesSupport(VkPhysicalDevice device, std::map<VkStructureType, std::vector<const char*>> physicalDeviceFeatureRequirements) {
        bool res = true;
        VulkanExtensionHeader* pStructChainIterator = reinterpret_cast<VulkanExtensionHeader*>(&m_physicalFeaturesStructChain);
        while (pStructChainIterator != nullptr)
        {
            VkStructureType sType = pStructChainIterator->sType;
            if (physicalDeviceFeatureRequirements.find(sType) != physicalDeviceFeatureRequirements.end())
                res = res && checkDeviceFeaturesSupport(device, pStructChainIterator, physicalDeviceFeatureRequirements[sType]);
            pStructChainIterator = (VulkanExtensionHeader*)pStructChainIterator->pNext;
        }
        return true;
    }

    bool ExampleBase::checkDeviceFeaturesSupport(VkPhysicalDevice device, void* pFeatures, std::vector<const char*> requiredFeatures) {
        std::string structName = VulkanReflectionUtil::getVkBool32StructName(pFeatures);
        bool res = true;
        for (const char* featureName : requiredFeatures)
            res = res && VulkanReflectionUtil::getVkBool32StructValue(pFeatures, featureName);
        return res;
    }

    SwapChainSupportDetails ExampleBase::querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface) {
        SwapChainSupportDetails details;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

        //formats
        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
        if (formatCount != 0)
        {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
        }

        //present modes
        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
        if (presentModeCount != 0)
        {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
        }

        return details;
    }

    VkSurfaceFormatKHR ExampleBase::chooseSwapchainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
        for (const auto& availableFormat : availableFormats)
        {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                return availableFormat;
            }
        }
        return availableFormats[0];
    }

    /*
    * Specify the present mode format for the currrent Swapchain, prefer VK_PRESENT_MODE_MAILBOX_KHR
    */
    VkPresentModeKHR ExampleBase::chooseSwapchainPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
        for (const auto& availablePresentMode : availablePresentModes)
        {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                return availablePresentMode;
            }
        }
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D ExampleBase::chooseSwapchainExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
        if (capabilities.currentExtent.width != UINT32_MAX)
        {
            return capabilities.currentExtent;
        }
        else
        {
            int width, height;
            glfwGetFramebufferSize(m_window, &width, &height);
            VkExtent2D actualExtent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };

            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            return actualExtent;
        }
    }

    VkFormat ExampleBase::findDepthFormat(VkPhysicalDevice physicalDevice) {
        return findSupportedFormat(
            physicalDevice,
            { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
            VK_IMAGE_TILING_OPTIMAL,
            VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
        );
    }

    VkFormat ExampleBase::findSupportedFormat(VkPhysicalDevice physicalDevice, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
        for (VkFormat format : candidates) {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

            if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
                return format;
            }
            else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
                return format;
            }
        }

        throw std::runtime_error("failed to find supported format!");
    }


} // namespace PVulkanExamples