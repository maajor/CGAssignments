#pragma once
#include "../include/camera.h"
#include <glm/glm.hpp>
// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

class Ray
{
public:
	Ray(Camera camera, glm::vec2 hitPos, glm::vec2 screenSize)
	{
		GLdouble x, y, z;
		glm::mat4 projection = glm::perspective(camera.Zoom, (GLfloat)screenSize.x / (GLfloat)screenSize.y, 0.1f, 1000.0f);
		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);
		gluUnProject(hitPos.x, hitPos.y, 0, camera.GetViewMatrix, projection, viewport, &x, &y, &z);
		gluUnProject()
	
	}
	~Ray();

	bool checkHit(glm::vec3 bbxMin, glm::vec3 bbxMax, glm::vec3 &hitPos){
	
	}
private:
	glm::vec3 origin;
	glm::vec3 direction;
};