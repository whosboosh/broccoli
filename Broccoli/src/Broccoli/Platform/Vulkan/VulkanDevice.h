#pragma once

#include "Broccoli/Core/Ref.h"

#include "Broccoli/Core/Ref.h"

#include <vulkan/vulkan.h>

// std
#include <vector>
#include <stdexcept>
#include <iostream>
#include <vector>

struct GLFWwindow;

namespace Broccoli {
	
	// Indicies (locations) of Queue Families (if they exist at all)
	struct QueueFamilyIndicies {
		int graphicsFamily = -1; // Location of Graphics Queue Family
		int presentationFamily = -1;
		bool isValid()
		{
			return graphicsFamily >= 0 && presentationFamily >= 0;
		}
	};

	// Specify device extensions we want to support
	const std::vector<const char*> deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
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
		const VkPhysicalDeviceFeatures& getPhysicalDeviceFeatures() const { return deviceFeatures; }

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
		VulkanLogicalDevice(const Ref<VulkanPhysicalDevice>& physicalDevice);
		~VulkanLogicalDevice();
	private:
		VkDevice logicalDevice;

		VkQueue graphicsQueue;
		VkQueue presentationQueue;

	};

}