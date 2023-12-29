#include "VertexBuffer.h"

#include "Renderer/RendererAPI.h"

#include "Platform/Vulkan/VulkanVertexBuffer.h"

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