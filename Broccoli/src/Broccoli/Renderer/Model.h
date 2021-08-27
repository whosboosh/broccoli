#pragma once

#include "Broccoli/Asset/Asset.h"
#include "Broccoli/Renderer/Mesh.h"

#include "Broccoli/Renderer/Texture.h"

#include "Broccoli/Renderer/RenderObject.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

namespace Broccoli {
	class Model : public RenderObject
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

		Mesh* getMesh(size_t index) {
			if (index >= meshList.size()) {
				throw std::runtime_error("Attempted to access invalid Mesh index");
			}
			return meshList[index];
		}

		Ref<Texture>& getTexture() { return texture; }

	private:
		const std::string fileName;
		const aiScene* scene;

		Ref<Texture> texture;

		std::vector<Ref<Texture>> matToTex;

		std::vector<Mesh*> meshList;
		std::vector<std::string> textureList;
	};

}