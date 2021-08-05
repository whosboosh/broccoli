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

	void VulkanShader::cleanup()
	{
		std::cout << "Cleanup for vulkan shader\n";
	}

	VulkanShader::VulkanShader(const std::string& filePath, VkShaderStageFlagBits stageFlags) : stageFlags(stageFlags), filePath(filePath)
	{
		VkDevice logicalDevice = VulkanContext::get()->getLogicalDevice()->getLogicalDevice();
		VkPhysicalDevice physicalDevice = VulkanContext::get()->getLogicalDevice()->getPhysicalDevice()->getVulkanPhysicalDevice();
		VulkanSwapchain swapChain = VulkanContext::get()->getVulkanSwapChain();

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

		std::vector<VkDescriptorSetLayoutBinding> layoutBindings = {};

		// Generate uniform buffers based on shader content
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
			uniformBuffer->uniformBuffer.resize(swapChain.getSwapChainImageCount());
			uniformBuffer->uniformMemory.resize(swapChain.getSwapChainImageCount());

			for (size_t i = 0; i < swapChain.getSwapChainImageCount(); i++)
			{
				createBuffer(physicalDevice, logicalDevice, uniformBuffer->descriptorSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &uniformBuffer->uniformBuffer[i], &uniformBuffer->uniformMemory[i]);

				VkDescriptorBufferInfo bufferInfo = {};
				bufferInfo.buffer = uniformBuffer->uniformBuffer[i]; // Buffer to get data from
				bufferInfo.offset = 0; // Position where data starts
				bufferInfo.range = size; // Size of data 

				uniformBuffer->descriptor = bufferInfo;
			}

			// Create descriptor set layout for this uniform
			VkDescriptorSetLayoutBinding& layoutBinding = uniformBuffer->layoutBinding;
			layoutBinding.binding = binding;
			layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			layoutBinding.descriptorCount = 1;
			layoutBinding.stageFlags = stageFlags; // TODO: Auto compute if vertex/frag/compute shader, depending on what type change the stage flags here
			layoutBinding.pImmutableSamplers = nullptr;
			layoutBindings.push_back(layoutBinding);

			// Create pool size for this uniform buffer (Used when we create the descriptor pool)
			VkDescriptorPoolSize poolSize = {};
			poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			poolSize.descriptorCount = static_cast<uint32_t>(uniformBuffer->uniformBuffer.size());
			poolSizes.push_back(poolSize);

			shaderDescriptorSet.uniformBuffers[binding] = uniformBuffer;

			std::cout << "Name: " << name << " DescriptorSet: " << descriptorSet << " Binding: " << binding << "\n";
		}

		// Create Descriptor Pool
		// TODO: Move this to main renderer so we use the same descriptor pool for every shader
		VkDescriptorPoolCreateInfo descriptorPoolInfo = {};
		descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size()); //(uint32_t)typeCounts.at(set).size();
		descriptorPoolInfo.pPoolSizes = poolSizes.data();
		descriptorPoolInfo.maxSets = static_cast<uint32_t>(swapChain.getSwapChainImageCount());
		VkResult result = vkCreateDescriptorPool(logicalDevice, &descriptorPoolInfo, nullptr, &descriptorPool);
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create descriptor pool!");
		}

		// Loop over each descriptor "set" for creation
		for (size_t set = 0; set < shaderDescriptorSets.size(); set++)
		{
			auto& shaderDescriptorSet = shaderDescriptorSets[set];

			// Create Descriptor Set Layout with given bindings
			VkDescriptorSetLayoutCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			createInfo.bindingCount = static_cast<uint32_t>(layoutBindings.size()); // Number of binding infos
			createInfo.pBindings = layoutBindings.data(); // Pointer to binding info

			result = vkCreateDescriptorSetLayout(logicalDevice, &createInfo, nullptr, &shaderDescriptorSet.descriptorSetLayout);
			if (result != VK_SUCCESS) {
				throw std::runtime_error("Failed to create a Descriptor Set Layout!");
			}

			shaderDescriptorSet.descriptorSets.resize(swapChain.getSwapChainImageCount());
			shaderDescriptorSet.writeDescriptorSets.resize(swapChain.getSwapChainImageCount());

			std::vector<VkDescriptorSetLayout> setLayouts(swapChain.getSwapChainImageCount(), shaderDescriptorSet.descriptorSetLayout);

			VkDescriptorSetAllocateInfo setAllocInfo = {};
			setAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			setAllocInfo.descriptorPool = descriptorPool; // Pool to allocate descriptor set from
			setAllocInfo.descriptorSetCount = static_cast<uint32_t>(swapChain.getSwapChainImageCount()); // Number of sets to allocate
			setAllocInfo.pSetLayouts = setLayouts.data(); // Layouts to use to allocate sets (1:1 relationship)

			// Allocate descriptor sets
			VkResult result = vkAllocateDescriptorSets(logicalDevice, &setAllocInfo, shaderDescriptorSet.descriptorSets.data());
			if (result != VK_SUCCESS) {
				throw std::runtime_error("Failed to allocate Descriptor Sets");
			}

			for (size_t i = 0; i < swapChain.getSwapChainImageCount(); i++)
			{
				std::vector<VkWriteDescriptorSet> setWrites = {};
				for (auto& [binding, uniformBuffer] : shaderDescriptorSet.uniformBuffers)
				{
					VkWriteDescriptorSet& writeSet = shaderDescriptorSet.writeDescriptorSets[i][uniformBuffer->name];
					writeSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					writeSet.dstSet = shaderDescriptorSet.descriptorSets[i]; // Descriptor set to update
					writeSet.dstBinding = binding; // Binding to update (matches with binding on layout/shader)
					writeSet.dstArrayElement = 0; // Index in array to update
					writeSet.descriptorType = uniformBuffer->layoutBinding.descriptorType; // TODO hacky fix, might need to use a different data structure to store layout bindings
					writeSet.descriptorCount = 1;
					writeSet.pBufferInfo = &uniformBuffer->descriptor; // Information about buffer data to bind

					setWrites.push_back(writeSet);
				}
				vkUpdateDescriptorSets(logicalDevice, static_cast<uint32_t>(setWrites.size()), setWrites.data(), 0, nullptr);
			}


		}
	}

	void VulkanShader::updateDescriptorSet(int set, int binding, uint32_t imageIndex, ViewProjection data)
	{
		void* buffer;
		vkMapMemory(VulkanContext::get()->getLogicalDevice()->getLogicalDevice(), shaderDescriptorSets[set].uniformBuffers[binding]->uniformMemory[imageIndex], 0, sizeof(data), 0, &buffer);
		memcpy(buffer, &data, sizeof(ViewProjection));
		vkUnmapMemory(VulkanContext::get()->getLogicalDevice()->getLogicalDevice(), shaderDescriptorSets[set].uniformBuffers[binding]->uniformMemory[imageIndex]);

		//std::cout << "Updated descriptor set for shader: " << getName() << " with set: " << set << " at binding: " << binding << "\n";
	}
	
	VkShaderModule VulkanShader::createShaderModule(const std::vector<uint32_t>& shaderData)
	{
		VkDevice logicalDevice = VulkanContext::get()->getLogicalDevice()->getLogicalDevice();

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