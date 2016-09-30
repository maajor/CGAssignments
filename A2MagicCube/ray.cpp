#include "ray.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glut.h>
#include <GL/glu.h>

Ray::Ray(Camera camera, glm::vec2 hitPos, glm::vec2 screenSize)
{
	GLdouble x, y, z;
	glm::vec3 destination;
	glm::mediump_f64mat4 view = camera.GetViewMatrix();
	glm::mediump_f64mat4 projection = glm::perspective(camera.Zoom, (GLfloat)screenSize.x / (GLfloat)screenSize.y, 0.1f, 1000.0f);
	//GLdouble modelview[16];
	//GLdouble projection[16];
	//glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	//glGetDoublev(GL_PROJECTION_MATRIX, projection);
	GLint viewport[4] = { 0, 0, screenSize.x, screenSize.y };
	gluUnProject(hitPos.x, hitPos.y, 0, glm::value_ptr(view), glm::value_ptr(projection), viewport, &x, &y, &z);
	origin.x = x;
	origin.y = y;
	origin.z = z;
	gluUnProject(hitPos.x, hitPos.y, 1, glm::value_ptr(view), glm::value_ptr(projection), viewport, &x, &y, &z);
	destination.x = x;
	destination.y = y;
	destination.z = z;
	direction = glm::normalize(destination - origin);

	invdir.x = 1 / direction.x;
	invdir.y = 1 / direction.y;
	invdir.z = 1 / direction.z;
	sign[0] = (invdir.x < 0);
	sign[1] = (invdir.y < 0);
	sign[2] = (invdir.z < 0);
}

Ray::~Ray(){
	return;
}

bool Ray::checkHit(float &hitT, glm::vec3 bbx[2]){
	float tmin, tmax, tymin, tymax, tzmin, tzmax;

	tmin = (bbx[sign[0]].x - origin.x) * invdir.x;
	tmax = (bbx[1 - sign[0]].x - origin.x) * invdir.x;
	tymin = (bbx[sign[1]].y - origin.y) * invdir.y;
	tymax = (bbx[1 - sign[1]].y - origin.y) * invdir.y;

	if ((tmin > tymax) || (tymin > tmax))
		return false;
	if (tymin > tmin)
		tmin = tymin;
	if (tymax < tmax)
		tmax = tymax;

	tzmin = (bbx[sign[2]].z - origin.z) * invdir.z;
	tzmax = (bbx[1 - sign[2]].z - origin.z) * invdir.z;

	if ((tmin > tzmax) || (tzmin > tmax))
		return false;
	if (tzmin > tmin)
		tmin = tzmin;
	if (tzmax < tmax)
		tmax = tzmax;

	hitT = tmin;
	return true;
}
