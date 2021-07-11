#pragma once

#include "Broccoli/Renderer/RendererContext.h"

namespace Broccoli {
	class VulkanContext : public RendererContext
	{
	public:
		VulkanContext();
		virtual ~VulkanContext();
	
		virtual void init() override;
	};
}