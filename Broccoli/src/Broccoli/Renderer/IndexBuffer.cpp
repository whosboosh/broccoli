#pragma once

#include "IndexBuffer.h"

#include <stdint.h>
#include <iostream>
#include <string>

#include "Broccoli/Platform/Vulkan/VulkanIndexBuffer.h"

#include "Broccoli/Renderer/RendererAPI.h"
#include "Broccoli/Core/Ref.h"

namespace Broccoli {
	Ref<IndexBuffer> IndexBuffer::create(const std::vector<uint32_t>& indices)
	{
		switch (RendererAPI::getCurrent())
		{
		case RendererAPIType::None: return nullptr;
		case RendererAPIType::Vulkan: return Ref<VulkanIndexBuffer>::create(indices);
		//case RendererAPIType::OpenGL: return Ref<OpenGLIndexBuffer>::create(indices);
		}
	}
}