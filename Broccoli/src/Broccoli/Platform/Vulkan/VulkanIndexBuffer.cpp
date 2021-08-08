#pragma once

#include "VulkanIndexBuffer.h"

#include "Broccoli/Core/Application.h"
#include "Broccoli/Platform/Vulkan/VulkanContext.h"

#include <stdint.h>

namespace Broccoli {
	VulkanIndexBuffer::VulkanIndexBuffer(std::vector<uint32_t>* indices)
	{
		VulkanLogicalDevice* device = VulkanContext::get()->getLogicalDevice();
		VkDevice logicalDevice = device->getLogicalDevice();
		VkPhysicalDevice physicalDevice = device->getPhysicalDevice()->getVulkanPhysicalDevice();

		VkQueue graphicsQueue = device->getGraphicsQueue();
		VkCommandPool graphicsCommandPool = device->getGraphicsCommandPool();

		VkDeviceSize bufferSize = sizeof(uint32_t) * indices->size(); // Get size of buffer needed for indicies

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;

		createBuffer(physicalDevice, logicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);

		// Map memory to staging buffer
		void* data;
		vkMapMemory(logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, indices->data(), (size_t)bufferSize);
		vkUnmapMemory(logicalDevice, stagingBufferMemory);

		// Create buffer for index data on gpu access only area
		createBuffer(physicalDevice, logicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &indexBuffer, &indexBufferMemory);

		// Copy from staging buffer to gpu access buffer
		copyBuffer(logicalDevice, graphicsQueue, graphicsCommandPool, stagingBuffer, indexBuffer, bufferSize);

		// Destroy and release staging buffer resources
		vkDestroyBuffer(logicalDevice, stagingBuffer, nullptr);
		vkFreeMemory(logicalDevice, stagingBufferMemory, nullptr);

		std::cout << "Index buffer created!\n";
	}
	void VulkanIndexBuffer::bind()
	{
	}
	unsigned int VulkanIndexBuffer::getSize()
	{
		return 0;
	}
}