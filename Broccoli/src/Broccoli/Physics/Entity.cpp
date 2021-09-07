#include "Entity.h"

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

namespace Broccoli
{
	Entity::Entity(Model* model, bool canCollide, bool initialVelocity, bool hasGravity)
	{
		this->model = model;
		this->canCollide = canCollide;
		this->velocity = initialVelocity;
		this->hasGravity = hasGravity;

		setAssetType(AssetType::Model);
	}

	Entity::Entity(Mesh* mesh, bool canCollide, bool initialVelocity, bool hasGravity)
	{
		this->mesh = mesh;
		this->canCollide = canCollide;
		this->velocity = initialVelocity;
		this->hasGravity = hasGravity;

		setAssetType(AssetType::Mesh);
	}

	Entity::~Entity()
	{
		delete &mesh;
		delete &model;
	}

	void Entity::act(std::vector<Entity*> entityList)
	{
		if(hasGravity) actGravity(entityList);
	}

	void Entity::actGravity(std::vector<Entity*> entityList)
	{
		/*
		// Boundary checking (TODO: Add bounding boxes to entities)
		for (auto& entity : entityList)
		{
			if (entity->getAssetType() == AssetType::Mesh)
			{
				// True if the mesh is intersecting
				if (checkIntersection(mesh->getBoundingBox(), entity->getMesh()->getVertexBuffer()->As<VertexBuffer>()->getVertices()))
				{

				}
			}
			else if (entity->getAssetType() == AssetType::Model)
			{
				// Check each invidival mesh inside the model to see if colliding
				for (auto& modelMesh: entity->getModel()->getMeshList())
				{
					if (checkIntersection(mesh->getBoundingBox(), modelMesh->getVertexBuffer()->As<VertexBuffer>()->getVertices()));
					{

					}
				}
			}
		}*/

		if (mesh) {
			glm::vec3 currentTranslation = mesh->getTransformComponent().translation;
			mesh->setTranslation(glm::vec3(currentTranslation.x, currentTranslation.y - 0.01, currentTranslation.z));
		}
		else {
			glm::vec3 currentTranslation = model->getTransformComponent().translation;
			model->setTranslation(glm::vec3(currentTranslation.x, currentTranslation.y - 0.01, currentTranslation.z));
		}
	}

	void Entity::moveToPosition(glm::vec3 position, float velocity)
	{
		// Update transformation matrix until origin of entity is at position
		if (mesh)
		{
			//float x = (mesh->getOrigin().x * mesh->getTransform()[3][0]);
			//float y = (mesh->getOrigin().y * mesh->getTransform()[3][1]);
			//float z = (mesh->getOrigin().z * mesh->getTransform()[3][2]);

			//glm::vec3 currentPosition = glm::vec3(x,y,z);

			//std::cout << glm::to_string(currentPosition) << "\n";
			//std::cout << " Mesh transform: " << glm::to_string(mesh->getTransform()) << "\n";
			//std::cout << " Mesh Translation " << glm::to_string(currentTranslation) << "\n";
			//std::cout << " new position: " << glm::to_string(position) << "\n";

			for (int i = 0; i < velocity; i++)
			{
				glm::vec3 currentTranslation = mesh->getTransformComponent().translation;

				float xDifference = std::abs(position.x - currentTranslation.x);
				float yDifference = std::abs(position.y - currentTranslation.y);
				float zDifference = std::abs(position.z - currentTranslation.z);

				if (xDifference > movementSpeed || yDifference > movementSpeed || zDifference > movementSpeed)
				{
					float updatePosX;
					float updatePosY;
					float updatePosZ;

					if (currentTranslation.x > position.x)  updatePosX = currentTranslation.x - movementSpeed;
					else updatePosX = currentTranslation.x + movementSpeed;

					if (currentTranslation.y > position.y)  updatePosY = currentTranslation.y - movementSpeed;
					else updatePosY = currentTranslation.y + movementSpeed;

					if (currentTranslation.z > position.z)  updatePosZ = currentTranslation.z - movementSpeed;
					else updatePosZ = currentTranslation.z + movementSpeed;

					mesh->setTranslation(glm::vec3(updatePosX, updatePosY, updatePosZ));
				}
			}

		}
	}

	void Entity::moveToEntity(Entity* entity)
	{
		moveToPosition(entity->getMesh()->getTransformComponent().translation, 10);
	}

	bool Entity::checkIntersection(std::vector<Vertex>* object1, std::vector<Vertex>* object2)
	{
		// TODO: Create a gravity intersection function that only checks with the map
		// 

		// Apply transformation matrix on each vector in bounding box to get real 3D space coordinate
		// arctan of height / distance = angle between 2 points
		// tan(angle) * x coordinate of arbritrary point to find the height at that slope

		// Find the closest vertice in homogenous coordinate space
		// We only care about the bottom vertices 16/19 are the points in a square that relate to the bottom 4 vertices
		for (int i = 16; i < 20; i++)
		{
			//glm::vec3 transformedPoint = glm::vec3(object->getTransform() * glm::vec4(boundingBox->at(i).pos, 0.0));
			
			// Find closest vertex to transformedPoint
			//for (int j = 0; j < vertexBuffer)

		}

		return 0;
	}
}
