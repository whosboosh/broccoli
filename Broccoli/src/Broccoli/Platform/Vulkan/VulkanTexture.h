#pragma once

#include "Broccoli/Renderer/Texture.h"

namespace Broccoli {

	class VulkanTexture : public Texture
	{
	public:
		VulkanTexture(const std::string& fileName);
		~VulkanTexture();

	private:

		uint32_t mipLevels;
	};

}