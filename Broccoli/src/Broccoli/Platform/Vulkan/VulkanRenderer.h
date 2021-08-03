#pragma once

#include "Broccoli/Renderer/RendererAPI.h"

#include "Broccoli/Core/Ref.h"

#include "Broccoli/Renderer/Pipeline.h"
#include "Broccoli/Renderer/Mesh.h"
#include "Broccoli/Renderer/CommandBuffer.h"

#include "Broccoli/Platform/Vulkan/VulkanSwapchain.h"

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

		virtual void submitQueue() override;
		virtual void presentQueue() override;

		virtual void updateUniform(std::string& name, int set, int binding, void* data) override;

		virtual void renderMesh(Ref<Pipeline>, Ref<Mesh> mesh, const glm::mat4& transform) override;

		void recreateSwapChain();

	private:
		VulkanSwapchain* swapChain;
	};

}
