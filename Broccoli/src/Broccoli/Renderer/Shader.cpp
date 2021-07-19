#pragma once

#include "Shader.h"

#include "Broccoli/Platform/Vulkan/VulkanShader.h"

namespace Broccoli {

	// TODO: Auto detect VK stage flags for shader to make generalist with opengl
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

	const Ref<Shader>& ShaderLibrary::getShader(const std::string& name)
	{
		if (currentShaders.find(name) == currentShaders.end()) throw std::runtime_error("Failed to find shader " + name + " in list");
		return currentShaders.at(name);
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

