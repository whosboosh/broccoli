#pragma once

#include "VulkanShader.h"

#include "Broccoli/Platform/Vulkan/VulkanContext.h"

#include "Broccoli/Core/Application.h"

#include <spirv_cross/spirv_glsl.hpp>
#include <spirv-tools/libspirv.h>
#include <shaderc/shaderc.hpp>

namespace Broccoli {

	VulkanShader::VulkanShader(const std::string& filePath, VkShaderStageFlagBits stageFlags) : stageFlags(stageFlags), filePath(filePath)
	{
		std::cout << "Vulkan shader created: " + filePath + "\n";

		size_t found = filePath.find_last_of("/\\");
		name = found != std::string::npos ? filePath.substr(found + 1) : filePath;
		//found = name.find_last_of(".");
		//name = found != std::string::npos ? name.substr(0, found) : name;

		// TODO: Add loading shader from file and creating vulkan specific functions here
		auto shaderCode = readFile(filePath);

		// Compile and translate glsl to spirV using Vulkan SDK libs
		std::vector<uint32_t> outputBinary;
		compileGlslToSpirv(shaderCode, outputBinary);

		VkShaderModule shaderModule = createShaderModule(outputBinary);

		shaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStageCreateInfo.stage = stageFlags;
		shaderStageCreateInfo.module = shaderModule;
		shaderStageCreateInfo.pName = "main";


		std::cout << "Shader created with name: " << name << "\n";


		// Load descriptor sets from shader dynamically
		//spirv_cross::Compiler compiler(shaderCode);
	}

	VulkanShader::~VulkanShader()
	{
	}

	
	VkShaderModule VulkanShader::createShaderModule(const std::vector<uint32_t>& shaderData)
	{
		VkDevice logicalDevice = Application::get().getWindow().getRenderContext().As<VulkanContext>()->getLogicalDevice()->getLogicalDevice();

		// Shader module creation info
		VkShaderModuleCreateInfo shaderModuleCreateInfo = {};
		shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		shaderModuleCreateInfo.codeSize = shaderData.size() * sizeof(uint32_t); // Size of code
		shaderModuleCreateInfo.pCode = shaderData.data(); // Pointer to code (uint32_t pointer type)

		VkShaderModule shaderModule;

		VkResult result = vkCreateShaderModule(logicalDevice, &shaderModuleCreateInfo, nullptr, &shaderModule);
		if (result != VK_SUCCESS) {
			throw std::runtime_error("Failed to creata a shader module");
		}

		return shaderModule;
	}

	static const char* VkShaderStageCachedFileExtension(VkShaderStageFlagBits stage)
	{
		switch (stage)
		{
		case VK_SHADER_STAGE_VERTEX_BIT:    return ".cached_vulkan.vert";
		case VK_SHADER_STAGE_FRAGMENT_BIT:  return ".cached_vulkan.frag";
		case VK_SHADER_STAGE_COMPUTE_BIT:   return ".cached_vulkan.comp";
		}
		return "";
	}

	static shaderc_shader_kind VkShaderStageToShaderC(VkShaderStageFlagBits stage)
	{
		switch (stage)
		{
		case VK_SHADER_STAGE_VERTEX_BIT:    return shaderc_vertex_shader;
		case VK_SHADER_STAGE_FRAGMENT_BIT:  return shaderc_fragment_shader;
		case VK_SHADER_STAGE_COMPUTE_BIT:   return shaderc_compute_shader;
		}
		return (shaderc_shader_kind)0;
	}

	void VulkanShader::compileGlslToSpirv(std::vector<char> shaderCode, std::vector<uint32_t>& outputBinary)
	{
		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		
		options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);
		options.SetWarningsAsErrors();
		options.SetGenerateDebugInfo();

		const bool optimise = true;
		if (optimise) options.SetOptimizationLevel(shaderc_optimization_level_performance);

		// Compile the shader
		std::string shaderString(shaderCode.begin(), shaderCode.end());
		shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(shaderString, VkShaderStageToShaderC(stageFlags), filePath.c_str(), options);

		if (module.GetCompilationStatus() != shaderc_compilation_status_success)
		{
			throw std::runtime_error(module.GetErrorMessage());
		}

		const uint8_t* begin = (const uint8_t*)module.cbegin();
		const uint8_t* end = (const uint8_t*)module.cend();
		const ptrdiff_t size = end - begin;

		outputBinary = std::vector<uint32_t>(module.cbegin(), module.cend());
		
		
		// Cache compiled shader	
		auto extension = VkShaderStageCachedFileExtension(stageFlags);
		auto cachePath = filePath;

		std::string::size_type i = filePath.find(name);
		if (i != std::string::npos)
		{
			cachePath.erase(i, name.length());
		}
		cachePath = cachePath + "cache/" + extension;

		std::cout << cachePath;

		FILE* f;
		fopen_s(&f, cachePath.c_str(), "wb");
		fwrite(outputBinary.data(), sizeof(uint32_t), outputBinary.size(), f);
		fclose(f);
		
	}
}