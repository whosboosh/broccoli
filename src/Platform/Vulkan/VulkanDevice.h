#ifndef BROCCOLI_VULKAN_DEVICE_HEADER
#define BROCCOLI_VULKAN_DEVICE_HEADER

#include "Utilities/VulkanUtilities.h"

#include <vulkan/vulkan.h>

// std
#include <vector>
#include <stdexcept>
#include <iostream>
#include <vector>

struct GLFWwindow;

namespace Broccoli {

	/*
	struct VulkanDeviceCollection {
		VulkanPhysicalDevice physicalDevice;
		VulkanLogicalDevice logicalDevice;
	};*/

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
	class VulkanPhysicalDevice
	{
	public:
		VulkanPhysicalDevice();
		~VulkanPhysicalDevice();

		bool checkDeviceSuitable(VkPhysicalDevice device);
		QueueFamilyIndicies getQueueFamilies(VkPhysicalDevice device);
		bool checkDeviceExtensionSupport(VkPhysicalDevice device, std::vector<const char*> deviceExtentions);

		// Getters
		VkPhysicalDevice getVulkanPhysicalDevice() const { return physicalDevice; }
		const QueueFamilyIndicies& getQueueFamilyIndicies() const { return deviceIndices; }
		const VkPhysicalDeviceProperties& getPhysicalDeviceProperties() const { return deviceProperties; }
		const VkPhysicalDeviceFeatures& getPhysicalDeviceFeatures() const { return deviceFeatures; }

		VkFormat getDepthFormat() const { return depthFormat; }

	private:
		VkPhysicalDevice physicalDevice = nullptr;
		VkDeviceSize minUniformBufferOffset;
		QueueFamilyIndicies deviceIndices;
		VkPhysicalDeviceProperties deviceProperties;
		VkPhysicalDeviceFeatures deviceFeatures;

		VkFormat depthFormat = VK_FORMAT_UNDEFINED;

		friend class VulkanLogicalDevice;
	};

	class VulkanLogicalDevice
	{
	public:
		VulkanLogicalDevice(VulkanPhysicalDevice* physicalDevice);
		~VulkanLogicalDevice();

		VkDevice getLogicalDevice() const { return logicalDevice; }
		const VulkanPhysicalDevice* getPhysicalDevice() const { return physicalDevice; }

		VkQueue getGraphicsQueue() const { return graphicsQueue; }
		VkQueue getPresentationQueue() const { return presentationQueue; }

		VkCommandPool getGraphicsCommandPool() const { return graphicsCommandPool; }

		void cleanup();
	private:
		VkDevice logicalDevice;
		VulkanPhysicalDevice* physicalDevice;

		VkQueue graphicsQueue;
		VkQueue presentationQueue;

		VkCommandPool graphicsCommandPool;

	};

}

#endif