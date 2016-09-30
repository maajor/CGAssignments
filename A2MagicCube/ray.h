#pragma once
#include "../include/camera.h"
#include <glm/glm.hpp>

class Ray
{
public:
	Ray(Camera camera, glm::vec2 hitPos, glm::vec2 screenSize);
	~Ray();

	bool checkHit(float &hitT, glm::vec3 bbx[2]);
private:
	glm::vec3 origin;
	glm::vec3 direction;
	glm::vec3 invdir;
	GLint sign[3];
};