#pragma once

#include "Broccoli/Renderer/Texture.h"

namespace Broccoli {

	class VulkanTexture : public Texture
	{
	public:
		VulkanTexture(const std::string& fileName, const std::string& uniformName);
		//~VulkanTexture();

		void createTextureImage();

		virtual uint32_t getHeight() override;
		virtual uint32_t getWidth() override;

		int getDescriptorLoc() { return descriptorLoc; }

	private:
		int width, height;
		VkDeviceSize imageSize;

		std::string fileName;
		uint32_t mipLevels;

		struct
		{
			VkImage image;
			VkDeviceMemory imageMemory;
			VkImageView imageView;
		} textureImage;

		int descriptorLoc;
	};

}