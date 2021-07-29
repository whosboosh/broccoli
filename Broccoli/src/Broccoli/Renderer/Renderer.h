#pragma once

#include "Broccoli/Core/Ref.h"

#include "Broccoli/Renderer/Shader.h"

#include "Broccoli/Renderer/RendererContext.h"

#include "Broccoli/Renderer/Pipeline.h"
#include "Broccoli/Renderer/Mesh.h"
#include "Broccoli/Renderer/CommandBuffer.h"

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

		void renderMesh(Ref<Pipeline> pipeline, Ref<Mesh> mesh, const glm::mat4& transform);

	private:
		Ref<Pipeline> graphicsPipeline;
	};

}