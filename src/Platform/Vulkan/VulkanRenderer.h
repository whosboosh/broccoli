#ifndef BROCCOLI_VULKAN_RENDERER_HEADER
#define BROCCOLI_VULKAN_RENDERER_HEADER

#include "Renderer/RendererAPI.h"

#include "Core/Ref.h"

#include "Renderer/Pipeline.h"
#include "Renderer/Mesh.h"

#include "Platform/Vulkan/VulkanSwapchain.h"

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

		virtual void updateUniform(const std::string& name, int set, int binding, void* data, int size) override;

		virtual void renderMesh(Ref<Pipeline> pipeline, Mesh* mesh) override;
		virtual void renderModel(Ref<Pipeline> pipeline, Model* model) override;

		void drawObject(Ref<Pipeline> pipeline, Mesh* mesh, glm::mat4 transform);

		void recreateSwapChain();

	private:
		VulkanSwapchain* swapChain;
	};

}

#endif
