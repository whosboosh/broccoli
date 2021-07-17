#pragma once

#include "VulkanShader.h"

#include "Broccoli/Platform/Vulkan/VulkanContext.h"

#include "Broccoli/Core/Application.h"

namespace Broccoli {

	VulkanShader::VulkanShader(const std::string& filePath, VkShaderStageFlagBits stageFlags) : stageFlags(stageFlags)
	{
		std::cout << "Vulkan shader created: " + filePath + "\n";

		size_t found = filePath.find_last_of("/\\");
		name = found != std::string::npos ? filePath.substr(found + 1) : filePath;
		found = name.find_last_of(".");
		name = found != std::string::npos ? name.substr(0, found) : name;

		// TODO: Add loading shader from file and creating vulkan specific functions here
		auto shaderCode = readFile(filePath);

		VkShaderModule shaderModule = createShaderModule(shaderCode);
	}

	VulkanShader::~VulkanShader()
	{
	}

	
	VkShaderModule VulkanShader::createShaderModule(const std::vector<char>& code)
	{
		VkDevice logicalDevice = Application::get().getWindow().getRenderContext().As<VulkanContext>()->getLogicalDevice()->getLogicalDevice();

		// Shader module creation info
		VkShaderModuleCreateInfo shaderModuleCreateInfo = {};
		shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		shaderModuleCreateInfo.codeSize = code.size(); // Size of code
		shaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(code.data()); // Pointer to code (uint32_t pointer type)

		VkShaderModule shaderModule;
		//Application::get().getWindow().getRenderContext().As<VulkanContext>()->getLogicalDevice()->getLogicalDevice()

		VkResult result = vkCreateShaderModule(logicalDevice, &shaderModuleCreateInfo, nullptr, &shaderModule);

		//if (result != VK_SUCCESS) {
			//throw std::runtime_error("Failed to creata a shader module");
		//}

		return shaderModule;
	}

}