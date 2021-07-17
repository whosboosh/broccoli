#pragma once

#include "VulkanShader.h"

namespace Broccoli {

	VulkanShader::VulkanShader(const std::string& filePath)
	{
		std::cout << "Vulkan shader created: "+filePath+"\n";

		size_t found = filePath.find_last_of("/\\");
		name = found != std::string::npos ? filePath.substr(found + 1) : filePath;
		found = name.find_last_of(".");
		name = found != std::string::npos ? name.substr(0, found) : name;

		// TODO: Add loading shader from file and creating vulkan specific functions here
	}

	VulkanShader::~VulkanShader()
	{
	}

}