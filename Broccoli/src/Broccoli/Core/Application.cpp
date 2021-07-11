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

namespace Broccoli {
	Application::Application(int argc, char** argvv)
	{
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
	}


	void Application::Run()
	{
		while (isRunning)
		{
			window->processEvents();
			if (!isMinimised)
			{
				std::cout << "New Frame " << frameCounter << "\n";
				// Begin render frame
				
				// Submit imgui

				//Swap buffers
			}

			float time = glfwGetTime();
			float delta = time - lastFrameTime;
			frameCounter++;
		}
	}
}
