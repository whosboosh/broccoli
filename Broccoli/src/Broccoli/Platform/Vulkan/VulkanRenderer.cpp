#include "VulkanRenderer.h"

#include "Broccoli/Platform/Vulkan/VulkanContext.h"
#include "Broccoli/Platform/Vulkan/VulkanSwapchain.h"
#include "Broccoli/Platform/Vulkan/VulkanPipeline.h"

#include "Broccoli/Renderer/VertexBuffer.h"
#include "Broccoli/Platform/Vulkan/VulkanVertexBuffer.h"
#include "Broccoli/Renderer/IndexBuffer.h"
#include "Broccoli/Platform/Vulkan/VulkanIndexBuffer.h"
#include "Broccoli/Platform/Vulkan/VulkanShader.h"

#include "Broccoli/Core/Application.h"

#include "Broccoli/Utilities/VulkanUtilities.h"

#include <vector>

namespace Broccoli {
	VulkanRenderer::VulkanRenderer()
	{
		
	}

	VulkanRenderer::~VulkanRenderer()
	{
		shutdown();
	}

	void VulkanRenderer::init()
	{
		swapChain = &VulkanContext::get()->getVulkanSwapChain();
	}

	void VulkanRenderer::beginFrame()
	{
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

		VkRenderPassBeginInfo renderPassBeginInfo = {};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = swapChain->getRenderPass().getRenderPass();
		renderPassBeginInfo.framebuffer = swapChain->getCurrentFrameBuffer();
		renderPassBeginInfo.renderArea.offset = { 0, 0 };
		renderPassBeginInfo.renderArea.extent = swapChain->getSwapExtent();

		clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f }; // Color attachment clear value
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassBeginInfo.pClearValues = clearValues.data(); // List of clear values

		vkCmdBeginRenderPass(swapChain->getCurrentCommandBuffer(), &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport = {};
		viewport.width = swapChain->getSwapExtent().width;
		viewport.height = swapChain->getSwapExtent().height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(swapChain->getCurrentCommandBuffer(), 0, 1, &viewport);

		VkRect2D scissor = {};
		scissor.extent = swapChain->getSwapExtent();
		scissor.offset.x = 0;
		scissor.offset.y = 0;
		vkCmdSetScissor(swapChain->getCurrentCommandBuffer(), 0, 1, &scissor);
	}

	void VulkanRenderer::endRenderPass()
	{
		vkCmdEndRenderPass(swapChain->getCurrentCommandBuffer());
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

		VkResult result = vkQueueSubmit(VulkanContext::get()->getLogicalDevice()->getGraphicsQueue(), 1, &submitInfo, swapChain->getCurrentDrawFence());
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
		presentInfo.pImageIndices = &swapChain->getCurrentImageIndex(); // Index of images in swapchains to present

		VkResult result = vkQueuePresentKHR(VulkanContext::get()->getLogicalDevice()->getPresentationQueue(), &presentInfo);
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

	void VulkanRenderer::updateUniform(const std::string& name, int set, int binding, void* data, int size)
	{
		Ref<VulkanPipeline> pipeline = Application::get().getRenderer().getGraphicsPipeline().As<VulkanPipeline>();

		Ref<VulkanShader> shader = pipeline->getShaderLibrary()->getShader(name).As<VulkanShader>();

		shader->updateDescriptorSet(set, binding, swapChain->getCurrentImageIndex(), data, size);
	}

	void VulkanRenderer::renderMesh(Ref<Pipeline> pipeline, Mesh* mesh)
	{
		std::cout << glm::to_string(mesh->getTransform().transform) << "\n";

		drawObject(pipeline, mesh, mesh->getTransform());
	}

	void VulkanRenderer::renderModel(Ref<Pipeline> pipeline, Model* model)
	{
		for (size_t i = 0; i < model->getMeshCount(); i++)
		{
			Mesh* mesh = model->getMesh(i);
			drawObject(pipeline, mesh, model->getTransform());
		}
	}

	void VulkanRenderer::drawObject(Ref<Pipeline> pipeline, Mesh* mesh, MeshInfo transform)
	{
		Ref<VulkanPipeline> vulkanPipeline = pipeline.As<VulkanPipeline>();

		// Potentially move binding pipeline out to its own function if multiple meshes all use the same pipeline, dont need to keep rebinding it
		vkCmdBindPipeline(swapChain->getCurrentCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline->getVulkanPipeline());

		VkBuffer vertexBuffer[] = { mesh->getVertexBuffer()->As<VulkanVertexBuffer>()->getVertexBuffer() };
		VkDeviceSize offsets[] = { 0 }; // Offsets into buffers being bound

		vkCmdBindVertexBuffers(swapChain->getCurrentCommandBuffer(), 0, 1, vertexBuffer, offsets); // Command to bind vertex buffer before drawing with them
		vkCmdBindIndexBuffer(swapChain->getCurrentCommandBuffer(), mesh->getIndexBuffer()->As<VulkanIndexBuffer>()->getIndexBuffer(), 0, VK_INDEX_TYPE_UINT32); // Bind mesh index buffer with 0 offset and using uint32 type

		std::vector descriptorSetGroup = vulkanPipeline->getShaderLibrary()->getShaderUniformDescriptorSets(swapChain->getCurrentImageIndex()); // Add all of the uniform descriptors (multiple because of multiple sets)
		if (mesh->getTransform().hasTexture) {
			descriptorSetGroup.push_back(vulkanPipeline->getShaderLibrary()->getShaderSamplerDescriptorSets(mesh->getTexture()->getShaderId(), mesh->getTexture()->getTextureId())); // Add the sampler descriptor
		}
		else {
			descriptorSetGroup.push_back(vulkanPipeline->getShaderLibrary()->getShaderSamplerDescriptorSets(0, 0)); // Use first texture loaded in
		}

		vkCmdBindDescriptorSets(swapChain->getCurrentCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline->getVulkanPipelineLayout(),
			0, static_cast<uint32_t>(descriptorSetGroup.size()),
			descriptorSetGroup.data(), 0, nullptr);

		for (const VkPushConstantRange& pushConstantRange : vulkanPipeline->getShaderLibrary()->getPushConstantRanges())
		{
			vkCmdPushConstants(swapChain->getCurrentCommandBuffer(), vulkanPipeline->getVulkanPipelineLayout(), pushConstantRange.stageFlags, pushConstantRange.offset, pushConstantRange.size, &transform);
		}

		vkCmdDrawIndexed(swapChain->getCurrentCommandBuffer(), mesh->getIndexCount(), 1, 0, 0, 0);
	}

	void VulkanRenderer::shutdown()
	{
		// Call cleanup for vulkan creations
		swapChain->cleanup();
		Application::get().getRenderer().getGraphicsPipeline().As<VulkanPipeline>()->cleanup();
		VulkanContext::get()->cleanup();
	}

	void VulkanRenderer::recreateSwapChain()
	{
		// Call recreate cleanups to vulkan objects
		swapChain->recreateSwapChain();
		Application::get().getRenderer().getGraphicsPipeline().As<VulkanPipeline>()->recreateSwapChain();
	}
}