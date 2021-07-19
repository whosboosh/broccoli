#pragma once

#include "Broccoli/Core/Ref.h"
#include "Broccoli/Renderer/RendererAPI.h"

#include "Broccoli/Renderer/Shader.h"

namespace Broccoli {
	
	enum class PrimitiveTopology
	{
		None = 0,
		Points,
		Lines,
		Triangles,
		LineStrip,
		TriangleStrip,
		TriangleFan
	};

	struct PipelineSpecification {
		bool backFaceCulling = false;
		bool depthTest = true;
		bool depthWrite = true;
		bool wireFrame = false;
		float lineWidth = 1.0f;
		PrimitiveTopology topology = PrimitiveTopology::Triangles;
	};

	class Pipeline : public RefCounted
	{
	public:
		Pipeline() = default;
		virtual ~Pipeline() = default;

		static Ref<Pipeline> create(const PipelineSpecification& spec, ShaderLibrary* shaderLibrary);
	private:
		
	};

}