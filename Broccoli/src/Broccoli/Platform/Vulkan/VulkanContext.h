#pragma once

// Project dependencies
#include "Broccoli/Renderer/RendererContext.h"

#include "vulkan/vulkan.h"

#include "VulkanDevice.h"
#include "Broccoli/Utilities/VulkanInitializers.hpp"
#include "Broccoli/Renderer/Renderer.h"

#include "Broccoli/Platform/Vulkan/VulkanSwapchain.h"

// Vendor
#include <vulkan/vulkan.h>

struct GLFWwindow;

namespace Broccoli {

	class VulkanContext : public RendererContext
	{
	public:
		VulkanContext();
		~VulkanContext();
	
		VulkanLogicalDevice* getLogicalDevice() { return logicalDevice; };
		VulkanPhysicalDevice* getPhysicalDevice() { return physicalDevice; }

		VulkanSwapchain& getVulkanSwapChain() { return swapChain; }

		static Ref<VulkanContext> get() { return Ref<VulkanContext>(Renderer::getContext()); }

		VkInstance getInstance() { return mainInstance; };
		VkSurfaceKHR& getSurface() { return surface; }

		virtual void init(GLFWwindow* windowHandle) override;

	private:
		VulkanPhysicalDevice* physicalDevice;
		VulkanLogicalDevice* logicalDevice;

		VulkanSwapchain swapChain;

		VkInstance mainInstance;
		VkSurfaceKHR surface;
	};
}