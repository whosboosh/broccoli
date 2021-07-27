#pragma once

#include "VulkanShader.h"

#include "Broccoli/Platform/Vulkan/VulkanContext.h"

#include "Broccoli/Core/Application.h"

#include <spirv_cross/spirv_glsl.hpp>
#include <spirv-tools/libspirv.h>
#include <shaderc/shaderc.hpp>

namespace Broccoli {

	VulkanShader::~VulkanShader()
	{
	}

	VulkanShader::VulkanShader(const std::string& filePath, VkShaderStageFlagBits stageFlags) : stageFlags(stageFlags), filePath(filePath)
	{
		VkDevice logicalDevice = Application::get().getWindow().getRenderContext().As<VulkanContext>()->getLogicalDevice()->getLogicalDevice();
		VkPhysicalDevice physicalDevice = Application::get().getWindow().getRenderContext().As<VulkanContext>()->getLogicalDevice()->getPhysicalDevice()->getVulkanPhysicalDevice();
		VulkanSwapchain swapChain = Application::get().getWindow().getVulkanSwapChain();

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
			uniformBuffer->size = size; // This might be redundant TODO
			uniformBuffer->name = name;
			uniformBuffer->shaderStage = stageFlags;
			uniformBuffer->descriptorSize = size;

			// Create memory for uniform buffer
			createBuffer(physicalDevice, logicalDevice, uniformBuffer->descriptorSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &uniformBuffer->uniformBuffer, &uniformBuffer->uniformMemory);

			VkDescriptorBufferInfo bufferInfo = {};
			bufferInfo.buffer = uniformBuffer->uniformBuffer; // Buffer to get data from
			bufferInfo.offset = 0; // Position where data starts
			bufferInfo.range = size; // Size of data 

			uniformBuffer->descriptor = bufferInfo;

			shaderDescriptorSet.uniformBuffers[binding] = uniformBuffer;

			std::cout << "Name: " << name << " DescriptorSet: " << descriptorSet << " Binding: " << binding << "\n";
		}



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

			// TODO: Move this to main renderer so we use the same descriptor pool for every shader
			// Reduces memory usage and is more optimised. PoolSizeCount will be an aggregation of typeCounts for every shader. But need to know typeCounts for every shader
			//  before creating the descriptor pool so some refactoring might be necessary
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

			// Create Descriptor set layout for this set
			std::vector<VkDescriptorSetLayoutBinding> layoutBindings = {};

			for (auto& [binding, uniformBuffer] : shaderDescriptorSet.uniformBuffers)
			{
				VkDescriptorSetLayoutBinding layoutBinding = {};
				layoutBinding.binding = binding;
				layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				layoutBinding.descriptorCount = 1;
				layoutBinding.stageFlags = stageFlags; // TODO: Auto compute if vertex/frag/compute shader, depending on what type change the stage flags here
				layoutBinding.pImmutableSamplers = nullptr;

				layoutBindings.push_back(layoutBinding);

				VkWriteDescriptorSet& setWrite = shaderDescriptorSet.writeDescriptorSets[uniformBuffer->name];
				//setWrite = {};
				setWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				setWrite.descriptorType = layoutBinding.descriptorType;
				setWrite.descriptorCount = 1;
				setWrite.dstBinding = layoutBinding.binding;
				setWrite.dstSet = set;
				setWrite.dstArrayElement = 0;
				setWrite.pBufferInfo = &uniformBuffer->descriptor; // Use later when we actually write info to this binding value VkDescriptorBufferInfo
			}

			std::cout << "LAYOUT BINDING SIZE: " << layoutBindings.size() << "\n";

			// Create the descriptor set layout for this set
			VkDescriptorSetLayoutCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			//createInfo.pNext = nullptr;
			createInfo.bindingCount = static_cast<uint32_t>(layoutBindings.size()); // Number of binding infos
			createInfo.pBindings = layoutBindings.data(); // Pointer to binding info

			result = vkCreateDescriptorSetLayout(logicalDevice, &createInfo, nullptr, &shaderDescriptorSet.descriptorSetLayout);
			if (result != VK_SUCCESS) {
				throw std::runtime_error("Failed to create a Descriptor Set Layout!");
			}

			// Allocate descriptor sets
			VkDescriptorSetAllocateInfo setAllocInfo = {};
			setAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			setAllocInfo.descriptorPool = descriptorPool;
			setAllocInfo.descriptorSetCount = 1; // TODO: Might need to create a descriptor set for each swapchainimage. See how it works with 1 for now? Thonk
			setAllocInfo.pSetLayouts = &shaderDescriptorSet.descriptorSetLayout;

			result = vkAllocateDescriptorSets(logicalDevice, &setAllocInfo, &shaderDescriptorSet.descriptorSet);
			if (result != VK_SUCCESS) {
				throw std::runtime_error("Failed to allocate Descriptor Sets");
			}

			// Now the descriptor sets are allocated, we can create a function to call vkUpdateDescriptorSets on the specific descriptor setWrite object
			std::vector<VkWriteDescriptorSet> setWrites = {};
			for (auto& [binding, uniformBuffer] : shaderDescriptorSet.uniformBuffers)
			{
				setWrites.push_back(shaderDescriptorSet.writeDescriptorSets[uniformBuffer->name]);
			}

			vkUpdateDescriptorSets(logicalDevice, static_cast<uint32_t>(setWrites.size()), setWrites.data(), 0, nullptr);
			
		}
	}

	void VulkanShader::updateDescriptorSet(int set, int binding)
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