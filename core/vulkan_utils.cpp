#include "vulkan_utils.h"

namespace PVulkanExamples
{
    /*
    * Initialize function pointers
    */
    void VulkanUtils::init()
    {
        m_pfn_vkSetDebugUtilsObjectNameEXT = (PFN_vkSetDebugUtilsObjectNameEXT)vkGetInstanceProcAddr(m_instance, "vkSetDebugUtilsObjectNameEXT");
    }

    /*
    * Set debug name of given object
    */
    void VulkanUtils::setObjectName(const uint64_t object, const std::string& name, VkObjectType t)
    {
        VkDebugUtilsObjectNameInfoEXT s{ VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT, nullptr, t, object, name.c_str() };
        m_pfn_vkSetDebugUtilsObjectNameEXT(m_device, &s);
    }
} // namespace PVulkanExamples