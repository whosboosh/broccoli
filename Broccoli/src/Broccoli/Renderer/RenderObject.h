#pragma once

#include "Broccoli/Asset/Asset.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

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

	protected:
		TransformComponent transform;
		bool hasTexture;

		// TODO: bounding box

	};

}