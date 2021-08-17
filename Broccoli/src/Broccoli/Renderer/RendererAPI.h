#pragma once

#include "Broccoli/Renderer/Pipeline.h"
#include "Broccoli/Renderer/Mesh.h"
#include "Broccoli/Renderer/Model.h"

#include "Broccoli/Platform/Vulkan/VulkanPipeline.h"

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

		virtual void renderModel(Ref<Pipeline> pipeline, Ref<Model> model) = 0;
		virtual void renderMesh(Ref<Pipeline> pipeline, Ref<Mesh> mesh) = 0;

		// Render mesh etc...
		static RendererAPIType getCurrent() { return currentRenderer; }

	private:
		// TODO: Don't make this const, use inline?
		const static RendererAPIType currentRenderer = RendererAPIType::Vulkan;
	};

}