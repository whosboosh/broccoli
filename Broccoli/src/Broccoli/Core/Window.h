#pragma once

#include <stdio.h>
#include <iostream>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_vulkan.h"

#if defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
#include <glad/glad.h>
#else
#include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#endif
#include <GLFW/glfw3.h>

namespace Broccoli {

	struct KeyAction {
		int key;
		int action;
	};

	struct WindowSpecification
	{
		std::string title = "Broccoli";
		uint32_t width = 1600;
		uint32_t height = 900;
		bool fullscreen = false;
		bool vsync = false;
	};


	// Creates a GLFW surface
	// Specific methods for opengl and vulkan are handled via switchGL() and switchVulkan()
	class Window
	{
	public:
		Window();
		Window(WindowSpecification windowSpec);

		void enableOpenGL();
		void enableVulkan();

		bool getShouldClose() { return glfwWindowShouldClose(mainWindow); }
		void swapBuffers() { glfwSwapBuffers(mainWindow); }
		KeyAction* getKeys() { return keys; }
		float getXChange();
		float getYChange();

		// OpenGL specific functions
		int getBufferWidth() { return bufferWidth; }
		int getBufferHeight() { return bufferHeight; }

		GLFWwindow* getWindow() { return mainWindow; }

		void setIsClosed(bool isClosed) { this->isClosed = isClosed; }
		void setShouldSwitch(bool shouldSwitch) { this->shouldSwitch = shouldSwitch; }
		bool getIsClosed() { return isClosed; }
		bool getShouldSwitch() { return shouldSwitch; }
		bool getIsControllingGame() { return isControllingGame; }

		~Window();
	private:
		GLFWwindow* mainWindow;

		bool isControllingGame = true;

		bool isClosed = true;
		bool shouldSwitch = false;

		int width, height;

		// OpenGL specific
		int bufferWidth, bufferHeight;

		// Global
		KeyAction keys[1024] = { 0, 0 };

		float lastX;
		float lastY;
		float xChange = 0.0f;
		float yChange = 0.0f;

		bool mouseFirstMoved = true;
		static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
		static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	};
}