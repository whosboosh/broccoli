#pragma once

#include <stdio.h>
#include <iostream>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_vulkan.h"

#if defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
//#include <glad/glad.h>
#else
#include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#endif
#include <GLFW/glfw3.h>

#include "Broccoli/Renderer/RendererContext.h"
#include "Broccoli/Platform/Vulkan/VulkanSwapchain.h"

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

		GLFWwindow* getWindow() { return mainWindow; }

		// New refcactor methods
		void init();
		void processEvents();
		void swapBuffers() { glfwSwapBuffers(mainWindow); }

		// Get the current window size
		static std::pair<uint32_t, uint32_t> getSize() { return { width, height }; }
		std::pair<uint32_t, uint32_t> getWindowPos();

		Ref<RendererContext> getRenderContext() { return rendererContext; }

		void setVsync(bool param);
		bool getIsControllingGame() { return isControllingGame; }

		static WindowSpecification getWindowSpec() { return windowSpec; }

		inline static bool frameBufferResized;

		// Mouse functions
		float getXChange();
		float getYChange();
		KeyAction* getKeys() { return keys; }

		~Window();
	private:
		GLFWwindow* mainWindow;
		
		inline static WindowSpecification windowSpec;
		inline static int width, height;

		bool glfwInitialised = false;
		bool isControllingGame = true;

		// Global
		KeyAction keys[1024] = { 0, 0 };

		// Mouse callback data
		bool mouseFirstMoved = true;
		float lastX;
		float lastY;
		float xChange = 0.0f;
		float yChange = 0.0f;

		static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
		static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void framebuffer_size_callback(GLFWwindow* window, int width, int height);


		Ref<RendererContext> rendererContext;
	};
}