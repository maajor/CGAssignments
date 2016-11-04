#pragma once

#include <gl/glew.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include <SOIL.h>

#include "../include/shader.h"

class Terrain
{
public:
	Terrain(Shader shader);
	~Terrain();

	Shader terrainShader;

	void loadHeightmap(const GLchar* pathname, float heightoffset);
	void loadTexture(const GLchar* diffuse, const GLchar* specular, const GLchar* normal, const GLchar* detail);
	void render();

	void renderReflection();
	void renderRefraction();
	void renderAboveWater();

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

	GLuint texDiffuse, texSpec, texNorm, texDeta;
};

