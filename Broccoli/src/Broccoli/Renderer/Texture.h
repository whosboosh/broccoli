#pragma once

#include "Broccoli/Asset/Asset.h"

namespace Broccoli {

	class Texture : public Asset
	{
	public:
		Texture(const std::string& fileName);
		virtual ~Texture() = 0;
	private:
		int width, height;
		int imageSize;

		std::string fileName;
	};

}