#include "Mesh.h"

#include <unordered_map>
#include <limits>
#include <glm/gtx/string_cast.hpp>

namespace Broccoli {
	Mesh::Mesh(std::vector<Vertex>* vertices, std::vector<uint32_t>* indices, glm::vec3 translate, glm::vec3 scale, glm::vec3 rotate)
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

	Mesh::Mesh(std::vector<Vertex>* vertices, std::vector<uint32_t>* indices, Ref<Texture> texture, glm::vec3 translate, glm::vec3 scale, glm::vec3 rotate)
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

	// Explicitly set transform as a matrix instead of individual components
	Mesh::Mesh(std::vector<Vertex>* vertices, std::vector<uint32_t>* indices, glm::mat4 transform)
	{
		indexCount = indices->size();
		vertexCount = vertices->size();

		this->texture = texture;

		vertexBuffer = VertexBuffer::create(vertices);
		indexBuffer = IndexBuffer::create(indices);

		this->hasTexture = false;
		this->transform.setTransform(transform);

		calculateBoundingBox();
	}

	// Explicitly set transform as a matrix instead of individual components (With texture)
	Mesh::Mesh(std::vector<Vertex>* vertices, std::vector<uint32_t>* indices, Ref<Texture> texture, glm::mat4 transform)
	{
		indexCount = indices->size();
		vertexCount = vertices->size();

		this->texture = texture;

		vertexBuffer = VertexBuffer::create(vertices);
		indexBuffer = IndexBuffer::create(indices);

		this->hasTexture = true;
		this->transform.setTransform(transform);

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

		int xMin = std::numeric_limits<int>::max(); // left
		int xMax = std::numeric_limits<int>::min(); // right
		int yMin = std::numeric_limits<int>::max(); // bottom
		int yMax = std::numeric_limits<int>::min(); // top
		int zMin = std::numeric_limits<int>::max(); // front
		int zMax = std::numeric_limits<int>::min(); // back

		// Iterate through vertices of the mesh
		for (int i = 0; i < vertexBuffer->getVertices()->size(); i++)
		{
			Vertex vertex = vertexBuffer->getVertices()->at(i);

			//std::cout << vertex.pos.x << " " << xMin << " " << vertex.pos.y << " " << yMin << " " << vertex.pos.z << " " << zMin << "\n";

			if (vertex.pos.x < xMin) {
				xMin = vertex.pos.x;
			}
			else if (vertex.pos.x > xMax) {
				xMax = vertex.pos.x;
			}
			if (vertex.pos.y < yMin) {
				yMin = vertex.pos.y;
			}
			else if (vertex.pos.y > yMax) {
				yMax = vertex.pos.y;
			}
			if (vertex.pos.z < zMin) {
				zMin = vertex.pos.z;
			}
			else if (vertex.pos.z > zMax)
			{
				zMax = vertex.pos.z;
			}
		}

		width = std::abs(xMax - xMin);
		height = std::abs(yMax - yMin);
		depth = std::abs(zMax - zMin);

		//std::cout << width << " " << height << " " << depth << "\n";
		//std::cout << "Min dimensions for bounding box: " << xMin << " " << xMax << " " << yMin << " " << yMax << " " << zMin << " " << zMax << "\n";

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

		// TODO: Put this somewhere that makes sense
		calculateAngleOfInclination();
	}

	void Mesh::calculateAngleOfInclination()
	{
		// The map model file has been separated out into meshes for areas that have height differences.
		// If an entity is intersecting with part of a mesh that is inclined, (use bounding box), use trig to find the angle inclination from lowest part of mesh to highest part
		// All slopes are a single gradient so this works
		
		// Find the vertices in the mesh with the highest and lowest y axis coordinates
		// We'll use these for calculating the angle
		Vertex maxY = { { 0,std::numeric_limits<int>::min(),0 }, {0,0,0}, {0,0}, {0,0,0} };
		Vertex minY = { { 0,std::numeric_limits<int>::max(),0 }, {0,0,0}, {0,0}, {0,0,0} };

		for (int i = 0; i < vertexBuffer->getVertices()->size(); i++)
		{
			Vertex point = vertexBuffer->getVertices()->at(i);
			if (point.pos.y > maxY.pos.y) maxY = point;
			else if (point.pos.y < minY.pos.y) minY = point;
		}

		// Get distance between y and z coordinates between maxY and minY
		float zDistance = std::abs(maxY.pos.z - minY.pos.z);
		float yDistance = std::abs(maxY.pos.y - minY.pos.y);
		float xDistance = std::abs(maxY.pos.x - minY.pos.x);

		std::cout << "x distance: " << xDistance << " y distance: " << yDistance << " z distance: " << zDistance << "\n";
		//std::cout << "Col: "<<maxY.col.r << " " << maxY.col.g << " " << maxY.col.b << " " << "Max y points are x : " << maxY.pos.x << " y : " << maxY.pos.y << " z : " << maxY.pos.z <<"\n";
		//std::cout << "Col: " << minY.col.r << " " << minY.col.g << " " << minY.col.b << " " << "Min y points are x: " << minY.pos.x << " y: " << minY.pos.y << " z: " << minY.pos.z << "\n";
		std::cout << "Col: " << maxY.col.r << " " << maxY.col.g << " " << maxY.col.b << " " << "Max y points are x : " << glm::to_string(glm::vec3(transform.transform * glm::vec4(maxY.pos, 0))) << "\n";
		std::cout << "Col: " << minY.col.r << " " << minY.col.g << " " << minY.col.b << " " << "Min y points are x : " << glm::to_string(glm::vec3(transform.transform * glm::vec4(minY.pos, 0))) << "\n";

		double xAngle = glm::atan(zDistance, yDistance);
		double zAngle = glm::atan(xDistance, yDistance);
		
		std::cout << "x angle of mesh is: " << glm::degrees(xAngle) << "\n";
		std::cout << "z angle of mesh is: " << glm::degrees(zAngle) << "\n";

		// Find the y position of any coordinate on the plane using tan
		std::cout << "Y axis of point z: -110: " << 550 * glm::tan(glm::degrees(zAngle)) << "\n";
		
	}
}