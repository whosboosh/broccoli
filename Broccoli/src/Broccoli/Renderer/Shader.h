#pragma once

#include "Broccoli/Core/Ref.h"
#include "Broccoli/Renderer/RendererAPI.h"

#include <vulkan/vulkan.h>

#include <unordered_map>

namespace Broccoli {
	
	class Shader : public RefCounted
	{
	public:
		Shader() = default;
		virtual ~Shader() = default;

		static Ref<Shader> create(const std::string& filePath, VkShaderStageFlagBits stageFlags);

		virtual const std::string& getName() const = 0;

		// Vulkan specific
		virtual const VkShaderStageFlagBits getStageFlags() const = 0;
		virtual const VkPipelineShaderStageCreateInfo getShaderStageInfo() const = 0;

	private:
		friend class ShaderLibrary;
	};

	class ShaderLibrary
	{
	public:
		ShaderLibrary();
		~ShaderLibrary();

		const Ref<Shader>& getShader(const std::string& name);

		std::unordered_map<std::string, Ref<Shader>> getCurrentShaders() { return currentShaders; }
		
		void loadShader(const std::string& path, VkShaderStageFlagBits stageFlags);

	private:
		std::unordered_map<std::string, Ref<Shader>> currentShaders;
	};

}