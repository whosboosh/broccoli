#pragma once

#include "Broccoli/Renderer/Mesh.h"
#include "Broccoli/Renderer/Model.h"
#include "Broccoli/Core/Ref.h"

namespace Broccoli
{
	class Entity : Asset
	{
	public:
		Entity(Mesh* object, bool canCollide, bool initialVelocity, bool hasGravity);
		Entity(Model* object, bool canCollide, bool initialVelocity, bool hasGravity);
		~Entity();

		// Carry out per frame actions for this entity, E.g. affected by gravity or animation
		void act();
		void actGravity();

		RenderObject* getRenderObject() { return object; }

	private:
		MeshInfo transform;

		RenderObject* object;

		bool hasGravity = true;
		float velocity = 0.0f;
		bool canCollide = true;
	};
}