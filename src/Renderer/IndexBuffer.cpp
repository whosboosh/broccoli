#include "IndexBuffer.h"

#include <stdint.h>
#include <iostream>
#include <string>

#include "Platform/Vulkan/VulkanIndexBuffer.h"

#include "Renderer/RendererAPI.h"
#include "Core/Ref.h"

namespace Broccoli {
    Ref<IndexBuffer> IndexBuffer::create(std::vector<uint32_t>* indices)
    {
        switch (RendererAPI::getCurrent())
        {
        case RendererAPIType::None: return nullptr;
        case RendererAPIType::Vulkan: return Ref<VulkanIndexBuffer>::create(indices);
        //case RendererAPIType::OpenGL: return Ref<OpenGLIndexBuffer>::create(indices);
        default:
            return nullptr;
        }
    }
}