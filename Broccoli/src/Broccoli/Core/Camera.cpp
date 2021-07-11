#include "Camera.h"
#include <iostream>

namespace Broccoli {
	Camera::Camera()
	{
	}

	Camera::Camera(glm::vec3 startPos, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch, GLfloat startMoveSpeed, GLfloat startTurnSpeed)
	{
		position = startPos;
		worldUp = startUp;
		yaw = startYaw;
		pitch = startPitch;
		front = glm::vec3(0.0f, 0.0f, -1.0f);

		initialMoveSpeed = startMoveSpeed;
		moveSpeed = startMoveSpeed;
		turnSpeed = startTurnSpeed;

		update();
	}

	void Camera::keyControl(KeyAction* keys, GLfloat deltaTime, Window* window)
	{
		//std::cout << position.x << " " << position.y << " " << position.z << "\n";

		GLfloat velocity = moveSpeed * deltaTime;

		if (keys[GLFW_KEY_W].key)
		{
			position += front * velocity;
		}

		if (keys[GLFW_KEY_S].key)
		{
			position -= front * velocity;
		}

		if (keys[GLFW_KEY_A].key)
		{
			position -= right * velocity;
		}

		if (keys[GLFW_KEY_D].key)
		{
			position += right * velocity;
		}

		if (keys[GLFW_KEY_R].key)
		{
			// TODO: Switch renderer on hotkey
			//window->enableOpenGL();
		}

		if (keys[GLFW_KEY_LEFT_SHIFT].key && keys[GLFW_KEY_LEFT_SHIFT].action == GLFW_PRESS)
		{
			moveSpeed = initialMoveSpeed + 25;
		}
		else if (!keys[GLFW_KEY_LEFT_SHIFT].key && keys[GLFW_KEY_LEFT_SHIFT].action == GLFW_RELEASE) {
			moveSpeed = initialMoveSpeed;
		}
	}

	void Camera::mouseControl(GLfloat xChange, GLfloat yChange)
	{
		xChange *= turnSpeed;
		yChange *= turnSpeed;

		yaw += xChange;
		pitch += yChange;
		if (pitch > 89.0f) pitch = 89.0f;
		if (pitch < -89.0f) pitch = -89.0f;

		update();
	}

	glm::mat4 Camera::calculateViewMatrix()
	{
		return glm::lookAt(position, position + front, up);
	}

	glm::vec3* Camera::getCameraPosition()
	{
		return &position;
	}

	void Camera::update()
	{
		glm::vec3 direction(1.0f);
		direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		direction.y = sin(glm::radians(pitch));
		direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		front = glm::normalize(direction);

		right = glm::normalize(glm::cross(front, worldUp));
		up = glm::normalize(glm::cross(right, front));
	}

	Camera::~Camera()
	{
	}
}