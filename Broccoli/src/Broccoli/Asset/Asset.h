#pragma once

#include <stdint.h>

#include <iostream>
#include <string>

#include "Broccoli/Core/Ref.h"

#include "Broccoli/Asset/AssetType.h"

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