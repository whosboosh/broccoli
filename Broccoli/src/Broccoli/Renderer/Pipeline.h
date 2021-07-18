#pragma once

#include "Broccoli/Core/Ref.h"
#include "Broccoli/Renderer/RendererAPI.h"

namespace Broccoli {
	
	class Pipeline : public RefCounted
	{
	public:
		Pipeline() = default;
		virtual ~Pipeline() = default;

		//static Ref<Pipeline> create(const PipelineSpecification& spec);
	private:
		;
	};

}