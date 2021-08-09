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
#include "Broccoli/Core/Window.h"

namespace Broccoli {

	Application* Application::appInstance = nullptr;

	Application::Application(int argc, char** argvv)
	{
		appInstance = this;

		appInfo.Name = "Cloud Gaming Engine";
		appInfo.windowWidth = 1280;
		appInfo.windowHeight = 720;
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

		camera = new Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -10.0f, 10.0f, 0.05f);

		// Initialise renderer (shaders etc) and ImGui
		renderer = new Renderer();
		renderer->init();

		/*
		std::vector<Vertex> floorVertices = {
			{ { -40, 0, -40}, { 1.0f, 0.0f, 1.0f}, { 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f} }, //BL
			{ { 40, 0, -40}, { 1.0f, .0f, 0.0f}, { 1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f} },//BR
			{ { -40, 0, 40 }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f} },//FL
			{ { 40, 0, 40 }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f} }//FR
		};

		std::vector<uint32_t> floorIndices = {
			0, 2, 1,
			1, 2, 3
		};*/

		std::vector<Vertex> vertices = {
		{ { -1.0, -1.0, 1.0 }, { 1.0, 0.0, 1.0 }, { 0.0, 0.0 }, { 0.0, 0.0, 0.0 }}, // 0
		{ { 1.0, -1.0, 1.0 }, { 1.0, 0.0, 1.0 }, { 1.0, 0.0 }, { 0.0, 0.0, 0.0 }},
		{ { -1.0, 1.0, 1.0 }, { 1.0, 0.0, 1.0 }, { 0.0, 1.0 }, { 0.0, 0.0, 0.0 }},
		{ { 1.0, 1.0, 1.0 }, { 1.0, 0.0, 1.0 }, { 1.0, 1.0 }, { 0.0, 0.0, 0.0 }},

		{ { 1.0, -1.0, -1.0 }, { 0.0, 1.0, 1.0 }, { 1.0, 0.0 }, { 0.0, 0.0, 0.0 }}, // 4
		{ { 1.0, -1.0, 1.0 }, { 0.0, 1.0, 1.0 }, { 0.0, 0.0 }, { 0.0, 0.0, 0.0 }},
		{ { 1.0, 1.0, 1.0 }, { 0.0, 1.0, 1.0 }, { 0.0, 1.0 }, { 0.0, 0.0, 0.0 }},
		{ { 1.0, 1.0, -1.0 }, { 0.0, 1.0, 1.0 }, { 1.0, 1.0 }, { 0.0, 0.0, 0.0 }},

		{ { 1.0, -1.0, -1.0 }, { 1.0, 1.0, 0.0 }, { 1.0, 0.0 }, { 0.0, 0.0, 0.0 }}, // 8 back
		{ { -1.0, -1.0, -1.0 }, { 1.0, 1.0, 0.0 }, { 0.0, 0.0 }, { 0.0, 0.0, 0.0 }},
		{ { 1.0, 1.0, -1.0 }, { 1.0, 1.0, 0.0 }, { 1.0, 1.0 }, { 0.0, 0.0, 0.0 }},
		{ { -1.0, 1.0, -1.0 }, { 1.0, 1.0, 0.0 }, { 0.0, 1.0 }, { 0.0, 0.0, 0.0 }},

		{ { -1.0, -1.0, -1.0 }, { 1.0, 0.0, 0.0 }, { 0.0, 0.0 }, { 0.0, 0.0, 0.0 }}, // 12 left
		{ { -1.0, -1.0, 1.0 }, { 1.0, 0.0, 0.0 }, { 1.0, 0.0 }, { 0.0, 0.0, 0.0 }},
		{ { -1.0, 1.0, -1.0 }, { 1.0, 0.0, 0.0 }, { 0.0, 1.0 }, { 0.0, 0.0, 0.0 }},
		{ { -1.0, 1.0, 1.0 }, { 1.0, 0.0, 0.0 }, { 1.0, 1.0 }, { 0.0, 0.0, 0.0 }},

		{ { -1.0, -1.0, -1.0 }, { 0.0, 0.0, 1.0 }, { 0.0, 1.0 }, { 0.0, 0.0, 0.0 }}, // 16
		{ { 1.0, -1.0, -1.0 }, { 0.0, 0.0, 1.0 }, { 0.0, 0.0 }, { 0.0, 0.0, 0.0 }},
		{ { -1.0, -1.0, 1.0 }, { 0.0, 0.0, 1.0 }, { 1.0, 1.0 }, { 0.0, 0.0, 0.0 }},
		{ { 1.0, -1.0, 1.0 }, { 0.0, 0.0, 1.0 }, { 1.0, 0.0 }, { 0.0, 0.0, 0.0 }},

		{ { -1.0, 1.0, 1.0 }, { 1.0, 0.0, 0.0 }, { 0.0, 0.0 }, { 0.0, 0.0, 0.0 }}, // 20
		{ { 1.0, 1.0, 1.0 }, { 1.0, 0.0, 0.0 }, { 0.0, 1.0 }, { 0.0, 0.0, 0.0 }},
		{ { -1.0, 1.0, -1.0 }, { 1.0, 0.0, 0.0 }, { 1.0, 0.0 }, { 0.0, 0.0, 0.0 }},
		{ { 1.0, 1.0, -1.0 }, { 1.0, 0.0, 0.0 }, { 1.0, 1.0 }, { 0.0, 0.0, 0.0 }},
		};

