#include "terrain.h"


Terrain::Terrain(Shader shader)
{
	this->gridSize = 0.01f;
	this->heightScale = 0.001f;
	this->terrainShader = shader;
}


Terrain::~Terrain()
{
	delete[] vertices;
	vertices = NULL;

	delete[] indices;
	indices = NULL;
}


void Terrain::loadHeightmap(const GLchar* pathname){
	FILE * file;

	fopen_s(&file, pathname, "rb");

	unsigned char info[54];
	fread(info, sizeof(unsigned char), 54, file);

	this->width = *(int*)&info[18];
	this->height = *(int*)&info[22];

	if (file == NULL) return ;
	
	this->verticesSize = this->width * this->height;
	this->faceSize = (width - 1) * (height - 1) * 2;
	unsigned char* data = new unsigned char[this->verticesSize]; // allocate 1 bytes per pixel
	fread(data, this->verticesSize, 1, file);
	fclose(file);

	float dx, dz;
	dx = 1.0f / (float)this->width;
	dz = 1.0f / (float)this->height;

	this->vertices = new GLfloat[this->verticesSize * 8];

	for (int i = 0; i < this->width * this->height; ++i)
	{
		int vetHeight = (int)data[i];
		int vetIndex = 8 * i;
		int x = i / this->height;
		int z = i - x * this->height;

		if (i > 255*255){
			//std::cout << i << " height: " << vetHeight << std::endl;
		}

		this->vertices[vetIndex] = x * this->gridSize;
		this->vertices[vetIndex + 2] = z * this->gridSize;
		this->vertices[vetIndex + 1] = vetHeight * this->heightScale;

		this->vertices[vetIndex + 6] = z * dz;
		this->vertices[vetIndex + 7] = 1 - x * dx;

		if (z == 0 || z == width - 1 || x == 0 || x == height - 1){
			this->vertices[vetIndex + 3] = 0;
			this->vertices[vetIndex + 4] = 0;
			this->vertices[vetIndex + 5] = 1;
		}
		else{
			float heightL = (int)data[i - height];
			float heightT = (int)data[i + 1];
			float heightB = (int)data[i - 1];
			float heightR = (int)data[i + height];
			glm::vec3 vecL = glm::vec3(0, (heightL - vetHeight) * this->heightScale, -this->gridSize);
			glm::vec3 vecR = glm::vec3(0, (heightR - vetHeight) * this->heightScale, this->gridSize);
			glm::vec3 vecT = glm::vec3(this->gridSize, (heightT - vetHeight) * this->heightScale, 0);
			glm::vec3 vecB = glm::vec3(-this->gridSize, (heightB - vetHeight) * this->heightScale, 0);

			glm::vec3 normalTL = glm::cross(vecT, vecL);
			glm::vec3 normalLB = glm::cross(vecL, vecB);
			glm::vec3 normalBR = glm::cross(vecB, vecR);
			glm::vec3 normalRT = glm::cross(vecR, vecT);

			glm::vec3 normalAver = glm::normalize(normalTL + normalLB + normalBR + normalRT);
			this->vertices[vetIndex + 3] = normalAver.x;
			this->vertices[vetIndex + 4] = normalAver.z;
			this->vertices[vetIndex + 5] = normalAver.y;
		}

		//std::cout << "normal: " << x << "," << z << ":" << this->vertices[vetIndex + 3] << "," << this->vertices[vetIndex + 4] << "," << this->vertices[vetIndex + 5] << std::endl;
	}

	this->indices = new GLuint[this->faceSize * 3];

	for (int i = 0; i < (this->width - 1)* (this->height - 1); ++i)
	{
		int faceInitIndex = 6 * i;
		int x = i / (this->height - 1);
		int z = i - x * (this->height - 1);
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
	glBufferData(GL_ARRAY_BUFFER, this->verticesSize * 8 * sizeof(GL_FLOAT), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->faceSize * 3 * sizeof(GL_UNSIGNED_INT), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);

	delete [] data;
	data = NULL;

}

void Terrain::loadTexture(const GLchar* diffuse, const GLchar* specular, const GLchar* normal, const GLchar* detail){
	if (diffuse != NULL){
		int wid, hei;
		unsigned char* image = SOIL_load_image(diffuse, &wid, &hei, 0, SOIL_LOAD_RGB);

		glGenTextures(1, &texDiffuse);
		glBindTexture(GL_TEXTURE_2D, texDiffuse);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// Set texture filtering
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, wid, hei, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);
		SOIL_free_image_data(image);
		glBindTexture(GL_TEXTURE_2D, 0);

		glActiveTexture(texDiffuse);
		glBindTexture(GL_TEXTURE_2D, texDiffuse);
		glUniform1i(glGetUniformLocation(terrainShader.Program, "texture_diffuse1"), 0);

	}
	if (specular != NULL){
		int wid, hei;
		unsigned char* image = SOIL_load_image(specular, &wid, &hei, 0, SOIL_LOAD_RGB);

		glGenTextures(1, &texSpec);
		glBindTexture(GL_TEXTURE_2D, texSpec);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// Set texture filtering
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, wid, hei, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);
		SOIL_free_image_data(image);
		glBindTexture(GL_TEXTURE_2D, 0);

		glActiveTexture(texSpec);
		glBindTexture(GL_TEXTURE_2D, texSpec);
		glUniform1i(glGetUniformLocation(terrainShader.Program, "texture_specular1"), 0);
	}
	if (normal != NULL){
		int wid, hei;
		unsigned char* image = SOIL_load_image(normal, &wid, &hei, 0, SOIL_LOAD_RGB);

		glGenTextures(1, &texNorm);
		glBindTexture(GL_TEXTURE_2D, texNorm);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// Set texture filtering
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, wid, hei, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);
		SOIL_free_image_data(image);
		glBindTexture(GL_TEXTURE_2D, 0);

		glActiveTexture(texNorm);
		glBindTexture(GL_TEXTURE_2D, texNorm);
		glUniform1i(glGetUniformLocation(terrainShader.Program, "texture_normal1"), 0);
	}
	if (detail != NULL){
		int wid, hei;
		unsigned char* image = SOIL_load_image(detail, &wid, &hei, 0, SOIL_LOAD_RGB);

		glGenTextures(1, &texDeta);
		glBindTexture(GL_TEXTURE_2D, texDeta);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// Set texture filtering
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, wid, hei, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);
		SOIL_free_image_data(image);
		glBindTexture(GL_TEXTURE_2D, 0);

		glActiveTexture(texDeta);
		glBindTexture(GL_TEXTURE_2D, texDeta);
		glUniform1i(glGetUniformLocation(terrainShader.Program, "texture_detail1"), 0);
	}
	
}

void Terrain::render(){
	glm::mat4 model = glm::mat4();;
	glUniformMatrix4fv(glGetUniformLocation(terrainShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));

	glActiveTexture(texDiffuse);
	glBindTexture(GL_TEXTURE_2D, texDiffuse);
	glUniform1i(glGetUniformLocation(terrainShader.Program, "texture_diffuse1"), 0);

	glBindVertexArray(this->VAO);
	glDrawElements(GL_TRIANGLES, this->faceSize * 3, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
