#include "Model.h"

#include "Broccoli/Renderer/Texture.h"

namespace Broccoli {
	Model::Model(const std::string& fileName, glm::mat4 transform)
	{
		setTransform(transform);

		Assimp::Importer importer;
		scene = importer.ReadFile(fileName, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices);
		if (!scene) {
			throw std::runtime_error("Failed to load model (" + fileName + ")");
		}

		// Generate an array of string names for each required material
		loadMaterials();

		// Mapping from material string values to descriptor ids
		std::vector<int> matToTex(textureList.size());

		for (size_t i = 0; i < textureList.size(); i++) {
			if (textureList[i].empty()) {
				matToTex[i] = 0; // If there are no mateirals in the model, use the default texture
			}
			else {
				// Otherwise create texture from material name and set value to index of new texture inside sampler
				//matToTex[i] = Texture::create(textureList[i]); // TODO: Create texture importer
			}
		}

		loadNode(scene->mRootNode);
	}

	void Model::loadNode(const aiNode* node)
	{
		for (size_t i = 0; i < node->mNumMeshes; i++)
		{
			loadMesh(scene->mMeshes[node->mMeshes[i]]);
		}
		for (size_t i = 0; i < node->mNumChildren; i++)
		{
			loadNode(node->mChildren[i]);
		}
	}

	void Model::loadMesh(const aiMesh* mesh)
	{
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

		vertices.resize(mesh->mNumVertices);

		// Add verts
		for (size_t i = 0; i < mesh->mNumVertices; i++)
		{
			vertices[i].pos = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
			vertices[i].col = { 1.0f, 1.0f, 1.0f };
			vertices[i].normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };

			if (mesh->mTextureCoords[0]) {
				vertices[i].tex = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
			}
			else {
				vertices[i].tex = { 0.0f, 0.0f };
			}
		}

		// Add indices
		for (size_t i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (size_t j = 0; j < face.mNumIndices; j++)
			{
				indices.push_back(face.mIndices[j]);
			}
		}


		Ref<Mesh> newMesh = Ref<Mesh>::create(&vertices, &indices, modelTransform.transform);
		meshList.push_back(newMesh);
	}

	void Model::loadMaterials()
	{
		// Create 1:1 sized list of textures
		textureList.resize(scene->mNumMaterials);

		// Go through each material and copy its texture file name (if it exists)
		for (size_t i = 0; i < scene->mNumMaterials; i++) {
			aiMaterial* material = scene->mMaterials[i]; // Get material

			// Initalise the texture to empty string (will be replaced if texture exists)
			textureList[i] = "";
			// Check for a diffuse texture (standard detail texture)
			if (material->GetTextureCount(aiTextureType_DIFFUSE)) {
				// Get the path of the texture file
				aiString path;
				if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS) {
					// Cut off any directory information already present
					int index = std::string(path.data).rfind("\\"); // Find the position of the last backslash in the string (e.g. C:\users\Ben\Documents\thing.obj
					std::string fileName = std::string(path.data).substr(index + 1); // Get the "thing.obj" (+1 from the backslash)

					textureList[i] = fileName;
				}
			}
		}
	}


	Model::~Model()
	{
	}

}