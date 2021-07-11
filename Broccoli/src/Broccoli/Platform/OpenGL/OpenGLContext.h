#pragma once

#include "Broccoli/Renderer/RendererContext.h"

namespace Broccoli {
	class OpenGLContext : public RendererContext
	{
	public:
		OpenGLContext();
		virtual ~OpenGLContext();

		virtual void init() override;
	};
}