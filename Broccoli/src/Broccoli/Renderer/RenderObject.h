#pragma once

#include "Broccoli/Asset/Asset.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <vector>
#include "Broccoli/Renderer/VertexBuffer.h"

namespace Broccoli {
	struct TransformComponent
	{
		glm::vec3 translation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 scale = { 1.0f, 1.0f, 1.0f };

		glm::mat4 transform = glm::mat4(1.0f);
		glm::mat4 inverseTransform = glm::mat4(1.0f);

		TransformComponent() = default;
		TransformComponent(const TransformComponent& other) = default;
		TransformComponent(const glm::vec3& translation) { this->translation = translation; this->inverseTransform = glm::inverse(glm::transpose(getTransform())); }
 
		void computeNewTransform()
		{
			this->transform = glm::translate(glm::mat4(1.0f), translation) * glm::toMat4(glm::quat(rotation)) * glm::scale(glm::mat4(1.0f), scale);
			this->inverseTransform = glm::inverse(glm::transpose(this->transform));
		}

		void updateScale(glm::vec3 scale)
		{
			this->scale = scale;
			computeNewTransform();
		}

		void updateRotation(glm::vec3 rotation)
		{
			this->rotation = rotation;
			computeNewTransform();
		}

		void updateTranslation(glm::vec3 translation)
		{
			this->translation = translation;
			computeNewTransform();
		}

		void setTransform(glm::mat4 transform)
		{
			this->transform = transform;
			this->inverseTransform = glm::inverse(glm::transpose(this->transform));
		}

		glm::mat4 getTransform() const
		{
			return transform;
		}
		glm::mat4 getInverseTransform() const
		{
			return inverseTransform;
		}
	};

	struct MeshInfo {
		TransformComponent transform;
		glm::mat4 inverseTransform;
	};


	class RenderObject : public Asset
	{

	public:
		virtual ~RenderObject() {};

		void setTranslation(glm::vec3 translation)
		{
			transform.updateTranslation(translation);
		}
		void setRotation(glm::vec3 rotation)
		{
			transform.updateRotation(rotation);
		}
		void setScale(glm::vec3 scale)
		{
			transform.updateScale(scale);
		}

		TransformComponent getTransformComponent() { return transform; }
		glm::mat4 getTransform() { return transform.getTransform(); }

		virtual void calculateBoundingBox() = 0;

		std::vector<Vertex>* getBoundingBox() { return &boundingBox; }
		std::vector<uint32_t>* getBoundingIndices() { return &boundingIndices; }

		virtual void findMaxAndMinHeight() = 0;
		virtual double calculateAngleOfInclination(glm::vec3 point) = 0;
		virtual bool isInsideBoundingBox(RenderObject* object) = 0;

		int width, height, depth;
		int xMin, xMax, yMin, yMax, zMin, zMax;
		glm::vec3 origin;
		glm::vec3 startPointSlope;
		glm::vec3 endPointSlope;

	protected:
		TransformComponent transform;
		bool hasTexture;


		std::vector<Vertex> boundingBox = {};
		std::vector<uint32_t> boundingIndices = {
			1,3,2, 2,0,1,  //Face front
			4,7,6, 6,5,4, //Face right
			10,8,9, 9,11,10, // Back
			14,12,13, 13,15,14, // Left
			16,17,19, 19,18,16, // Bottom
			23,22,20, 20,21,23, // Top
		};
	};

}