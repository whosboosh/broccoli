#pragma once

#include <stdint.h>

#include <iostream>
#include <string>

#include "Broccoli/Core/Ref.h"

namespace Broccoli {
	
	enum class VertexBufferUsage
	{
		None = 0,
		Static = 1,
		Dynamic = 2
	};

	class VertexBuffer : public RefCounted
	{
	public:
		virtual void setData(void* buffer, uint32_t size, uint32_t offset = 0) = 0;
		virtual void bind() = 0;

		virtual unsigned int getSize() = 0;

		static Ref<VertexBuffer> create(void* data, uint32_t size, VertexBufferUsage usage = VertexBufferUsage::Static);
		static Ref<VertexBuffer> create(void* data, VertexBufferUsage usage = VertexBufferUsage::Dynamic);
	};
}