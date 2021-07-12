#pragma once

#include "Broccoli/Renderer/RendererContext.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Broccoli {
	class OpenGLContext : public RendererContext
	{
	public:
		OpenGLContext();
		virtual ~OpenGLContext();

		virtual void init() override;

	private:
		GLFWwindow* windowHandle;
	};
}