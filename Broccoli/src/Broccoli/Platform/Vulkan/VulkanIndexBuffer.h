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
		VulkanIndexBuffer(const std::vector<uint32_t>& indices);

		virtual void bind() override;

		virtual unsigned int getSize() override;
	};
}