#pragma once

#include "VertexBuffer.h"

#include "Broccoli/Renderer/RendererAPI.h"

#include "Broccoli/Platform/Vulkan/VulkanVertexBuffer.h"

#include <stdint.h>

namespace Broccoli {

	
	Ref<VertexBuffer> VertexBuffer::create(std::vector<Vertex> vertices, VertexBufferUsage usage)
	{
		switch (RendererAPI::getCurrent()) {
			case RendererAPIType::None: return nullptr;
			case RendererAPIType::Vulkan: return Ref<VulkanVertexBuffer>::create(vertices, usage);
			//case RendererAPIType::OpenGL: return ;
		}
		return nullptr;
	}
}