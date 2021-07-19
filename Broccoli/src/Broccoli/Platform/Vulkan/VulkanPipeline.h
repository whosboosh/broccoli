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
		VulkanPipeline(const PipelineSpecification& spec, const ShaderLibrary* shaderLibrary);

		~VulkanPipeline();

	private:

	};
}