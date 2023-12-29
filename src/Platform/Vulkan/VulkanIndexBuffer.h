#ifndef BROCCOLI_VULKAN_INDEX_BUFFER_HEADER
#define BROCCOLI_VULKAN_INDEX_BUFFER_HEADER

#include "Renderer/IndexBuffer.h"

#include "Core/Ref.h"

#include <stdint.h>
#include <iostream>
#include <string>

namespace Broccoli {

	class VulkanIndexBuffer : public IndexBuffer
	{
	public:
		VulkanIndexBuffer(std::vector<uint32_t>* indices);

		virtual void bind() override;

		virtual unsigned int getSize() override;

		virtual VkBuffer getIndexBuffer() override { return indexBuffer; };

	private:
		VkBuffer indexBuffer;
		VkDeviceMemory indexBufferMemory;
	};
}

#endif