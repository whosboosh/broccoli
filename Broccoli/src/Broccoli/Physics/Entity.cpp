#include "Entity.h"

namespace Broccoli
{
	Entity::Entity(Ref<Model> model, bool canCollide, bool initialVelocity, bool hasGravity)
	{
		this->model = model;
		this->canCollide = canCollide;
		this->velocity = initialVelocity;
		this->hasGravity = hasGravity;
	}

	Entity::Entity(Ref<Mesh> mesh, bool canCollide, bool initialVelocity, bool hasGravity)
	{
		this->mesh = mesh;
		this->canCollide = canCollide;
		this->velocity = initialVelocity;
		this->hasGravity = hasGravity;
	}

	Entity::~Entity()
	{
	}
}