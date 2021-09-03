#include "Entity.h"

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Broccoli
{
	Entity::Entity(Model* model, bool canCollide, bool initialVelocity, bool hasGravity)
	{
		this->model = model;
		this->canCollide = canCollide;
		this->velocity = initialVelocity;
		this->hasGravity = hasGravity;
	}

	Entity::Entity(Mesh* mesh, bool canCollide, bool initialVelocity, bool hasGravity)
	{
		this->mesh = mesh;
		this->canCollide = canCollide;
		this->velocity = initialVelocity;
		this->hasGravity = hasGravity;
	}

	Entity::~Entity()
	{
		delete &mesh;
		delete &model;
	}

	void Entity::act()
	{
		if(hasGravity) actGravity();
	}

	void Entity::actGravity()
	{
		// Boundary checking (TODO: Add bounding boxes to entities)
		if (mesh) {
			glm::vec3 currentTranslation = mesh->getTransformComponent().translation;
			mesh->setTranslation(glm::vec3(currentTranslation.x, currentTranslation.y - 0.01, currentTranslation.z));
		}
		else {
			glm::vec3 currentTranslation = model->getTransformComponent().translation;
			model->setTranslation(glm::vec3(currentTranslation.x, currentTranslation.y - 0.01, currentTranslation.z));
		}
	}
}
