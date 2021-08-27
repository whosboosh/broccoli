#pragma once

#include "Broccoli/Asset/Asset.h"

#include <vector>

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
		virtual ~RenderObject();

		MeshInfo getTransform() { return transform; }

		void setTransform(glm::mat4 transform) { 
			this->transform.transform = transform;
			this->transform.inverseTransform = glm::transpose(glm::inverse(transform));
		}

	protected:
		MeshInfo transform;
	};

}