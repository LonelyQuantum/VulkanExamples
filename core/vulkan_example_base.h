#pragma once

#include <vulkan/vulkan_core.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <optional>
#include <map>
#include <string>


namespace PVulkanExamples
{
	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> transferFamily;
		std::optional<uint32_t> computeFamily;
		std::optional<uint32_t> presentFamily;

		bool isComplete() {
			return graphicsFamily.has_value() && transferFamily.has_value() && computeFamily.has_value() && presentFamily.has_value();
		}
	};

	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR        capabilities{};
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR>   presentModes;
	};

	class ExampleBase
	{
	public:
		ExampleBase() {};
		~ExampleBase() {};

		void init();
		void run();
		void cleanup();

		void setup();
		void createInstance();
		void createDebugMessenger();
		void createSurface();
		void createPhysicalDevice();
		void createLogicalDevice();
		void initializeCommandPools();
		void initializeCommandBuffers();
		void createDescriptorPools();
		void createSyncObjects();

		void drawFrame();
		void updateUniformBuffers();


		void addDeviceExtension(const char* extension, void* pPhysicalDeviceFeatureStruct = VK_NULL_HANDLE, std::vector<const char*> featureRequirements = {});
		void addPhysicalDeviceFeatureRequirement(VkStructureType featureStructType, const char* feature);

	private:
		// Private helpers
		bool checkValidationLayerSupport();
		void constructStructChain();
		bool isDeviceSuitable(VkPhysicalDevice device);
		QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
		bool checkDeviceExtensionSupport(VkPhysicalDevice device, std::vector<const char*> deviceExtensions);
		bool checkDeviceFeaturesSupport(VkPhysicalDevice device, std::map<VkStructureType, std::vector<const char*>> physicalDeviceFeatureRequirements);
		bool checkDeviceFeaturesSupport(VkPhysicalDevice device, void* pFeatures, std::vector<const char*> requiredFeatures);
		SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
		VkSurfaceFormatKHR chooseSwapchainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR chooseSwapchainPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D chooseSwapchainExtent(const VkSurfaceCapabilitiesKHR& capabilities);
		VkFormat findDepthFormat(VkPhysicalDevice physicalDevice);
		VkFormat findSupportedFormat(VkPhysicalDevice physicalDevice, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

	private:
		// Debug object name setters
		void setObjectName(const uint64_t object, const std::string& name, VkObjectType t);
		inline void setObjectName(VkBuffer object, const std::string& name) { setObjectName((uint64_t)object, name, VK_OBJECT_TYPE_BUFFER); }
		inline void setObjectName(VkBufferView object, const std::string& name) { setObjectName((uint64_t)object, name, VK_OBJECT_TYPE_BUFFER_VIEW); }
		inline void setObjectName(VkCommandBuffer object, const std::string& name) { setObjectName((uint64_t)object, name, VK_OBJECT_TYPE_COMMAND_BUFFER); }
		inline void setObjectName(VkCommandPool object, const std::string& name) { setObjectName((uint64_t)object, name, VK_OBJECT_TYPE_COMMAND_POOL); }
		inline void setObjectName(VkDescriptorPool object, const std::string& name) { setObjectName((uint64_t)object, name, VK_OBJECT_TYPE_DESCRIPTOR_POOL); }
		inline void setObjectName(VkDescriptorSet object, const std::string& name) { setObjectName((uint64_t)object, name, VK_OBJECT_TYPE_DESCRIPTOR_SET); }
		inline void setObjectName(VkDescriptorSetLayout object, const std::string& name) { setObjectName((uint64_t)object, name, VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT); }
		inline void setObjectName(VkDevice object, const std::string& name) { setObjectName((uint64_t)object, name, VK_OBJECT_TYPE_DEVICE); }
		inline void setObjectName(VkDeviceMemory object, const std::string& name) { setObjectName((uint64_t)object, name, VK_OBJECT_TYPE_DEVICE_MEMORY); }
		inline void setObjectName(VkFramebuffer object, const std::string& name) { setObjectName((uint64_t)object, name, VK_OBJECT_TYPE_FRAMEBUFFER); }
		inline void setObjectName(VkFence object, const std::string& name) { setObjectName((uint64_t)object, name, VK_OBJECT_TYPE_FENCE); }
		inline void setObjectName(VkImage object, const std::string& name) { setObjectName((uint64_t)object, name, VK_OBJECT_TYPE_IMAGE); }
		inline void setObjectName(VkImageView object, const std::string& name) { setObjectName((uint64_t)object, name, VK_OBJECT_TYPE_IMAGE_VIEW); }
		inline void setObjectName(VkPipeline object, const std::string& name) { setObjectName((uint64_t)object, name, VK_OBJECT_TYPE_PIPELINE); }
		inline void setObjectName(VkPipelineLayout object, const std::string& name) { setObjectName((uint64_t)object, name, VK_OBJECT_TYPE_PIPELINE_LAYOUT); }
		inline void setObjectName(VkQueryPool object, const std::string& name) { setObjectName((uint64_t)object, name, VK_OBJECT_TYPE_QUERY_POOL); }
		inline void setObjectName(VkQueue object, const std::string& name) { setObjectName((uint64_t)object, name, VK_OBJECT_TYPE_QUEUE); }
		inline void setObjectName(VkRenderPass object, const std::string& name) { setObjectName((uint64_t)object, name, VK_OBJECT_TYPE_RENDER_PASS); }
		inline void setObjectName(VkSampler object, const std::string& name) { setObjectName((uint64_t)object, name, VK_OBJECT_TYPE_SAMPLER); }
		inline void setObjectName(VkSemaphore object, const std::string& name) { setObjectName((uint64_t)object, name, VK_OBJECT_TYPE_SEMAPHORE); }
		inline void setObjectName(VkShaderModule object, const std::string& name) { setObjectName((uint64_t)object, name, VK_OBJECT_TYPE_SHADER_MODULE); }
		inline void setObjectName(VkSwapchainKHR object, const std::string& name) { setObjectName((uint64_t)object, name, VK_OBJECT_TYPE_SWAPCHAIN_KHR); }

	public:
		//Windwo and basic components
		GLFWwindow*			m_window{ nullptr };
		VkInstance			m_instance{ VK_NULL_HANDLE };
		VkPhysicalDevice	m_physicalDevice{ VK_NULL_HANDLE };
		VkSurfaceKHR		m_surface{ VK_NULL_HANDLE };
		VkDevice			m_device{ VK_NULL_HANDLE };

		// Additional components
		VkAllocationCallbacks*		m_defaultAllocator{ nullptr };
		VkDebugUtilsMessengerEXT	m_debugMessenger{ VK_NULL_HANDLE };

		// Queue families and queues
		QueueFamilyIndices	m_queueFamilyIndices{};
		VkQueue				m_graphicsQueue;
		VkQueue				m_computeQueue;
		VkQueue				m_transferQueue;
		VkQueue				m_presentQueue;

		// Efficient function pointers
		PFN_vkSetDebugUtilsObjectNameEXT m_pfn_vkSetDebugUtilsObjectNameEXT;

		// Command pools, command buffers and sychronization objects
		VkCommandPool					m_commandPool{ VK_NULL_HANDLE };
		std::vector<VkCommandBuffer>	m_commandBuffers{};
		std::vector<VkSemaphore>		m_imageAvaliableForRenderSemaphore{};
		std::vector<VkSemaphore>		m_imageRenderFinishedForPresentSemaphores{};
		std::vector<VkFence>			m_imageInFlightFences{};
		uint8_t							m_maxFrameInFlight{ 3 };
		uint8_t							m_currentFrameIndex{ 0 };


	public:
		bool m_debugMode{ false };

		// Window settings
		uint32_t m_windowWidth{ 800 };
		uint32_t m_windowHeight{ 600 };
		const char* m_title{ "ExampleBase" };
		
		// Api settings
		uint32_t m_apiMajor{ 1 };
		uint32_t m_apiMinor{ 2 };
		uint32_t m_apiVersion{ VK_API_VERSION_1_2 };

		// Extensions and features
		std::vector<const char*>							m_instanceLayers{ "VK_LAYER_KHRONOS_validation" };
		bool												m_enableValidationLayers{ false };
		std::vector<const char*>							m_instanceExtensions{};
		std::vector<void* >									m_EXTPhysicalDeviceFeatureStructs{};
		std::map<VkStructureType, std::vector<const char*>> m_physicalDeviceFeatureRequirements{};
		std::vector<const char*>							m_deviceExtensions{};

		// Physical Device Features
		VkPhysicalDeviceFeatures2							m_physicalFeaturesStructChain{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 };
		VkPhysicalDeviceFeatures							m_features10{};
		VkPhysicalDeviceVulkan11Features					m_features11{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES };
		VkPhysicalDeviceVulkan12Features					m_features12{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES };
		VkPhysicalDeviceAccelerationStructureFeaturesKHR	m_accelFeature{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR };
		VkPhysicalDeviceRayTracingPipelineFeaturesKHR		m_rtPipelineFeature{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR };

		// Descriptor pool settings
		uint32_t m_maxVertexBlendingMeshCount{ 256 };
		uint32_t m_maxMaterialCount{ 256 };
	};
} // namespace PVulkanExamples