#pragma once

#include "Broccoli/Renderer/RendererContext.h"

#include <iostream>
#include <vector>

#include <vulkan/vulkan.h>

namespace Broccoli {

	struct VulkanDevice {
		VkPhysicalDevice physicalDevice;
		VkDevice logicalDevice;
	};

	class VulkanContext : public RendererContext
	{
	public:
		VulkanContext();
		virtual ~VulkanContext();
	
		virtual VulkanDevice getDevice() { return mainDevice; };
		static VkInstance getInstance() { return mainInstance; };

		virtual void createInstance();

		virtual void init() override;

	private:
		VulkanDevice mainDevice;
		inline static VkInstance mainInstance;
	};
}