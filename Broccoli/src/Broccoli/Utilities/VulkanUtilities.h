#pragma once

#include "Broccoli/Renderer/Pipeline.h"

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

	static VkPrimitiveTopology GetVulkanTopology(PrimitiveTopology topology)
	{
		switch (topology)
		{
		case PrimitiveTopology::Points:			return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
		case PrimitiveTopology::Lines:			return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
		case PrimitiveTopology::Triangles:		return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		case PrimitiveTopology::LineStrip:		return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
		case PrimitiveTopology::TriangleStrip:	return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
		case PrimitiveTopology::TriangleFan:	return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
		}

		std::cout << "Unknown toplogy\n";
		return VK_PRIMITIVE_TOPOLOGY_MAX_ENUM;
	}

	static void createBuffer(VkPhysicalDevice physicalDevice, VkDevice device, VkDeviceSize bufferSize, VkBufferUsageFlags bufferUsage,
		VkMemoryPropertyFlags bufferProperties, VkBuffer* buffer, VkDeviceMemory* bufferMemory)
	{
		VkBufferCreateInfo bufferCreateInfo = {};
		bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferCreateInfo.size = bufferSize; // Size of one vertex * number of vertices
		bufferCreateInfo.usage = bufferUsage; // Multiple types of buffer are possible, use vertex buffer
		bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // Similar to swapchain images can share vertex buffers

		VkResult result = vkCreateBuffer(device, &bufferCreateInfo, nullptr, buffer);
		if (result != VK_SUCCESS) {
			throw std::runtime_error("Failed to create a vertex buffer");
		}

		// Get buffer memory requirements
		VkMemoryRequirements memoryRequirements;
		vkGetBufferMemoryRequirements(device, *buffer, &memoryRequirements);

		// Allocate memory to buffer
		VkMemoryAllocateInfo memoryAllocInfo = {};
		memoryAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memoryAllocInfo.allocationSize = memoryRequirements.size;
		memoryAllocInfo.memoryTypeIndex = findMemoryTypeIndex(physicalDevice, memoryRequirements.memoryTypeBits, // Index of memory type on physical device that has required bit flags
			bufferProperties); // VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT: The buffer is visible to the CPU 
							   // VK_MEMORY_PROPERTY_HOST_COHERENT_BIT: Allows placement of data straight into buffer after mapping (otherwise would have to specify manually)
							   // VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT: Memory is optimised for GPU usage, only accessible by the gpu and not the host cpu. Can only be interacted by commands

		// Allocate memory to VkDeviceMemory
		result = vkAllocateMemory(device, &memoryAllocInfo, nullptr, bufferMemory);
		if (result != VK_SUCCESS) {
			throw std::runtime_error("Failed to allocate memory for vertex buffer");
		}

		// Allocate memory to given vertex buffer
		vkBindBufferMemory(device, *buffer, *bufferMemory, 0);
	}

	static VkCommandBuffer beginCommandBuffer(VkDevice device, VkCommandPool commandPool) {
		// Command buffer to hold transfer commands
		VkCommandBuffer commandBuffer;

		// Command buffer details
		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandBufferCount = 1;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = commandPool;

		// Allocate command buffer from pool
		vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

		// Information to begin the command buffer record
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT; // We're only using the command buffer once, so set up for one time submit

		// Begin recording transfer commands
		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		return commandBuffer;
	}

	static void endAndSubmitCommandBuffer(VkDevice device, VkCommandPool commandPool, VkQueue queue, VkCommandBuffer commandBuffer) {
		// End commands
		vkEndCommandBuffer(commandBuffer);

		// Queue submission information
		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		// Submit transfer command to transfer queue and wait until finishes
		vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(queue);

		// Free the temporary command buffer back to pool
		vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
	}

	static void copyBuffer(VkDevice device, VkQueue transferQueue, VkCommandPool transferCommandPool, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize bufferSize)
	{
		VkCommandBuffer transferCommandBuffer = beginCommandBuffer(device, transferCommandPool);

		// Region of data to copy from and to
		VkBufferCopy bufferCopyRegion = {};
		bufferCopyRegion.srcOffset = 0;
		bufferCopyRegion.dstOffset = 0;
		bufferCopyRegion.size = bufferSize;

		// Command to copy src buffer to dst buffer
		vkCmdCopyBuffer(transferCommandBuffer, srcBuffer, dstBuffer, 1, &bufferCopyRegion);

		endAndSubmitCommandBuffer(device, transferCommandPool, transferQueue, transferCommandBuffer);
	}

	static void copyImageBuffer(VkDevice device, VkQueue transferQueue, VkCommandPool transferCommandPool, VkBuffer srcBuffer, VkImage image, uint32_t width, uint32_t height) {
		// Create buffer
		VkCommandBuffer transferCommandBuffer = beginCommandBuffer(device, transferCommandPool);

		VkBufferImageCopy imageRegion = {};
		imageRegion.bufferOffset = 0; // Offset into data
		imageRegion.bufferRowLength = 0; // Row length of data to calculate data spacing
		imageRegion.bufferImageHeight = 0; // Image height to calculate data spacing (tightly packed as 0)
		imageRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT; // Which aspect of image to copy
		imageRegion.imageSubresource.mipLevel = 0; // MIPMAP level to copy
		imageRegion.imageSubresource.layerCount = 1; // Number of layers to copy starting at baseArrayLayer
		imageRegion.imageOffset = { 0,0,0 }; // Offset into image as opposed to raw data in bufferOffset
		imageRegion.imageExtent = { width, height, 1 }; // Size of region to copy (x, y, z) values

		vkCmdCopyBufferToImage(transferCommandBuffer, srcBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageRegion);

		endAndSubmitCommandBuffer(device, transferCommandPool, transferQueue, transferCommandBuffer);
	}

	static void generateMipmaps(VkDevice logicalDevice, VkPhysicalDevice physicalDevice, VkQueue queue, VkCommandPool commandPool, VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels) {
		// Check if image format supports linear blitting
		VkFormatProperties formatProperties;
		vkGetPhysicalDeviceFormatProperties(physicalDevice, imageFormat, &formatProperties);

		if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
			throw std::runtime_error("texture image format does not support linear blitting!");
		}

		VkCommandBuffer commandBuffer = beginCommandBuffer(logicalDevice, commandPool);

		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.image = image;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		barrier.subresourceRange.levelCount = 1;

		int32_t mipWidth = texWidth;
		int32_t mipHeight = texHeight;

		for (uint32_t i = 1; i < mipLevels; i++) {
			barrier.subresourceRange.baseMipLevel = i - 1;
			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

			vkCmdPipelineBarrier(commandBuffer,
				VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
				0, nullptr,
				0, nullptr,
				1, &barrier);

			VkImageBlit blit{};
			blit.srcOffsets[0] = { 0, 0, 0 };
			blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
			blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.srcSubresource.mipLevel = i - 1;
			blit.srcSubresource.baseArrayLayer = 0;
			blit.srcSubresource.layerCount = 1;
			blit.dstOffsets[0] = { 0, 0, 0 };
			blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
			blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.dstSubresource.mipLevel = i;
			blit.dstSubresource.baseArrayLayer = 0;
			blit.dstSubresource.layerCount = 1;

			vkCmdBlitImage(commandBuffer,
				image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				1, &blit,
				VK_FILTER_LINEAR);

			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			vkCmdPipelineBarrier(commandBuffer,
				VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
				0, nullptr,
				0, nullptr,
				1, &barrier);

			if (mipWidth > 1) mipWidth /= 2;
			if (mipHeight > 1) mipHeight /= 2;
		}

		barrier.subresourceRange.baseMipLevel = mipLevels - 1;
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(commandBuffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
			0, nullptr,
			0, nullptr,
			1, &barrier);

		endAndSubmitCommandBuffer(logicalDevice, commandPool, queue, commandBuffer);
	}


	static void transitionImageLayout(VkDevice device, VkQueue queue, VkCommandPool commandPool, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels) {

		// Create Buffer
		VkCommandBuffer commandBuffer = beginCommandBuffer(device, commandPool);

		VkImageMemoryBarrier imageMemoryBarrier = {};
		imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		imageMemoryBarrier.oldLayout = oldLayout; // Layout to transition from
		imageMemoryBarrier.newLayout = newLayout; // Layout to transition to
		imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED; // Queue family to transition to
		imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED; // // From
		imageMemoryBarrier.image = image;
		imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT; // Aspect of image being altered
		imageMemoryBarrier.subresourceRange.baseMipLevel = 0; // First mip level to start alteration on
		imageMemoryBarrier.subresourceRange.layerCount = 1; // Number of layers ot alter starting from baseArrayLayer
		imageMemoryBarrier.subresourceRange.levelCount = mipLevels; // Number of muip levels to alter starting from mipBaseLevel
		imageMemoryBarrier.subresourceRange.baseArrayLayer = 0; // First layer to start alterations on

		VkPipelineStageFlags srcStage;
		VkPipelineStageFlags dstStage;

		// If transitioning from new image to image ready to receive data
		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
			imageMemoryBarrier.srcAccessMask = 0; // Memory access stage transition must after
			imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT; // Memory access stage must before

			srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		// If transitioning from transfer destination to shader readable
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}

		vkCmdPipelineBarrier(
			commandBuffer,
			srcStage, dstStage,  // Pipeline stages (match to src and dst access masks)
			0, // Dependancy flags
			0, nullptr, // Memory barrier count + data
			0, nullptr, // Buffer memory barrier count + data
			1, &imageMemoryBarrier // Image memory barrier count + data
		);

		endAndSubmitCommandBuffer(device, commandPool, queue, commandBuffer);
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