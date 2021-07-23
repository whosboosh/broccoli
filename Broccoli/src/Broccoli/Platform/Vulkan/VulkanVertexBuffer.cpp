#pragma once

#include "VulkanVertexBuffer.h"

#include <stdint.h>

namespace Broccoli {
	
	VulkanVertexBuffer::VulkanVertexBuffer(const std::vector<Vertex>& vertices, VertexBufferUsage usage)
	{
		std::cout << "Vertex buffer created!\n";
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