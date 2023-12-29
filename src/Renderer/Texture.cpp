#include "Texture.h"

#include "Renderer/RendererAPI.h"

#include "Platform/Vulkan/VulkanTexture.h"

namespace Broccoli {
	Ref<Texture> Texture::create(const std::string& filePath, const std::string& shaderName, const std::string& uniformName)
	{
		switch (RendererAPI::getCurrent())
		{
		case RendererAPIType::None: return nullptr;
		//case RendererAPIType::OpenGL: return Ref<OpenGLTexture>::create(path, properties);
		case RendererAPIType::Vulkan: return Ref<VulkanTexture>::create(filePath, shaderName, uniformName);
		default:
			return nullptr;
		}
	}

	stbi_uc* Texture::loadTextureFile(const std::string fileName, int* width, int* height, uint64_t* imageSize)
	{
		// Number of channels the image uses
		int channels;

		// Load pixel data for image
		std::string fileLoc = "" + fileName;
		stbi_uc* image = stbi_load(fileLoc.c_str(), width, height, &channels, STBI_rgb_alpha);

		if (!image) {
			throw std::runtime_error("Failed to load a texture file! (" + fileName + ")");
		}

		// Calculate image size using given data
		*imageSize = *width * *height * 4;

		return image;
	}
}