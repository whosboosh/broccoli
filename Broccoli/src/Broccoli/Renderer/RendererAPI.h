#pragma once

//#include "Broccoli/Core/Ref.h"

#include "Broccoli/Renderer/Pipeline.h"
#include "Broccoli/Renderer/Mesh.h"
#include "Broccoli/Renderer/CommandBuffer.h"

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

		virtual void renderMesh(Ref<CommandBuffer>, Ref<Pipeline>, Ref<Mesh> mesh, const glm::mat4& transform) = 0;

		// Render mesh etc...
		static RendererAPIType getCurrent() { return currentRenderer; }

	private:
		// TODO: Don't make this const, use inline?
		const static RendererAPIType currentRenderer = RendererAPIType::Vulkan;
	};

}