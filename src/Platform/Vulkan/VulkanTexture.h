#ifndef BROCCOLI_VULKAN_TEXTURE
#define BROCCOLI_VULKAN_TEXTURE

#include "Renderer/Texture.h"
#include "Renderer/RendererAPI.h"

namespace Broccoli {

	class VulkanTexture : public Texture
	{
	public:
		VulkanTexture(const std::string& filePath, const std::string& shaderName, const std::string& uniformName);
		//~VulkanTexture();

		void createTextureImage();

	private:
		VkDeviceSize imageSize;

		uint32_t mipLevels;

		struct
		{
			VkImage image;
			VkDeviceMemory imageMemory;
			VkImageView imageView;
		} textureImage;
	};

}

#endif