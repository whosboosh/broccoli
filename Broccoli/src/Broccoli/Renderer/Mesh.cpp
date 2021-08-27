#include "Mesh.h"

namespace Broccoli {
	Mesh::Mesh(std::vector<Vertex>* vertices, std::vector<uint32_t>* indices, glm::mat4 transform)
	{
		indexCount = indices->size();
		vertexCount = vertices->size();

		vertexBuffer = VertexBuffer::create(vertices);
		indexBuffer = IndexBuffer::create(indices);

		setTransform(transform);
		setAssetType(AssetType::Mesh);
	}

	Mesh::Mesh(std::vector<Vertex>* vertices, std::vector<uint32_t>* indices, glm::mat4 transform, Ref<Texture> texture)
	{
		indexCount = indices->size();
		vertexCount = vertices->size();

		this->texture = texture;

		vertexBuffer = VertexBuffer::create(vertices);
		indexBuffer = IndexBuffer::create(indices);

		setTransform(transform);
		setAssetType(AssetType::Mesh);
	}

	Mesh::~Mesh()
	{
	}
}