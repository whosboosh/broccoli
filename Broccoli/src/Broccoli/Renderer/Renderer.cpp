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
		geometryShaderLibrary->loadShader("resources/shaders/geometry.vert", VK_SHADER_STAGE_VERTEX_BIT);
		geometryShaderLibrary->loadShader("resources/shaders/geometry.frag", VK_SHADER_STAGE_FRAGMENT_BIT);
		geometryShaderLibrary->setPushConstantRangesFromShaders();
		geometryShaderLibrary->setDescriptorSetsfromShaders();

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

	void Renderer::updateUniform(const std::string& name, int set, int binding, void* data, int size)
	{
		renderAPI->updateUniform(name, set, binding, data, size);
	}

	void Renderer::renderMesh(Ref<Pipeline> pipeline, Ref<Mesh> mesh)
	{
		renderAPI->renderMesh(pipeline, mesh);
	}

	void Renderer::renderModel(Ref<Pipeline> pipeline, Ref<Model> model)
	{
		renderAPI->renderModel(pipeline, model);
	}

	// Keep the render API more primitive
	// Entity is an engine concept that contains a mesh/model for cleaner abstraction
	void Renderer::renderEntity(Ref<Pipeline> pipeline, Entity* entity)
	{
		if (entity->getModel())
		{
			renderAPI->renderModel(pipeline, entity->getModel());
		}
		else if (entity->getMesh()){
			renderAPI->renderMesh(pipeline, entity->getMesh());
		}
	}

	Ref<RendererContext> Renderer::getContext()
	{
		return Application::get().getWindow().getRenderContext();
	}

	void Renderer::shutdown()
	{
		renderAPI->shutdown();
	}
}