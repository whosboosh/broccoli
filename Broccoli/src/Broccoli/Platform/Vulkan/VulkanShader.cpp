#pragma once

#include "VulkanShader.h"

#include "Broccoli/Platform/Vulkan/VulkanContext.h"

#include "Broccoli/Core/Application.h"

#include <spirv_cross/spirv_glsl.hpp>
#include <spirv-tools/libspirv.h>
#include <shaderc/shaderc.hpp>

#include <glm/gtx/string_cast.hpp>

namespace Broccoli {

	VulkanShader::~VulkanShader()
	{
	}

	void VulkanShader::cleanup()
	{
		VkDevice logicalDevice = VulkanContext::get()->getLogicalDevice()->getLogicalDevice();
		uint32_t swapChainImageCount = VulkanContext::get()->getVulkanSwapChain().getSwapChainImageCount();

		// Cleanup descriptor pool
		for (int i = 0; i < shaderDescriptorSets.size(); i++)
		{
			vkDestroyDescriptorPool(logicalDevice, shaderDescriptorSets[i].uniformDescriptors.descriptorPool, nullptr);
			vkDestroyDescriptorPool(logicalDevice, shaderDescriptorSets[i].samplerDescriptors.descriptorPool, nullptr);
		}

		// Loop over each "set" in this shader
		for (int i = 0; i < shaderDescriptorSets.size(); i++)
		{
			// Cleanup the layout
			vkDestroyDescriptorSetLayout(logicalDevice, shaderDescriptorSets[i].uniformDescriptors.descriptorSetLayout, nullptr);
			vkDestroyDescriptorSetLayout(logicalDevice, shaderDescriptorSets[i].samplerDescriptors.descriptorSetLayout, nullptr);

			// Loop over each uniform buffer binding for that set
			for (int j = 0; j < shaderDescriptorSets[i].uniformBuffers.size(); j++)
			{
				for (int k = 0; k < swapChainImageCount; k++)
				{
					vkDestroyBuffer(logicalDevice, shaderDescriptorSets[i].uniformBuffers[j]->uniformBuffer[k], nullptr);
					vkFreeMemory(logicalDevice, shaderDescriptorSets[i].uniformBuffers[j]->uniformMemory[k], nullptr);
				}
			}

		}
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

		auto shaderCode = readFile(filePath);
		//auto shaderCodeSPV = readFile(filePath+".spv");

		// Compile and translate glsl to spirV using Vulkan SDK libs
		std::vector<uint32_t> outputBinary;
		compileGlslToSpirv(shaderCode, outputBinary);

		VkShaderModule shaderModule = createShaderModule(outputBinary);

		shaderStageCreateInfo = {};
		shaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStageCreateInfo.stage = stageFlags;
		shaderStageCreateInfo.module = shaderModule;
		shaderStageCreateInfo.pName = "main";

		std::cout << "Shader created with name: " << name << "\n";

		// Load descriptor sets from shader dynamically
		spirv_cross::Compiler compiler(outputBinary);
		auto resources = compiler.get_shader_resources();

		// Determine if there is a push constant in the shader
		// Only ever 1 push constant per shader so no reason to loop
		if (resources.push_constant_buffers.size() > 0)
		{
			std::cout << "Push constant detected: " << resources.push_constant_buffers[0].name << "\n";
			auto& bufferType = compiler.get_type(resources.push_constant_buffers[0].base_type_id);
			uint32_t size = (uint32_t)compiler.get_declared_struct_size(bufferType);

			pushConstantRange.size = size;
			pushConstantRange.offset = 0;
			pushConstantRange.stageFlags = stageFlags;
		}

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
			shaderDescriptorSet.uniformBuffers[binding] = uniformBuffer;

			// Create descriptor set layout for this uniform
			VkDescriptorSetLayoutBinding& layoutBinding = uniformBuffer->layoutBinding;
			layoutBinding.binding = binding;
			layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			layoutBinding.descriptorCount = 1;
			layoutBinding.stageFlags = stageFlags;
			layoutBinding.pImmutableSamplers = nullptr;

			shaderDescriptorSet.uniformDescriptors.layoutBindings.push_back(layoutBinding);

			// Create pool size for this uniform buffer (Used when we create the descriptor pool)
			VkDescriptorPoolSize poolSize = {};
			poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			poolSize.descriptorCount = static_cast<uint32_t>(uniformBuffer->uniformBuffer.size());
			shaderDescriptorSet.uniformDescriptors.poolSizes.push_back(poolSize);



			std::cout << "Uniform Buffer Name: " << name << " DescriptorSet: " << descriptorSet << " Binding: " << binding << "\n";
		}


