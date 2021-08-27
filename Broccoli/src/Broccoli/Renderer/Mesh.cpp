#include "Mesh.h"

namespace Broccoli {
	Mesh::Mesh(std::vector<Vertex>* vertices, std::vector<uint32_t>* indices, glm::mat4 transform)
	{
		indexCount = indices->size();
		vertexCount = vertices->size();

		vertexBuffer = VertexBuffer::create(vertices);
		indexBuffer = IndexBuffer::create(indices);

		this->transform.transform = transform;
		this->transform.inverseTransform = glm::transpose(glm::inverse(transform));
		this->transform.hasTexture = false;
	}

	Mesh::Mesh(std::vector<Vertex>* vertices, std::vector<uint32_t>* indices, glm::mat4 transform, Ref<Texture> texture)
	{
		indexCount = indices->size();
		vertexCount = vertices->size();

		this->texture = texture;

		vertexBuffer = VertexBuffer::create(vertices);
		indexBuffer = IndexBuffer::create(indices);

		this->transform.transform = transform;
		this->transform.inverseTransform = glm::transpose(glm::inverse(transform));
		this->transform.hasTexture = true;
	}

	Mesh::~Mesh()
	{
	}
}