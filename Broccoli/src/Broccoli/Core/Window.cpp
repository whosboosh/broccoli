#include "Broccoli/Core/Window.h"
#include "Broccoli/Renderer/RendererAPI.h"

namespace Broccoli {



	Window::Window(WindowSpecification windowSpec)
	{
		this->windowSpec = windowSpec;
	}

	// Main window initialisation method
	void Window::init()
	{
		if (!glfwInitialised)
		{
			int status = glfwInit();
			if (!status)
			{
				std::cout << "Failed to initialise GLFW " << status;
				glfwTerminate();
				return;
			}
			glfwInitialised = true;
		}

		if (RendererAPI::getCurrent() == RendererAPIType::Vulkan)
		{
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		}
		mainWindow = glfwCreateWindow(windowSpec.width, windowSpec.height, windowSpec.title.c_str(), nullptr, nullptr);
		if (!mainWindow)
		{
			std::cout << "Error creating GLFW window!";
			glfwTerminate();
			return;
		}

		// Creating renderer context
		rendererContext = RendererContext::create();
		rendererContext->init();

		// Create the swapchain including surface initialisation

		glfwSetWindowUserPointer(mainWindow, this);
		glfwSetFramebufferSizeCallback(mainWindow, framebuffer_size_callback);
		glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwSetCursorPosCallback(mainWindow, mouse_callback);
		glfwSetKeyCallback(mainWindow, key_callback);

	}

	float Window::getXChange()
	{
		GLfloat change = xChange;
		xChange = 0.0f;
		return change;
	}

	float Window::getYChange()
	{
		GLfloat change = yChange;
		yChange = 0.0f;
		return change;
	}

	void Window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}

		if (key == GLFW_KEY_GRAVE_ACCENT && action == GLFW_PRESS)
		{
			theWindow->isControllingGame = !theWindow->isControllingGame;
		}

		if (key >= 0 && key < 1024)
		{
			if (action == GLFW_PRESS)
			{
				theWindow->keys[key] = { true, action };
			}
			else if (action == GLFW_RELEASE)
			{
				theWindow->keys[key] = { false, action };
			}
		}
	}

	void Window::framebuffer_size_callback(GLFWwindow* window, int width, int height)
	{
		auto app = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
		//app->frameBufferResized = true;
	}

	void Window::mouse_callback(GLFWwindow* window, double xpos, double ypos)
	{
		Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
		if (theWindow->mouseFirstMoved)
		{
			theWindow->lastX = xpos;
			theWindow->lastY = ypos;
			theWindow->mouseFirstMoved = false;
		}
		theWindow->xChange = xpos - theWindow->lastX;
		theWindow->yChange = theWindow->lastY - ypos;

		theWindow->lastX = xpos;
		theWindow->lastY = ypos;
	}

	void Window::processEvents()
	{
		glfwPollEvents();
	}

	std::pair<uint32_t, uint32_t> Window::getWindowPos()
	{
		int x, y;
		glfwGetWindowPos(mainWindow, &x, &y);
		return { x, y };
	}

	void Window::setVsync(bool param)
	{
		windowSpec.vsync = param;
		if (RendererAPI::getCurrent() == RendererAPIType::OpenGL)
		{
			param == 1 ? glfwSwapInterval(1) : glfwSwapInterval(0);
		}
		// TODO: Vulkan custom implementation for vsync
	}

	Window::~Window()
	{
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
	}
}