		std::vector<uint32_t> indices = {
			1,3,2, 2,0,1,  //Face front
			4,7,6, 6,5,4, //Face right
			10,8,9, 9,11,10, // Back
			14,12,13, 13,15,14, // Left
			16,17,19, 19,18,16, // Bottom
			23,22,20, 20,21,23, // Top
		};

		glm::mat4 transformTest(1.0f);
		transformTest = glm::translate(transformTest, glm::vec3(0, 0, -5));

		// TODO: Don't use absolute path xd
		modelList.push_back(Ref<Model>::create("C:/Users/natha/source/repos/Broccoli/Broccoli/resources/models/chair_01.fbx", glm::scale(glm::mat4(1.0f), glm::vec3(0.4, 0.4, 0.4))));
		meshList.push_back(Ref<Mesh>::create(&vertices, &indices, transformTest));
	}

	void Application::updateUniforms()
	{
		viewProjection.projection = glm::perspective(glm::radians(70.0f), (float)Window::getSize().first/ (float)Window::getSize().second, 0.1f, 100.0f);
		viewProjection.projection[1][1] *= -1; // Invert the y axis for vulkan (GLM was made for opengl which uses +y as up)
		viewProjection.view = camera->calculateViewMatrix();

		renderer->updateUniform("geometry.vert", 0, 0, &viewProjection, sizeof(viewProjection));

		for (Ref<Mesh> mesh: meshList)
		{
			renderer->updateUniform("geometry.vert", 0, 1, &mesh->getMeshInfo(), sizeof(mesh->getMeshInfo()));
		}
		for (Ref<Model> model : modelList)
		{
			renderer->updateUniform("geometry.vert", 0, 1, &model->getTransform(), sizeof(model->getTransform()));
		}
	}

	void Application::processEvents()
	{
		glfwPollEvents();
		
		if (window->getIsControllingGame())
		{
			camera->mouseControl(window->getXChange(), window->getYChange());
			camera->keyControl(window->getKeys(), deltaTime);
			glfwSetInputMode(window->getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else {
			glfwSetInputMode(window->getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}

		double now = glfwGetTime();
		deltaTime = now - lastFrameTime;
		lastFrameTime = now;
		currentFpsTime += deltaTime;
	}


	void Application::Run()
	{
		while (!window->getShouldClose())
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

				for (Ref<Mesh> mesh : meshList)
				{
					renderer->renderMesh(renderer->getGraphicsPipeline(), mesh);
				}
				for (Ref<Model> model : modelList)
				{
					renderer->renderModel(renderer->getGraphicsPipeline(), model);
				}

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


		glfwDestroyWindow(window->getWindow());
		glfwTerminate();
		//renderer->shutdown();
	}
}