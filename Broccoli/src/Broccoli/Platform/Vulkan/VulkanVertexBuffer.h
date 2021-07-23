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
		VulkanVertexBuffer(const std::vector<Vertex>& vertices, VertexBufferUsage usage);

		
		virtual void setData(void* buffer, uint32_t size, uint32_t offset = 0) override;
		virtual void bind() override;

		virtual unsigned int getSize() override;
	};
}