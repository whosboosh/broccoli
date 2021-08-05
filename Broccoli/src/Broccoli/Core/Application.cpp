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

		camera = new Camera(glm::vec3(42.0f, 28.0f, 16.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -10.0f, 10.0f, 0.05f);

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

	void Application::updateUniforms()
	{
		viewProjection.projection = glm::perspective(glm::radians(70.0f), (float)appInfo.windowWidth / (float)appInfo.windowHeight, 0.1f, 100.0f);
		viewProjection.projection[1][1] *= -1; // Invert the y axis for vulkan (GLM was made for opengl which uses +y as up)
		viewProjection.view = camera->calculateViewMatrix();

		renderer->updateUniform("geometry.vert", 0, 0, &viewProjection);
		//renderer->updateUniform("geometry.vert", 0, 1, &mesh->getMeshInfo());
	}

	void Application::processEvents()
	{
		double now = glfwGetTime();
		deltaTime = now - lastFrameTime;
		lastFrameTime = now;
		currentFpsTime += deltaTime;

		if (window->getIsControllingGame())
		{
			camera->mouseControl(window->getXChange(), window->getYChange());
			camera->keyControl(window->getKeys(), deltaTime);
		}
		window->processEvents();
	}


	void Application::Run()
	{
		while (isRunning)
		{
			if (!isMinimised)
			{
				processEvents();
				// Update descriptor sets
				updateUniforms();

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
				// Also increments the image index for vulkan and other low level synchronisation stuff
				renderer->presentQueue();

				window->swapBuffers();

				frameCounter++;
			}
		}
	}
}