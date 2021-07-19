#pragma once

#include "Broccoli/Core/Ref.h"

#include "Broccoli/Renderer/Shader.h"

#include "Broccoli/Renderer/RendererContext.h"

#include "Broccoli/Renderer/Pipeline.h"

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

		Ref<Pipeline> graphicsPipeline;
	};

}