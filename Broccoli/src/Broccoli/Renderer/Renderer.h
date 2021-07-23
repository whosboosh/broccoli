#pragma once

#include "Broccoli/Core/Ref.h"

#include "Broccoli/Renderer/Shader.h"

#include "Broccoli/Renderer/RendererContext.h"

#include "Broccoli/Renderer/Pipeline.h"
#include "Broccoli/Renderer/Mesh.h"
#include "Broccoli/Renderer/CommandBuffer.h"

#include <glm/glm.hpp>

namespace Broccoli {

	class Renderer
	{
	public:
		Renderer();
		~Renderer();

		static Ref<RendererContext> getContext();

		ShaderLibrary* getShaderLibrary() { return geometryShaderLibrary; }
		Ref<Pipeline> getGraphicsPipeline() { return graphicsPipeline; }

		void renderMesh(Ref<CommandBuffer> commandBuffer, Ref<Pipeline> pipeline, Ref<Mesh> mesh, const glm::mat4& transform);

	private:
		ShaderLibrary* geometryShaderLibrary;

		Ref<Pipeline> graphicsPipeline;
	};

}