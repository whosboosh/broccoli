#include "Model.h"

namespace Broccoli {
	Model::Model(const std::string& fileName, glm::mat4 transform) : fileName(fileName)
	{

		loadModel();

		setTransform(transform);
		setAssetType(AssetType::Model);
	}

	Model::Model(const std::string& fileName, glm::mat4 transform, Ref<Texture> texture) : fileName(fileName)
	{
		this->texture = texture;

		loadModel();

		setTransform(transform);
		setAssetType(AssetType::Model);
	}

	void Model::loadModel()
	{
		Assimp::Importer importer;
		scene = importer.ReadFile(fileName, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices);
		if (!scene) {
			throw std::runtime_error("Failed to load model (" + fileName + ")");
		}

		// Loads textureList with an array of string names for each required material
		loadMaterials();

		// Mapping from material string values to descriptor ids
		matToTex.resize(textureList.size());

		// Remove final contents after last / in fileName for 
		std::string fileDirectory = fileName.substr(0, fileName.find_last_of("/"));
		//std::cout << fileDirectory << "\n";

		for (size_t i = 0; i < textureList.size(); i++) {
			if (texture != NULL)
			{
				matToTex[i] = texture; // Use provided texture
			}
			if (textureList[i].empty()) {
				matToTex[i] = 0; // If there are no mateirals in the model, use the first texture ever loaded in
			}
			else {
				// Otherwise create texture from material name and set value to index of new texture inside sampler
				//std::cout << "Creating texture with fileName: " << fileDirectory + "/textures/" + textureList[i].substr(0, textureList[i].find_last_of(".")) + ".png\n";
				std::string filePath = fileDirectory + "/textures/" + textureList[i].substr(0, textureList[i].find_last_of(".")) + ".png";

				for (Ref<Texture> tex : matToTex)
				{
					if (tex != NULL)
					{
						//std::cout << tex->getFilePath() << " : ";
						///std::cout << filePath << "\n";
						if (tex->getFilePath() == filePath) 
						{
							matToTex[i] = tex;
							break;
						}
					}
				}
				
				if (matToTex[i] == nullptr) {
					matToTex[i] = Texture::create(fileDirectory + "/textures/" + textureList[i].substr(0, textureList[i].find_last_of(".")) + ".png", "geometry.frag", "textureSampler");					
				}
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

		//std::cout <<"Creating mesh with material index: "<< mesh->mMaterialIndex << " " << matToTex[mesh->mMaterialIndex]->getTextureId() << "\n";

		Mesh* newMesh = new Mesh(&vertices, &indices, transform.transform, matToTex[mesh->mMaterialIndex]); // TODO: cleanup meshes when model is destroyed

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
		for (auto& mesh : meshList)
		{
			delete mesh;
		}
	}

}