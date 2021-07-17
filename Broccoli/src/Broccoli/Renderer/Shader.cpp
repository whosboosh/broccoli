#pragma once

#include "Shader.h"

#include "Broccoli/Platform/Vulkan/VulkanShader.h"

namespace Broccoli {

	Ref<Shader> Shader::create(const std::string& filePath)
	{
		switch (RendererAPI::getCurrent())
		{
		case RendererAPIType::None: return nullptr;
		case RendererAPIType::Vulkan: return Ref<VulkanShader>::create();
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

	void ShaderLibrary::loadShader(const std::string& path)
	{
		// With no name, just use the file name
		auto shader = Shader::create(path);
		auto& name = shader->getName();
		if (currentShaders.find(name) == currentShaders.end()) throw std::runtime_error("Shader already exists in shader list " + name);
		currentShaders[name] = shader;
	}

}

