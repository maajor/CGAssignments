#include "Water.h"


Water::Water(float size, float height)
{
	this->waterShader = Shader("shader/waterVertexShader.glsl", "shader/waterFragmentShader.glsl");

	GLfloat verts[] = {
		size, height, size, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		-size, height, size, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		-size, height, -size, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,

		-size, height, -size, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		size, height, -size, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		size, height, size, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
	};

	GLuint inds[] = {
		0, 1, 2,
		2, 3, 0
	};

	//verts[0] = size;
	//verts[2] = size;
	//verts[10] = size;
	//verts[24] = size;

	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBO);
	glBindVertexArray(this->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, 6 * 8 * sizeof(GLfloat), &verts, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);


}


Water::~Water()
{
}

void Water::loadTexture(const GLchar* diffuse, const GLchar* normal, const GLchar* normal2, const GLchar* flow, const GLchar* noise){
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

		//glActiveTexture(texDiffuse);
		//glBindTexture(GL_TEXTURE_2D, texDiffuse);
		//glUniform1i(glGetUniformLocation(this->waterShader.Program, "texture_diffuse"), 0);

	}
	if (normal != NULL){
		int wid, hei;
		unsigned char* image = SOIL_load_image(normal, &wid, &hei, 0, SOIL_LOAD_RGB);

		glGenTextures(1, &texNormal);
		glBindTexture(GL_TEXTURE_2D, texNormal);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// Set texture filtering
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, wid, hei, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);
		SOIL_free_image_data(image);
		glBindTexture(GL_TEXTURE_2D, 0);

		//glActiveTexture(texNormal);
		//glBindTexture(GL_TEXTURE_2D, texNormal);
		//glUniform1i(glGetUniformLocation(this->waterShader.Program, "texture_normal"), 1);
	}
	if (normal2 != NULL){
		int wid, hei;
		unsigned char* image = SOIL_load_image(normal2, &wid, &hei, 0, SOIL_LOAD_RGB);

		glGenTextures(1, &texNormal2);
		glBindTexture(GL_TEXTURE_2D, texNormal2);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// Set texture filtering
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, wid, hei, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);
		SOIL_free_image_data(image);
		glBindTexture(GL_TEXTURE_2D, 0);

		//glActiveTexture(texNormal2);
		//glBindTexture(GL_TEXTURE_2D, texNormal2);
		//glUniform1i(glGetUniformLocation(this->waterShader.Program, "texture_normal2"), 2);
	}
	if (flow != NULL){
		int wid, hei;
		unsigned char* image = SOIL_load_image(flow, &wid, &hei, 0, SOIL_LOAD_RGB);

		glGenTextures(1, &texFlow);
		glBindTexture(GL_TEXTURE_2D, texFlow);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// Set texture filtering
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, wid, hei, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);
		SOIL_free_image_data(image);
		glBindTexture(GL_TEXTURE_2D, 0);

		//glActiveTexture(texFlow);
		//glBindTexture(GL_TEXTURE_2D, texFlow);
		//glUniform1i(glGetUniformLocation(this->waterShader.Program, "texture_flowmap"), 3);
	}
	if (noise != NULL){
		int wid, hei;
		unsigned char* image = SOIL_load_image(noise, &wid, &hei, 0, SOIL_LOAD_RGB);

		glGenTextures(1, &texNoise);
		glBindTexture(GL_TEXTURE_2D, texNoise);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// Set texture filtering
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, wid, hei, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);
		SOIL_free_image_data(image);
		glBindTexture(GL_TEXTURE_2D, 0);

		//glActiveTexture(texFlow);
		//glBindTexture(GL_TEXTURE_2D, texFlow);
		//glUniform1i(glGetUniformLocation(this->waterShader.Program, "texture_flowmap"), 3);
	}

	std::cout << texDiffuse << " " << texNormal << " " << texNormal2 << " " << texFlow << std::endl;
}

void Water::render(GLuint skyTex){
	glUniform1i(glGetUniformLocation(this->waterShader.Program, "tilingx"), 20);
	glUniform1i(glGetUniformLocation(this->waterShader.Program, "tilingy"), 20);
	//glUniform1f(glGetUniformLocation(this->waterShader.Program, "_time"), Time);

	GLint64 timer;
	glGetInteger64v(GL_TIMESTAMP, &timer);
	glUniform1f(glGetUniformLocation(this->waterShader.Program, "_time"), (float) timer / 1000000000);
	//std::cout << "time:" << (float) timer / 1000000000 << std::endl;

	glm::mat4 model = glm::mat4();
	glUniformMatrix4fv(glGetUniformLocation(this->waterShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));

	
	glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, texDiffuse);
	glUniform1i(glGetUniformLocation(this->waterShader.Program, "texture_diffuse"), 0);
	glBindTexture(GL_TEXTURE_2D, texDiffuse);
	//(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D, texNormal);
	glUniform1i(glGetUniformLocation(this->waterShader.Program, "texture_normal"), 1);
	glBindTexture(GL_TEXTURE_2D, texNormal);//
	//glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE2);
	//glBindTexture(GL_TEXTURE_2D, texNormal2);
	glUniform1i(glGetUniformLocation(this->waterShader.Program, "texture_normal2"), 2);
	glBindTexture(GL_TEXTURE_2D, texNormal2);
	//glBindTexture(GL_TEXTURE_2D, 0);
	
	glActiveTexture(GL_TEXTURE3);
	//glBindTexture(GL_TEXTURE_2D, texFlow);
	glUniform1i(glGetUniformLocation(this->waterShader.Program, "texture_flowmap"), 3);
	glBindTexture(GL_TEXTURE_2D, texFlow);
	//glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE4);
	//glBindTexture(GL_TEXTURE_2D, texFlow);
	glUniform1i(glGetUniformLocation(this->waterShader.Program, "texture_noise"), 3);
	glBindTexture(GL_TEXTURE_2D, texNoise);
	
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(this->waterShader.Program, "skybox"), 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyTex);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBindVertexArray(this->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, 0);
}