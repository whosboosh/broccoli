#include "Entity.h"

namespace Broccoli
{
	Entity::Entity(Model* model, bool canCollide, bool initialVelocity, bool hasGravity)
	{
		this->object = renderObject;
		this->canCollide = canCollide;
		this->velocity = initialVelocity;
		this->hasGravity = hasGravity;
	}

	Entity::Entity(Mesh* mesh, bool canCollide, bool initialVelocity, bool hasGravity)
	{
		this->object = renderObject;
		this->canCollide = canCollide;
		this->velocity = initialVelocity;
		this->hasGravity = hasGravity;
	}


	Entity::~Entity()
	{

	}

	void Entity::act()
	{
		if(hasGravity) actGravity();
	}

	void Entity::actGravity()
	{
		// Boundary checking (TODO: Add bounding boxes to entities)
		object->setTransform(object->getTransform().transform);
		
	}
}
