#ifndef BROCCOLI_ASSET_TYPE_HEADER
#define BROCCOLI_ASSET_TYPE_HEADER

#include <stdint.h>

#include <iostream>
#include <string>

namespace Broccoli {
	enum class AssetType : uint16_t
	{
		None = 0,
		Mesh = 1,
		Model = 2,
		Texture = 3,
		Map = 4
	};

	static AssetType assetTypeFromString(const std::string& assetType)
	{
		if (assetType == "None") { return AssetType::None; }
		if (assetType == "Mesh") { return AssetType::Mesh; }
		if (assetType == "Model") { return AssetType::Model; }
		if (assetType == "Texture") { return AssetType::Texture; }
		if (assetType == "Map") { return AssetType::Map; }

		std::cout << "No asset found for: " << assetType << "\n";
		return AssetType::None;
	}
}

#endif