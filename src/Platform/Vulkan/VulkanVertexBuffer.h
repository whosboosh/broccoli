#ifndef BROCCOLI_VULKAN_VERTEX_BUFFER
#define BROCCOLI_VULKAN_VERTEX_BUFFER

#include <stdint.h>

#include <iostream>
#include <string>

#include "Renderer/VertexBuffer.h"

#include "Core/Ref.h"

namespace Broccoli {

	class VulkanVertexBuffer : public VertexBuffer
	{
	public:
		VulkanVertexBuffer(std::vector<Vertex> vertices, VertexBufferUsage usage);
		
		virtual void setData(void* buffer, uint32_t size, uint32_t offset = 0) override;
		virtual void bind() override;

		virtual unsigned int getSize() override;

		virtual VkBuffer getVertexBuffer() override { return vertexBuffer; }

	private:
		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;
	};
}

#endif