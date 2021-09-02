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

#include "Broccoli/Renderer/Texture.h"
#include "Broccoli/Platform/Vulkan/VulkanTexture.h"

#include "Broccoli/Physics/Entity.h"

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
		appInfo.nearPlane = 0.01f;
		appInfo.farPlane = 400.0f;

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
		{ { -1.0, -1.0, 2.0 }, { 1.0, 0.0, 1.0 }, { 0.0, 0.0 }, { 0.0, 0.0, 0.0 }}, // 0
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

		{ { -1.0, -1.0, -1.0 }, { 0.0, 0.0, 1.0 }, { 0.0, 1.0 }, { 0.0, 0.0, 0.0 }}, // 16 bottom
		{ { 1.0, -1.0, -1.0 }, { 0.0, 0.0, 1.0 }, { 0.0, 0.0 }, { 0.0, 0.0, 0.0 }},
		{ { -1.0, -1.0, 1.0 }, { 0.0, 0.0, 1.0 }, { 1.0, 1.0 }, { 0.0, 0.0, 0.0 }},
		{ { 1.0, -1.0, 1.0 }, { 0.0, 0.0, 1.0 }, { 1.0, 0.0 }, { 0.0, 0.0, 0.0 }},

		{ { -1.0, 1.0, 1.0 }, { 1.0, 0.0, 0.0 }, { 0.0, 0.0 }, { 0.0, 0.0, 0.0 }}, // 20 Top
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

		glm::mat4 oldTransform = glm::scale(glm::mat4(1.0f), glm::vec3(0.4, 0.4, 0.4));

		glm::mat4 mapTransform = glm::mat4(1.0f);
		mapTransform = glm::rotate(mapTransform, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		mapTransform = glm::rotate(mapTransform, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		mapTransform = glm::scale(mapTransform, glm::vec3(0.1f, 0.1f, 0.1f));

		// TODO: Don't use absolute path xd
		Ref<VulkanTexture> textureTest = Texture::create("resources/textures/brickwall.jpg", "geometry.frag", "textureSampler");
		//modelList.push_back(Ref<Model>::create("resources/models/dust2/source/de_dust2.fbx", mapTransform));
		//meshList.push_back(Ref<Mesh>::create(&vertices, &indices, glm::mat4(1.0f)));

		Ref<Mesh> mesh = Ref<Mesh>::create(&vertices, &indices, glm::vec3(1.0f));

		entityList.push_back(new Entity(mesh, 1, 0.0f, 1)); // Testing character entity (cube)
		entityList.push_back(new Entity(Ref<Model>::create("resources/models/dust2/source/de_dust2.fbx", mapTransform), 1, 0.0f, 0)); // Map entity
	}

	void Application::updateUniforms()
	{
		viewProjection.projection = glm::perspective(glm::radians(70.0f), (float)Window::getSize().first/ (float)Window::getSize().second, appInfo.nearPlane, appInfo.farPlane);
		viewProjection.projection[1][1] *= -1; // Invert the y axis for vulkan (GLM was made for opengl which uses +y as up)
		viewProjection.view = camera->calculateViewMatrix();

		lightSpace.lightSpace = camera->calculateViewMatrix();
			
		renderer->updateUniform("geometry.vert", 0, 0, &viewProjection, sizeof(viewProjection));
		renderer->updateUniform("geometry.vert", 0, 1, &lightSpace, sizeof(lightSpace));

		//renderer->updateUniform("geometry.frag", 0, 1, camera->getCameraPosition(), sizeof(glm::vec3));
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

				updateUniforms();

				//std::cout << "New Frame " << frameCounter << "\n";

				// Record commands
				renderer->beginFrame();
				
				renderer->beginRenderPass();

				for (Entity* entity : entityList)
				{
					entity->act(); // Animate, Physiscs etc
					renderer->renderEntity(renderer->getGraphicsPipeline(), entity);
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