#pragma once

#include "Broccoli/Renderer/RendererAPI.h"

#include "Broccoli/Core/Ref.h"

#include "Broccoli/Renderer/Pipeline.h"
#include "Broccoli/Renderer/Mesh.h"
#include "Broccoli/Renderer/CommandBuffer.h"

#include <glm/glm.hpp>

namespace Broccoli {

	class VulkanRenderer : public RendererAPI
	{
	public:
		VulkanRenderer();
		~VulkanRenderer();

		virtual void init() override;
		virtual void shutdown() override;

		virtual void beginFrame() override;
		virtual void endFrame() override;

		virtual void beginRenderPass() override;
		virtual void endRenderPass() override;

		virtual void renderMesh(Ref<CommandBuffer>, Ref<Pipeline>, Ref<Mesh> mesh, const glm::mat4& transform) override;

	private:

	};

}
