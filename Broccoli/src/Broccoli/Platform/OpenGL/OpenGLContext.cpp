#include "OpenGLContext.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

namespace Broccoli {
	Broccoli::OpenGLContext::OpenGLContext()
	{
	}

	Broccoli::OpenGLContext::~OpenGLContext()
	{
	}

	void OpenGLContext::init(GLFWwindow* windowHandle)
	{
		glfwMakeContextCurrent(windowHandle);

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD" << std::endl;
			return;
		}

		// OpenGL version
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		// Core Profile
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		// Allow forward compatiblity
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	}
}