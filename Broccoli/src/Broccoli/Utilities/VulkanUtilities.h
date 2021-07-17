#pragma once

#include <vulkan/vulkan.h>

#include <vector>
#include <array>
#include <stdexcept>
#include <iostream>
#include <stdexcept>
#include <fstream>

namespace Broccoli {
	static uint32_t findMemoryTypeIndex(VkPhysicalDevice physicalDevice, uint32_t allowedTypes, VkMemoryPropertyFlags properties);


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

	static VkImage createImage(VkPhysicalDevice physicalDevice, VkDevice logicalDevice, uint32_t width, uint32_t height, uint32_t mipLevels, VkFormat format, VkImageTiling tiling, VkImageUsageFlags useFlags, VkMemoryPropertyFlags propFlags, VkDeviceMemory* imageMemory, VkSampleCountFlagBits numSamples)
	{
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

	static VkImageView createImageView(VkDevice logicalDevice, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
	{
		VkImageViewCreateInfo viewCreateInfo = {};
		viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewCreateInfo.image = image; // Image to create view for
		viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D; // Type of image (1D, 2D, 3D, Cube etc)
		viewCreateInfo.format = format; // Format of image data
		viewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY; // Allows remapping of rgba components to other rgba values
		viewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		// Subresources allow the view to view only a part of an images
		viewCreateInfo.subresourceRange.aspectMask = aspectFlags; // Which aspect of image to view (e.g. COLOR_BIT for viewing color)
		viewCreateInfo.subresourceRange.baseMipLevel = 0; // Start mipmap level to view from
		viewCreateInfo.subresourceRange.levelCount = 1; // Number of mipmap levels to view
		viewCreateInfo.subresourceRange.baseArrayLayer = 0; // Start array level to view from
		viewCreateInfo.subresourceRange.layerCount = 1; // How many array levels to view

		// Create image view and return it
		VkImageView imageView;
		VkResult result = vkCreateImageView(logicalDevice, &viewCreateInfo, nullptr, &imageView);
		if (result != VK_SUCCESS) {
			throw std::runtime_error("Failed to create an image view");
		}
		else {
			std::cout << "Created image view\n";
		}

		return imageView;
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

	static std::vector<char> readFile(const std::string& fileName) {
		// Open stream from given file
		// std::ios::binary tells stream to read as binary
		// std::ios::ate tells stream to start reading from end of file
		std::ifstream file(fileName, std::ios::binary | std::ios::ate);

		// Check if file stream successfully opened
		if (!file.is_open())
		{
			throw std::runtime_error("Failed to open a file");
		}

		size_t fileSize = (size_t)file.tellg(); // Get length of the file for creating vector
		std::vector<char> fileBuffer(fileSize); // Create vector with same size as file

		// Reset pointer to the start of file
		file.seekg(0);

		// Read the file data into the buffer (stream "fileSize" in total)  
		file.read(fileBuffer.data(), fileSize);

		// Close the stream
		file.close();

		return fileBuffer;
	}

	/*
	std::unordered_map<VkShaderStageFlagBits, std::string> VulkanShader::PreProcess(const std::string& source)
	{
		std::unordered_map<VkShaderStageFlagBits, std::string> shaderSources;

		const char* typeToken = "#type";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = source.find(typeToken, 0);
		while (pos != std::string::npos)
		{
			size_t eol = source.find_first_of("\r\n", pos);
			HZ_CORE_ASSERT(eol != std::string::npos, "Syntax error");
			size_t begin = pos + typeTokenLength + 1;
			std::string type = source.substr(begin, eol - begin);
			HZ_CORE_ASSERT(type == "vertex" || type == "fragment" || type == "pixel" || type == "compute", "Invalid shader type specified");

			size_t nextLinePos = source.find_first_not_of("\r\n", eol);
			pos = source.find(typeToken, nextLinePos);
			auto shaderType = ShaderTypeFromString(type);
			shaderSources[shaderType] = source.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));
		}

		return shaderSources;
	}*/

}