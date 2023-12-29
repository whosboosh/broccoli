#include "Renderer/RendererContext.h"

#include "Renderer/RendererAPI.h"

#include "Platform/OpenGL/OpenGLContext.h"
#include "Platform/Vulkan//VulkanContext.h"

#include <GLFW/glfw3.h>

namespace Broccoli {
	Ref<RendererContext> RendererContext::create()
	{
		switch (RendererAPI::getCurrent())
		{
			case RendererAPIType::None: return nullptr;
			case RendererAPIType::Vulkan: return Ref<VulkanContext>::create();
			case RendererAPIType::OpenGL: return Ref<OpenGLContext>::create();
		}
		// No match
		return nullptr;
	}
}