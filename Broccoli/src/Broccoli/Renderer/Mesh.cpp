#include "Mesh.h"

namespace Broccoli {
	Mesh::Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices, glm::mat4& transform)
	{
		vertexBuffer = VertexBuffer::create(vertices);
		indexBuffer = IndexBuffer::create(indices);

		meshInfo.transform = transform;
		meshInfo.inverseTransform = glm::transpose(glm::inverse(transform));
		meshInfo.hasTexture = false;
	}

	Mesh::~Mesh()
	{
	}


	void Mesh::setTransform(glm::mat4 newModel)
	{
		meshInfo.transform = newModel;
		meshInfo.inverseTransform = glm::transpose(glm::inverse(newModel));
	}
}