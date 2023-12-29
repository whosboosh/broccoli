#ifndef BROCCOLI_MESH_HEADER
#define BROCCOLI_MESH_HEADER

#include "Asset/Asset.h"

#include "Renderer/RenderObject.h"

#include "Renderer/VertexBuffer.h"
#include "Renderer/IndexBuffer.h"
#include "Renderer/Texture.h"

#include <vector>

#include <glm/glm.hpp>

namespace Broccoli {
	// Mesh is a collection of vertices
	// This class will contain functions like getting the vertex buffers and index buffers
	class Mesh : public RenderObject
	{

	public:
		Mesh(std::vector<Vertex> vertices, std::vector<uint32_t>* indices, glm::vec3 translate = glm::vec3(0,0,0), glm::vec3 scale = glm::vec3(1,1,1), glm::vec3 rotate = glm::vec3(0,0,0));
		Mesh(std::vector<Vertex> vertices, std::vector<uint32_t>* indices, Ref<Texture> texture, glm::vec3 translate = glm::vec3(0, 0, 0), glm::vec3 scale = glm::vec3(1, 1, 1), glm::vec3 rotate = glm::vec3(0, 0, 0));

		Mesh(std::vector<Vertex> vertices, std::vector<uint32_t>* indices, glm::mat4 transform);
		Mesh(std::vector<Vertex> vertices, std::vector<uint32_t>* indices, Ref<Texture> texture, glm::mat4 transform);
		~Mesh();

		Ref<VertexBuffer>* getVertexBuffer() { return &vertexBuffer; }
		Ref<IndexBuffer>* getIndexBuffer() { return &indexBuffer; }

		int getIndexCount() { return indexCount; };
		int getVertexCount() { return vertexCount; };


		void setHasTexture(bool state) { this->hasTexture = state; }
		bool getHasTexture() { return this->hasTexture; }

		virtual void calculateBoundingBox() override;

		virtual void findMinMaxVerticies() override;

		virtual void findMaxAndMinHeight() override;
		virtual double calculateAngleOfInclination(RenderObject* object) override;
		virtual bool isInsideBoundingBox(RenderObject* object) override;

		Ref<Texture> getTexture() { return texture; }

	private:
		int vertexCount;
		int indexCount;

		Ref<VertexBuffer> vertexBuffer;
		Ref<IndexBuffer> indexBuffer;

		Ref<Texture> texture;
	};

}

#endif