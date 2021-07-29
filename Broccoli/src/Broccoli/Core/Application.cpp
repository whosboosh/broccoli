#include "Application.h"

#define GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_NONE
#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <math.h>
#include <map>
#include <utility>
#include <map>
#include <utility>
#include <ctime>
#include <chrono>

#include "Broccoli/Platform/Vulkan/VulkanRenderer.h"
#include "Broccoli/Platform/Vulkan/VulkanContext.h"
#include "Broccoli/Platform/Vulkan/VulkanSwapchain.h"

namespace Broccoli {

	Application* Application::appInstance = nullptr;

	Application::Application(int argc, char** argvv)
	{
		appInstance = this;

		appInfo.Name = "Cloud Gaming Engine";
		appInfo.windowHeight = 900;
		appInfo.windowWidth = 1600;
		appInfo.fullscreen = true;
		appInfo.vsync = false;

		WindowSpecification windowSpec;
		windowSpec.title = appInfo.Name;
		windowSpec.width = appInfo.windowWidth;
		windowSpec.height = appInfo.windowHeight;
		windowSpec.fullscreen = appInfo.fullscreen;
		windowSpec.vsync = appInfo.vsync;

		window = new Window(windowSpec);
		window->init();
		window->setVsync(false);

		// Initialise renderer (shaders etc) and ImGui
		renderer = new Renderer();
		renderer->init();

		// TODO: All of this is just test code to run the engine
		Ref<CommandBuffer> commandBuffer = Ref<CommandBuffer>::create();

		std::vector<Vertex> floorVertices = {
			{ { -40, 0, -40}, { 1.0f, 0.0f, 1.0f}, { 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f} }, //BL
			{ { 40, 0, -40}, { 1.0f, .0f, 0.0f}, { 1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f} },//BR
			{ { -40, 0, 40 }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f} },//FL
			{ { 40, 0, 40 }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f} }//FR
		};

		std::vector<uint32_t> floorIndices = {
			0, 2, 1,
			1, 2, 3
		};

		glm::mat4 transformTest(1.0f);

		mesh = Ref<Mesh>::create(floorVertices, floorIndices, transformTest);
	}


	void Application::Run()
	{
		VkDevice logicalDevice = window->getRenderContext().As<VulkanContext>()->getLogicalDevice()->getLogicalDevice();
		VulkanSwapchain swapChain = window->getVulkanSwapChain();

		while (isRunning)
		{
			window->processEvents();
			if (!isMinimised)
			{
				// Stop running code until the fence is opened, only opened when the frame is finished drawing
				vkWaitForFences(logicalDevice, 1, &swapChain.getCurrentDrawFence(), VK_TRUE, std::numeric_limits<uint64_t>::max());
				vkResetFences(logicalDevice, 1, &swapChain.getCurrentDrawFence()); // Unsignal fence (close it so other frames can't enter)

				// Increments the currentBufferIndex
				swapChain.acquireNextImage();

				// TODO: Recreation of the vulkan swapchain if framebuffer is resized

				//std::cout << "New Frame " << frameCounter << "\n";

				// Record commands
				renderer->beginFrame();
				
				renderer->beginRenderPass();

				renderer->renderMesh(renderer->getGraphicsPipeline(), mesh, glm::mat4(1.0f));

				renderer->endRenderPass();

				renderer->endFrame();

				VkSubmitInfo submitInfo = {};
				submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
				submitInfo.waitSemaphoreCount = 1;
				submitInfo.pWaitSemaphores = &swapChain.getCurrentImageAvailableSemaphore(); // ?

				VkPipelineStageFlags waitStages[] = {
					VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
				};

				std::array<VkCommandBuffer, 1> submitCommandBuffers =
				{ swapChain.getCurrentCommandBuffer() };

				submitInfo.pWaitDstStageMask = waitStages;
				submitInfo.commandBufferCount = static_cast<uint32_t>(submitCommandBuffers.size());
				submitInfo.pCommandBuffers = submitCommandBuffers.data();
				submitInfo.signalSemaphoreCount = 1;
				submitInfo.pSignalSemaphores = &swapChain.getCurrentRenderFinishedSemaphore();

				// TODO: Use static Ref to VulkanContext to get queue
				VkResult result = vkQueueSubmit(window->getRenderContext().As<VulkanContext>()->getLogicalDevice()->getGraphicsQueue(), 1, &submitInfo, swapChain.getCurrentDrawFence());
				if (result != VK_SUCCESS) {
					throw std::runtime_error("Failed to submit command buffer to queue");
				}

				// PRESENT RENDERED IMAGE TO SCREEN
				VkPresentInfoKHR presentInfo = {};
				presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
				presentInfo.waitSemaphoreCount = 1; // Number of semaphores to wait on
				presentInfo.pWaitSemaphores = &swapChain.getCurrentRenderFinishedSemaphore(); // Semaphores to wait on
				presentInfo.swapchainCount = 1; // Number of swapchains to present to
				presentInfo.pSwapchains = &swapChain.getSwapChain(); // Swapchains to present images to
				presentInfo.pImageIndices = &swapChain.getCurrentBufferIndex(); // Index of images in swapchains to present

				result = vkQueuePresentKHR(window->getRenderContext().As<VulkanContext>()->getLogicalDevice()->getPresentationQueue(), &presentInfo);

				swapChain.incrementCurrentFrame();

				window->swapBuffers();
			}

			float time = glfwGetTime();
			float delta = time - lastFrameTime;
			frameCounter++;
		}
	}
}