#pragma once

#include "Broccoli/Core/Ref.h"
#include "Broccoli/Renderer/Shader.h"

#include "Broccoli/Utilities/VulkanUtilities.h"

#include <iostream>

namespace Broccoli {

	class VulkanShader : public Shader
	{
	public:
		VulkanShader(const std::string& filePath, VkShaderStageFlagBits stageFlags);
		~VulkanShader();

		virtual const std::string& getName() const override { return name; }

		virtual const VkShaderStageFlagBits getStageFlags() const override { return stageFlags; }

		VkShaderModule createShaderModule(const std::vector<char>& code);

	private:
		VkShaderStageFlagBits stageFlags;

		std::string name;
	};
}