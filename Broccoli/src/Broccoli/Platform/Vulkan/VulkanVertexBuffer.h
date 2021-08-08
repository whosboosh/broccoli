#pragma once

#include <stdint.h>

#include <iostream>
#include <string>

#include "Broccoli/Renderer/VertexBuffer.h"

#include "Broccoli/Core/Ref.h"

namespace Broccoli {

	class VulkanVertexBuffer : public VertexBuffer
	{
	public:
		VulkanVertexBuffer(std::vector<Vertex>* vertices, VertexBufferUsage usage);
		
		virtual void setData(void* buffer, uint32_t size, uint32_t offset = 0) override;
		virtual void bind() override;

		virtual unsigned int getSize() override;

		std::vector<Vertex>* getVertices() { return vertices; }

		virtual VkBuffer getVertexBuffer() override { return vertexBuffer; }

	private:
		std::vector<Vertex>* vertices;

		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;
	};
}