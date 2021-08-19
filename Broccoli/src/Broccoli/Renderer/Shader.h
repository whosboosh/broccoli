#pragma once

#include "Broccoli/Core/Ref.h"

#include <vulkan/vulkan.h>

#include <unordered_map>

#include <iostream>

namespace Broccoli {
	
	class Shader : public RefCounted
	{
	public:
		Shader() = default;
		virtual ~Shader() = default;

		virtual void cleanup() = 0;

		static Ref<Shader> create(const std::string& filePath, VkShaderStageFlagBits stageFlags);

		// Vulkan specific
		virtual const VkShaderStageFlagBits getStageFlags() const = 0;
		virtual const VkPipelineShaderStageCreateInfo getShaderStageInfo() const = 0;

		int getShaderId() { return shaderId; }
		const std::string& getName() { return name; }

		friend class ShaderLibrary;
	protected:
		int shaderId;
		std::string name;
	};

	class ShaderLibrary
	{
	public:
		ShaderLibrary();
		~ShaderLibrary();

		void cleanup();

		const Ref<Shader>& getShader(const std::string& name);

		std::unordered_map<std::string, Ref<Shader>> getCurrentShaders() { return currentShaders; }

		std::array<Ref<Shader>, 2> getShaderGroup(std::string& name);

		// Vulkan specific
		void setDescriptorSetsfromShaders();
		void setSamplerDescriptorSetsFromShader();
		std::vector<VkDescriptorSet> getShaderUniformDescriptorSets(int imageIndex) { return shaderUniformDescriptorSets[imageIndex]; }
		VkDescriptorSet getShaderSamplerDescriptorSets(int shaderId, int texId) { return shaderSamplerDescriptorSets[shaderId][texId]; }
		
		void setPushConstantRangesFromShaders();
		const std::vector<VkPushConstantRange>& getPushConstantRanges() { return pushConstantRanges; }
		
		void loadShader(const std::string& path, VkShaderStageFlagBits stageFlags);
	private:
		std::unordered_map<std::string, Ref<Shader>> currentShaders;
		
		// Store push constant ranges and descriptor sets in shader library as expensive to recompute in loop every frame
		std::vector<VkPushConstantRange> pushConstantRanges;

		std::unordered_map<int, std::vector<VkDescriptorSet>> shaderUniformDescriptorSets;
		std::unordered_map<int, std::vector<VkDescriptorSet>> shaderSamplerDescriptorSets; // First index is the shaderId (1 is vert, 0 is frag) (position in vector is the texture id)
	};

}