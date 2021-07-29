#include "VulkanRenderer.h"

#include "Broccoli/Platform/Vulkan/VulkanContext.h"
#include "Broccoli/Platform/Vulkan/VulkanSwapchain.h"
#include "Broccoli/Platform/Vulkan/VulkanPipeline.h"

#include "Broccoli/Core/Application.h"

#include "Broccoli/Renderer/VertexBuffer.h"
#include "Broccoli/Platform/Vulkan/VulkanVertexBuffer.h"
#include "Broccoli/Renderer/IndexBuffer.h"
#include "Broccoli/Platform/Vulkan/VulkanIndexBuffer.h"
#include "Broccoli/Platform/Vulkan/VulkanShader.h"

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
		//VkDevice device = VulkanContext::get()->getLogicalDevice()->getLogicalDevice(); // TODO update all other logical device refs with new static ref to vulkancontext

		// Stop code until fence is open
		swapChain->waitForFrences();

		// Increments the currentBufferIndex
		swapChain->acquireNextImage();

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

	void VulkanRenderer::submitQueue()
	{
		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &swapChain->getCurrentImageAvailableSemaphore();

		VkPipelineStageFlags waitStages[] = {
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
		};

		std::array<VkCommandBuffer, 1> submitCommandBuffers =
		{ swapChain->getCurrentCommandBuffer() };

		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = static_cast<uint32_t>(submitCommandBuffers.size());
		submitInfo.pCommandBuffers = submitCommandBuffers.data();
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &swapChain->getCurrentRenderFinishedSemaphore();

		// TODO: Use static Ref to VulkanContext to get queue
		VkResult result = vkQueueSubmit(Application::get().getWindow().getRenderContext().As<VulkanContext>()->getLogicalDevice()->getGraphicsQueue(), 1, &submitInfo, swapChain->getCurrentDrawFence());
		if (result != VK_SUCCESS) {
			throw std::runtime_error("Failed to submit command buffer to queue");
		}
	}

	void VulkanRenderer::presentQueue()
	{
		// PRESENT RENDERED IMAGE TO SCREEN
		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1; // Number of semaphores to wait on
		presentInfo.pWaitSemaphores = &swapChain->getCurrentRenderFinishedSemaphore(); // Semaphores to wait on
		presentInfo.swapchainCount = 1; // Number of swapchains to present to
		presentInfo.pSwapchains = &swapChain->getSwapChain(); // Swapchains to present images to
		presentInfo.pImageIndices = &swapChain->getCurrentBufferIndex(); // Index of images in swapchains to present

		VkResult result = vkQueuePresentKHR(Application::get().getWindow().getRenderContext().As<VulkanContext>()->getLogicalDevice()->getPresentationQueue(), &presentInfo);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR  || Window::frameBufferResized) {
			Window::frameBufferResized = false;
			recreateSwapChain();
			std::cout << "Frame buffer out of date\n";
		}
		else if (result != VK_SUCCESS) {
			throw std::runtime_error("Failed to present image!");
		}

		// Increments the currentFrame in swapchain, value between 0 and 1 (double buffering)
		swapChain->incrementCurrentFrame();
	}

	void VulkanRenderer::renderMesh(Ref<Pipeline> pipeline, Ref<Mesh> mesh, const glm::mat4& transform)
	{
		Ref<VulkanPipeline> vulkanPipeline = pipeline.As<VulkanPipeline>();

		// Potentially move binding pipeline out to its own function if multiple meshes all use the same pipeline, dont need to keep rebinding it
		vkCmdBindPipeline(swapChain->getCurrentCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline->getVulkanPipeline());
		
		mesh->getVertexBuffer();

		VkBuffer vertexBuffer[] = { mesh->getVertexBuffer()->As<VulkanVertexBuffer>()->getVertexBuffer() };
		VkDeviceSize offsets[] = { 0 }; // Offsets into buffers being bound

		vkCmdBindVertexBuffers(swapChain->getCurrentCommandBuffer(), 0, 1, vertexBuffer, offsets); // Command to bind vertex buffer before drawing with them
		vkCmdBindIndexBuffer(swapChain->getCurrentCommandBuffer(), mesh->getIndexBuffer()->As<VulkanIndexBuffer>()->getIndexBuffer(), 0, VK_INDEX_TYPE_UINT32); // Bind mesh index buffer with 0 offset and using uint32 type

		vkCmdBindDescriptorSets(swapChain->getCurrentCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline->getVulkanPipelineLayout(),
			0, static_cast<uint32_t>(vulkanPipeline->getShaderLibrary()->getShaderDescriptorSets(swapChain->getCurrentBufferIndex()).size()), vulkanPipeline->getShaderLibrary()->getShaderDescriptorSets(swapChain->getCurrentBufferIndex()).data(), 0, nullptr);

		vkCmdDrawIndexed(swapChain->getCurrentCommandBuffer(), mesh->getIndexCount(), 1, 0, 0, 0);
	}

	void VulkanRenderer::recreateSwapChain()
	{
		// Call recreate cleanups to vulkan objects
		Application::get().getWindow().getVulkanSwapChain().recreateSwapChain();
		Application::get().getRenderer().getGraphicsPipeline().As<VulkanPipeline>()->recreateSwapChain();

	}
}