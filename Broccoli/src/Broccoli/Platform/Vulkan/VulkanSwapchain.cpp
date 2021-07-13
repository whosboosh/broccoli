#include "VulkanSwapchain.h"

namespace Broccoli {
	VulkanSwapchain::VulkanSwapchain()
	{
	}

	void VulkanSwapchain::init(VkInstance instance, const Ref<VulkanLogicalDevice>& logicalDevice)
	{
		this->instance = instance;
		this->logicalDevice = logicalDevice;
	}

	void VulkanSwapchain::create(uint32_t* width, uint32_t* height, bool vsync)
	{
		this->vsync = vsync;

		VkDevice device = logicalDevice->getLogicalDevice();
		VkPhysicalDevice physicalDevice = logicalDevice->getPhysicalDevice()->getVulkanPhysicalDevice();
	}
	VulkanSwapchain::~VulkanSwapchain()
	{
	}
}
