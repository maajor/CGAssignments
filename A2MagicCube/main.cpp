/*
* Assignment A2 Magic Cube
*
* @Ma Yidong 2015210056
*/

// Std. Includes
#include <string>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

#include <gl/glut.h>
#include <gl/glu.h>

// GL includes
#include "../include/shader.h"
#include "../include/camera.h"
#include "../include/model.h"

// GLM Mathemtics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Other Libs
#include <SOIL.h>

#include "MagicCube.h"
#include "ray.h"

// Properties
GLuint screenWidth = 800, screenHeight = 600;

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mousebutton_callback(GLFWwindow* window, int button, int action, int mode);
void Do_Movement();
void mymenu(int value);
void RenderQuad();

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
//Camera camera(glm::vec3(0.0f, 0.0f, 10.0f));
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
float cursorPosX, cursorPosY;
float prevPosX, prevPosY;
float lastHitX, lastHitY;
bool firstMouse = true;
bool inViewRotate = false;
bool inViewPan = false;
bool inModelRotate = false;
int lastModelRotateDirection = 0;
float lastModelRotationAngle = 0;
int lastModelRotationRow = 0;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

glm::vec3 hitIndex;
int hitSide;

MagicCube myCube;
// The MAIN function, from here we start our application and run our Game loop
int main()
{
	// Init GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "AssignmentA2", nullptr, nullptr); // Windowed
	glfwMakeContextCurrent(window);

	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetMouseButtonCallback(window, mousebutton_callback);

	// Initialize GLEW to setup the OpenGL Function pointers
	glewExperimental = GL_TRUE;
	glewInit();

	// Define the viewport dimensions
	glViewport(0, 0, screenWidth, screenHeight);

	// Setup some OpenGL options
	glEnable(GL_DEPTH_TEST);

	
	//setup G-buffer
	GLuint gBuffer;
	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	GLuint gPosition, gNormalMetal, gAlbedoRough;

	// - Position color buffer
	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

	// - Normal color buffer
	glGenTextures(1, &gNormalMetal);
	glBindTexture(GL_TEXTURE_2D, gNormalMetal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormalMetal, 0);

	// - Color + Specular color buffer
	glGenTextures(1, &gAlbedoRough);
	glBindTexture(GL_TEXTURE_2D, gAlbedoRough);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenWidth, screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoRough, 0);

	// - Tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
	glDrawBuffers(3, attachments);
	GLuint rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screenWidth, screenHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	// - Finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Setup and compile our shaders
	Shader shader("shader/vertexShader.glsl", "shader/fragmentShader.glsl");
	Shader gPassShader("shader/gPassVertex.glsl", "shader/gPassFragment.glsl");
	Shader lightPassShader("shader/lightPassVertex.glsl", "shader/lightPassFragment.glsl");

	gPassShader.Use();
	glUniform1i(glGetUniformLocation(gPassShader.Program, "gPosition"), 0);
	glUniform1i(glGetUniformLocation(gPassShader.Program, "gNormal"), 1);
	glUniform1i(glGetUniformLocation(gPassShader.Program, "gAlbedoSpec"), 2);

	Model ourModel("maya/cube.obj");
	myCube = MagicCube(3, "maya/cube.obj");
	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Set frame time
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Check and call events
		glfwPollEvents();
		Do_Movement();

		// Clear the colorbuffer
		glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		gPassShader.Use();
		gPassShader.SetCameraProperty(screenWidth, screenHeight, 0.1f, 100.0f, camera);
		myCube.render(gPassShader);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		lightPassShader.Use();
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(glGetUniformLocation(lightPassShader.Program, "gPosition"), 0);
		glBindTexture(GL_TEXTURE_2D, gPosition);
		glActiveTexture(GL_TEXTURE1);
		glUniform1i(glGetUniformLocation(lightPassShader.Program, "gNormalMetal"), 1);
		glBindTexture(GL_TEXTURE_2D, gNormalMetal);
		glActiveTexture(GL_TEXTURE2);
		glUniform1i(glGetUniformLocation(lightPassShader.Program, "gAlbedoRough"), 2);
		glBindTexture(GL_TEXTURE_2D, gAlbedoRough);
		
		//shader.Use();
		//shader.SetDefaultLight();
		//shader.SetCameraProperty(screenWidth, screenHeight, 0.1f, 100.0f, camera);

		//lightPassShader.SetDefaultLight();
		lightPassShader.SetCameraProperty(screenWidth, screenHeight, 0.1f, 100.0f, camera);
		//myCube.render(shader);

		RenderQuad();
		
		// Swap the buffers
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}

#pragma region "User input"

