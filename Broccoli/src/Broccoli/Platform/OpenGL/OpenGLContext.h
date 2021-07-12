#pragma once

#include "Broccoli/Renderer/RendererContext.h"

struct GLFWwindow;

namespace Broccoli {
	class OpenGLContext : public RendererContext
	{
	public:
		OpenGLContext();
		virtual ~OpenGLContext();

		virtual void init(GLFWwindow* windowHandle) override;

	private:
		GLFWwindow* windowHandle;
	};
}