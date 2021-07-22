#pragma once

#include "Broccoli/Asset/Asset.h"

#include "Broccoli/Renderer/Renderer.h"
#include "Broccoli/Renderer/VertexBuffer.h"
#include "Broccoli/Renderer/IndexBuffer.h"

#include <vector>

#include <glm/glm.hpp>

namespace Broccoli {

	// Mesh is a collection of vertices
	// This class will contain functions like getting the vertex buffers and index buffers
	class Mesh : public Asset
	{
	public:
		Mesh(const std::vector<Vertex> vertices, std::vector<uint32_t> indices, const glm::mat4& transform);

		~Mesh();


	private:
		Ref<VertexBuffer> vertexBuffer;
		Ref<IndexBuffer> indexBuffer;
	};

}