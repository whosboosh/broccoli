#pragma once

// Project dependencies
#include "Broccoli/Renderer/RendererContext.h"

#include "vulkan/vulkan.h"

#include "VulkanDevice.h"
#include "Broccoli/Utilities/VulkanInitializers.hpp"

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

		static VkInstance getInstance() { return mainInstance; };
		static VkSurfaceKHR getSurface() { return surface; }

		virtual void init(GLFWwindow* windowHandle) override;

	private:
		VulkanPhysicalDevice* physicalDevice;
		VulkanLogicalDevice* logicalDevice;

		inline static VkInstance mainInstance;
		inline static VkSurfaceKHR surface;
	};
}