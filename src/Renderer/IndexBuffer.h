#ifndef BROCCOLI_INDEX_BUFFER_HEADER
#define BROCCOLI_INDEX_BUFFER_HEADER

#include <stdint.h>

#include <iostream>
#include <string>


#include <vulkan/vulkan.h>

#include "Core/Ref.h"

#include <vector>

namespace Broccoli {

	class IndexBuffer : public RefCounted
	{
	public:
		virtual ~IndexBuffer() {}

		virtual void bind() = 0;
		virtual unsigned int getSize() = 0;

		static Ref<IndexBuffer> create(std::vector<uint32_t>* indices);

		// Vulkan Specific
		virtual VkBuffer getIndexBuffer() = 0;
	};
}

#endif