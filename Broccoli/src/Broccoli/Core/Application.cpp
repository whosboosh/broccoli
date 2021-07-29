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
		while (isRunning)
		{
			window->processEvents();
			if (!isMinimised)
			{
				// TODO: Recreation of the vulkan swapchain if framebuffer is resized

				//std::cout << "New Frame " << frameCounter << "\n";

				// Record commands
				renderer->beginFrame();
				
				renderer->beginRenderPass();

				renderer->renderMesh(renderer->getGraphicsPipeline(), mesh, glm::mat4(1.0f));

				renderer->endRenderPass();

				renderer->endFrame();

				// Submit the recorded command buffer to the graphics queue
				renderer->submitQueue();

				// Present the rendered image to the screen
				renderer->presentQueue();

				window->swapBuffers();
			}

			float time = glfwGetTime();
			float delta = time - lastFrameTime;
			frameCounter++;
		}
	}
}