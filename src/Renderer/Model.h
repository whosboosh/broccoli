#ifndef BROCCOLI_MODEL_HEADER
#define BROCCOLI_MODEL_HEADER

#include "Asset/Asset.h"
#include "Renderer/Mesh.h"

#include "Renderer/RenderObject.h"

#include "Renderer/Texture.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <vector>

#include <glm/glm.hpp>

namespace Broccoli {
	class Model : public RenderObject
	{

	public:
		Model(const std::string& fileName, glm::vec3 translate = glm::vec3(0, 0, 0), glm::vec3 scale = glm::vec3(1, 1, 1), glm::vec3 rotate = glm::vec3(0, 0, 0));
		Model(const std::string& fileName, Ref<Texture> texture, glm::vec3 translate = glm::vec3(0, 0, 0), glm::vec3 scale = glm::vec3(1, 1, 1), glm::vec3 rotate = glm::vec3(0, 0, 0));
		~Model();

		void loadModel();
		void loadMaterials();
		void loadNode(const aiNode* node);
		void loadMesh(const aiMesh* mesh, float col);

		size_t getMeshCount() { return meshList.size(); };

		Mesh* getMesh(size_t index) {
			if (index >= meshList.size()) {
				throw std::runtime_error("Attempted to access invalid Mesh index");
			}
			return meshList[index];
		}

		std::vector<Mesh*> getMeshList() { return meshList; }

		virtual void calculateBoundingBox() override {};

		Ref<Texture>& getTexture() { return texture; }

		virtual void findMinMaxVerticies() override {};
		virtual void findMaxAndMinHeight() override {};
		virtual double calculateAngleOfInclination(RenderObject* object) override { return 0; };
		virtual bool isInsideBoundingBox(RenderObject* object) override { return 1; };
		virtual Mesh* getCollidingMesh(RenderObject* object);

	private:
		const std::string fileName;
		const aiScene* scene;

		MeshInfo modelTransform;
		Ref<Texture> texture;

		std::vector<Ref<Texture>> matToTex;

		std::vector<Mesh*> meshList;
		std::vector<std::string> textureList;
	};

}

#endif