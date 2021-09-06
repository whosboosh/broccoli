#pragma once

#include "Broccoli/Renderer/Model.h"
#include "Broccoli/Core/Ref.h"
#include "Broccoli/Asset/Asset.h"

namespace Broccoli
{
	class Entity : Asset
	{
	public:
		Entity(Mesh* mesh, bool canCollide, bool initialVelocity, bool hasGravity);
		Entity(Model* model, bool canCollide, bool initialVelocity, bool hasGravity);
		~Entity();

		Model* getModel() { return model; }
		Mesh* getMesh() { return mesh; }

		// Carry out per frame actions for this entity
		// E.g. affected by gravity or animation
		void act(std::vector<Entity*> entityList);
		void actGravity(std::vector<Entity*> entityList);

		void moveToPosition(glm::vec3 position, float velocity);
		void moveToEntity(Entity* entity);

		bool checkIntersection(std::vector<Vertex>* object1, std::vector<Vertex>* object2);

	private:
		Model* model;
		Mesh* mesh;

		float movementSpeed = 0.01f;

		bool hasGravity = true;
		float velocity = 0.0f;
		bool canCollide = true;
	};
}