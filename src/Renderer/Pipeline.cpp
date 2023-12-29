#include "Pipeline.h"

#include "Platform/Vulkan/VulkanPipeline.h"
#include "Core/Ref.h"

#include "Renderer/RendererAPI.h"

#include <iostream>


namespace Broccoli {



	Ref<Pipeline> Pipeline::create(const PipelineSpecification& spec, ShaderLibrary* shaderLibrary)
	{
		switch (RendererAPI::getCurrent())
		{
		case RendererAPIType::None: return nullptr;
		case RendererAPIType::Vulkan: return Ref<VulkanPipeline>::create(spec, shaderLibrary);
		//case RendererAPIType::OpenGL return Ref<OpenGLPipeline>::create(spec);
		}

		std::cout << "Unknown renderer API";
		return nullptr;
	}

}

