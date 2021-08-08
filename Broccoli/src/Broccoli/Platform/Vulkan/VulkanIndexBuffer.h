#pragma once

#include "Broccoli/Renderer/IndexBuffer.h"

#include "Broccoli/Core/Ref.h"

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