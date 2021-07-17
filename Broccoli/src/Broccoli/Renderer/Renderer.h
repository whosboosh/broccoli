#pragma once

#include "Broccoli/Core/Ref.h"

#include "Broccoli/Renderer/Shader.h"

namespace Broccoli {
	
	class Renderer
	{
	public:
		Renderer();
		~Renderer();

		void init();

		ShaderLibrary* getShaderLibrary() { return shaderLibrary; }

	private:
		ShaderLibrary* shaderLibrary;
	};

}