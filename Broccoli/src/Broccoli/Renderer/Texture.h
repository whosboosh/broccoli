#pragma once

#include "Broccoli/Asset/Asset.h"

#ifndef STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif

namespace Broccoli {

	class Texture : public Asset
	{
	public:
		virtual ~Texture() {}
		static Ref<Texture> create(const std::string& filePath, const std::string& shaderName, const std::string& uniformName);

		stbi_uc* loadTextureFile(const std::string fileName, int* width, int* height, uint64_t* imageSize);

		uint32_t getHeight() { return height; }
		uint32_t getWidth() { return width; }

		int getShaderId() { return shaderId; }

		int getTextureId() { return textureId; }
		void setTextureId(int texId) { textureId = texId; }

		std::string getFilePath() { return filePath; }

	private:


	protected:
		std::string filePath;

		int shaderId;
		int textureId;
		int width = 0, height = 0;
	};
}