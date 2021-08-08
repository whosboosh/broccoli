#pragma once

#include <stdint.h>

#include <iostream>
#include <string>
#include <vector>

#include <glm/glm.hpp>

#include <vulkan/vulkan.h>

#include "Broccoli/Core/Ref.h"

namespace Broccoli {

	struct Vertex {
		glm::vec3 pos; // vertex position (x,y,z)
		glm::vec3 col; // vertex color (r,g,b)
		glm::vec2 tex; // Texture coords (u, v)
		glm::vec3 normal; // Normals
	};
	
	enum class VertexBufferUsage
	{
		None = 0,
		Static = 1,
		Dynamic = 2
	};

	class VertexBuffer : public RefCounted
	{
	public:
		virtual void setData(void* buffer, uint32_t size, uint32_t offset = 0) = 0;
		virtual void bind() = 0;

		virtual unsigned int getSize() = 0;
		

		static Ref<VertexBuffer> create(std::vector<Vertex>* vertices, VertexBufferUsage usage = VertexBufferUsage::Dynamic);

		// Vulkan Specific
		virtual VkBuffer getVertexBuffer() = 0;
	};
}