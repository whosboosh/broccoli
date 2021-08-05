#pragma once

#include "Broccoli/Core/Ref.h"
#include "Broccoli/Renderer/Shader.h"

#include "Broccoli/Utilities/VulkanUtilities.h"

#include <iostream>

namespace Broccoli {

	struct UniformBuffer
	{
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

	struct ShaderDescriptorSet
	{
		std::unordered_map<uint32_t, UniformBuffer*> uniformBuffers;
		//std::unordered_map<uint32_t, ImageSampler> ImageSamplers; // TODO: Add texture sampler support later
		std::vector<std::unordered_map<std::string, VkWriteDescriptorSet>> writeDescriptorSets;
		std::vector<VkDescriptorSet> descriptorSets;
		VkDescriptorSetLayout descriptorSetLayout;

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

		//std::vector<VkDescriptorSet> getAllDescriptorSets() { return descriptorSets; }

		//VkDescriptorSet GetDescriptorSet() { return m_DescriptorSet; }
		//VkDescriptorSetLayout getDescriptorSetLayout(uint32_t set) { return descriptorSetLayouts.at(set); }
		//std::vector<VkDescriptorSetLayout> getAllDescriptorSetLayouts() { return descriptorSetLayouts; }

		// TODO: set back to void* data
		void updateDescriptorSet(int set, int binding, uint32_t imageIndex, ViewProjection data);

		VkShaderModule createShaderModule(const std::vector<uint32_t>& shaderData);

		void compileGlslToSpirv(std::vector<char> shaderCode, std::vector<uint32_t>& outputBinary);

	private:
		std::string filePath;

		VkShaderStageFlagBits stageFlags;
		VkPipelineShaderStageCreateInfo shaderStageCreateInfo = {};

		//std::vector<VkDescriptorSet> descriptorSets;
		//std::vector<VkDescriptorSetLayout> descriptorSetLayouts;

		std::vector<ShaderDescriptorSet> shaderDescriptorSets;

		std::vector<VkDescriptorPoolSize> poolSizes;
		//std::unordered_map<uint32_t, std::vector<VkDescriptorPoolSize>> typeCounts;
		VkDescriptorPool descriptorPool;

		std::string name;
	};
}