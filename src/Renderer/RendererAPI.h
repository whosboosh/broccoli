#ifndef BROCCOLI_RENDERER_API_HEADER
#define BROCCOLI_RENDERER_API_HEADER

#include "Renderer/Pipeline.h"
#include "Renderer/Mesh.h"
#include "Renderer/Model.h"

#include "Platform/Vulkan/VulkanPipeline.h"

#include <glm/glm.hpp>

namespace Broccoli {

	enum class RendererAPIType
	{
		None,
		Vulkan,
		OpenGL
	};

	enum class PrimitiveType
	{
		None = 0, Triangles, Lines
	};

	class RendererAPI
	{
	public:
		virtual void init() = 0;
		virtual void shutdown() = 0;

		virtual void beginFrame() = 0;
		virtual void endFrame() = 0;

		virtual void beginRenderPass() = 0;
		virtual void endRenderPass() = 0;

		virtual void submitQueue() = 0;
		virtual void presentQueue() = 0;

		virtual void updateUniform(const std::string& name, int set, int binding, void* data, int size) = 0;

		virtual void renderModel(Ref<Pipeline> pipeline, Model* model) = 0;
		virtual void renderMesh(Ref<Pipeline> pipeline, Mesh* mesh) = 0;

		// Render mesh etc...
		static RendererAPIType getCurrent() { return currentRenderer; }

	private:
		// TODO: Don't make this const, use inline?
		const static RendererAPIType currentRenderer = RendererAPIType::Vulkan;
	};

}

#endif