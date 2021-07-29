#pragma once

#include "Broccoli/Platform/Vulkan/VulkanDevice.h"

// Vendor
#include <vulkan/vulkan.h>

#include <vector>
#include <array>

namespace Broccoli {

	class VulkanRenderpass
	{
	public:
		VulkanRenderpass(VulkanLogicalDevice* logicalDevice, VkFormat colourFormat);

		~VulkanRenderpass();

		void cleanup();

		VkRenderPass getRenderPass() const { return renderPass; }

	private:
		VkRenderPass renderPass;

		VulkanLogicalDevice* logicalDevice;
	};
}