#pragma once

// Project dependencies
#include "Broccoli/Utilities/VulkanInitializers.hpp"
#include "Broccoli/Platform/Vulkan/VulkanDevice.h"
#include "Broccoli/Core/Ref.h"

// Vendor
#include <vulkan/vulkan.h>

#include <vector>

namespace Broccoli {

	struct SwapChainDetails {
		VkSurfaceCapabilitiesKHR surfaceCapabilities; // Surface properties (e.g. image size/extent)
		std::vector<VkSurfaceFormatKHR> formats;	  // Surface Image formats (e.g. RGBA and size of each colour)
		std::vector<VkPresentModeKHR> presentationMode; // How images should be presented to screen
	};

	class VulkanSwapchain
	{
	public:
		VulkanSwapchain();

		void init(VkInstance instance, const Ref<VulkanLogicalDevice>& logicalDevice, VkSurfaceKHR surface);
		void create(uint32_t* width, uint32_t* height, bool vsync);

		SwapChainDetails getSwapchainDetails(VkPhysicalDevice physicalDevice);
		VkSurfaceFormatKHR chooseBestSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats);
		VkPresentModeKHR chooseBestPresentationMode(const std::vector<VkPresentModeKHR>& presentationModes);
		VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities, uint32_t* width, uint32_t* height);

		~VulkanSwapchain();

	private:
		bool vsync;

		VkSurfaceKHR surface;
		VkInstance instance;
		Ref<VulkanLogicalDevice> logicalDevice;

		VkExtent2D swapChainExtent;
	};
}