#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Window.h"

namespace Broccoli {
	class Camera
	{
	public:
		Camera();
		Camera(glm::vec3 startPos, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch, GLfloat startMoveSpeed, GLfloat startTurnSpeed);

		void keyControl(KeyAction* keys, GLfloat deltaTime);
		void mouseControl(GLfloat xChange, GLfloat yChange);
		glm::mat4 calculateViewMatrix();
		glm::vec3* getCameraPosition();
		float* getCameraX() { return &position.x; }
		float* getCameraY() { return &position.y; }
		float* getCameraZ() { return &position.z; }

		//static Camera get() { return Camera(Application::get().getCamera()); }

		void setCameraPosX(float x) { this->position.x = x; }
		void setCameraPosY(float y) { this->position.y = y; }
		void setCameraPosZ(float z) { this->position.z = z; }

		~Camera();
	private:
		glm::vec3 position;
		glm::vec3 front;
		glm::vec3 up;
		glm::vec3 right;
		glm::vec3 worldUp;

		GLfloat yaw;
		GLfloat pitch;

		GLfloat initialMoveSpeed;
		GLfloat moveSpeed;
		GLfloat turnSpeed;

		void update();
	};
}