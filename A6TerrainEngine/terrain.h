#pragma once

#include <gl/glew.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include <SOIL.h>

#include "../include/shader.h"

class Terrain
{
public:
	Terrain();
	~Terrain();

	void loadFromFile(const GLchar* pathname);
	void render(Shader shader);
private:
	int width;
	int height;
	
	int verticesSize;
	int faceSize;

	float gridSize;
	float heightScale;

	GLuint VAO, VBO, EBO;

	GLfloat* vertices;
	GLuint* indices;
};

