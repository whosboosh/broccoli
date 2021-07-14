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
	
		//Ref<VulkanLogicalDevice> getLogicalDevice() { return logicalDevice; };
		//Ref<VulkanPhysicalDevice> getPhysicalDevice() { return physicalDevice; }

		VulkanDeviceCollection getDevices() { return deviceCollection; }

		static VkInstance getInstance() { return mainInstance; };
		static VkSurfaceKHR getSurface() { return surface; }

		virtual void init(GLFWwindow* windowHandle) override;

	private:
		//Ref<VulkanPhysicalDevice> physicalDevice;
		//Ref<VulkanLogicalDevice> logicalDevice;

		VulkanDeviceCollection deviceCollection;

		inline static VkInstance mainInstance;
		inline static VkSurfaceKHR surface;
	};
}