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

		xMin = glm::vec3(std::numeric_limits<int>::max()); // left
		xMax = glm::vec3(std::numeric_limits<int>::min()); // right
		yMin = glm::vec3(std::numeric_limits<int>::max()); // bottom
		yMax = glm::vec3(std::numeric_limits<int>::min()); // top
		zMin = glm::vec3(std::numeric_limits<int>::max()); // front
		zMax = glm::vec3(std::numeric_limits<int>::min()); // back

		findMinMaxVerticies();

		origin = glm::vec3(xMin.x + xMax.x / 2, yMin.y+yMax.x /2, zMin.z+zMax.z /2);


		width = std::abs(xMax.x - xMin.x);
		height = std::abs(yMax.y - yMin.y);
		depth = std::abs(zMax.z - zMin.z);

		//std::cout << width << " " << height << " " << depth << "\n";
		//std::cout << "Min dimensions for bounding box: " << xMin << " " << xMax << " " << yMin << " " << yMax << " " << zMin << " " << zMax << "\n";

		boundingBox = {
			{ { xMin.x, yMin.y, zMax.z }, { 1.0, 0.0, 1.0 }, { 0.0, 0.0 }, { 0.0, 0.0, 0.0 }}, // 0
			{ {xMax.x, yMin.y, zMax.z }, { 1.0, 0.0, 1.0 }, { 1.0, 0.0 }, { 0.0, 0.0, 0.0 }},
			{ { xMin.x, yMax.y, zMax.z }, { 1.0, 0.0, 1.0 }, { 0.0, 1.0 }, { 0.0, 0.0, 0.0 }},
			{ { xMax.x, yMax.y, zMax.z }, { 1.0, 0.0, 1.0 }, { 1.0, 1.0 }, { 0.0, 0.0, 0.0 }},

			{ { xMax.x, yMin.y, zMin.z }, { 0.0, 1.0, 1.0 }, { 1.0, 0.0 }, { 0.0, 0.0, 0.0 }}, // 4
			{ { xMax.x, yMin.y, zMax.z }, { 0.0, 1.0, 1.0 }, { 0.0, 0.0 }, { 0.0, 0.0, 0.0 }},
			{ { xMax.x, yMax.y, zMax.z }, { 0.0, 1.0, 1.0 }, { 0.0, 1.0 }, { 0.0, 0.0, 0.0 }},
			{ { xMax.x, yMax.y, zMin.z }, { 0.0, 1.0, 1.0 }, { 1.0, 1.0 }, { 0.0, 0.0, 0.0 }},

			{ { xMax.x, yMin.y, zMin.z }, { 1.0, 1.0, 0.0 }, { 1.0, 0.0 }, { 0.0, 0.0, 0.0 }}, // 8 back
			{ { xMin.x, yMin.y, zMin.z }, { 1.0, 1.0, 0.0 }, { 0.0, 0.0 }, { 0.0, 0.0, 0.0 }},
			{ { xMax.x, yMax.y, zMin.z }, { 1.0, 1.0, 0.0 }, { 1.0, 1.0 }, { 0.0, 0.0, 0.0 }},
			{ { xMin.x, yMax.y, zMin.z }, { 1.0, 1.0, 0.0 }, { 0.0, 1.0 }, { 0.0, 0.0, 0.0 }},

			{ { xMin.x, yMin.y, zMin.z }, { 1.0, 0.0, 0.0 }, { 0.0, 0.0 }, { 0.0, 0.0, 0.0 }}, // 12 left
			{ { xMin.x, yMin.y, zMax.z }, { 1.0, 0.0, 0.0 }, { 1.0, 0.0 }, { 0.0, 0.0, 0.0 }},
			{ { xMin.x, yMax.y, zMin.z }, { 1.0, 0.0, 0.0 }, { 0.0, 1.0 }, { 0.0, 0.0, 0.0 }},
			{ { xMin.x, yMax.y, zMax.z }, { 1.0, 0.0, 0.0 }, { 1.0, 1.0 }, { 0.0, 0.0, 0.0 }},

			{ { xMin.x, yMin.y, zMin.z }, { 0.0, 0.0, 1.0 }, { 0.0, 1.0 }, { 0.0, 0.0, 0.0 }}, // 16 bottom
			{ { xMax.x, yMin.y, zMin.z }, { 0.0, 0.0, 1.0 }, { 0.0, 0.0 }, { 0.0, 0.0, 0.0 }},
			{ { xMin.x, yMin.y, zMax.z }, { 0.0, 0.0, 1.0 }, { 1.0, 1.0 }, { 0.0, 0.0, 0.0 }},
			{ { xMax.x, yMin.y, zMax.z }, { 0.0, 0.0, 1.0 }, { 1.0, 0.0 }, { 0.0, 0.0, 0.0 }},

			{ { xMin.x, yMax.y, zMax.z }, { 1.0, 0.0, 0.0 }, { 0.0, 0.0 }, { 0.0, 0.0, 0.0 }}, // 20 Top
			{ { xMax.x, yMax.y, zMax.z }, { 1.0, 0.0, 0.0 }, { 0.0, 1.0 }, { 0.0, 0.0, 0.0 }},
			{ { xMin.x, yMax.y, zMin.z }, { 1.0, 0.0, 0.0 }, { 1.0, 0.0 }, { 0.0, 0.0, 0.0 }},
			{ { xMax.x, yMax.y, zMin.z }, { 1.0, 0.0, 0.0 }, { 1.0, 1.0 }, { 0.0, 0.0, 0.0 }},
		};

		// TODO: Put this somewhere that makes sense
		findMaxAndMinHeight();
	}

	void Mesh::findMinMaxVerticies()
	{
		//glm::vec3 pos;

		// Iterate through vertices of the mesh
		for (int i = 0; i < vertexBuffer->getVertices().size(); i++)
		{
			Vertex vertex = vertexBuffer->getVertices().at(i);


			//pos = glm::vec3(transform.transform * glm::vec4(vertex.pos, 1));

			//std::cout << vertex.pos.x << " " << xMin << " " << vertex.pos.y << " " << yMin << " " << vertex.pos.z << " " << zMin << "\n";

			if (vertex.pos.x < xMin.x) {
				xMin = vertex.pos;
			}
			else if (vertex.pos.x > xMax.x) {
				xMax = vertex.pos;
			}
			if (vertex.pos.y < yMin.y) {
				yMin = vertex.pos;
			}
			else if (vertex.pos.y > yMax.y) {
				yMax = vertex.pos;
			}
			if (vertex.pos.z < zMin.z) {
				zMin = vertex.pos;
			}
			else if (vertex.pos.z > zMax.z)
			{
				zMax = vertex.pos;
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

			glm::vec3 pointTransform = glm::vec3(transform.transform * glm::vec4(point.pos, 1));

			//std::cout << "Point " << i << " has coordinates " << glm::to_string(point.pos) << "\n";

			if (pointTransform.y > maxY.y) maxY = pointTransform;
			else if (pointTransform.y < minY.y) minY = pointTransform;
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

		//std::cout << "Max y points are x : " << startPointSlope.x << " y : " << startPointSlope.y << " z : " << startPointSlope.z << "\n";
		//std::cout << "Min y points are x: " << endPointSlope.x << " y: " << endPointSlope.y << " z: " << endPointSlope.z << "\n";
		//std::cout << "x distance: " << xDistance << " y distance: " << yDistance << " z distance: " << zDistance << "\n";

		int height;

		if (zDistance > xDistance) {
			int point;
			int offset = 0;

			// Check that the distance between endSlope.z - zMin is greater than startSlope.z - zMin
			
			if (startPointSlope.z > endPointSlope.z)
			{
				point = object->zMax.z;
				offset = std::abs(object->origin.y - glm::vec3(object->getTransform() * glm::vec4(object->endPointSlope, 1)).y);
			}
			else {
				point = object->zMin.z;
				offset = -std::abs(object->origin.y - glm::vec3(object->getTransform() * glm::vec4(object->endPointSlope, 1)).y);
			}

			height = (((point - endPointSlope.z) / (startPointSlope.z - endPointSlope.z)) * (startPointSlope.y - endPointSlope.y) + endPointSlope.y) + offset;
			
		}
		else {
			int point;
			int offset = 0;

			// Check that the distance between endSlope.z - zMin is greater than startSlope.z - zMin

			if (startPointSlope.x < endPointSlope.x)
			{
				//std::cout << std::abs(xMin - endPointSlope.x) << " " << std::abs(xMin - startPointSlope.x) << "\n";

				/*
				if (std::abs(xMin - endPointSlope.x) < std::abs(xMin - startPointSlope.x))
				{
					offset = -std::abs(object->origin.y - glm::vec3(object->getTransform() * glm::vec4(object->endPointSlope, 1)).y) / 2;
				}
				else {
					offset = std::abs(object->origin.y - glm::vec3(object->getTransform() * glm::vec4(object->endPointSlope, 1)).y) / 2;
				}*/
				point = object->xMin.x;
			}
			else {
				point = object->xMax.x;
				//offset = std::abs(object->origin.y - glm::vec3(object->getTransform() * glm::vec4(object->endPointSlope, 1)).y);
			}


			height = (((point - endPointSlope.x) / (startPointSlope.x - endPointSlope.x)) * (startPointSlope.y - endPointSlope.y) + endPointSlope.y); // + offset;
		}

		return height;
		
	}
	bool Mesh::isInsideBoundingBox(RenderObject* object)
	{
		int xMinTrans = glm::vec3(transform.transform * glm::vec4(xMin,1)).x;
		int yMinTrans = glm::vec3(transform.transform * glm::vec4(yMin, 1)).y;
		int zMinTrans = glm::vec3(transform.transform * glm::vec4(zMin, 1)).z;
		int xMaxTrans = glm::vec3(transform.transform * glm::vec4(xMax, 1)).x;
		int yMaxTrans = glm::vec3(transform.transform * glm::vec4(yMax, 1)).y;
		int zMaxTrans = glm::vec3(transform.transform * glm::vec4(zMax, 1)).z;

		int objectXMinTrans = glm::vec3(transform.transform * glm::vec4(object->xMin, 1)).x;
		int objectYMinTrans = glm::vec3(transform.transform * glm::vec4(object->yMin, 1)).y;
		int objectZMinTrans = glm::vec3(transform.transform * glm::vec4(object->zMin, 1)).z;
		int objectXMaxTrans = glm::vec3(transform.transform * glm::vec4(object->xMax, 1)).x;
		int objectYMaxTrans = glm::vec3(transform.transform * glm::vec4(object->yMax, 1)).y;
		int objectZMaxTrans = glm::vec3(transform.transform * glm::vec4(object->zMax, 1)).z;


		//glm::vec3 minTrans = glm::vec3(transform.transform * glm::vec4(xMin, yMin, zMin, 1));
		//glm::vec3 maxTrans = glm::vec3(transform.transform * glm::vec4(xMax, yMax, zMax, 1));

		//glm::vec3 objectMinTrans = glm::vec3(object->getTransform() * glm::vec4(object->xMin, object->yMin, object->zMin, 1));
		//glm::vec3 objectMaxTrans = glm::vec3(object->getTransform() * glm::vec4(object->xMax, object->yMax, object->zMax, 1));

		std::cout << "Local min: " << glm::to_string(glm::vec3(xMinTrans, yMinTrans, zMinTrans)) << "\n";
		std::cout << "Local max: " << glm::to_string(glm::vec3(xMaxTrans, yMaxTrans, zMaxTrans)) << "\n";
		std::cout << "Object min: " << glm::to_string(glm::vec3(objectXMinTrans, objectYMinTrans, objectZMinTrans)) << "\n";
		std::cout << "Object max: " << glm::to_string(glm::vec3(objectXMaxTrans, objectYMaxTrans, objectZMaxTrans)) << "\n";

		//return (objectMinTrans.x <= minTrans.x && objectMaxTrans.x >= maxTrans.x) &&
			//(objectMinTrans.y <= minTrans.y && objectMaxTrans.y >= maxTrans.y) &&
			//(objectMinTrans.z <= minTrans.z && objectMaxTrans.z >= maxTrans.z);

		return (objectXMinTrans <= xMinTrans && objectXMaxTrans >= xMaxTrans) &&
			(objectYMinTrans <= yMinTrans && objectYMaxTrans >= yMaxTrans) &&
			(objectZMinTrans <= zMinTrans && objectZMaxTrans >= zMaxTrans);
		
		//return (object->xMin <= xMax && object->xMax >= xMin) &&
			//(object->yMin <= yMax && object->yMax >= yMin) &&
			//(object->zMin <= zMax && object->zMax >= zMin);
	}
}	