#ifndef BROCCOLI_ENTITY_HEADER
#define BROCCOLI_ENTITY_HEADER

#include "Renderer/Model.h"
#include "Core/Ref.h"
#include "Asset/Asset.h"

namespace Broccoli
{
	class Entity : Asset
	{
	public:
		Entity(Mesh* mesh, bool canCollide, bool initialVelocity, bool hasGravity, bool shouldDrawBoundingBox);
		~Entity();

		Mesh* getMesh() { return mesh; }

		// Carry out per frame actions for this entity
		// E.g. affected by gravity or animation
		void act(std::vector<Entity*> entityList);
		void actGravity(std::vector<Entity*> entityList);

		void moveToPosition(glm::vec3 position, float velocity);
		void moveToEntity(Entity* entity);

		bool getShouldDrawBoundingBox() { return shouldDrawBoundingBox; }

	private:
		Mesh* mesh;

		float movementSpeed = 0.001f;

		bool hasGravity = true;
		float velocity = 0.0f;
		bool canCollide = true;
		bool shouldDrawBoundingBox = false;
	};
}

#endif