#pragma once

#include "Shader.h"

#include "Broccoli/Platform/Vulkan/VulkanShader.h"
#include "Broccoli/Platform/Vulkan/VulkanSwapchain.h"
#include "Broccoli/Platform/Vulkan/VulkanContext.h"

#include "Broccoli/Renderer/RendererAPI.h"

namespace Broccoli {

	// TODO: Auto detect VK stage flags from shader to make generalist with opengl
	Ref<Shader> Shader::create(const std::string& filePath, VkShaderStageFlagBits stageFlags)
	{
		switch (RendererAPI::getCurrent())
		{
		case RendererAPIType::None: return nullptr;
		case RendererAPIType::Vulkan: return Ref<VulkanShader>::create(filePath, stageFlags);
			//case RendererAPIType::OpenGL: return Ref<OpenGLContext>::create();
		}
		// No match

		return nullptr;
	}

	ShaderLibrary::ShaderLibrary()
	{
	}

	ShaderLibrary::~ShaderLibrary()
	{
	}

	void ShaderLibrary::cleanup()
	{
		for (std::pair<std::string, Ref<Shader>> shader : getCurrentShaders())
		{
			shader.second.As<Shader>()->cleanup();
		}
	}

	const Ref<Shader>& ShaderLibrary::getShader(const std::string& name)
	{
		if (currentShaders.find(name) == currentShaders.end()) throw std::runtime_error("Failed to find shader " + name + " in list");
		return currentShaders.at(name);
	}

	std::array<Ref<Shader>, 2> ShaderLibrary::getShaderGroup(std::string& name)
	{
		// If paramaeter is "geometry"
		// We just have to add the .vert and .frag suffix using the getShader method, add them both to an array and return
		std::array<Ref<Shader>, 2> shaderGroup = {};
		shaderGroup[0] = getShader(name + ".vert");
		shaderGroup[1] = getShader(name + ".frag");

		return shaderGroup;
	}

	void ShaderLibrary::setDescriptorSetsfromShaders()
	{
		VulkanSwapchain swapChain = VulkanContext::get()->getVulkanSwapChain();

		for (std::pair<std::string, Ref<Shader>> shader : currentShaders)
		{
			for (ShaderDescriptorSet shaderDescriptor : shader.second.As<VulkanShader>()->getShaderDescriptorSets())
			{
				for (int i = 0; i < swapChain.getSwapChainImageCount(); i++)
				{
					shaderDescriptorSets[i].push_back(shaderDescriptor.uniformDescriptors.descriptorSets[i]);

					// TODO: Should probably separate out texture descriptors and uniforms
					//shaderDescriptorSets[i].push_back(shaderDescriptor.samplerDescriptors.descriptorSets[0]);
				}
			}
		}
	}

	void ShaderLibrary::setPushConstantRangesFromShaders()
	{
		for (std::pair<std::string, Ref<Shader>> shader : currentShaders)
		{
			VkPushConstantRange pushConstantRange = shader.second.As<VulkanShader>()->getPushConstantRange();
			// If size of the range is 0 then the push constant doesn't exist
			if (pushConstantRange.size != 0)
			{
				pushConstantRanges.push_back(pushConstantRange);
			}
		}
	}

	void ShaderLibrary::loadShader(const std::string& filePath, VkShaderStageFlagBits stageFlags)
	{
		// With no name, just use the file name
		auto shader = Shader::create(filePath, stageFlags);
		auto& name = shader->getName();
		if (currentShaders.find(name) != currentShaders.end()) throw std::runtime_error("Shader already exists in shader list " + name);
		currentShaders[name] = shader;
	}

}

