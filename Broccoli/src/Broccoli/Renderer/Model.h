#pragma once

#include "Broccoli/Asset/Asset.h"
#include "Broccoli/Renderer/Mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <vector>

#include <glm/glm.hpp>

namespace Broccoli {
	class Model : public Asset
	{

	public:
		Model(const std::string& fileName, glm::mat4 transform);
		~Model();

		void loadMaterials();
		void loadNode(const aiNode* node);
		void loadMesh(const aiMesh* mesh);

		size_t getMeshCount() { return meshList.size(); };

		Ref<Mesh>* getMesh(size_t index) {
			if (index >= meshList.size()) {
				throw std::runtime_error("Attempted to access invalid Mesh index");
			}
			return &meshList[index];
		}

		void setTransform(glm::mat4 transform) { 
			modelTransform.transform = transform; 
			modelTransform.inverseTransform = glm::transpose(glm::inverse(transform));
		}
		MeshInfo& getTransform() { return modelTransform; }

	private:
		const aiScene* scene;

		MeshInfo modelTransform;

		std::vector<Ref<Mesh>> meshList;
		std::vector<std::string> textureList;
	};

}