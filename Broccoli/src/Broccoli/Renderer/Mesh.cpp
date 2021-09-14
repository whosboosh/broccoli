#include "Mesh.h"

#include <unordered_map>
#include <limits>
#include <math.h>
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

	// TODO: Remove this constructor as it doesn't set the individual components
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

		xMin = std::numeric_limits<int>::max(); // left
		xMax = std::numeric_limits<int>::min(); // right
		yMin = std::numeric_limits<int>::max(); // bottom
		yMax = std::numeric_limits<int>::min(); // top
		zMin = std::numeric_limits<int>::max(); // front
		zMax = std::numeric_limits<int>::min(); // back

		glm::vec3 pos;

		// Iterate through vertices of the mesh
		for (int i = 0; i < vertexBuffer->getVertices()->size(); i++)
		{
			Vertex vertex = vertexBuffer->getVertices()->at(i);


			pos = glm::vec3(transform.transform * glm::vec4(vertex.pos, 1));


			//std::cout << vertex.pos.x << " " << xMin << " " << vertex.pos.y << " " << yMin << " " << vertex.pos.z << " " << zMin << "\n";

			if (pos.x < xMin) {
				xMin = pos.x;
			}
			else if (pos.x > xMax) {
				xMax = pos.x;
			}
			if (pos.y < yMin) {
				yMin = pos.y;
			}
			else if (pos.y > yMax) {
				yMax = pos.y;
			}
			if (pos.z < zMin) {
				zMin = pos.z;
			}
			else if (pos.z > zMax)
			{
				zMax = pos.z;
			}
		}

		origin = glm::vec3(xMin + xMax / 2, yMin+yMax /2, zMin+zMax /2);


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
		findMaxAndMinHeight();
	}

	void Mesh::findMaxAndMinHeight()
	{
		glm::vec3 maxY = { 0,std::numeric_limits<int>::min(),0 };
		glm::vec3 minY = { 0,std::numeric_limits<int>::max(),0 };

		for (int i = 0; i < vertexBuffer->getVertices()->size(); i++)
		{
			Vertex point = vertexBuffer->getVertices()->at(i);

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

	double Mesh::calculateAngleOfInclination(glm::vec3 point)
	{
		// The map model file has been separated out into meshes for areas that have height differences.
		// If an entity is intersecting with part of a mesh that is inclined, (use bounding box), use trig to find the angle inclination from lowest part of mesh to highest part
		// All slopes are a single gradient so this works

		// Find the vertices in the mesh with the highest and lowest y axis coordinates
		// We'll use these for calculating the angle

		//glm::mat4 mapTransform = glm::mat4(1.0f);
		//mapTransform = glm::rotate(mapTransform, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		//mapTransform = glm::rotate(mapTransform, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		//mapTransform = glm::scale(mapTransform, glm::vec3(0.1f, 0.1f, 0.1f));

		// Transform the local coordinates using the model matrix to world space
		//maxY.pos = glm::vec3(transform.transform * glm::vec4(maxY.pos, 0));
		//minY.pos = glm::vec3(transform.transform * glm::vec4(minY.pos, 0));

		//std::cout << "Transform matrix: " << glm::to_string(transform.transform) << "\n";

		// Get distance between y and z coordinates between maxY and minY
		float zDistance = std::abs(startPointSlope.z - endPointSlope.z);
		float yDistance = std::abs(startPointSlope.y - endPointSlope.y);
		float xDistance = std::abs(startPointSlope.x - endPointSlope.x);

		std::cout << "Max y points are x : " << startPointSlope.x << " y : " << startPointSlope.y << " z : " << startPointSlope.z << "\n";
		std::cout << "Min y points are x: " << endPointSlope.x << " y: " << endPointSlope.y << " z: " << endPointSlope.z << "\n";
		std::cout << "x distance: " << xDistance << " y distance: " << yDistance << " z distance: " << zDistance << "\n";

		if (zDistance > xDistance) return ((point.z - endPointSlope.z) / (startPointSlope.z - endPointSlope.z)) * (startPointSlope.y - endPointSlope.y) + endPointSlope.y;
		else return ((point.x - endPointSlope.x) / (startPointSlope.x - endPointSlope.x))* (startPointSlope.y - endPointSlope.y) + endPointSlope.y;

		//int height =  ((endPointSlope.x - endPointSlope.x) / (startPointSlope.x - endPointSlope.x)) * (startPointSlope.y - endPointSlope.y) + endPointSlope.y;

		//std::cout << "Final height: " << height << "\n";
		//return height;

		//std::cout << "Col: " << maxY.r << " " << maxY.g << " " << maxY.b << " " << "Max y points are x : " << glm::to_string(maxY) << "\n";
		//std::cout << "Col: " << minY.r << " " << minY.g << " " << minY.b << " " << "Min y points are x : " << glm::to_string(minY) << "\n";
		//double zAngle = std::atan(yDistance / zDistance);
		//double xAngle = std::atan(yDistance / xDistance);

		//std::cout << "x angle of mesh is: " << glm::degrees(xAngle) << "\n";
		//std::cout << "z angle of mesh is: " << glm::degrees(zAngle) << "\n";
		// Find the y position of any coordinate on the plane using tan
		//std::cout << "Tan of z angle: " << std::tan(zAngle) << " When multiplied by z position: " << maxY.z * std::tan(zAngle) << "\n";
		//std::cout << "Tan of x angle: " << std::tan(xAngle) << " When multiplied by x position: " << maxY.x * std::tan(xAngle) << "\n";
		
	}
	bool Mesh::isInsideBoundingBox(RenderObject* object)
	{
		//glm::vec3 objectPosMin = glm::vec3(object->getTransform() * glm::vec4(glm::vec3(object->xMin, object->yMin, object->zMin), 1));
		//glm::vec3 objectPosMax = glm::vec3(object->getTransform() * glm::vec4(glm::vec3(object->xMax, object->yMax, object->zMax), 1));

		//glm::vec3 posMin = glm::vec3(transform.transform * glm::vec4(glm::vec3(xMin, yMin, yMax), 1));
		//glm::vec3 posMax = glm::vec3(transform.transform * glm::vec4(glm::vec3(xMax, yMax, zMax), 1));

		//std::cout << "object xMin,yMin,zMin transformed: " << objectPosMin.x << " " << objectPosMin.y << " " << objectPosMin.z << "\n";
		//std::cout << "Pos xMin,yMin,zMin transformed: " << posMin.x << " " << posMin.y << " " << posMin.z << "\n";
		//std::cout << "object x,y,z min/max: " << object->xMin << " " << object->xMax << " " << object->yMin << " " << object->yMax << " " << object->zMin << " " << object->zMax << "\n";
		//std::cout << "comparision x,y,z min/max" << xMin << " " << xMax << " " << yMin << " " << yMax << " " << zMin << " " << zMax << "\n";


		//return (objectPosMin.x <= posMax.x && objectPosMax.x >= posMin.x) &&
			//(objectPosMin.y <= posMax.y && objectPosMax.y >= posMin.y) &&
			//(objectPosMin.z <= posMax.z && objectPosMax.z >= posMin.z);

		
		return (object->xMin <= xMax && object->xMax >= xMin) &&
			(object->yMin <= yMax && object->yMax >= yMin) &&
			(object->zMin <= zMax && object->zMax >= zMin);
	}
}	