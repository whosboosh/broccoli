#pragma once

#include "Renderer.h"

#include <iostream>

#include "Broccoli/Core/Application.h"

#include "Broccoli/Renderer/Pipeline.h"

#include "Broccoli/Platform/Vulkan/VulkanRenderer.h"

namespace Broccoli {

	static RendererAPI* initRendererAPI()
	{
		switch (RendererAPI::getCurrent())
		{
		case RendererAPIType::Vulkan: return new VulkanRenderer();
		}
	}

	Renderer::Renderer()
	{
		std::cout << "Renderer created!\n";

		PipelineSpecification spec = {};
		spec.backFaceCulling = false;
		spec.depthTest = true;
		spec.lineWidth = 1.0f;
		spec.topology = PrimitiveTopology::Triangles;
		spec.wireFrame = false;

		// Initialise geometry shader library
		geometryShaderLibrary = new ShaderLibrary();

		//TODO: Fix relative paths for shaders
		//shaderLibrary->loadShader("Broccoli/resources/shaders/geometry.vert");
		geometryShaderLibrary->loadShader("C:/Users/natha/source/repos/Broccoli/Broccoli/resources/shaders/geometry.vert", VK_SHADER_STAGE_VERTEX_BIT);
		geometryShaderLibrary->loadShader("C:/Users/natha/source/repos/Broccoli/Broccoli/resources/shaders/geometry.frag", VK_SHADER_STAGE_FRAGMENT_BIT);
		
		graphicsPipeline = Pipeline::create(spec, geometryShaderLibrary);
	}

	Renderer::~Renderer()
	{
		delete geometryShaderLibrary;
	}

	
	Ref<RendererContext> Renderer::getContext()
	{
		return Application::get().getWindow().getRenderContext();
	}

}