#pragma once

#include <chrono>
#include "glm\glm.hpp"

using namespace std::chrono;

class Object
{
	milliseconds startTime;
	glm::vec3 initialVelocity;
	glm::vec3 acceleration;
	glm::vec3 resistance;

public:
	glm::vec3 position;
	glm::vec3 velocity;
	bool isStatic = false;
	Object(glm::vec3 _position, glm::vec3 _velocity, glm::vec3 _acceleration, glm::vec3 _resistance);
	void Update();
};