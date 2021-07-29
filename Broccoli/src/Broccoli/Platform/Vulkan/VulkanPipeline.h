#pragma once

#include "Broccoli/Renderer/Pipeline.h"

// Project dependencies
#include "Broccoli/Utilities/VulkanInitializers.hpp"
#include "Broccoli/Renderer/Shader.h"

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