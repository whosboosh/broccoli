#ifndef BROCCOLI_RENDERER_CONTEXT_HEADER
#define BROCCOLI_RENDERER_CONTEXT_HEADER

#include "Core/Ref.h"

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

#endif