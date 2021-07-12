#pragma once

#include "Broccoli/Core/Ref.h"

struct GLFWwindow;

namespace Broccoli {
	class RendererContext : public RefCounted
	{
	public:
		RendererContext() = default;
		virtual ~RendererContext() = default;

		virtual void init(GLFWwindow* windowHandle) = 0;

		static Ref<RendererContext> create();
	};

}