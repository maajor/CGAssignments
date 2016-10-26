#pragma once
#include <gl/glew.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include <SOIL.h>

#include "../include/shader.h"

class Water
{
public:
	Water(float size,float height);
	~Water();

	void render(GLuint skyTex);

	Shader waterShader;

	void loadTexture(const GLchar* diffuse, const GLchar* normal, const GLchar* normal2);


private:
	int size;

	//GLfloat* vertices;
	GLuint VAO, VBO;
	GLuint texDiffuse, texNormal, texNormal2;
};

