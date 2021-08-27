#pragma once

#include "Broccoli/Asset/Asset.h"

#include "Broccoli/Renderer/VertexBuffer.h"
#include "Broccoli/Renderer/IndexBuffer.h"
#include "Broccoli/Renderer/Texture.h"

#include "Broccoli/Renderer/RenderObject.h"

#include <vector>

#include <glm/glm.hpp>

namespace Broccoli {
	// Mesh is a collection of vertices
	// This class will contain functions like getting the vertex buffers and index buffers
	class Mesh : public RenderObject
	{

	public:
		Mesh(std::vector<Vertex>* vertices, std::vector<uint32_t>* indices, glm::mat4 transform);
		Mesh(std::vector<Vertex>* vertices, std::vector<uint32_t>* indices, glm::mat4 transform, Ref<Texture> texture);
		~Mesh();

		Ref<VertexBuffer>* getVertexBuffer() { return &vertexBuffer; }
		Ref<IndexBuffer>* getIndexBuffer() { return &indexBuffer; }

		int getIndexCount() { return indexCount; };
		int getVertexCount() { return vertexCount; };

		void setHasTexture(bool state) { transform.hasTexture = state; }
		bool getHasTexture() { return transform.hasTexture; }

		Ref<Texture> getTexture() { return texture; }

	private:
		int vertexCount;
		int indexCount;

		Ref<VertexBuffer> vertexBuffer;
		Ref<IndexBuffer> indexBuffer;

		Ref<Texture> texture;
	};

}