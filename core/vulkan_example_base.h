#include <vulkan/vulkan.h>
#include <vulkan_utils.h>
#include <GLFW/glfw3.h>


namespace PVulkanExamples
{
	class ExampleBase
	{
	public:
		ExampleBase() {};
		~ExampleBase() {};

		void init();
		void createInstance();
		void createDebugMessenger();
		void createSurface();
		void createPhysicalDevice();
		void createLogicalDevice();
		void initializeCommandPools();
		void initializeCommandBuffers();
		void createDescriptorPools();
		void createSyncObjects();

	public:
		GLFWwindow* m_window{ nullptr };

		VkInstance			m_instance{ VK_NULL_HANDLE };
		VkPhysicalDevice	m_physicalDevice{ VK_NULL_HANDLE };
		VkSurfaceKHR		m_surface{ VK_NULL_HANDLE };
		VkDevice			m_device{ VK_NULL_HANDLE };


	public:
		// Settings
		uint32_t m_vkApiVersion{ VK_API_VERSION_1_2 };

	private:
		VulkanUtils vkUtils{};
	};
} // namespace PVulkanExamples