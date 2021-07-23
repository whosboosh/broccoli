#include "Mesh.h"

namespace Broccoli {
	Mesh::Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices, const glm::mat4& transform)
	{
		std::cout << vertices[0].pos.x;

		Ref<VertexBuffer> vertexBuffer = VertexBuffer::create(vertices);
		Ref<IndexBuffer> indexBuffer = IndexBuffer::create(indices);

	}

	Mesh::~Mesh()
	{
	}
}