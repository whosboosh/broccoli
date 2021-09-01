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

		glm::mat4 inverseTransform = glm::mat4(1.0f);

		TransformComponent() = default;
		TransformComponent(const TransformComponent& other) = default;
		TransformComponent(const glm::vec3& translation) { this->translation = translation; this->inverseTransform = glm::inverse(glm::transpose(getTransform())); }
 
		glm::mat4 getTransform() const
		{
			return glm::translate(glm::mat4(1.0f), translation) * glm::toMat4(glm::quat(rotation)) * glm::scale(glm::mat4(1.0f), scale);
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
			this->transform.translation = translation;
		}
		void setRotation(glm::vec3 rotation)
		{
			this->transform.rotation = rotation;
		}
		void setScale(glm::vec3 scale)
		{
			this->transform.scale = scale;
		}

		glm::mat4 getTransform() { return transform.getTransform(); }

		virtual void calculateBoundingBox() = 0;

	protected:
		TransformComponent transform;

		// TODO: bounding box

	};

}