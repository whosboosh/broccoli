#ifndef BROCCOLI_VULKAN_CONTEXT_HEADER
#define BROCCOLI_VULKAN_CONTEXT_HEADER

// Project dependencies
#include "Renderer/RendererContext.h"

#include "VulkanDevice.h"
#include "Utilities/VulkanInitializers.hpp"
#include "Renderer/Renderer.h"

#include "Platform/Vulkan/VulkanSwapchain.h"

// Vendor
#include <vulkan/vulkan.h>

struct GLFWwindow;

namespace Broccoli {

	class VulkanContext : public RendererContext
	{
	public:
		VulkanContext();
		~VulkanContext();
		void cleanup();

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

#endif