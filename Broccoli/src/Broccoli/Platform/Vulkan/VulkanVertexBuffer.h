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
		VulkanVertexBuffer(void* data, uint32_t size, VertexBufferUsage usage = VertexBufferUsage::Static);
		VulkanVertexBuffer(void* data, VertexBufferUsage usage = VertexBufferUsage::Dynamic);

		virtual void setData(void* buffer, uint32_t size, uint32_t offset = 0) override;
		virtual void bind() override;

		virtual unsigned int getSize() override;
	};
}