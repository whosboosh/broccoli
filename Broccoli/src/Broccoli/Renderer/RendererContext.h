#pragma once

#include "Broccoli/Core/Ref.h"

namespace Broccoli {
	class RendererContext : public RefCounted
	{
	public:
		RendererContext() = default;
		virtual ~RendererContext() = default;

		virtual void init() = 0;

		static Ref<RendererContext> create();
	};

}