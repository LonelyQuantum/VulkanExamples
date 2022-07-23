#include "vulkan_example_base.h"
#include <iostream>

namespace PVulkanExamples
{
	class TriangleExample : public ExampleBase
	{

	};

} // namespace PVulkanExamples

int main()
{
	PVulkanExamples::TriangleExample example;
	example.init();
	example.cleanup();
	return 0;
}