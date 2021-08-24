#pragma once

#include "Broccoli/Asset/Asset.h"
#include "Broccoli/Renderer/Mesh.h"

#include "Broccoli/Renderer/Texture.h"

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
		Model(const std::string& fileName, glm::mat4 transform, Ref<Texture> texture);
		~Model();

		void loadModel();
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

		Ref<Texture>& getTexture() { return texture; }

	private:
		const std::string fileName;
		const aiScene* scene;

		MeshInfo modelTransform;
		Ref<Texture> texture;

		std::vector<Ref<Texture>> matToTex;

		std::vector<Ref<Mesh>> meshList;
		std::vector<std::string> textureList;
	};

}