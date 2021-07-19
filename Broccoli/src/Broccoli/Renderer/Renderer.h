#pragma once

#include "Broccoli/Core/Ref.h"

#include "Broccoli/Renderer/Shader.h"

#include "Broccoli/Renderer/RendererContext.h"

#include "Broccoli/Renderer/Pipeline.h"

#include <glm/glm.hpp>

namespace Broccoli {
	
	struct Vertex {
		glm::vec3 pos; // vertex position (x,y,z)
		glm::vec3 col; // vertex color (r,g,b)
		glm::vec2 tex; // Texture coords (u, v)
		glm::vec3 normal; // Normals
	};

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