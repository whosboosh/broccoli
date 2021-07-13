#pragma once

// Project dependencies
#include "Broccoli/Utilities/VulkanInitializers.hpp"
#include "Broccoli/Platform/Vulkan/VulkanDevice.h"
#include "Broccoli/Core/Ref.h"

// Vendor
#include <vulkan/vulkan.h>

namespace Broccoli {

	class VulkanSwapchain
	{
	public:
		VulkanSwapchain();

		void init(VkInstance instance, const Ref<VulkanLogicalDevice>& logicalDevice);
		void create(uint32_t width, uint32_t height, bool vsync);

		~VulkanSwapchain();

	private:
		bool vsync;

		VkInstance instance;
		Ref<VulkanLogicalDevice> logicalDevice;
	};
}