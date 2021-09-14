#include "Entity.h"

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

namespace Broccoli
{
	Entity::Entity(Model* model, bool canCollide, bool initialVelocity, bool hasGravity, bool shouldDrawBoundingBox)
	{
		this->model = model;
		this->canCollide = canCollide;
		this->velocity = initialVelocity;
		this->hasGravity = hasGravity;
		this->shouldDrawBoundingBox = shouldDrawBoundingBox;

		setAssetType(AssetType::Model);
	}

	Entity::Entity(Mesh* mesh, bool canCollide, bool initialVelocity, bool hasGravity, bool shouldDrawBoundingBox)
	{
		this->mesh = mesh;
		this->canCollide = canCollide;
		this->velocity = initialVelocity;
		this->hasGravity = hasGravity;
		this->shouldDrawBoundingBox = shouldDrawBoundingBox;

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
		// TODO: maybe object collides with more than 1?
		Mesh* collidingMesh;
		int yDepth = 0;

		// Boundary checking
		for (auto& entity : entityList)
		{
			if (entity == this) continue;
			if (getAssetType() == AssetType::Mesh)
			{
				if (entity->getAssetType() == AssetType::Mesh)
				{
					//if (entity->getMesh()->isInsideBoundingBox(mesh))
					//{
					//	collidingMesh = entity->getMesh();
					//	std::cout << "Object is colliding with mesh\n";
					//	yDepth = collidingMesh->calculateAngleOfInclination(mesh->getTransformComponent().translation); // TODO: Create origin, transform by transform matrix
					//}
				}
				
				else if (entity->getAssetType() == AssetType::Model)
				{
					// Return mesh that is colliding with inside model
					collidingMesh = entity->getModel()->getCollidingMesh(mesh);
					if (collidingMesh != NULL)
					{
						std::cout << "Object is colliding with a mesh of model : height : "<< mesh->height << "\n";
						yDepth = collidingMesh->calculateAngleOfInclination(mesh->origin);
					}
				}	

			}

			/*
			// TODO: Refactor so I don't have to check type of mesh/model
			else if (getAssetType() == AssetType::Model)
			{
				if (entity->getAssetType() == AssetType::Mesh)
				{
					if (entity->getMesh()->isInsideBoundingBox(model))
					{
						collidingMesh = entity->getMesh();
						yDepth = collidingMesh->calculateAngleOfInclination(model->getTransformComponent().translation);
					}
				}
				else if (entity->getAssetType() == AssetType::Model)
				{
					collidingMesh = entity->getModel()->getCollidingMesh(model);
					if (collidingMesh != NULL)
					{
						yDepth = collidingMesh->calculateAngleOfInclination(model->getTransformComponent().translation);
					}
				}
			}*/
		}

		std::cout << "Y depth for component: " << yDepth << "\n";

		if (mesh) {
			mesh->findMaxAndMinHeight();
			// Get the lowest vertex in the mesh (endPointSlope)
			// Make sure that endPointSlope.y > yDepth
			// Transform the mesh to that yDepth until ^^

			glm::vec3 currentTranslation = mesh->getTransformComponent().translation;
			int lowestHeight = glm::vec3(mesh->getTransform() * glm::vec4(mesh->endPointSlope, 1)).y - (mesh->height /2);

			int difference = std::abs(yDepth - currentTranslation.y);
			int lowerBound = yDepth + difference;

			std::cout << lowestHeight <<" " << currentTranslation.y << "\n";

			if (std::abs(lowestHeight - yDepth) <= 0.01)
			{
				
			}
			if (lowestHeight > yDepth)
			{
				for (int i = 0; i < 10; i++)
				{
					mesh->setTranslation(glm::vec3(currentTranslation.x, currentTranslation.y - 0.1, currentTranslation.z));
				}
			}
			else if (lowestHeight < yDepth)
			{
				for (int i = 0; i < 10; i++)
				{
					mesh->setTranslation(glm::vec3(currentTranslation.x, currentTranslation.y + 0.1, currentTranslation.z));
				}

			}
		}
		else {
			// TODO: Enable y testing
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
