#include "terrain.h"


Terrain::Terrain()
{
	this->gridSize = 0.01f;
	this->heightScale = 0.001f;
}


Terrain::~Terrain()
{
}


void Terrain::loadFromFile(const GLchar* pathname){
	FILE * file;

	fopen_s(&file, pathname, "rb");

	unsigned char info[54];
	fread(info, sizeof(unsigned char), 54, file);

	this->width = *(int*)&info[18];
	this->height = *(int*)&info[22];

	if (file == NULL) return ;
	
	this->verticesSize = width * height;
	this->faceSize = (width - 1) * (height - 1) * 2;
	unsigned char* data = new unsigned char[this->verticesSize]; // allocate 1 bytes per pixel
	fread(data, this->verticesSize, 1, file);
	fclose(file);

	this->vertices = new GLfloat[this->verticesSize * 3];

	for (int i = 0; i < this->width * this->height; ++i)
	{
		int vetHeight = (int)data[i];
		int vetIndex = 3 * i;
		int z = i / this->height;
		int x = i - z * this->height;

		this->vertices[vetIndex] = x * this->gridSize;
		this->vertices[vetIndex + 2] = z * this->gridSize;
		this->vertices[vetIndex + 1] = vetHeight * this->heightScale;

		/*
		To do
		add normal and uv
		*/
	}

	this->indices = new GLuint[this->faceSize * 3];

	for (int i = 0; i < (this->width - 1)* (this->height - 1); ++i)
	{
		int faceInitIndex = 6 * i;
		int z = i / (this->height - 1);
		int x = i - z * (this->height - 1);
		int firstVetIndex = x + this->height * z;

		this->indices[faceInitIndex] = firstVetIndex;
		this->indices[faceInitIndex + 1] = firstVetIndex + height;
		this->indices[faceInitIndex + 2] = firstVetIndex + 1;
		this->indices[faceInitIndex + 3] = firstVetIndex + 1;
		this->indices[faceInitIndex + 4] = firstVetIndex + height;
		this->indices[faceInitIndex + 5] = firstVetIndex + height + 1;

	}

	glGenBuffers(1, &this->VBO);
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->EBO);

	glBindVertexArray(this->VAO);

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, this->verticesSize * 3 * sizeof(GL_FLOAT), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->faceSize * 3 * sizeof(GL_UNSIGNED_INT), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	//To Do. Add normal and uv attribpointer

	glBindVertexArray(0);

}
void Terrain::render(Shader shader){
	glm::mat4 model = glm::mat4();;
	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glBindVertexArray(this->VAO);
	glDrawElements(GL_TRIANGLES, this->faceSize * 3, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}