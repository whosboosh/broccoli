#pragma once

#include "Renderer.h"

#include <iostream>

#include "Broccoli/Core/Application.h"

#include "Broccoli/Renderer/Pipeline.h"

namespace Broccoli {

	Renderer::Renderer()
	{
		std::cout << "Renderer created!\n";

		// Initialise the shader library
		shaderLibrary = new ShaderLibrary();

		//TODO: Fix relative paths for shaders
		//shaderLibrary->loadShader("Broccoli/resources/shaders/geometry.vert");
		shaderLibrary->loadShader("C:/Users/natha/source/repos/Broccoli/Broccoli/resources/shaders/geometry.vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
		

		// Vulkan Pipeline creation requires:
		// Needs renderpass
		// Needs shader stages
		// Needs descriptor sets and layouts (held in shader) - also texture descriptor
		// Needs swapchain extent
		// Needs vertex struct
		// Later could use msaaSamples
		
		PipelineSpecification spec = {};
		spec.backFaceCulling = false;
		spec.depthTest = true;
		spec.lineWidth = 1.0f;
		spec.topology = PrimitiveTopology::Triangles;
		spec.wireFrame = false;

		graphicsPipeline = Pipeline::create(spec, shaderLibrary);
	}

	Renderer::~Renderer()
	{
		delete shaderLibrary;
	}

	
	Ref<RendererContext> Renderer::getContext()
	{
		return Application::get().getWindow().getRenderContext();
	}

}