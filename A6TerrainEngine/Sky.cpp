#include "Sky.h"
#include <sstream>


Sky::Sky(GLchar* texPrefix)
{
	GLfloat skyboxVertices[] = {
		// Positions          
		-1.0f, 1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,

		-1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,

		-1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f
	};

	glGenVertexArrays(1, &this->skyVAO);
	glGenBuffers(1, &this->skyVBO);
	glBindVertexArray(this->skyVAO);
	glBindBuffer(GL_ARRAY_BUFFER, this->skyVBO);
	glBufferData(GL_ARRAY_BUFFER, 36 * 3 * sizeof(GLfloat), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);

	this->skyShader = Shader("shader/skyVertexShader.glsl", "shader/skyFragmentShader.glsl");

	std::vector<const GLchar*> faces;
	/*
	std::string texName1;
	std::string postfix1;
	postfix1 = "1.bmp";
	texName1 = texPrefix + postfix1;
	faces.push_back(texName1.c_str());

	std::string texName2;
	std::string postfix2;
	postfix2 = "3.bmp";
	texName2 = texPrefix + postfix2;
	faces.push_back(texName2.c_str());

	std::string texName3;
	std::string postfix3;
	postfix3 = "4.bmp";
	texName3 = texPrefix + postfix3;
	faces.push_back(texName3.c_str());

	//4th is bottom
	std::string texName4;
	std::string postfix4;
	postfix4 = "5.bmp";
	texName4 = texPrefix + postfix4;
	faces.push_back(texName4.c_str());

	std::string texName5;
	std::string postfix5;
	postfix5 = "2.bmp";
	texName5 = texPrefix + postfix5;
	faces.push_back(texName5.c_str());

	std::string texName6;
	std::string postfix6;
	postfix6 = "0.bmp";
	texName6 = texPrefix + postfix6;
	faces.push_back(texName6.c_str());*/
	
	faces.push_back("textures/right.jpg");
	faces.push_back("textures/left.jpg");
	faces.push_back("textures/top.jpg");
	faces.push_back("textures/bottom.jpg");
	faces.push_back("textures/back.jpg");
	faces.push_back("textures/front.jpg");
	
	/*
	faces.push_back("textures/SkyBox1.bmp");
	faces.push_back("textures/SkyBox3.bmp");
	faces.push_back("textures/SkyBox4.bmp");
	faces.push_back("textures/SkyBox4.bmp");
	faces.push_back("textures/SkyBox2.bmp");
	faces.push_back("textures/SkyBox0.bmp");*/
	this->cubeTex = loadCubemap(faces);

}


Sky::~Sky()
{
}

void Sky::render(){
	glDepthMask(GL_FALSE);

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(this->skyShader.Program, "skybox"), 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, this->cubeTex);
	glBindVertexArray(this->skyVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthMask(GL_TRUE);
}


GLuint Sky::loadCubemap(std::vector<const GLchar*> faces)
{
	GLuint textureID;
	glGenTextures(1, &textureID);

	int width, height;
	unsigned char* image;

	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	for (GLuint i = 0; i < faces.size(); i++)
	{
		image = SOIL_load_image(faces[i], &width, &height, 0, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		SOIL_free_image_data(image);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return textureID;
}