#pragma once

#include "Broccoli/Core/Ref.h"
#include "Broccoli/Renderer/Shader.h"

#include "Broccoli/Utilities/VulkanUtilities.h"

#include <iostream>

namespace Broccoli {

	struct UniformBuffer
	{
		VkDescriptorBufferInfo descriptor;
		uint32_t size = 0;
		uint32_t bindingPoint = 0;
		std::string name;
		VkShaderStageFlagBits shaderStage = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
	};

	struct ShaderDescriptorSet
	{
		std::unordered_map<uint32_t, UniformBuffer*> uniformBuffers;
		//std::unordered_map<uint32_t, ImageSampler> ImageSamplers; // TODO: Add texture sampler support later
		std::unordered_map<std::string, VkWriteDescriptorSet> writeDescriptorSets;

		operator bool() const { return !(uniformBuffers.empty()); }
	};

	class VulkanShader : public Shader
	{
	public:
		VulkanShader(const std::string& filePath, VkShaderStageFlagBits stageFlags);
		~VulkanShader();

		virtual const std::string& getName() const override { return name; }

		virtual const VkShaderStageFlagBits getStageFlags() const override { return stageFlags; }
		virtual const VkPipelineShaderStageCreateInfo getShaderStageInfo() const override { return shaderStageCreateInfo; }

		const std::vector<ShaderDescriptorSet>& getShaderDescriptorSets() const { return shaderDescriptorSets; }

		//VkDescriptorSet GetDescriptorSet() { return m_DescriptorSet; }
		VkDescriptorSetLayout getDescriptorSetLayout(uint32_t set) { return descriptorSetLayouts.at(set); }
		std::vector<VkDescriptorSetLayout> getAllDescriptorSetLayouts() { return descriptorSetLayouts; }

		VkShaderModule createShaderModule(const std::vector<uint32_t>& shaderData);

		void compileGlslToSpirv(std::vector<char> shaderCode, std::vector<uint32_t>& outputBinary);

	private:
		std::string filePath;

		VkShaderStageFlagBits stageFlags;
		VkPipelineShaderStageCreateInfo shaderStageCreateInfo = {};

		std::vector<VkDescriptorSet> descriptorSets;
		std::vector<VkDescriptorSetLayout> descriptorSetLayouts;

		std::vector<ShaderDescriptorSet> shaderDescriptorSets;

		std::unordered_map<uint32_t, std::vector<VkDescriptorPoolSize>> typeCounts;
		VkDescriptorPool descriptorPool;

		std::string name;
	};
}