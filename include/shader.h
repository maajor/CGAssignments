#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <random>
#include <time.h>

#include "camera.h"

class Shader
{
public:
    GLuint Program;

	Shader(){
		this->Program = glCreateProgram();
	}

    // Constructor generates the shader on the fly
    Shader(const GLchar* vertexPath, const GLchar* fragmentPath, const GLchar* geometryPath = nullptr)
    {
		pointLightsCount = 0;
		srand(time(NULL));

        // 1. Retrieve the vertex/fragment source code from filePath
        std::string vertexCode;
        std::string fragmentCode;
        std::string geometryCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        std::ifstream gShaderFile;
        // ensures ifstream objects can throw exceptions:
        vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        gShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        try 
        {
            // Open files
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            // Read file's buffer contents into streams
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();		
            // close file handlers
            vShaderFile.close();
            fShaderFile.close();
            // Convert stream into string
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();			
			// If geometry shader path is present, also load a geometry shader
			if(geometryPath != nullptr)
			{
                gShaderFile.open(geometryPath);
                std::stringstream gShaderStream;
				gShaderStream << gShaderFile.rdbuf();
				gShaderFile.close();
				geometryCode = gShaderStream.str();
			}
        }
        catch (std::ifstream::failure e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        }
        const GLchar* vShaderCode = vertexCode.c_str();
        const GLchar * fShaderCode = fragmentCode.c_str();
        // 2. Compile shaders
        GLuint vertex, fragment;
        GLint success;
        GLchar infoLog[512];
        // Vertex Shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");
        // Fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
		checkCompileErrors(fragment, "FRAGMENT");
		// If geometry shader is given, compile geometry shader
		GLuint geometry;
		if(geometryPath != nullptr)
		{
			const GLchar * gShaderCode = geometryCode.c_str();
			geometry = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(geometry, 1, &gShaderCode, NULL);
			glCompileShader(geometry);
			checkCompileErrors(geometry, "GEOMETRY");
		}
        // Shader Program
        this->Program = glCreateProgram();
        glAttachShader(this->Program, vertex);
        glAttachShader(this->Program, fragment);
		if(geometryPath != nullptr)
			glAttachShader(this->Program, geometry);
        glLinkProgram(this->Program);
        checkCompileErrors(this->Program, "PROGRAM");
        // Delete the shaders as they're linked into our program now and no longer necessery
        glDeleteShader(vertex);
        glDeleteShader(fragment);
		if(geometryPath != nullptr)
			glDeleteShader(geometry);

    }
    // Uses the current shader
    void Use() { glUseProgram(this->Program); }

	void SetDirLight(glm::vec3 lightColor, glm::vec3 direction, float intensity){
		glUniform3f(glGetUniformLocation(this->Program, "dirlight.lightColor"), lightColor.r, lightColor.g, lightColor.b);
		glUniform3f(glGetUniformLocation(this->Program, "dirlight.direction"), direction.x, direction.y, direction.z);
		glUniform1f(glGetUniformLocation(this->Program, "dirlight.intensity"), intensity);
	}

	void SetPointLight(int index, glm::vec3 position, glm::vec3 lightColor, float k0, float k1, float k2){
		std::stringstream indexStringStream;
		indexStringStream << index;
		glUniform3f(glGetUniformLocation(this->Program, std::string("pointLight[" + indexStringStream.str() + "].position").c_str()), position.x, position.y, position.z);
		glUniform3f(glGetUniformLocation(this->Program, std::string("pointLight[" + indexStringStream.str() + "].lightColor").c_str()), lightColor.r, lightColor.g, lightColor.b);
		glUniform1f(glGetUniformLocation(this->Program, std::string("pointLight[" + indexStringStream.str() + "].k0").c_str()), k0);
		glUniform1f(glGetUniformLocation(this->Program, std::string("pointLight[" + indexStringStream.str() + "].k1").c_str()), k1);
		glUniform1f(glGetUniformLocation(this->Program, std::string("pointLight[" + indexStringStream.str() + "].k2").c_str()), k2);
	}

	void RemovePointLightAt(int index){
		std::stringstream indexStringStream;
		indexStringStream << index;
		glUniform3f(glGetUniformLocation(this->Program, std::string("pointLight[" + indexStringStream.str() + "].position").c_str()), 0, 0, 0);
		glUniform3f(glGetUniformLocation(this->Program, std::string("pointLight[" + indexStringStream.str() + "].lightColor").c_str()), 0, 0, 0);
		glUniform1f(glGetUniformLocation(this->Program, std::string("pointLight[" + indexStringStream.str() + "].k0").c_str()), 0);
		glUniform1f(glGetUniformLocation(this->Program, std::string("pointLight[" + indexStringStream.str() + "].k1").c_str()), 0);
		glUniform1f(glGetUniformLocation(this->Program, std::string("pointLight[" + indexStringStream.str() + "].k2").c_str()), 0);
	}