		// Generate texture samplers if they exist
		for (const auto& resource : resources.sampled_images)
		{
			const auto& name = resource.name;
			auto& bufferType = compiler.get_type(resource.base_type_id);
			auto& type = compiler.get_type(resource.type_id);
			int memberCount = (uint32_t)bufferType.member_types.size();
			uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding); // Binding value
			uint32_t descriptorSet = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet); // Set value
			uint32_t arraySize = type.array[0];
			if (arraySize == 0)
				arraySize = 1;

			if (descriptorSet >= shaderDescriptorSets.size())
			{
				shaderDescriptorSets.resize(descriptorSet + 1);
			}

			ShaderDescriptorSet& shaderDescriptorSet = shaderDescriptorSets[descriptorSet];
			
			ImageSampler* imageSampler = new ImageSampler();
			imageSampler->binding = binding;
			imageSampler->set = descriptorSet;
			imageSampler->name = name;
			imageSampler->arraySize = arraySize;
			shaderDescriptorSet.imageSamplers[binding] = imageSampler;

			VkDescriptorSetLayoutBinding& layoutBinding = imageSampler->layoutBinding;
			layoutBinding.binding = binding;
			layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			layoutBinding.descriptorCount = 1;
			layoutBinding.stageFlags = stageFlags;
			layoutBinding.pImmutableSamplers = nullptr;
			shaderDescriptorSet.samplerDescriptors.layoutBindings.push_back(layoutBinding);

			VkDescriptorPoolSize poolSize = {};
			poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			poolSize.descriptorCount = 1;
			shaderDescriptorSet.samplerDescriptors.poolSizes.push_back(poolSize); // TODO: Maybe put poolSizes in individual groups?

			std::cout << "Sampler Name: " << name << " DescriptorSet: " << descriptorSet << " Binding: " << binding << "\n";
		}

		// Loop over each descriptor "set" for creation
		for (size_t set = 0; set < shaderDescriptorSets.size(); set++)
		{
			auto& shaderDescriptorSet = shaderDescriptorSets[set];

			// TODO: Refactor pool creation, only create if poolsizes is > 0 for that set
			// Create Uniform Descriptor Pool
			VkDescriptorPoolCreateInfo descriptorPoolInfo = {};
			descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(shaderDescriptorSet.uniformDescriptors.poolSizes.size());
			descriptorPoolInfo.pPoolSizes = shaderDescriptorSet.uniformDescriptors.poolSizes.data();
			descriptorPoolInfo.maxSets = static_cast<uint32_t>(swapChain.getSwapChainImageCount());
			VkResult result = vkCreateDescriptorPool(logicalDevice, &descriptorPoolInfo, nullptr, &shaderDescriptorSet.uniformDescriptors.descriptorPool);
			if (result != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create uniform descriptor pool!");
			}

			// Create Descriptor Set Layout with given bindings
			VkDescriptorSetLayoutCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			createInfo.bindingCount = static_cast<uint32_t>(shaderDescriptorSet.uniformDescriptors.layoutBindings.size()); // Number of binding infos
			createInfo.pBindings = shaderDescriptorSet.uniformDescriptors.layoutBindings.data(); // Pointer to binding info

			result = vkCreateDescriptorSetLayout(logicalDevice, &createInfo, nullptr, &shaderDescriptorSet.uniformDescriptors.descriptorSetLayout);
			if (result != VK_SUCCESS) {
				throw std::runtime_error("Failed to create a Descriptor Set Layout!");
			}

			shaderDescriptorSet.uniformDescriptors.descriptorSets.resize(swapChain.getSwapChainImageCount());
			shaderDescriptorSet.uniformDescriptors.writeDescriptorSets.resize(swapChain.getSwapChainImageCount());

			std::vector<VkDescriptorSetLayout> setLayouts(swapChain.getSwapChainImageCount(), shaderDescriptorSet.uniformDescriptors.descriptorSetLayout);

			VkDescriptorSetAllocateInfo setAllocInfo = {};
			setAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			setAllocInfo.descriptorPool = shaderDescriptorSet.uniformDescriptors.descriptorPool; // Pool to allocate descriptor set from
			setAllocInfo.descriptorSetCount = static_cast<uint32_t>(swapChain.getSwapChainImageCount()); // Number of sets to allocate
			setAllocInfo.pSetLayouts = setLayouts.data(); // Layouts to use to allocate sets (1:1 relationship)

			// Allocate descriptor sets
			result = vkAllocateDescriptorSets(logicalDevice, &setAllocInfo, shaderDescriptorSet.uniformDescriptors.descriptorSets.data());
			if (result != VK_SUCCESS) {
				throw std::runtime_error("Failed to allocate Descriptor Sets");
			}

			for (size_t i = 0; i < swapChain.getSwapChainImageCount(); i++)
			{
				std::vector<VkWriteDescriptorSet> setWrites = {};
				for (auto& [binding, uniformBuffer] : shaderDescriptorSet.uniformBuffers)
				{
					VkWriteDescriptorSet& writeSet = shaderDescriptorSet.uniformDescriptors.writeDescriptorSets[i][uniformBuffer->name];
					writeSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					writeSet.dstSet = shaderDescriptorSet.uniformDescriptors.descriptorSets[i]; // Descriptor set to update
					writeSet.dstBinding = binding; // Binding to update (matches with binding on layout/shader)
					writeSet.dstArrayElement = 0; // Index in array to update
					writeSet.descriptorType = uniformBuffer->layoutBinding.descriptorType;
					writeSet.descriptorCount = 1;
					writeSet.pBufferInfo = &uniformBuffer->descriptor; // Information about buffer data to bind

					setWrites.push_back(writeSet);
				}

				vkUpdateDescriptorSets(logicalDevice, static_cast<uint32_t>(setWrites.size()), setWrites.data(), 0, nullptr);
			}
		}
	}

	int VulkanShader::updateTextureWriteBinding(int set, int binding, VkImageView textureImageView, const std::string& name)
	{
		VkDevice logicalDevice = VulkanContext::get()->getLogicalDevice()->getLogicalDevice();
		VulkanSwapchain swapChain = VulkanContext::get()->getVulkanSwapChain();
		auto& shaderDescriptorSet = shaderDescriptorSets[set];

		VkDescriptorSet textureDescriptorSet;

		// Create Sampler Descriptor Pool
		VkDescriptorPoolCreateInfo descriptorPoolInfo = {};
		descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(shaderDescriptorSet.samplerDescriptors.poolSizes.size());
		descriptorPoolInfo.pPoolSizes = shaderDescriptorSet.samplerDescriptors.poolSizes.data();
		descriptorPoolInfo.maxSets = MAX_OBJECTS;
		VkResult result = vkCreateDescriptorPool(logicalDevice, &descriptorPoolInfo, nullptr, &shaderDescriptorSet.samplerDescriptors.descriptorPool);
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create sampler descriptor pool!");
		}

		// Create Descriptor Set Layout with given bindings
		VkDescriptorSetLayoutCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		createInfo.bindingCount = static_cast<uint32_t>(shaderDescriptorSet.samplerDescriptors.layoutBindings.size()); // Number of binding infos
		createInfo.pBindings = shaderDescriptorSet.samplerDescriptors.layoutBindings.data(); // Pointer to binding info

		result = vkCreateDescriptorSetLayout(logicalDevice, &createInfo, nullptr, &shaderDescriptorSet.samplerDescriptors.descriptorSetLayout);
		if (result != VK_SUCCESS) {
			throw std::runtime_error("Failed to create a Descriptor Set Layout!");
		}

		// Workaroud, normal uniform descriptors use 3 sets (1 for each swap image) - only need 1 for sampler
		shaderDescriptorSet.samplerDescriptors.descriptorSets.resize(1);
		shaderDescriptorSet.samplerDescriptors.writeDescriptorSets.resize(1);

		VkDescriptorSetAllocateInfo setAllocInfo = {};
		setAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		setAllocInfo.descriptorPool = shaderDescriptorSet.samplerDescriptors.descriptorPool; // Pool to allocate descriptor set from
		setAllocInfo.descriptorSetCount = 1;
		setAllocInfo.pSetLayouts = &shaderDescriptorSet.samplerDescriptors.descriptorSetLayout; // Layouts to use to allocate sets (1:1 relationship)

		// Allocate descriptor sets
		result = vkAllocateDescriptorSets(logicalDevice, &setAllocInfo, &textureDescriptorSet);
		if (result != VK_SUCCESS) {
			throw std::runtime_error("Failed to allocate Descriptor Sets");
		}

		// Texture image info
		VkDescriptorImageInfo imageInfo = {};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; // Image layout when in use
		imageInfo.imageView = textureImageView; // Image to bind to set
		imageInfo.sampler = swapChain.getTextureSampler(); // Sampler to use for set

		VkWriteDescriptorSet& writeSet = shaderDescriptorSet.samplerDescriptors.writeDescriptorSets[0][name];
		writeSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeSet.dstSet = textureDescriptorSet;
		writeSet.dstBinding = binding;
		writeSet.dstArrayElement = 0;
		writeSet.descriptorType = shaderDescriptorSet.imageSamplers[binding]->layoutBinding.descriptorType;
		writeSet.descriptorCount = 1;//shaderDescriptorSet.imageSamplers[binding]->arraySize;
		writeSet.pImageInfo = &imageInfo;

		vkUpdateDescriptorSets(logicalDevice, 1, &writeSet, 0, nullptr);


		shaderDescriptorSet.samplerDescriptors.descriptorSets.push_back(textureDescriptorSet);
		return shaderDescriptorSet.samplerDescriptors.descriptorSets.size() - 1;
	}

	void VulkanShader::updateDescriptorSet(int set, int binding, uint32_t imageIndex, void* data, int size)
	{
		void* buffer;
		vkMapMemory(VulkanContext::get()->getLogicalDevice()->getLogicalDevice(), shaderDescriptorSets[set].uniformBuffers[binding]->uniformMemory[imageIndex], 0, sizeof(data), 0, &buffer);
		memcpy(buffer, data, size);
		vkUnmapMemory(VulkanContext::get()->getLogicalDevice()->getLogicalDevice(), shaderDescriptorSets[set].uniformBuffers[binding]->uniformMemory[imageIndex]);

		//std::cout << "Updated descriptor set for shader: " << getName() << " with set: " << set << " at binding: " << binding << "\n";
	}
	
	VkShaderModule VulkanShader::createShaderModuleSPV(const std::vector<char>& code)
	{
		VkDevice logicalDevice = VulkanContext::get()->getLogicalDevice()->getLogicalDevice();

		// Shader module creation info
		VkShaderModuleCreateInfo shaderModuleCreateInfo = {};
		shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO; 
		shaderModuleCreateInfo.codeSize = code.size(); // Size of code
		shaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(code.data()); // Pointer to code (uint32_t pointer type)

		VkShaderModule shaderModule;

		VkResult result = vkCreateShaderModule(logicalDevice, &shaderModuleCreateInfo, nullptr, &shaderModule);
		if (result != VK_SUCCESS) {
			throw std::runtime_error("Failed to creata a shader module");
		}

		return shaderModule;
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