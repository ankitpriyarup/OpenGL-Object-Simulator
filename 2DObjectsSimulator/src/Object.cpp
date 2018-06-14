#include "Object.h"
#include <iostream>

Object::Object(glm::vec3 _position, glm::vec3 _velocity, glm::vec3 _acceleration, glm::vec3 _resistance) :
	startTime(duration_cast< milliseconds >(system_clock::now().time_since_epoch())),
	initialVelocity(_velocity), acceleration(_acceleration), resistance(_resistance), position(_position)
{ }

void Object::Update()
{
	milliseconds curTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
	const long double t = (curTime.count() - startTime.count()) / (double)1000;
	const long double tSquare = t * t;

	glm::vec3 resultantAcceleration = acceleration - resistance;
	velocity = glm::vec3(initialVelocity.x + (resultantAcceleration.x * t),
		initialVelocity.y + (resultantAcceleration.y * t),
		initialVelocity.z + (resultantAcceleration.z * t));

	glm::vec3 vectorA = glm::vec3(initialVelocity.x * t, initialVelocity.y * t, initialVelocity.z * t);
	glm::vec3 vectorB = glm::vec3(0.5f * resultantAcceleration.x * tSquare,
		0.5f * resultantAcceleration.y * tSquare,
		0.5f * resultantAcceleration.z * tSquare);

	if (velocity.x > 0)
		position += vectorA + vectorB;
	else
		isStatic = true;
}
