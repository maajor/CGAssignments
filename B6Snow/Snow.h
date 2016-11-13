#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <time.h> 
#include <random>
#include<algorithm>

#include <glm\glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SOIL.h>

// GLEW
#define GLEW_STATIC
#include <gl/glew.h>

#include "../include/shader.h"

class Snow;
class Snowflake;

class Snowflake{
	glm::vec3 position;
	float _scale;
	float _velocity;

	friend class flakeComparer;
public:
	
	Snowflake(glm::vec3 startPos, float scale, float velocity){
		position = startPos;
		_scale = scale;
		_velocity = velocity;
	}

	void Update(){
		//srand(time(0) + position.x + position.y + position.z);
		glm::vec3 speed = glm::vec3((rand() % 1000 - 500) / (float)(1000), 0, (rand() % 1000 - 500) / (float)(1000));
		speed += glm::vec3(0, -1, 0);
		speed = glm::normalize(speed);
		speed *= _velocity;
		position += speed;
	}


	void SetModelMatrix(Shader shader){
		glm::mat4 model = glm::mat4();
		//model = glm::rotate(model, 1.2f, glm::vec3(1.0, 0.0, 0.0));
		//model = glm::scale(model, glm::vec3(_scale));
		//model = glm::scale(model, glm::vec3(0.1));
		model = glm::translate(model, position);
		model = glm::scale(model, glm::vec3(_scale));

		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));

	}

	bool isDead(){
		return position.y < 0;
	}

};
class flakeComparer{
	glm::vec3 _viewPos;
	glm::vec3 _frontDir;
public:
	flakeComparer(glm::vec3 viewPos, glm::vec3 front){
		_viewPos = viewPos;
		_frontDir = front;
		_frontDir.y = 0;
	}

	bool operator()(const Snowflake& flake1, const Snowflake& flake2) {
		glm::vec3 viewDir1 = - flake1.position + _viewPos;
		viewDir1.y = 0;
		glm::vec3 viewDir2 = - flake2.position + _viewPos;
		viewDir2.y = 0;
		return dot(viewDir1, _frontDir) < dot(viewDir2, _frontDir);
		//return flake1.position.z < flake2.position.z;
	}


};

class Snow{
	std::vector<Snowflake> flakes;
	//float startTime;
	time_t startTime;

	GLuint VAO, VBO, EBO;
	std::vector<GLfloat> vertices;
	std::vector<GLuint> indices;
	GLuint texture;
	Shader snowShader;

	void initiate(std::string texturePath, Shader shader){
		vertices = std::vector<GLfloat>{
			-1, 0, -1, 0, 1, 0, 0, 0,
				1, 0, 1, 0, 1, 0, 1, 1,
				-1, 0, 1, 0, 1, 0, 0, 1,
				1, 0, -1, 0, 1, 0, 1, 0
		};
		indices = std::vector<GLuint>{
			0, 1, 2, 1, 0, 3
		};

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
		// Load data into vertex buffers
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

		// Set the vertex attribute pointers
		// Vertex Positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
		// Vertex Normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		// uv
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));

		glBindVertexArray(0);

		snowShader = shader;

		int wid, hei;
		unsigned char* image = SOIL_load_image(texturePath.c_str(), &wid, &hei, 0, SOIL_LOAD_RGBA);

		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// Set texture filtering
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, wid, hei, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);
		SOIL_free_image_data(image);
		glBindTexture(GL_TEXTURE_2D, 0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glUniform1i(glGetUniformLocation(snowShader.Program, "textureD"), 0);

	}

	void SpawnFlakes(){
		time_t currentTime = time(NULL);
		float elapsedTime = currentTime - startTime;
		
		int currentSize = min_size + (elapsedTime / 10) * increment_size;
		//srand(currentTime);

		for (int i = 0; i < currentSize; i++){
			//srand(currentTime + i);
			glm::vec3 spawnPos = glm::vec3((getRand() - 0.5) * spawn_area, 50, (getRand() - 0.5) * spawn_area);
			Snowflake newflake(spawnPos, getRand() * 0.2 + 0.1, getRand() * 0.2 + 0.1);
			flakes.push_back(newflake);
		}
	}

	double getRand(){
		double t = (rand() % 10000) / 10000.0;
		//std::cout << t << std::endl;
		return t;
	}

public:
	int min_size;
	int increment_size;
	float spawn_area;

	Snow(std::string texturePath, Shader shader){
		startTime = time(NULL);
		min_size = 3;
		increment_size = 10;
		spawn_area = 100;
		srand(startTime);
		initiate(texturePath, shader);
	}

	void Update(){
		//create new flakes
		SpawnFlakes();

		//update and move flakes
		int flakesize = flakes.size();
		for (int i = 0; i <flakesize; i++){
			flakes[i].Update();
		}

		//remove dead flakes
		
		for (std::vector<Snowflake>::iterator iter = flakes.begin(); iter != flakes.end();)
		{
			if ((*iter).isDead())
				iter = flakes.erase(iter);
			else
				iter++;
		}
		
	}

	void Draw(Camera camera, int width, int height){
		snowShader.Use();
		snowShader.SetCameraProperty(width, height, 0.1f, 1000.0f, camera);

		sort(flakes.begin(), flakes.end(), flakeComparer(camera.Position, camera.Front));

		for (int i = 0; i < flakes.size(); i++){
			glActiveTexture(GL_TEXTURE0);
			glUniform1i(glGetUniformLocation(snowShader.Program, "textureD"), 0);
			glBindTexture(GL_TEXTURE_2D, texture);

			glUniform4f(glGetUniformLocation(snowShader.Program, "tintColor"), 1.0f, 1.0f, 1.0f, 1.0f);

			flakes[i].SetModelMatrix(snowShader);

			glBindVertexArray(VAO);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}

	void testModel(){
		glm::vec3 spawnPos = glm::vec3(0,0,0);
		Snowflake newflake(spawnPos, 1, 0);
		flakes.push_back(newflake);
	}

	void DrawGround(Shader groundshader, Camera camera, int width, int height){
		groundshader.Use();
		groundshader.SetCameraProperty(width, height, 0.1f, 1000.0f, camera);

		glm::mat4 model = glm::mat4();
		model = glm::scale(model, glm::vec3(1000));

		glUniformMatrix4fv(glGetUniformLocation(groundshader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));

		glActiveTexture(GL_TEXTURE0);
		glUniform1i(glGetUniformLocation(groundshader.Program, "textureD"), 0);
		glBindTexture(GL_TEXTURE_2D, texture);

		glUniform4f(glGetUniformLocation(groundshader.Program, "tintColor"), 0.8f, 0.8f, 0.8f, 1.0f);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

};