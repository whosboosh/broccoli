#pragma once

#include "Broccoli/Renderer/Model.h"
#include "Broccoli/Core/Ref.h"

namespace Broccoli
{
	class Entity
	{
	public:
		Entity(Mesh* mesh, bool canCollide, bool initialVelocity, bool hasGravity);
		Entity(Model* model, bool canCollide, bool initialVelocity, bool hasGravity);
		~Entity();

		Model* getModel() { return model; }
		Mesh* getMesh() { return mesh; }

		// Carry out per frame actions for this entity
		// E.g. affected by gravity or animation
		void act();
		void actGravity();

	private:
		Model* model;
		Mesh* mesh;

		bool hasGravity = true;
		float velocity = 0.0f;
		bool canCollide = true;
	};
}