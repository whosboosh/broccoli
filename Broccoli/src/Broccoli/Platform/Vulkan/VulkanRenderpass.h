#pragma once

#include "Broccoli/Core/Ref.h"
#include "Broccoli/Platform/Vulkan/VulkanDevice.h"

// Vendor
#include <vulkan/vulkan.h>

#include <vector>
#include <array>

namespace Broccoli {

	class VulkanRenderpass
	{
	public:
		VulkanRenderpass(const Ref<VulkanLogicalDevice>& logicalDevice, VkFormat colourFormat);

		~VulkanRenderpass();

		VkRenderPass getRenderPass() const { return renderPass; }

	private:
		VkRenderPass renderPass;
	};
}