#ifndef BROCCOLI_VULKAN_PIPELINE_HEADER
#define BROCCOLI_VULKAN_PIPELINE_HEADER

// Project dependencies
#include "Utilities/VulkanInitializers.hpp"
#include "Renderer/Shader.h"
#include "Renderer/Pipeline.h"
#include "VulkanSwapchain.h"

// Vendor
#include <vulkan/vulkan.h>

namespace Broccoli {

	class VulkanPipeline : public Pipeline
	{
	public:
		VulkanPipeline(const PipelineSpecification& spec, ShaderLibrary* shaderLibrary);
		~VulkanPipeline();

		void create();

		void cleanup();
		void destroyPipeline();
		void recreateSwapChain();

		ShaderLibrary* getShaderLibrary() { return shaderLibrary; }

		VkPipeline getVulkanPipeline() { return pipeline; }
		VkPipelineLayout getVulkanPipelineLayout() { return pipelineLayout; }

	private:
		ShaderLibrary* shaderLibrary;
		PipelineSpecification spec;

		VkPipelineLayout pipelineLayout;
		VkPipeline pipeline;
	};
}

#endif