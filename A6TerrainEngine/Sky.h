#pragma once

#include <gl/glew.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include <vector>
#include <SOIL.h>

#include "../include/shader.h"

class Sky
{
public:
	Sky(GLchar* texPrefix);
	~Sky();

	void render();

	Shader skyShader;

private:
	GLuint cubeTex;

	GLuint skyVAO, skyVBO;

	GLuint loadCubemap(std::vector<const GLchar*> faces);
	
};

