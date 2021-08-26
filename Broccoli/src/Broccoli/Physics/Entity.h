#pragma once

#include "Broccoli/Renderer/Model.h"
#include "Broccoli/Core/Ref.h"

namespace Broccoli
{
	class Entity
	{
	public:
		Entity(Ref<Mesh> mesh, bool canCollide, bool initialVelocity, bool hasGravity);
		Entity(Ref<Model> model, bool canCollide, bool initialVelocity, bool hasGravity);
		~Entity();

		Ref<Model> getModel() { return model; }
		Ref<Model> getMesh() { return mesh; }

	private:
		Ref<Model> model;
		Ref<Mesh> mesh;

		bool hasGravity = true;
		float velocity = 0.0f;
		bool canCollide = true;
	};
}