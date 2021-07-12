#pragma once

// Project dependencies
#include "Broccoli/Renderer/RendererContext.h"

#include "vulkan/vulkan.h"

#include "VulkanDevice.h"
#include "Broccoli/Utilities/VulkanInitializers.hpp"

namespace Broccoli {

	class VulkanContext : public RendererContext
	{
	public:
		VulkanContext();
		~VulkanContext();
	
		//Ref<VulkanLogicalDevice> getLogicalDevice() { return logicalDevice; };
		static VkInstance getInstance() { return mainInstance; };

		virtual void init() override;

	private:
		Ref<VulkanPhysicalDevice> physicalDevice;
		Ref<VulkanLogicalDevice> logicalDevice;

		inline static VkInstance mainInstance;
	};
}