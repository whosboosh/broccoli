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
		Broccoli::ApplicationInfo info;
		info.Name = "Cloud Gaming Engine";
		info.windowHeight = 900;
		info.windowWidth = 1600;
		info.fullscreen = true;
		info.vsync = false;

		WindowSpecification windowSpec;
		windowSpec.title = appInfo.Name;
		windowSpec.width = appInfo.windowWidth;
		windowSpec.height = appInfo.windowHeight;
		windowSpec.fullscreen = appInfo.fullscreen;

		window = new Window(windowSpec);
		window->enableVulkan();
	}


	void Application::Run()
	{
		while (isRunning)
		{
			frameCounter = 0;

			//glfwPollEvents
			if (!isMinimised)
			{
				std::cout << "New Frame";
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
