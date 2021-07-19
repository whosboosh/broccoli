#include "VulkanPipeline.h"

#include "Broccoli/Utilities/VulkanUtilities.h"

#include "Broccoli/Core/Application.h"

#include "VulkanSwapChain.h"
#include "VulkanRenderpass.h"

#include "Broccoli/Renderer/Shader.h"

namespace Broccoli {
	VulkanPipeline::VulkanPipeline(const PipelineSpecification& spec, const ShaderLibrary* shaderLibrary)
	{
		std::cout << "Pipeline created!";
		
		VulkanSwapchain swapChain = Application::get().getWindow().getVulkanSwapChain();

		VulkanRenderpass renderPass = swapChain.getRenderPass();
		std::vector<VkFramebuffer> swapChainFramebuffers = swapChain.getSwapChainFrameBuffers();

	}
	VulkanPipeline::~VulkanPipeline()
	{
	}
}