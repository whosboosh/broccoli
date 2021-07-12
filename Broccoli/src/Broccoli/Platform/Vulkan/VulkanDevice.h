#pragma once

#include "Broccoli/Core/Ref.h"

#include "Broccoli/Core/Ref.h"

#include <vulkan/vulkan.h>

// std
#include <vector>
#include <stdexcept>
#include <iostream>
#include <vector>

namespace Broccoli {
	
	// Indicies (locations) of Queue Families (if they exist at all)
	struct QueueFamilyIndicies {
		int graphicsFamily = -1; // Location of Graphics Queue Family
		bool isValid()
		{
			return graphicsFamily >= 0;
		}
	};

	// Provides functionality for creating vulkan device, logical and physical device, device features, queues and buffers
	class VulkanPhysicalDevice : public RefCounted
	{
	public:
		VulkanPhysicalDevice();
		~VulkanPhysicalDevice();

		static Ref<VulkanPhysicalDevice> selectDevice();
		bool checkDeviceSuitable(VkPhysicalDevice device);
		QueueFamilyIndicies getQueueFamilies(VkPhysicalDevice device);
		bool checkDeviceExtensionSupport(VkPhysicalDevice device, std::vector<const char*> deviceExtentions);

		// Getters
		VkPhysicalDevice getPhysicalDevice() const { return physicalDevice; }
		const QueueFamilyIndicies& getQueueFamilyIndicies() const { return deviceIndices; }
		const VkPhysicalDeviceProperties& getPhysicalDeviceProperties() const { return deviceProperties; }

	private:
		VkPhysicalDevice physicalDevice = nullptr;
		VkDeviceSize minUniformBufferOffset;
		QueueFamilyIndicies deviceIndices;
		VkPhysicalDeviceProperties deviceProperties;
		VkPhysicalDeviceFeatures deviceFeatures;

		friend class VulkanLogicalDevice;
	};

	class VulkanLogicalDevice : public RefCounted
	{
	public:
		VulkanLogicalDevice();
		~VulkanLogicalDevice();
	private:
		VkDevice logicalDevice;
	};

}