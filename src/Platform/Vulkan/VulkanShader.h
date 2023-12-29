#ifndef BROCCOLI_VULKAN_SHADER_HEADER
#define BROCCOLI_VULKAN_SHADER_HEADER

#include "Core/Ref.h"
#include "Renderer/Shader.h"
#include "Utilities/VulkanUtilities.h"

#include <iostream>
#include <vector>

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
		VkDescriptorSetLayout descriptorSetLayout;
		std::vector<std::unordered_map<std::string, VkWriteDescriptorSet>> writeDescriptorSets;
		std::vector<VkDescriptorSet> descriptorSets;
		std::vector<VkDescriptorSet> samplerDescriptorSets;
	};

	struct ShaderDescriptorSet
	{
		VkDescriptorPool descriptorPool;
		std::vector<VkDescriptorPoolSize> poolSizes;
		std::unordered_map<uint32_t, ImageSampler*> imageSamplers;
		std::unordered_map<uint32_t, UniformBuffer*> uniformBuffers;
		DescriptorGroup uniformDescriptors;
		DescriptorGroup samplerDescriptors;
		bool isEmpty() const { return (uniformBuffers.empty() && imageSamplers.empty()); }
	};

	class VulkanShader : public Shader
	{
	public:
		VulkanShader(const std::string& filePath, VkShaderStageFlagBits stageFlags);
		~VulkanShader();

		virtual void cleanup() override;

		virtual const VkShaderStageFlagBits getStageFlags() const override { return stageFlags; }
		virtual const VkPipelineShaderStageCreateInfo getShaderStageInfo() const override { return shaderStageCreateInfo; }

		void allocateDescriptorSet(VkDevice logicalDevice, std::vector<VkDescriptorSetLayout>& descriptorSetLayouts, std::vector<VkDescriptorSet> descriptorSetTarget, VkDescriptorPool& descriptorPool, int descriptorCount);
		void createDescriptorSetLayout(VkDevice logicalDevice, VkDescriptorSetLayout& descriptorSetLayout, std::vector<VkDescriptorSetLayoutBinding> &layoutBindings);

		const std::vector<ShaderDescriptorSet>& getShaderDescriptorSets() const { return shaderDescriptorSets; }
		VkPushConstantRange getPushConstantRange() { return pushConstantRange; }

		int updateTextureWriteBinding(int set, int binding, VkImageView textureImageView, const std::string& name);

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

		friend class ShaderLibrary;
	};
}

#endif