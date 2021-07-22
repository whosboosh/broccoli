#pragma once

#include "VertexBuffer.h"

#include "Broccoli/Renderer/RendererAPI.h"

#include "Broccoli/Platform/Vulkan/VulkanVertexBuffer.h"

#include <stdint.h>

namespace Broccoli {

	// Static buffer usage
	Ref<VertexBuffer> VertexBuffer::create(void* data, uint32_t size, VertexBufferUsage usage)
	{
		switch (RendererAPI::getCurrent()) {
		case RendererAPIType::None: return nullptr;
		case RendererAPIType::Vulkan: return Ref<VulkanVertexBuffer>::create(data, size, usage);
		//case RendererAPIType::OpenGL: return ;
		}
	}

	// Dynamic buffer usage
	Ref<VertexBuffer> VertexBuffer::create(void* data, VertexBufferUsage usage)
	{
		switch (RendererAPI::getCurrent()) {
		case RendererAPIType::None: return nullptr;
		case RendererAPIType::Vulkan: return Ref<VulkanVertexBuffer>::create(data, usage);
			//case RendererAPIType::OpenGL: return ;
		}
	}
}