#pragma once

#include "Renderer.h"

#include <iostream>

#include "Broccoli/Core/Application.h"

#include "Broccoli/Renderer/Pipeline.h"

#include "Broccoli/Platform/Vulkan/VulkanRenderer.h"

//#include "Broccoli/Renderer/VertexBuffer.h"

namespace Broccoli {

	static RendererAPI* renderAPI = nullptr;


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

		renderAPI = initRendererAPI();

		PipelineSpecification spec = {};
		spec.backFaceCulling = false;
		spec.depthTest = true;
		spec.lineWidth = 1.0f;
		spec.topology = PrimitiveTopology::Triangles;
		spec.wireFrame = false;

		// Initialise geometry shader library
		ShaderLibrary* geometryShaderLibrary = new ShaderLibrary();

		//TODO: Fix relative paths for shaders
		//shaderLibrary->loadShader("Broccoli/resources/shaders/geometry.vert");
		geometryShaderLibrary->loadShader("C:/Users/natha/source/repos/Broccoli/Broccoli/resources/shaders/geometry.vert", VK_SHADER_STAGE_VERTEX_BIT);
		geometryShaderLibrary->loadShader("C:/Users/natha/source/repos/Broccoli/Broccoli/resources/shaders/geometry.frag", VK_SHADER_STAGE_FRAGMENT_BIT);
		
		graphicsPipeline = Pipeline::create(spec, geometryShaderLibrary);
	}

	Renderer::~Renderer()
	{
	}

	void Renderer::init()
	{
		renderAPI->init();
	}

	void Renderer::beginFrame()
	{
		renderAPI->beginFrame();
	}

	void Renderer::endFrame()
	{
		renderAPI->endFrame();
	}

	void Renderer::beginRenderPass()
	{
		renderAPI->beginRenderPass();
	}

	void Renderer::endRenderPass()
	{
		renderAPI->endRenderPass();
	}

	void Renderer::submitQueue()
	{
		renderAPI->submitQueue();
	}

	void Renderer::presentQueue()
	{
		renderAPI->presentQueue();
	}

	void Renderer::renderMesh(Ref<Pipeline> pipeline, Ref<Mesh> mesh, const glm::mat4& transform)
	{
		renderAPI->renderMesh(pipeline, mesh, transform);
	}
	
	Ref<RendererContext> Renderer::getContext()
	{
		return Application::get().getWindow().getRenderContext();
	}

}