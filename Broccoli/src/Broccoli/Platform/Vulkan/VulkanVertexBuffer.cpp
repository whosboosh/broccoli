#pragma once

#include "VulkanVertexBuffer.h"

#include "Broccoli/Utilities/VulkanUtilities.h"

#include "Broccoli/Core/Application.h"
#include "Broccoli/Platform/Vulkan/VulkanContext.h"

#include <stdint.h>

namespace Broccoli {
	
	VulkanVertexBuffer::VulkanVertexBuffer(const std::vector<Vertex>& vertices, VertexBufferUsage usage)
	{
		std::cout << "Vertex buffer created!\n";
		VulkanLogicalDevice* device = Application::get().getWindow().getRenderContext().As<VulkanContext>()->getLogicalDevice();
		VkDevice logicalDevice = device->getLogicalDevice();
		VkPhysicalDevice physicalDevice = device->getPhysicalDevice()->getVulkanPhysicalDevice();

		VkQueue graphicsQueue = device->getGraphicsQueue();
		VkCommandPool graphicsCommandPool = device->getGraphicsCommandPool();

		// Create buffer
		VkDeviceSize bufferSize = sizeof(Vertex) * vertices.size();

		// Temporary buffer to "stage" vertex data before transferring to GPU
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;

		// Create staging buffer and Allocate Memory to it
		createBuffer(physicalDevice, logicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);

		// Map memory to staging buffer
		void* data; // Create a pointer to point in normal memory
		vkMapMemory(logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data); // Map the staging buffer memory to that point
		memcpy(data, vertices.data(), (size_t)bufferSize); // Copy memory from vertices vector to that point
		vkUnmapMemory(logicalDevice, stagingBufferMemory); // Unmap the staging buffer memory

		// Create buffer with TRANSFER_DST_BIT to mark as recipient of transfer data (also VERTEX_BUFFER)
		// Buffer memory is to be DEVICE_LOCAL_BIT meaning memory is on the GPU and only acessible by it and not the CPU (host)
		createBuffer(physicalDevice, logicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vertexBuffer, &vertexBufferMemory);

		copyBuffer(logicalDevice, graphicsQueue, graphicsCommandPool, stagingBuffer, vertexBuffer, bufferSize); // Copy staging buffer to vertex buffer on gpu

		// Cleanup staging buffer
		vkDestroyBuffer(logicalDevice, stagingBuffer, nullptr);
		vkFreeMemory(logicalDevice, stagingBufferMemory, nullptr);


	}
	void VulkanVertexBuffer::setData(void* buffer, uint32_t size, uint32_t offset)
	{
	}

	void VulkanVertexBuffer::bind()
	{
	}

	unsigned int VulkanVertexBuffer::getSize()
	{
		return 0;
	}

}