	void SetSpotLight(int index, glm::vec3 position, glm::vec3 direction, glm::vec3 ligtColor, float k0, float k1, float k2, float innerCutoffAngle, float outerCutoffAngle){
		std::stringstream indexStringStream;
		indexStringStream << index;
		glUniform3f(glGetUniformLocation(this->Program, std::string("spotLight[" + indexStringStream.str() + "].position").c_str()), position.x, position.y, position.z);
		glUniform3f(glGetUniformLocation(this->Program, std::string("spotLight[" + indexStringStream.str() + "].direction").c_str()), direction.x, direction.y, direction.z);
		glUniform3f(glGetUniformLocation(this->Program, std::string("spotLight[" + indexStringStream.str() + "].lightColor").c_str()), ligtColor.r, ligtColor.g, ligtColor.b);
		glUniform1f(glGetUniformLocation(this->Program, std::string("spotLight[" + indexStringStream.str() + "].k0").c_str()), k0);
		glUniform1f(glGetUniformLocation(this->Program, std::string("spotLight[" + indexStringStream.str() + "].k1").c_str()), k1);
		glUniform1f(glGetUniformLocation(this->Program, std::string("spotLight[" + indexStringStream.str() + "].k2").c_str()), k2);
		glUniform1f(glGetUniformLocation(this->Program, std::string("spotLight[" + indexStringStream.str() + "].innerCutOff").c_str()), glm::cos(glm::radians(innerCutoffAngle)));
		glUniform1f(glGetUniformLocation(this->Program, std::string("spotLight[" + indexStringStream.str() + "].outerCutOff").c_str()), glm::cos(glm::radians(outerCutoffAngle)));
	}

	void SetDefaultLight(){
		glUniform3f(glGetUniformLocation(this->Program, "AmbientColor"), 0.2f, 0.2f, 0.2f);
		SetDirLight(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f);
	}

	void SetDefaultPointLights(int rank){
		float size = (float) rank / 2.0f + 0.5f;
		for (int i = 0; i < 8; i++){
			int x = size  * 2 * ((i & 4) >> 2) - size;
			int y = size * 2 * ((i & 2) >> 1) - size;
			int z = size * 2 * (i & 1) - size;
			SetPointLight(i, glm::vec3( x, y, z), glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, 0.0f, 0.0f);
			pointLightsCount++;
		}
	}

	void RemovePointLight(){
		if (pointLightsCount <= 0)
			return;
		RemovePointLightAt(pointLightsCount);
		pointLightsCount--;
	}

	void AddRandomPointLight(int rank){
		pointLightsCount++;
		float size = (float)rank / 2.0f + 0.5f;
		int side = rand() % 3;
		float x, y, z;
		if (side == 0){
			x = size;
			y = (rand() % 1000) / 1000 * size;
			z = (rand() % 1000) / 1000 * size;
		}
		else if (side == 1){
			y = size;
			x = (rand() % 1000) / 1000 * size;
			z = (rand() % 1000) / 1000 * size;
		}
		else {
			z = size;
			x = (rand() % 1000) / 1000 * size;
			y = (rand() % 1000) / 1000 * size;
		}
		glm::vec3 randDir = glm::vec3(((rand() % 2) * 2) - 1, ((rand() % 2) * 2) - 1, ((rand() % 2) * 2) - 1);
		glm::vec3 randPos = glm::vec3(x * randDir.x, y * randDir.y, z * randDir.z);
		std::cout << "a point light add at " << randPos.x << ", " << randPos.y << ", " << randPos.z << std::endl;
		SetPointLight(pointLightsCount, randPos, glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, 0.0f, 0.0f);
	}

	void SetCameraProperty(int width, int height, float near, float far, Camera camera){
		// Create camera transformations
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(camera.Zoom, (GLfloat)width / (GLfloat)height, near, far);
		// Get the uniform locations
		GLint viewLoc = glGetUniformLocation(this->Program, "view");
		GLint projLoc = glGetUniformLocation(this->Program, "projection");
		// Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		glUniform3f(glGetUniformLocation(this->Program, "ViewPos"), camera.Position.x, camera.Position.y, camera.Position.z);
	}

	void SetNontransCameraProperty(int width, int height, float near, float far, Camera camera){
		// Create camera transformations
		glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
		glm::mat4 projection = glm::perspective(camera.Zoom, (GLfloat)width / (GLfloat)height, near, far);
		// Get the uniform locations
		GLint viewLoc = glGetUniformLocation(this->Program, "view");
		GLint projLoc = glGetUniformLocation(this->Program, "projection");
		// Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		glUniform3f(glGetUniformLocation(this->Program, "ViewPos"), camera.Position.x, camera.Position.y, camera.Position.z);
	}

private:
    void checkCompileErrors(GLuint shader, std::string type)
	{
		GLint success;
		GLchar infoLog[1024];
		if(type != "PROGRAM")
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if(!success)
			{
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "| ERROR::::SHADER-COMPILATION-ERROR of type: " << type << "|\n" << infoLog << "\n| -- --------------------------------------------------- -- |" << std::endl;
			}
		}
		else
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if(!success)
			{
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "| ERROR::::PROGRAM-LINKING-ERROR of type: " << type << "|\n" << infoLog << "\n| -- --------------------------------------------------- -- |" << std::endl;
			}
		}
	}

	int pointLightsCount;
};

#endif