#pragma once

#include "Broccoli/Asset/Asset.h"
#include "Broccoli/Renderer/RendererAPI.h"

#ifndef STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif

namespace Broccoli {

	class Texture : public Asset
	{
	public:
		virtual ~Texture() {}
		static Ref<Texture> create(const std::string& filePath, const std::string& uniformName);

		stbi_uc* loadTextureFile(const std::string fileName, int* width, int* height, uint64_t* imageSize);

		virtual uint32_t getHeight() = 0;
		virtual uint32_t getWidth() = 0;
	};
}