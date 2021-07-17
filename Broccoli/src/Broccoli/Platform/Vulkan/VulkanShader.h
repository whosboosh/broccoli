#pragma once

#include "Broccoli/Core/Ref.h"
#include "Broccoli/Renderer/Shader.h"

#include <iostream>

namespace Broccoli {

	class VulkanShader : public Shader
	{
	public:
		VulkanShader(const std::string& filePath);
		~VulkanShader();

		virtual const std::string& getName() const override { return  name; }

	private:
		std::string name;
	};
}