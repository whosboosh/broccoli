#include "Mesh.h"

#include <unordered_map>

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

		calculateBoundingBox();
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

	// Find the furthest points from each face for each side
	void Mesh::calculateBoundingBox()
	{
		// 0 is front (z > current z)
		// 1 is right (x > current x)
		// 2 is back (z < current z)
		// 3 is left (x < current x)
		// 4 is bottom (y < current y)
		// 5 is top (y > current y)

		int lowestZ = 0; // front
		int highestX = 0; // right
		int highestZ = 0; // back
		int lowestX = 0; // left
		int lowestY = 0; // bottom
		int highestY = 0; // top

		std::cout << "\n\n";

		// Each face
		for (int i = 0; i < 6; i++)
		{
			// Each vert in face
			for (int j = 0; j < 4; j++)
			{
				Vertex vertex = vertexBuffer->getVertices()->at((4 * i) + j);

				if (i == 0)
				{
					if (vertex.pos.z > highestZ) highestZ = vertex.pos.z;
				}
				else if (i == 1)
				{
					if (vertex.pos.x > highestX) highestX = vertex.pos.x;
				}
				else if (i == 2)
				{
					if (vertex.pos.z < lowestZ) lowestZ = vertex.pos.z;
				}
				else if (i == 3)
				{
					if (vertex.pos.x < lowestX) lowestX = vertex.pos.x;
				}
				else if (i == 4)
				{
					if (vertex.pos.y < lowestY) lowestY = vertex.pos.y;
				}
				else if (i == 5)
				{
					if (vertex.pos.y > highestY) highestY = vertex.pos.y;
				}
			}

			// Add face with set bounds to boundingBox vector
			// TODO: This wont work because need to update all positions not just ones for that face
			for (int k = 0; k < 4; k++)
			{
				std::cout << "highest z: " << lowestZ << " " << k << "\n";
				std::cout << "lowest z: " << highestZ << " " << k << "\n";
				std::cout << "lowest y: " << lowestY << " " << k << "\n";
				std::cout << "highest y: " << highestY << " " << k << "\n";
				std::cout << "lowest x: " << lowestZ << " " << k << "\n";
				std::cout << "highest x: " << lowestZ << " " << k << "\n";

				Vertex vertex = vertexBuffer->getVertices()->at((4 * i) + k);

				if (i == 0) vertex.pos.z = lowestZ+0.2;
				else if (i == 1) vertex.pos.x = highestX+0.2;
				else if (i == 2) vertex.pos.z = highestZ-0.2;
				else if (i == 3) vertex.pos.x = lowestX-0.2;
				else if (i == 4) vertex.pos.y = lowestY-0.2;
				else if (i == 5) vertex.pos.y = highestY+0.2;

				boundingBox.push_back(vertex);
			}
		}
	}
}