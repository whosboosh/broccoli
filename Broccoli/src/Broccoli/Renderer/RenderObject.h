#pragma once

#include "Broccoli/Asset/Asset.h"

#include <glm/glm.hpp>

namespace Broccoli {
	struct MeshInfo {
		glm::mat4 transform;
		glm::mat4 inverseTransform;
		bool hasTexture;
	};

	class RenderObject : public Asset
	{

	public:
		virtual ~RenderObject() {};

		void setTransform(glm::mat4 transform)
		{
			this->transform.transform = transform;
			this->transform.inverseTransform = glm::inverse(glm::transpose(transform));
		}
		MeshInfo& getTransform() { return transform; }

		void calculateBoundingBox();

	protected:
		MeshInfo transform;

		// TODO: bounding box

	};

}