// Moves/alters the camera positions based on user input
void Do_Movement()
{
	// Camera controls
	if (keys[GLFW_KEY_W])
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_S])
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_A])
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (keys[GLFW_KEY_D])
		camera.ProcessKeyboard(RIGHT, deltaTime);

	if (inViewRotate){
		float xoffset =  - cursorPosX + prevPosX;
		float yoffset =  - cursorPosY + prevPosY;
		camera.ProcessMouseRotate(xoffset, yoffset);
	}
	if (inViewPan){
		float xoffset = - cursorPosX + prevPosX;
		float yoffset = cursorPosY - prevPosY;
		camera.ProcessMousePan(xoffset, yoffset);
	}
	if (inModelRotate){
		float sensit = 0.01f;
		float xoffset =  cursorPosX - lastHitX;
		float yoffset =  cursorPosY - lastHitY;
		float xoffsetAbs = std::abs(xoffset);
		float yoffsetAbs = std::abs(yoffset);

		if ((xoffsetAbs >= yoffsetAbs) && (hitSide == 0)){
			if (lastModelRotateDirection == 2)
				myCube.resetTransform();
			myCube.rotateYTo(hitIndex.y, xoffset * sensit);
			lastModelRotateDirection = 1;
			lastModelRotationRow = hitIndex.y;
			lastModelRotationAngle = xoffset * sensit;
		}
		else if ((xoffsetAbs < yoffsetAbs) && (hitSide == 0)){
			if (lastModelRotateDirection == 1)
				myCube.resetTransform();
			myCube.rotateZTo(hitIndex.z, yoffset * sensit);
			lastModelRotateDirection = 2;
			lastModelRotationRow = hitIndex.z;
			lastModelRotationAngle = yoffset * sensit;
		}
		else if ((xoffsetAbs >= yoffsetAbs) && (hitSide == 1)){
			if (lastModelRotateDirection == 2)
				myCube.resetTransform();
			myCube.rotateXTo(hitIndex.x, xoffset * sensit);
			lastModelRotateDirection = 0;
			lastModelRotationRow = hitIndex.x;
			lastModelRotationAngle = xoffset * sensit;
		}
		else if ((xoffsetAbs < yoffsetAbs) && (hitSide == 1)){
			if (lastModelRotateDirection == 0)
				myCube.resetTransform();
			myCube.rotateZTo(hitIndex.z, yoffset * sensit);
			lastModelRotateDirection = 2;
			lastModelRotationRow = hitIndex.z;
			lastModelRotationAngle = yoffset * sensit;
		}
		else if ((xoffsetAbs >= yoffsetAbs) && (hitSide == 2)){
			if (lastModelRotateDirection == 0)
				myCube.resetTransform();
			myCube.rotateYTo(hitIndex.y, xoffset * sensit);
			lastModelRotateDirection = 1;
			lastModelRotationRow = hitIndex.y;
			lastModelRotationAngle = xoffset * sensit;
		}
		else if ((xoffsetAbs < yoffsetAbs) && (hitSide == 2)){
			if (lastModelRotateDirection == 1)
				myCube.resetTransform();
			myCube.rotateXTo(hitIndex.x, yoffset * sensit);
			lastModelRotateDirection = 0;
			lastModelRotationRow = hitIndex.x;
			lastModelRotationAngle = yoffset * sensit;
		}
	}
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (action == GLFW_PRESS)
		keys[key] = true;
	else if (action == GLFW_RELEASE)
		keys[key] = false;

	if (key == GLFW_KEY_F2 && action == GLFW_PRESS)
		myCube = MagicCube(2, "maya/cube.obj");
	if (key == GLFW_KEY_F3 && action == GLFW_PRESS)
		myCube = MagicCube(3, "maya/cube.obj");
	if (key == GLFW_KEY_F4 && action == GLFW_PRESS)
		myCube = MagicCube(4, "maya/cube.obj");
	if (key == GLFW_KEY_F5 && action == GLFW_PRESS)
		myCube = MagicCube(5, "maya/cube.obj");
	if (key == GLFW_KEY_F6 && action == GLFW_PRESS)
		myCube = MagicCube(6, "maya/cube.obj");
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	prevPosX = cursorPosX;
	prevPosY = cursorPosY;
	cursorPosX = xpos;
	cursorPosY = ypos;
}

void mousebutton_callback(GLFWwindow* window, int button, int action, int mode){
	if (button == GLFW_MOUSE_BUTTON_LEFT && keys[GLFW_KEY_LEFT_ALT] == true){
		if (action == GLFW_RELEASE){
			inViewRotate = false;
		}
		else{
			inViewRotate = true;
		}
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT && keys[GLFW_KEY_LEFT_ALT] == true){
		if (action == GLFW_RELEASE){
			inViewPan = false;
		}
		else{
			inViewPan = true;
		}
	}
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && keys[GLFW_KEY_LEFT_CONTROL] == true){
		Ray hitray(camera, glm::vec2(cursorPosX, cursorPosY), glm::vec2(screenWidth, screenHeight));
		if (myCube.findHit(hitray, hitIndex, hitSide)){
			std::cout << hitIndex.x << " " << hitIndex.y << " " << hitIndex.z << " "<< hitSide <<std::endl;
			inModelRotate = true; 
			lastHitX = cursorPosX;
			lastHitY = cursorPosY;
			lastModelRotateDirection = (hitSide + 1) % 3;
			std::cout << "last model dir: " << lastModelRotateDirection << " " << hitSide << std::endl;
			//lastModelRotateDirection = hitSide;
		}
	}
	else{
		if (keys[GLFW_KEY_LEFT_CONTROL] == true){
			myCube.resetCube(lastModelRotateDirection, lastModelRotationRow, lastModelRotationAngle);
			std::cout << "finish Rotate" << lastModelRotateDirection << " axis" <<std::endl;
		}
		else
			myCube.resetTransform();
		inModelRotate = false;
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

void mymenu(int value){

	if (value == 1){

		glClear(GL_COLOR_BUFFER_BIT);

		glutSwapBuffers();

	}

	if (value == 2){

		exit(0);

	}

}

GLuint quadVAO = 0;
GLuint quadVBO;
void RenderQuad()
{
	if (quadVAO == 0)
	{
		GLfloat quadVertices[] = {
			// Positions        // Texture Coords
			-1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// Setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

#pragma endregion