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

		void init(VkInstance instance, VulkanLogicalDevice* logicalDevice, bool vsync);
		void create(bool vsync);

		VkSwapchainKHR& getSwapChain() { return swapChain; }

		static SwapChainDetails getSwapchainDetails(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);

		VkSurfaceFormatKHR chooseBestSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats);
		VkPresentModeKHR chooseBestPresentationMode(const std::vector<VkPresentModeKHR>& presentationModes);
		VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities);

		std::vector<VkCommandBuffer> getCommandBuffers() { return commandBuffers; }
		VulkanRenderpass& getRenderPass() { return *renderPass; }
		std::vector<VkFramebuffer> getSwapChainFrameBuffers() { return swapChainFramebuffers; }
		VkExtent2D getSwapExtent() { return swapChainExtent; }

		uint32_t getSwapChainImageCount() { return swapChainImageCount; }

		void acquireNextImage() { vkAcquireNextImageKHR(getLogicalDevice(), swapChain, std::numeric_limits<uint64_t>::max(), imageAvailable[currentFrame], VK_NULL_HANDLE, &imageIndex); }

		void incrementCurrentFrame() { currentFrame = (currentFrame + 1) % MAX_FRAME_DRAWS; }

		void waitForFrences();

		VkFence& getCurrentDrawFence()
		{
			return drawFences[currentFrame];
		}

		VkSemaphore& getCurrentImageAvailableSemaphore()
		{
			return imageAvailable[currentFrame];
		}
		VkSemaphore& getCurrentRenderFinishedSemaphore()
		{
			return renderFinished[currentFrame];
		}

		VkCommandBuffer getCurrentCommandBuffer()
		{
			return commandBuffers[imageIndex];
		}

		VkFramebuffer getCurrentFrameBuffer()
		{ 
			return swapChainFramebuffers[imageIndex];
			//if (imageIndex < swapChainFramebuffers.size()) 	
			//else throw std::runtime_error("Referenced Framebuffer is out of bounds!");
		}

		uint32_t& getCurrentImageIndex() { return imageIndex; }

		void createDepthStencil();
		void createSynchronisation();

		void cleanup();
		void recreateSwapChain();

		~VulkanSwapchain();

	private:
		bool vsync;

		struct
		{
			VkImage image;
			VkDeviceMemory imageMemory;
			VkImageView imageView;
		} depthStencil;

		VkInstance instance;

		VkSurfaceFormatKHR surfaceFormat;

		VulkanLogicalDevice* logicalDevice;

		VkDevice getLogicalDevice() { return logicalDevice->getLogicalDevice(); }
		VkPhysicalDevice getPhysicalDevice() { return logicalDevice->getPhysicalDevice()->getVulkanPhysicalDevice(); }

		VkSwapchainKHR swapChain = VK_NULL_HANDLE;
		uint32_t swapChainImageCount = 0;
		std::vector<SwapChainImage> swapChainImages;

		VkExtent2D swapChainExtent;

		std::vector<VkCommandBuffer> commandBuffers;

		std::vector<VkSemaphore> imageAvailable;
		std::vector<VkSemaphore> renderFinished;
		std::vector<VkFence> drawFences;

		std::vector<VkFramebuffer> swapChainFramebuffers;

		uint32_t imageIndex = 0;
		int currentFrame = 0;

		VulkanRenderpass* renderPass;
	};
}