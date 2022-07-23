#include <vulkan/vulkan.h>
#include <string>
namespace PVulkanExamples
{
	class VulkanUtils
	{
	public:
		VulkanUtils() {};
		VulkanUtils(VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice device) :
			m_instance(instance), m_physicalDevice(physicalDevice), m_device(device) {};
		~VulkanUtils() {};

		void init();

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
		// Basic settings
		VkInstance			m_instance;
		VkPhysicalDevice	m_physicalDevice;
		VkDevice			m_device;

		// Efficient function pointers
		PFN_vkSetDebugUtilsObjectNameEXT m_pfn_vkSetDebugUtilsObjectNameEXT;
	};
} // namespace PVulkanExmaples