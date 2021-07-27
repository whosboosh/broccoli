#include "VulkanRenderer.h"

#include "Broccoli/Platform/Vulkan/VulkanContext.h"
#include "Broccoli/Platform/Vulkan/VulkanSwapchain.h"
#include "Broccoli/Core/Application.h"

#include "Broccoli/Renderer/VertexBuffer.h"
#include <vector>

namespace Broccoli {
	VulkanRenderer::VulkanRenderer()
	{
		
	}

	VulkanRenderer::~VulkanRenderer()
	{
	}
	void VulkanRenderer::init()
	{
		swapChain = &Application::get().getWindow().getVulkanSwapChain();
	}
	void VulkanRenderer::shutdown()
	{
	}

	void VulkanRenderer::beginFrame()
	{
		//VulkanSwapchain& swapChain = Application::get().getWindow().getVulkanSwapChain();
		VkDevice device = VulkanContext::get()->getLogicalDevice()->getLogicalDevice(); // TODO update all other logical device refs with new static ref to vulkancontext

		// Begin command buffer
		VkCommandBufferBeginInfo cmdBufferBeginInfo = {};
		cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		VkResult result = vkBeginCommandBuffer(swapChain->getCurrentCommandBuffer(), &cmdBufferBeginInfo);
		if (result != VK_SUCCESS) {
			throw std::runtime_error("Failed to start recording to a command buffer!");
		}
	}

	void VulkanRenderer::endFrame()
	{
		VkResult result = vkEndCommandBuffer(swapChain->getCurrentCommandBuffer());
		if (result != VK_SUCCESS) {
			throw std::runtime_error("Failed to stop recording to a command buffer!");
		}
	}

	void VulkanRenderer::beginRenderPass()
	{
		std::array<VkClearValue, 2> clearValues = {};

		VkViewport viewport = {};
		viewport.width = swapChain->getSwapExtent().width;
		viewport.height = swapChain->getSwapExtent().height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(swapChain->getCurrentCommandBuffer(), 0, 1, &viewport);

		VkRect2D scissor = {};
		viewport.width = swapChain->getSwapExtent().width;
		viewport.height = swapChain->getSwapExtent().height;
		scissor.offset.x = 0;
		scissor.offset.y = 0;
		vkCmdSetScissor(swapChain->getCurrentCommandBuffer(), 0, 1, &scissor);

		VkRenderPassBeginInfo renderPassBeginInfo = {};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = swapChain->getRenderPass().getRenderPass();
		renderPassBeginInfo.framebuffer = swapChain->getCurrentFrameBuffer();
		renderPassBeginInfo.renderArea.offset = { 0, 0 };
		renderPassBeginInfo.renderArea.extent.width = swapChain->getSwapExtent().width;
		renderPassBeginInfo.renderArea.extent.height = swapChain->getSwapExtent().height;
		clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f }; // Color attachment clear value
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassBeginInfo.pClearValues = clearValues.data(); // List of clear values

		vkCmdBeginRenderPass(swapChain->getCurrentCommandBuffer(), &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
	}

	void VulkanRenderer::endRenderPass()
	{
		VulkanSwapchain& swapChain = Application::get().getWindow().getVulkanSwapChain();

		vkCmdEndRenderPass(swapChain.getCurrentCommandBuffer());
	}

	void VulkanRenderer::renderMesh(Ref<Pipeline> pipeline, Ref<Mesh> mesh, const glm::mat4& transform)
	{
		VulkanSwapchain& swapChain = Application::get().getWindow().getVulkanSwapChain();

	}
}