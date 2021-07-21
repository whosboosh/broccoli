#pragma once

#include "Broccoli/Core/Ref.h"

struct GLFWwindow;

namespace Broccoli {
	class CommandBuffer : public RefCounted
	{
	public:
		CommandBuffer();
		~CommandBuffer();
	};

}