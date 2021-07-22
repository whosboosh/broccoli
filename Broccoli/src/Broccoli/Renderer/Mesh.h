#pragma once

#include "Broccoli/Asset/Asset.h"

struct GLFWwindow;

namespace Broccoli {

	// Mesh is a collection of vertices
	// This class will contain functions like getting the vertex buffers and index buffers
	class Mesh : public Asset
	{
	public:
		Mesh();
		~Mesh();
	};

}