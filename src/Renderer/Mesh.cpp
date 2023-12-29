#include "Mesh.h"

#include <unordered_map>
#include <limits>
#include <math.h>
#include <glm/gtx/string_cast.hpp>

namespace Broccoli {
	Mesh::Mesh(std::vector<Vertex> vertices, std::vector<uint32_t>* indices, glm::vec3 translate, glm::vec3 scale, glm::vec3 rotate)
	{
		indexCount = indices->size();
		vertexCount = vertices.size();

		vertexBuffer = VertexBuffer::create(vertices);
		indexBuffer = IndexBuffer::create(indices);

		this->transform.translation = translate;
		this->transform.scale = scale;
		this->transform.rotation = rotate;
		this->transform.computeNewTransform();

		this->hasTexture = false;

		calculateBoundingBox();
	}

	Mesh::Mesh(std::vector<Vertex> vertices, std::vector<uint32_t>* indices, Ref<Texture> texture, glm::vec3 translate, glm::vec3 scale, glm::vec3 rotate)
	{
		indexCount = indices->size();
		vertexCount = vertices.size();

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

	// TODO: Remove this constructor as it doesn't set the individual components
	// Explicitly set transform as a matrix instead of individual components
	Mesh::Mesh(std::vector<Vertex> vertices, std::vector<uint32_t>* indices, glm::mat4 transform)
	{
		indexCount = indices->size();
		vertexCount = vertices.size();

		this->texture = texture;

		vertexBuffer = VertexBuffer::create(vertices);
		indexBuffer = IndexBuffer::create(indices);

		this->hasTexture = false;
		this->transform.setTransform(transform);

		calculateBoundingBox();
	}

	// Explicitly set transform as a matrix instead of individual components (With texture)
	Mesh::Mesh(std::vector<Vertex> vertices, std::vector<uint32_t>* indices, Ref<Texture> texture, glm::mat4 transform)
	{
		indexCount = indices->size();
		vertexCount = vertices.size();

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

		//xMin = std::numeric_limits<int>::max(); // left
		//xMax = std::numeric_limits<int>::min(); // right
		//yMin = std::numeric_limits<int>::max(); // bottom
		//yMax = std::numeric_limits<int>::min(); // top
		//zMin = std::numeric_limits<int>::max(); // front
		//zMax = std::numeric_limits<int>::min(); // back

		int maxInt = std::numeric_limits<int>::max();
		int minInt = std::numeric_limits<int>::min();



		maxPoint = glm::vec3(minInt, minInt, minInt);
		minPoint = glm::vec3(maxInt, maxInt, maxInt);

		findMinMaxVerticies();

		origin = glm::vec3(minPoint.x + maxPoint.x / 2, minPoint.y+maxPoint.y /2, minPoint.z + maxPoint.z /2);


		width = std::abs(maxPoint.x - minPoint.x);
		height = std::abs(maxPoint.y - minPoint.y);
		depth = std::abs(maxPoint.z - minPoint.z);

		//std::cout << width << " " << height << " " << depth << "\n";
		//std::cout << "Min dimensions for bounding box: " << xMin << " " << xMax << " " << yMin << " " << yMax << " " << zMin << " " << zMax << "\n";

		boundingBox = {
			{ { minPoint.x, minPoint.y, maxPoint.z }, { 1.0, 0.0, 1.0 }, { 0.0, 0.0 }, { 0.0, 0.0, 0.0 }}, // 0
			{ {maxPoint.x, minPoint.y, maxPoint.z }, { 1.0, 0.0, 1.0 }, { 1.0, 0.0 }, { 0.0, 0.0, 0.0 }},
			{ { minPoint.x, maxPoint.y, maxPoint.z }, { 1.0, 0.0, 1.0 }, { 0.0, 1.0 }, { 0.0, 0.0, 0.0 }},
			{ { maxPoint.x, maxPoint.y, maxPoint.z }, { 1.0, 0.0, 1.0 }, { 1.0, 1.0 }, { 0.0, 0.0, 0.0 }},

			{ { maxPoint.x, minPoint.y, minPoint.z }, { 0.0, 1.0, 1.0 }, { 1.0, 0.0 }, { 0.0, 0.0, 0.0 }}, // 4
			{ { maxPoint.x, minPoint.y, minPoint.z }, { 0.0, 1.0, 1.0 }, { 0.0, 0.0 }, { 0.0, 0.0, 0.0 }},
			{ { maxPoint.x, maxPoint.y, maxPoint.z }, { 0.0, 1.0, 1.0 }, { 0.0, 1.0 }, { 0.0, 0.0, 0.0 }},
			{ { maxPoint.x, maxPoint.y, minPoint.z }, { 0.0, 1.0, 1.0 }, { 1.0, 1.0 }, { 0.0, 0.0, 0.0 }},

			{ { maxPoint.x, minPoint.y, minPoint.z }, { 1.0, 1.0, 0.0 }, { 1.0, 0.0 }, { 0.0, 0.0, 0.0 }}, // 8 back
			{ { minPoint.x, minPoint.y, minPoint.z }, { 1.0, 1.0, 0.0 }, { 0.0, 0.0 }, { 0.0, 0.0, 0.0 }},
			{ { maxPoint.x, maxPoint.y, minPoint.z }, { 1.0, 1.0, 0.0 }, { 1.0, 1.0 }, { 0.0, 0.0, 0.0 }},
			{ { minPoint.x, maxPoint.y, minPoint.z }, { 1.0, 1.0, 0.0 }, { 0.0, 1.0 }, { 0.0, 0.0, 0.0 }},

			{ { minPoint.x, minPoint.y, minPoint.z }, { 1.0, 0.0, 0.0 }, { 0.0, 0.0 }, { 0.0, 0.0, 0.0 }}, // 12 left
			{ { minPoint.x, minPoint.y, maxPoint.z }, { 1.0, 0.0, 0.0 }, { 1.0, 0.0 }, { 0.0, 0.0, 0.0 }},
			{ { minPoint.x, maxPoint.y, minPoint.z }, { 1.0, 0.0, 0.0 }, { 0.0, 1.0 }, { 0.0, 0.0, 0.0 }},
			{ { minPoint.x, maxPoint.y, maxPoint.z }, { 1.0, 0.0, 0.0 }, { 1.0, 1.0 }, { 0.0, 0.0, 0.0 }},

			{ { minPoint.x, minPoint.y, minPoint.z }, { 0.0, 0.0, 1.0 }, { 0.0, 1.0 }, { 0.0, 0.0, 0.0 }}, // 16 bottom
			{ { maxPoint.x, minPoint.y, minPoint.z }, { 0.0, 0.0, 1.0 }, { 0.0, 0.0 }, { 0.0, 0.0, 0.0 }},
			{ { minPoint.x, minPoint.y, maxPoint.z }, { 0.0, 0.0, 1.0 }, { 1.0, 1.0 }, { 0.0, 0.0, 0.0 }},
			{ { maxPoint.x, minPoint.y, maxPoint.z }, { 0.0, 0.0, 1.0 }, { 1.0, 0.0 }, { 0.0, 0.0, 0.0 }},

			{ { minPoint.x, maxPoint.y, maxPoint.z }, { 1.0, 0.0, 0.0 }, { 0.0, 0.0 }, { 0.0, 0.0, 0.0 }}, // 20 Top
			{ { maxPoint.x, maxPoint.y, maxPoint.z }, { 1.0, 0.0, 0.0 }, { 0.0, 1.0 }, { 0.0, 0.0, 0.0 }},
			{ { minPoint.x, maxPoint.y, minPoint.z }, { 1.0, 0.0, 0.0 }, { 1.0, 0.0 }, { 0.0, 0.0, 0.0 }},
			{ { maxPoint.x, maxPoint.y, minPoint.z }, { 1.0, 0.0, 0.0 }, { 1.0, 1.0 }, { 0.0, 0.0, 0.0 }},
		};

		// TODO: Put this somewhere that makes sense
		findMaxAndMinHeight();
	}

	void Mesh::findMinMaxVerticies()
	{
		glm::vec3 pos;

		// Iterate through vertices of the mesh
		for (int i = 0; i < vertexBuffer->getVertices().size(); i++)
		{
			Vertex vertex = vertexBuffer->getVertices().at(i);


			pos = glm::vec3(transform.transform * glm::vec4(vertex.pos, 1));


			//std::cout << vertex.pos.x << " " << xMin << " " << vertex.pos.y << " " << yMin << " " << vertex.pos.z << " " << zMin << "\n";

			if (pos.x < minPoint.x) {
				minPoint.x = pos.x;
				//xMin = vertex.pos.x;
			}
			else if (pos.x > maxPoint.x) {
				maxPoint.x = pos.x;
				//xMax = vertex.pos.x;
			}
			if (pos.y < minPoint.y) {
				minPoint.y = pos.y;
				//yMin = vertex.pos.y;
			}
			else if (pos.y > maxPoint.y) {
				maxPoint.y = pos.y;
				//yMax = vertex.pos.y;
			}
			if (pos.z < minPoint.z) {
				minPoint.z = pos.z;
				//zMin = vertex.pos.z;
			}
			else if (pos.z > maxPoint.z)
			{
				maxPoint.z = pos.z;
				//zMax = vertex.pos.z;
			}
		}
	}

	void Mesh::findMaxAndMinHeight()
	{
		glm::vec3 maxY = { 0,std::numeric_limits<int>::min(),0 };
		glm::vec3 minY = { 0,std::numeric_limits<int>::max(),0 };

		for (int i = 0; i < vertexBuffer->getVertices().size(); i++)
		{
			Vertex point = vertexBuffer->getVertices().at(i);

			//glm::vec3 pointTransform = glm::vec3(transform.transform * glm::vec4(point.pos, 1));

			//std::cout << "Point " << i << " has coordinates " << glm::to_string(point.pos) << "\n";

			if (point.pos.y > maxY.y) maxY = point.pos;
			else if (point.pos.y < minY.y) minY = point.pos;
		}

		//maxY = glm::vec3(transform.transform * glm::vec4(maxY, 0));
		//minY = glm::vec3(transform.transform * glm::vec4(minY, 0));

		startPointSlope = maxY;
		endPointSlope = minY;
	}

	double Mesh::calculateAngleOfInclination(RenderObject* object)
	{
		float zDistance = std::abs(startPointSlope.z - endPointSlope.z);
		float yDistance = std::abs(startPointSlope.y - endPointSlope.y);
		float xDistance = std::abs(startPointSlope.x - endPointSlope.x);

		// Transform start and end point using transform matrix
		glm::vec3 startPointSlope = glm::vec3(transform.transform * glm::vec4(startPointSlope, 1));
		glm::vec3 endPointSlope = glm::vec3(transform.transform * glm::vec4(endPointSlope, 1));

		std::cout << "Max y points are x : " << startPointSlope.x << " y : " << startPointSlope.y << " z : " << startPointSlope.z << "\n";
		std::cout << "Min y points are x: " << endPointSlope.x << " y: " << endPointSlope.y << " z: " << endPointSlope.z << "\n";
		std::cout << "x distance: " << xDistance << " y distance: " << yDistance << " z distance: " << zDistance << "\n";

		int height;

		if (zDistance > xDistance) {
			int point;
			// Check that the distance between endSlope.z - zMin is greater than startSlope.z - zMin
			
			if (startPointSlope.z > endPointSlope.z)
			{
				point = object->maxPoint.z;
			}
			else {
				point = object->minPoint.z;
			}

			height = (((point - endPointSlope.z) / (startPointSlope.z - endPointSlope.z)) * (startPointSlope.y - endPointSlope.y) + endPointSlope.y);
			
		}
		else {
			int point;

			// Check that the distance between endSlope.z - zMin is greater than startSlope.z - zMin

			if (startPointSlope.x < endPointSlope.x)
			{
				//std::cout << std::abs(xMin - endPointSlope.x) << " " << std::abs(xMin - startPointSlope.x) << "\n";
				point = object->minPoint.x;
			}
			else {
				point = object->maxPoint.x;
			}


			height = (((point - endPointSlope.x) / (startPointSlope.x - endPointSlope.x)) * (startPointSlope.y - endPointSlope.y) + endPointSlope.y);
		}

		return height;
		
	}
	bool Mesh::isInsideBoundingBox(RenderObject* object)
	{
		//findMinMaxVerticies();
		//object->findMinMaxVerticies();

		//glm::vec3 objectPosMin = glm::vec3(object->getTransform() * glm::vec4(object->minPoint, 1));
		//glm::vec3 objectPosMax = glm::vec3(object->getTransform() * glm::vec4(object->maxPoint, 1));
		//glm::vec3 posMin = glm::vec3(transform.transform * glm::vec4(minPoint, 1));
		//glm::vec3 posMax = glm::vec3(transform.transform * glm::vec4(maxPoint, 1));

		glm::vec3 minTrans = glm::vec3(transform.transform * glm::vec4(minPoint.x, minPoint.y, minPoint.z, 1));
		glm::vec3 maxTrans = glm::vec3(transform.transform * glm::vec4(maxPoint.x, maxPoint.y, maxPoint.z, 1));

		glm::vec3 objectMinTrans = glm::vec3(object->getTransform() * glm::vec4(object->minPoint.x, object->minPoint.x, object->minPoint.z, 1));
		glm::vec3 objectMaxTrans = glm::vec3(object->getTransform() * glm::vec4(object->maxPoint.x, object->maxPoint.y, object->maxPoint.z, 1));

		//std::cout << "object xMin,yMin,zMin transformed: " << objectPosMin.x << " " << objectPosMin.y << " " << objectPosMin.z << " xMax, yMax, zMax: " << objectPosMax.x << " " << objectPosMax.y << " " << objectPosMax.z << "\n";
		//std::cout << "Pos xMin,yMin,zMin transformed: " << posMin.x << " " << posMin.y << " " << posMin.z << " xMax, yMax, zMax: " << posMax.x << " " << posMax.y << " " << posMax.z << "\n";
		//std::cout << "object x,y,z min/max: " << object->xMin << " " << object->xMax << " " << object->yMin << " " << object->yMax << " " << object->zMin << " " << object->zMax << "\n";
		//std::cout << "comparision x,y,z min/max" << xMin << " " << xMax << " " << yMin << " " << yMax << " " << zMin << " " << zMax << "\n";

		//std::cout << "Object min: " << glm::to_string(objectMinTrans) << "\n";
		//std::cout << "Object max: " << glm::to_string(objectMaxTrans) << "\n";
		//std::cout << "Local min: " << glm::to_string(minTrans) << "\n";
		//std::cout << "Local max: " << glm::to_string(maxTrans) << "\n";

		std::cout << "Object min: " << glm::to_string(object->minPoint) << "\n";
		std::cout << "Object max: " << glm::to_string(object->maxPoint) << "\n";
		std::cout << "Local min: " << glm::to_string(minPoint) << "\n";
		std::cout << "Local max: " << glm::to_string(maxPoint) << "\n";

		//return (objectMinTrans.x <= minTrans.x && objectMaxTrans.x >= maxTrans.x) &&
			//(objectMinTrans.y <= minTrans.y && objectMaxTrans.y >= maxTrans.y) &&
			//(objectMinTrans.z <= minTrans.z && objectMaxTrans.z >= maxTrans.z);
		
		return (object->minPoint.x <= maxPoint.x && object->maxPoint.x >= minPoint.x) &&
			(object->minPoint.y <= maxPoint.y && object->maxPoint.y >= minPoint.y) &&
			(object->minPoint.z <= maxPoint.z && object->maxPoint.z >= minPoint.z);
	}
}	