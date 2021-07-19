#pragma once

#include "Broccoli/Renderer/Pipeline.h"

// Project dependencies
#include "Broccoli/Utilities/VulkanInitializers.hpp"

// Vendor
#include <vulkan/vulkan.h>

namespace Broccoli {

	class VulkanPipeline : public Pipeline
	{
	public:
		VulkanPipeline(const PipelineSpecification& spec, ShaderLibrary* shaderLibrary);

		~VulkanPipeline();

	private:
		VkPipelineLayout pipelineLayout;
		VkPipeline pipeline;
	};
}