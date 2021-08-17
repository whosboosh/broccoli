#pragma once

#include "Broccoli/Core/Ref.h"
#include "Broccoli/Renderer/Shader.h"

#include "Broccoli/Utilities/VulkanUtilities.h"

#include <iostream>

namespace Broccoli {

	struct UniformBuffer
	{
		//std::string descriptorType;
		VkDescriptorBufferInfo descriptor;
		VkDeviceSize descriptorSize = 0;
		uint32_t size = 0;
		uint32_t bindingPoint = 0;
		std::string name;
		VkShaderStageFlagBits shaderStage = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
		std::vector<VkDeviceMemory> uniformMemory;
		std::vector<VkBuffer> uniformBuffer;
		VkDescriptorSetLayoutBinding layoutBinding;
	};

	struct ImageSampler
	{
		uint32_t set = 0;
		uint32_t binding = 0;
		uint32_t arraySize = 0;
		std::string name;
		VkDescriptorSetLayoutBinding layoutBinding;
	};

	struct DescriptorGroup
	{
		std::vector<VkDescriptorSetLayoutBinding> layoutBindings;
		std::vector<std::unordered_map<std::string, VkWriteDescriptorSet>> writeDescriptorSets;
		std::vector<VkDescriptorSet> descriptorSets;
		VkDescriptorSetLayout descriptorSetLayout;
		VkDescriptorPool descriptorPool;
		std::vector<VkDescriptorPoolSize> poolSizes;
	};

	struct ShaderDescriptorSet
	{
		std::unordered_map<uint32_t, ImageSampler*> imageSamplers;
		std::unordered_map<uint32_t, UniformBuffer*> uniformBuffers;
		DescriptorGroup uniformDescriptors;
		DescriptorGroup samplerDescriptors;
		operator bool() const { return !(uniformBuffers.empty()); }
	};

	class VulkanShader : public Shader
	{
	public:
		VulkanShader(const std::string& filePath, VkShaderStageFlagBits stageFlags);
		~VulkanShader();

		virtual void cleanup() override;

		virtual const std::string& getName() const override { return name; }

		virtual const VkShaderStageFlagBits getStageFlags() const override { return stageFlags; }
		virtual const VkPipelineShaderStageCreateInfo getShaderStageInfo() const override { return shaderStageCreateInfo; }

		const std::vector<ShaderDescriptorSet>& getShaderDescriptorSets() const { return shaderDescriptorSets; }
		VkPushConstantRange getPushConstantRange() { return pushConstantRange; }
			
		//std::vector<VkDescriptorSet> getAllDescriptorSets() { return descriptorSets; }

		//VkDescriptorSet GetDescriptorSet() { return m_DescriptorSet; }
		//VkDescriptorSetLayout getDescriptorSetLayout(uint32_t set) { return descriptorSetLayouts.at(set); }
		//std::vector<VkDescriptorSetLayout> getAllDescriptorSetLayouts() { return descriptorSetLayouts; }

		void updateTextureWriteBinding(int set, int binding, VkImageView textureImageView, const std::string& name);

		void updateDescriptorSet(int set, int binding, uint32_t imageIndex, void* data, int size);

		VkShaderModule createShaderModuleSPV(const std::vector<char>& code);

		VkShaderModule createShaderModule(const std::vector<uint32_t>& shaderData);

		void compileGlslToSpirv(std::vector<char> shaderCode, std::vector<uint32_t>& outputBinary);

	private:
		std::string filePath;

		VkShaderStageFlagBits stageFlags;
		VkPipelineShaderStageCreateInfo shaderStageCreateInfo;

		VkPushConstantRange pushConstantRange;
		std::vector<ShaderDescriptorSet> shaderDescriptorSets;

		std::string name;
	};
}