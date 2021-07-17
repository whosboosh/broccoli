#pragma once

#include "Broccoli/Core/Ref.h"
#include "Broccoli/Renderer/RendererAPI.h"

#include <unordered_map>

namespace Broccoli {
	
	class Shader : public RefCounted
	{
	public:
		Shader() = default;
		virtual ~Shader() = default;

		static Ref<Shader> create(const std::string& filePath);

		virtual const std::string& getName() const = 0;

	private:
		friend class ShaderLibrary;
	};

	class ShaderLibrary
	{
	public:
		ShaderLibrary();
		~ShaderLibrary();

		const Ref<Shader>& getShader(const std::string& name);
		
		void loadShader(const std::string& path);

	private:
		std::unordered_map<std::string, Ref<Shader>> currentShaders;
	};

}