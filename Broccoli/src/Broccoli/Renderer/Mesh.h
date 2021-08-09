#pragma once

#include "Broccoli/Asset/Asset.h"

#include "Broccoli/Renderer/VertexBuffer.h"
#include "Broccoli/Renderer/IndexBuffer.h"

#include <vector>

#include <glm/glm.hpp>

namespace Broccoli {
	struct MeshInfo {
		glm::mat4 transform;
		glm::mat4 inverseTransform;
		bool hasTexture;
	};

	// Mesh is a collection of vertices
	// This class will contain functions like getting the vertex buffers and index buffers
	class Mesh : public Asset
	{

	public:
		Mesh(std::vector<Vertex>* vertices, std::vector<uint32_t>* indices, glm::mat4 transform);
		~Mesh();

		Ref<VertexBuffer>* getVertexBuffer() { return &vertexBuffer; }
		Ref<IndexBuffer>* getIndexBuffer() { return &indexBuffer; }

		int getIndexCount() { return indexCount; };
		int getVertexCount() { return vertexCount; };
		
		void setTransform(glm::mat4 newModel);
		MeshInfo& getMeshInfo() { return meshInfo; }

		void setHasTexture(bool state) { meshInfo.hasTexture = state; }
		bool getHasTexture() { return meshInfo.hasTexture; }

	private:
		MeshInfo meshInfo;

		int vertexCount;
		int indexCount;

		Ref<VertexBuffer> vertexBuffer;
		Ref<IndexBuffer> indexBuffer;
	};

}