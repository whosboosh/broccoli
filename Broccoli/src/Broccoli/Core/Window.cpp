#include "Window.h"

namespace Broccoli {



	Window::Window(WindowSpecification windowSpec)
	{
		width = windowSpec.width;
		height = windowSpec.height;
	}

	void Window::enableOpenGL()
	{
		std::cout << "opening open gl\n";
		if (!isClosed)
		{
			glfwSetWindowShouldClose(mainWindow, GL_TRUE);
			this->keys[1023] = { false };
			this->keys[82] = { false }; // Reset key for toggling api
			isClosed = true;
		}
		else {
			mouseFirstMoved = true;
			if (!glfwInit())
			{
				printf("Error Initialising GLFW");
				glfwTerminate();
				return;
			}

			// Setup GLFW Windows Properties
			// OpenGL version
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
			// Core Profile
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
			// Allow forward compatiblity
			glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

			// Enable multisampling buffer for MSAA
			//glfwWindowHint(GLFW_SAMPLES, 4);

			// Create the window
			mainWindow = glfwCreateWindow(width, height, "OpenGL", NULL, NULL);
			if (!mainWindow)
			{
				printf("Error creating GLFW window!");
				glfwTerminate();
				return;
			}

			// Get buffer size information
			glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

			// Set the current context
			glfwMakeContextCurrent(mainWindow);

			if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
			{
				std::cout << "Failed to initialize GLAD" << std::endl;
				return;
			}

			glEnable(GL_DEPTH_TEST);
			//glEnable(GL_MULTISAMPLE);

			// Create Viewport
			glViewport(0, 0, bufferWidth, bufferHeight);

			glfwSwapInterval(0); // Disable vsync

			glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			glfwSetWindowUserPointer(mainWindow, this);
			glfwSetCursorPosCallback(mainWindow, mouse_callback);
			glfwSetKeyCallback(mainWindow, key_callback);
			glfwSetFramebufferSizeCallback(mainWindow, framebuffer_size_callback);

			isClosed = false;

			// IMGUI context
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO(); (void)io;
			ImGui::StyleColorsDark();

			// Setup Platform/Renderer backends
			ImGui_ImplGlfw_InitForOpenGL(mainWindow, true);
			ImGui_ImplOpenGL3_Init("#version 330");
		}
	}

	void Window::enableVulkan()
	{
		std::cout << "opening vulkan \n";
		if (!isClosed)
		{
			glfwSetWindowShouldClose(mainWindow, GL_TRUE);
			this->keys[1023] = { false };
			this->keys[82] = { false }; // Reset key for toggling api
			isClosed = true;
		}
		else {
			mouseFirstMoved = true;
			// Initialse GLFW
			if (!glfwInit())
			{
				printf("Error Initialising GLFW");
				glfwTerminate();
				return;
			}

			// Set GLFW to not work with opengl
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

			mainWindow = glfwCreateWindow(width, height, "Vulkan", nullptr, nullptr);
			if (!mainWindow)
			{
				printf("Error creating GLFW window!");
				glfwTerminate();
				return;
			}

			glfwSetWindowUserPointer(mainWindow, this);
			glfwSetFramebufferSizeCallback(mainWindow, framebuffer_size_callback);
			glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			glfwSetCursorPosCallback(mainWindow, mouse_callback);
			glfwSetKeyCallback(mainWindow, key_callback);

			isClosed = false;
		}
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

	Window::~Window()
	{
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
	}
}