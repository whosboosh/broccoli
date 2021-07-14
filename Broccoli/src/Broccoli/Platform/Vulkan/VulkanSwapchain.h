#pragma once

// Project dependencies
#include "Broccoli/Utilities/VulkanInitializers.hpp"
#include "Broccoli/Utilities/VulkanUtilities.h"
#include "Broccoli/Platform/Vulkan/VulkanDevice.h"
#include "Broccoli/Core/Ref.h"

#include "VulkanFramebuffer.h"
#include "VulkanRenderpass.h"

// Vendor
#include <vulkan/vulkan.h>

#include <vector>

namespace Broccoli {

	// TODO: Move this to a global renderer config struct
	const int MAX_FRAME_DRAWS = 2;

	struct SwapChainDetails {
		VkSurfaceCapabilitiesKHR surfaceCapabilities; // Surface properties (e.g. image size/extent)
		std::vector<VkSurfaceFormatKHR> formats;	  // Surface Image formats (e.g. RGBA and size of each colour)
		std::vector<VkPresentModeKHR> presentationMode; // How images should be presented to screen
	};

	struct SwapChainImage {
		VkImage image;
		VkImageView imageView;
	};

	class VulkanSwapchain
	{
	public:
		VulkanSwapchain();

		void init(VkInstance instance, VulkanLogicalDevice* logicalDevice, VkSurfaceKHR surface);
		void create(uint32_t* width, uint32_t* height, bool vsync);

		SwapChainDetails getSwapchainDetails(VkPhysicalDevice physicalDevice);
		VkSurfaceFormatKHR chooseBestSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats);
		VkPresentModeKHR chooseBestPresentationMode(const std::vector<VkPresentModeKHR>& presentationModes);
		VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities, uint32_t* width, uint32_t* height);

		void createDepthStencil();
		void createSynchronisation();

		~VulkanSwapchain();

	private:
		bool vsync;

		struct
		{
			VkImage image;
			VkDeviceMemory imageMemory;
			VkImageView imageView;
		} depthStencil;

		VkSurfaceKHR surface;
		VkInstance instance;

		VulkanLogicalDevice* logicalDevice;

		VkDevice getLogicalDevice() { return logicalDevice->getLogicalDevice(); }
		VkPhysicalDevice getPhysicalDevice() { return logicalDevice->getPhysicalDevice()->getVulkanPhysicalDevice(); }

		VkSwapchainKHR swapChain = VK_NULL_HANDLE;
		uint32_t swapChainImageCount = 0;
		std::vector<SwapChainImage> swapChainImages;

		VkExtent2D swapChainExtent;

		VkCommandPool commandPool;
		std::vector<VkCommandBuffer> commandBuffers;

		std::vector<VkSemaphore> imageAvailable;
		std::vector<VkSemaphore> renderFinished;
		std::vector<VkFence> drawFences;

		std::vector<VkFramebuffer> swapChainFramebuffers;

		VulkanRenderpass* renderPass;
		VulkanFramebuffer* framebuffer;
	};
}