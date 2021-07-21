#pragma once

#include "Broccoli/Core/Ref.h"

struct GLFWwindow;

namespace Broccoli {
	class Mesh : public RefCounted
	{
	public:
		Mesh();
		~Mesh();
	};

}