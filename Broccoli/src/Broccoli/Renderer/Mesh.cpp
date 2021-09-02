#include "Mesh.h"

#include <unordered_map>

namespace Broccoli {
	Mesh::Mesh(std::vector<Vertex>* vertices, std::vector<uint32_t>* indices, glm::vec3 translate = glm::vec3(0, 0, 0), glm::vec3 scale = glm::vec3(1, 1, 1), glm::vec3 rotate = glm::vec3(0, 0, 0))
	{
		indexCount = indices->size();
		vertexCount = vertices->size();

		vertexBuffer = VertexBuffer::create(vertices);
		indexBuffer = IndexBuffer::create(indices);

		this->transform.translation = translate;
		this->transform.scale = scale;
		this->transform.rotation = rotate;
		this->transform.computeNewTransform();

		this->hasTexture = false;

		calculateBoundingBox();
	}

	Mesh::Mesh(std::vector<Vertex>* vertices, std::vector<uint32_t>* indices, Ref<Texture> texture, glm::vec3 translate = glm::vec3(0, 0, 0), glm::vec3 scale = glm::vec3(1, 1, 1), glm::vec3 rotate = glm::vec3(0, 0, 0))
	{
		indexCount = indices->size();
		vertexCount = vertices->size();

		this->texture = texture;

		vertexBuffer = VertexBuffer::create(vertices);
		indexBuffer = IndexBuffer::create(indices);

		this->transform.translation = translate;
		this->transform.scale = scale;
		this->transform.rotation = rotate;
		this->transform.computeNewTransform();

		this->hasTexture = true;

		calculateBoundingBox();
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

		int xMin = 0; // left
		int xMax = 0; // right
		int yMin = 0; // bottom
		int yMax = 0; // top
		int zMin = 0; // front
		int zMax = 0; // back

		std::cout << "\n\n";

		// Iterate through vertices of the mesh
		for (int i = 0; i < vertexBuffer->getVertices()->size(); i++)
		{
			Vertex vertex = vertexBuffer->getVertices()->at(i);

			if (vertex.pos.x < xMin) xMin = vertex.pos.x;
			else if (vertex.pos.x > xMax) xMax = vertex.pos.x;
			if (vertex.pos.y < yMin) yMin = vertex.pos.y;
			else if (vertex.pos.y > yMax) yMax= vertex.pos.y;
			if (vertex.pos.z < zMin) zMin = vertex.pos.z;
			else if (vertex.pos.z > zMax) zMax = vertex.pos.z;
		}

		//int width = std::abs(xMax - xMin);
		//int height = std::abs(yMax - yMin);
		//int depth = std::abs(zMax - zMin);

		//std::cout << width << " " << height << " " << depth << "\n";

		boundingBox = {
			{ { xMin, yMin, zMax }, { 1.0, 0.0, 1.0 }, { 0.0, 0.0 }, { 0.0, 0.0, 0.0 }}, // 0
			{ {xMax, yMin, zMax }, { 1.0, 0.0, 1.0 }, { 1.0, 0.0 }, { 0.0, 0.0, 0.0 }},
			{ { xMin, yMax, zMax }, { 1.0, 0.0, 1.0 }, { 0.0, 1.0 }, { 0.0, 0.0, 0.0 }},
			{ { xMax, yMax, zMax }, { 1.0, 0.0, 1.0 }, { 1.0, 1.0 }, { 0.0, 0.0, 0.0 }},

			{ { xMax, yMin, zMin }, { 0.0, 1.0, 1.0 }, { 1.0, 0.0 }, { 0.0, 0.0, 0.0 }}, // 4
			{ { xMax, yMin, zMax }, { 0.0, 1.0, 1.0 }, { 0.0, 0.0 }, { 0.0, 0.0, 0.0 }},
			{ { xMax, yMax, zMax }, { 0.0, 1.0, 1.0 }, { 0.0, 1.0 }, { 0.0, 0.0, 0.0 }},
			{ { xMax, yMax, zMin }, { 0.0, 1.0, 1.0 }, { 1.0, 1.0 }, { 0.0, 0.0, 0.0 }},

			{ { xMax, yMin, zMin }, { 1.0, 1.0, 0.0 }, { 1.0, 0.0 }, { 0.0, 0.0, 0.0 }}, // 8 back
			{ { xMin, yMin, zMin }, { 1.0, 1.0, 0.0 }, { 0.0, 0.0 }, { 0.0, 0.0, 0.0 }},
			{ { xMax, yMax, zMin }, { 1.0, 1.0, 0.0 }, { 1.0, 1.0 }, { 0.0, 0.0, 0.0 }},
			{ { xMin, yMax, zMin }, { 1.0, 1.0, 0.0 }, { 0.0, 1.0 }, { 0.0, 0.0, 0.0 }},

			{ { xMin, yMin, zMin }, { 1.0, 0.0, 0.0 }, { 0.0, 0.0 }, { 0.0, 0.0, 0.0 }}, // 12 left
			{ { xMin, yMin, zMax }, { 1.0, 0.0, 0.0 }, { 1.0, 0.0 }, { 0.0, 0.0, 0.0 }},
			{ { xMin, yMax, zMin }, { 1.0, 0.0, 0.0 }, { 0.0, 1.0 }, { 0.0, 0.0, 0.0 }},
			{ { xMin, yMax, zMax }, { 1.0, 0.0, 0.0 }, { 1.0, 1.0 }, { 0.0, 0.0, 0.0 }},

			{ { xMin, yMin, zMin }, { 0.0, 0.0, 1.0 }, { 0.0, 1.0 }, { 0.0, 0.0, 0.0 }}, // 16 bottom
			{ { xMax, yMin, zMin }, { 0.0, 0.0, 1.0 }, { 0.0, 0.0 }, { 0.0, 0.0, 0.0 }},
			{ { xMin, yMin, zMax }, { 0.0, 0.0, 1.0 }, { 1.0, 1.0 }, { 0.0, 0.0, 0.0 }},
			{ { xMax, yMin, zMax }, { 0.0, 0.0, 1.0 }, { 1.0, 0.0 }, { 0.0, 0.0, 0.0 }},

			{ { xMin, yMax, zMax }, { 1.0, 0.0, 0.0 }, { 0.0, 0.0 }, { 0.0, 0.0, 0.0 }}, // 20 Top
			{ { xMax, yMax, zMax }, { 1.0, 0.0, 0.0 }, { 0.0, 1.0 }, { 0.0, 0.0, 0.0 }},
			{ { xMin, yMax, zMin }, { 1.0, 0.0, 0.0 }, { 1.0, 0.0 }, { 0.0, 0.0, 0.0 }},
			{ { xMax, yMax, zMin }, { 1.0, 0.0, 0.0 }, { 1.0, 1.0 }, { 0.0, 0.0, 0.0 }},

		};
	}
}