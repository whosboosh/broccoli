#ifndef BROCCOLI_RENDERER_HEADER
#define BROCCOLI_RENDERER_HEADER

#include "Core/Ref.h"

#include "Renderer/Shader.h"

#include "Renderer/RendererContext.h"

#include "Renderer/Pipeline.h"
#include "Renderer/Mesh.h"
#include "Renderer/Model.h"
#include "Physics/Entity.h"

#include "Platform/Vulkan/VulkanPipeline.h"

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

		void renderMesh(Ref<Pipeline> pipeline, Mesh* mesh);
		void renderModel (Ref<Pipeline> pipeline, Model* model);
		void renderEntity(Ref<Pipeline> pipeline, Entity* entity);

	private:
		Ref<Pipeline> graphicsPipeline;
	};

}

#endif