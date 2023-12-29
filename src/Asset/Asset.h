#ifndef BROCCOLI_ASSET_HEADER
#define BROCCOLI_ASSET_HEADER

#include <stdint.h>

#include <iostream>
#include <string>

#include "Core/Ref.h"

#include "Asset/AssetType.h"

namespace Broccoli {
	class Asset : public RefCounted
	{
	public:
		virtual ~Asset() {}

		virtual AssetType getAssetType() const { return assetType; }
		virtual void setAssetType(AssetType assetType) { this->assetType = assetType; }

	private:
		AssetType assetType = AssetType::None;
	};
}

#endif