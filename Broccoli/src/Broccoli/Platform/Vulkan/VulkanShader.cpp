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
		spirv_cross::Compiler compiler(outputBinary);
		auto resources = compiler.get_shader_resources();

		std::cout << "Uniform buffers inside shader: " << name << "\n";
		for (const auto& resource : resources.uniform_buffers)
		{
			const auto& name = resource.name;
			auto& bufferType = compiler.get_type(resource.base_type_id);
			int memberCount = (uint32_t)bufferType.member_types.size();
			uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding); // Binding value
			uint32_t descriptorSet = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet); // Set value
			uint32_t size = (uint32_t)compiler.get_declared_struct_size(bufferType);

			if (descriptorSet >= shaderDescriptorSets.size())
			{
				shaderDescriptorSets.resize(descriptorSet + 1);
			}

			ShaderDescriptorSet& shaderDescriptorSet = shaderDescriptorSets[descriptorSet];

			UniformBuffer* uniformBuffer = new UniformBuffer();
			uniformBuffer->bindingPoint = binding;
			uniformBuffer->size = size;
			uniformBuffer->name = name;
			uniformBuffer->shaderStage = stageFlags;
			shaderDescriptorSet.uniformBuffers[binding] = uniformBuffer;
			//uniformBuffers.at(descriptorSet)[binding] = uniformBuffer;

			//shaderDescriptorSet.uniformBuffers[binding] = uniformBuffers.at(descriptorSet).at(binding);

			std::cout << "Name: " << name << " DescriptorSet: " << descriptorSet << " Binding: " << binding << "\n";
		}

		
		VkDevice logicalDevice = Application::get().getWindow().getRenderContext().As<VulkanContext>()->getLogicalDevice()->getLogicalDevice();

		// Create Descriptor Pool
		// Loop over each set in shader
		for (size_t set = 0; set < shaderDescriptorSets.size(); set++)
		{
			auto& shaderDescriptorSet = shaderDescriptorSets[set];

			if (shaderDescriptorSet.uniformBuffers.size()) // If any uniform buffers exist, calculate pool size
			{
				VkDescriptorPoolSize& typeCount = typeCounts[set].emplace_back();
				typeCount.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				typeCount.descriptorCount = (uint32_t)shaderDescriptorSet.uniformBuffers.size();

			}
			// TODO: Do the same ^^ But for image samplers later

			VkDescriptorPoolCreateInfo descriptorPoolInfo = {};
			descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			descriptorPoolInfo.pNext = nullptr;
			descriptorPoolInfo.poolSizeCount = (uint32_t)typeCounts.at(set).size();
			descriptorPoolInfo.pPoolSizes = typeCounts.at(set).data();
			descriptorPoolInfo.maxSets = 1;

			VkResult result = vkCreateDescriptorPool(logicalDevice, &descriptorPoolInfo, nullptr, &descriptorPool);
			if (result != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create descriptor pool!");
			}

			// Create Descriptor set layout
			std::vector<VkDescriptorSetLayoutBinding> layoutBindings;

			for (auto& [binding, uniformBuffer] : shaderDescriptorSet.uniformBuffers)
			{
				VkDescriptorSetLayoutBinding& layoutBinding = layoutBindings.emplace_back();
				layoutBinding.binding = binding;
				layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				layoutBinding.descriptorCount = 1;
				layoutBinding.stageFlags = stageFlags; // TODO: Auto compute if vertex/frag/compute shader, depending on what type change the stage flags here
				layoutBinding.pImmutableSamplers = nullptr;

				VkWriteDescriptorSet& set = shaderDescriptorSet.writeDescriptorSets[uniformBuffer->name];
				set = {};
				set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				set.descriptorType = layoutBinding.descriptorType;
				set.descriptorCount = 1;
				set.dstBinding = layoutBinding.binding;
			}

			VkDescriptorSetLayoutCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.bindingCount = static_cast<uint32_t>(layoutBindings.size()); // Number of binding infos
			createInfo.pBindings = layoutBindings.data(); // Pointer to binding info

			if (set >= descriptorSetLayouts.size())
			{
				descriptorSetLayouts.resize((size_t)(set + 1));
			}
			result = vkCreateDescriptorSetLayout(logicalDevice, &createInfo, nullptr, &descriptorSetLayouts[set]);
			if (result != VK_SUCCESS) {
				throw std::runtime_error("Failed to create a Descriptor Set Layout!");
			}
		}
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

		const bool optimise = false;
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