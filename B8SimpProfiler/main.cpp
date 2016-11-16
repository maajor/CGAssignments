#include <string>
#include <iostream>
#include <time.h> 

// GLEW
#define GLEW_STATIC
#include <gl/glew.h>
#include <GLFW\glfw3.h>
#include <gl/glut.h>
#include <gl/glu.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//#include "../include/camera.h"
//#include "../include/shader.h"

//#include <SOIL.h>
#include "../include/HalfEdge.h"

void main(){
	std::string readline;

	HalfEdgeModel mymodel("eight.uniform.obj");
	time_t curTime = time(0);
	mymodel.quadricSimplify(0.6);
	time_t curTime2 = time(0);
	std::cout << "time spend " << curTime2 - curTime << std::endl;
	std::cin >> readline;

}