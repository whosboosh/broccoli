#pragma once

#include <stdint.h>

#include <iostream>
#include <string>

namespace Broccoli {
	enum class AssetType : uint16_t
	{
		None = 0,
		Mesh = 1,
		Model = 2,
		Texture = 3
	};

	static AssetType assetTypeFromString(const std::string& assetType)
	{
		if (assetType == "None") { return AssetType::None; }
		if (assetType == "Mesh") { return AssetType::Mesh; }
		if (assetType == "Model") { return AssetType::Model; }
		if (assetType == "Texture") { return AssetType::Texture; }

		std::cout << "No asset found for: " << assetType << "\n";
		return AssetType::None;
	}
}