#pragma once

#include "Broccoli/Core/Ref.h"

#include "Broccoli/Renderer/Shader.h"

#include "Broccoli/Renderer/RendererContext.h"

namespace Broccoli {
	
	class Renderer
	{
	public:
		Renderer();
		~Renderer();

		static Ref<RendererContext> getContext();

		ShaderLibrary* getShaderLibrary() { return shaderLibrary; }

	private:
		ShaderLibrary* shaderLibrary;
	};

}