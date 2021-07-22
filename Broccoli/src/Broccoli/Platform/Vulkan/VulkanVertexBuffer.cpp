#pragma once

#include "VulkanVertexBuffer.h"

#include <stdint.h>

namespace Broccoli {
	VulkanVertexBuffer::VulkanVertexBuffer(void* data, uint32_t size, VertexBufferUsage usage)
	{
	}
	VulkanVertexBuffer::VulkanVertexBuffer(void* data, VertexBufferUsage usage)
	{
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