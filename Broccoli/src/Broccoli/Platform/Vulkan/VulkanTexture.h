#pragma once

#include "Broccoli/Renderer/Texture.h"

namespace Broccoli {

	class VulkanTexture : public Texture
	{
	public:
		VulkanTexture(const std::string& fileName);
		//~VulkanTexture();

		virtual uint32_t getHeight() override;
		virtual uint32_t getWidth() override;

	private:
		int width, height;
		VkDeviceSize imageSize;

		std::string fileName;
		uint32_t mipLevels;
	};

}