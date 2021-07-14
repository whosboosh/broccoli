#pragma once

#include <vulkan/vulkan.h>

#include <vector>
#include <array>
#include <stdexcept>

#include "Broccoli/Core/Ref.h"
#include "Broccoli/Platform/Vulkan/VulkanDevice.h"

namespace Broccoli {
	static VkFormat chooseSupportedFormat(VkPhysicalDevice vulkanDevice, const std::vector<VkFormat>& formats, VkImageTiling tiling, VkFormatFeatureFlags featureFlags)
	{
		// Loop through all formats and find a compatible one
		for (VkFormat format : formats) {
			// Get properties for given format on this device
			VkFormatProperties properties;
			vkGetPhysicalDeviceFormatProperties(vulkanDevice, format, &properties);

			if (tiling == VK_IMAGE_TILING_LINEAR && (properties.linearTilingFeatures & featureFlags) == featureFlags) {
				return format;
			}
			else if (tiling == VK_IMAGE_TILING_OPTIMAL && (properties.optimalTilingFeatures & featureFlags) == featureFlags) {
				return format;
			}
		}

		throw std::runtime_error("Failed to find a matching format!");
	}

	static VkImage createImage(VulkanDeviceCollection* deviceCollection, uint32_t width, uint32_t height, uint32_t mipLevels, VkFormat format, VkImageTiling tiling, VkImageUsageFlags useFlags, VkMemoryPropertyFlags propFlags, VkDeviceMemory* imageMemory, VkSampleCountFlagBits numSamples)
	{
		VkDevice logicalDevice = deviceCollection->logicalDevice->getLogicalDevice();
		VkPhysicalDevice physicalDevice = deviceCollection->physicalDevice->getVulkanPhysicalDevice();

		// CREATE IMAGE
		VkImageCreateInfo imageCreateInfo = {};
		imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageCreateInfo.imageType = VK_IMAGE_TYPE_2D; // Type of image (1D, 2D, 3D)
		imageCreateInfo.extent.width = width;
		imageCreateInfo.extent.height = height;
		imageCreateInfo.extent.depth = 1; // Depth of image (just 1, no 3D aspect)
		imageCreateInfo.mipLevels = mipLevels; // Number of mipmap levels
		imageCreateInfo.arrayLayers = 1; // Number of levels in image array
		imageCreateInfo.format = format; // Format type of image (depth format, colour etc)
		imageCreateInfo.tiling = tiling; // How image data should be tiled, (arranged for optimal reading)
		imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; // Layout of image data on creation
		imageCreateInfo.usage = useFlags; // Being used as a depth buffer attachment (Bit flag defining what image will be used for)
		imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // Whether image can be shared between queues
		imageCreateInfo.samples = numSamples;


		VkImage image;
		VkResult result = vkCreateImage(logicalDevice, &imageCreateInfo, nullptr, &image);
		if (result != VK_SUCCESS) {
			throw std::runtime_error("Failed to create an image!");
		}

		// CREATE MEMORY FOR IMAGE

		// Get memory requirements for a type of image
		VkMemoryRequirements memoryRequirements;
		vkGetImageMemoryRequirements(logicalDevice, image, &memoryRequirements);

		// Allocate memory using image requirements and user defined properties
		VkMemoryAllocateInfo memoryAllocInfo = {};
		memoryAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memoryAllocInfo.allocationSize = memoryRequirements.size;
		memoryAllocInfo.memoryTypeIndex = findMemoryTypeIndex(physicalDevice, memoryRequirements.memoryTypeBits, propFlags);

		result = vkAllocateMemory(logicalDevice, &memoryAllocInfo, nullptr, imageMemory);
		if (result != VK_SUCCESS) {
			throw std::runtime_error("Failed to allocate memory for image!");
		}

		// Connect memory to image
		vkBindImageMemory(logicalDevice, image, *imageMemory, 0);

		return image;
	}

	static uint32_t findMemoryTypeIndex(VkPhysicalDevice physicalDevice, uint32_t allowedTypes, VkMemoryPropertyFlags properties)
	{
		// Get properties of physical device memory
		VkPhysicalDeviceMemoryProperties memoryProperties;
		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

		for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++) {
			if ((allowedTypes & (1 << i)) // Index of memory type must match corresponding bit in allowedTypes
				&& (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties) // Desired property bit flags are part of memory type's property flags
			{
				return i; // Memory type is valid, return index 
			}
		}
	}

	static VkSampleCountFlagBits translateSampleIntToEnum(int level)
	{
		VkSampleCountFlagBits temp;
		if (level % 2 == 0 || level == 1) {
			std::cout << level;
			if (level == 1) temp = VK_SAMPLE_COUNT_1_BIT;
			if (level == 2) temp = VK_SAMPLE_COUNT_2_BIT;
			if (level == 4) temp = VK_SAMPLE_COUNT_4_BIT;
			if (level == 8) temp = VK_SAMPLE_COUNT_8_BIT;
		}
		return temp;
	}

}