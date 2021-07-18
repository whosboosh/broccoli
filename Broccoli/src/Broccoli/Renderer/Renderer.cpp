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

		//shaderLibrary->loadShader("Broccoli/resources/shaders/geometry.vert");
		//TODO: Fix relative paths for shaders
		shaderLibrary->loadShader("C:/Users/natha/source/repos/Broccoli/Broccoli/resources/shaders/geometry.vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
		
		Pipeline* graphicsPipeline = new Pipeline();
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