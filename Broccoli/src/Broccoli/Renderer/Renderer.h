#pragma once

#include "Broccoli/Core/Ref.h"

#include "Broccoli/Renderer/Shader.h"

#include "Broccoli/Renderer/RendererContext.h"

#include "Broccoli/Renderer/Pipeline.h"
#include "Broccoli/Renderer/Mesh.h"
#include "Broccoli/Renderer/Model.h"
#include "Broccoli/Physics/Entity.h"

#include "Broccoli/Platform/Vulkan/VulkanPipeline.h"

#include <glm/glm.hpp>

namespace Broccoli {

	class Renderer
	{
	public:
		Renderer();
		~Renderer();

		static Ref<RendererContext> getContext();

		Ref<Pipeline> getGraphicsPipeline() { return graphicsPipeline; }

		void init();
		void shutdown();

		void beginFrame();
		void endFrame();

		void beginRenderPass();
		void endRenderPass();

		void submitQueue();
		void presentQueue();

		void updateUniform(const std::string& name, int set, int binding, void* data, int size);

		void renderMesh(Ref<Pipeline> pipeline, Ref<Mesh> mesh);
		void renderModel (Ref<Pipeline> pipeline, Ref<Model> model);
		void renderEntity(Ref<Pipeline> pipeline, Entity* entity);

	private:
		Ref<Pipeline> graphicsPipeline;
	};

}