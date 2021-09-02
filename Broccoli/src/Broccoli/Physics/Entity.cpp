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
		// TODO: Fix transform
		//if (mesh) mesh->setTransform(glm::translate(mesh->getTransform(), glm::vec3(0.0f, -0.01f, 0.0f)));
		//else model->setTransform(glm::translate(model->getTransform(), glm::vec3(0.0f, -0.01f, 0.0f)));
		
	}
}
