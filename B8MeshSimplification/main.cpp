#include <string>
#include <iostream>
#include <time.h>

// GLEW
#define GLEW_STATIC
#include <gl/glew.h>
#include <GLFW\glfw3.h>
#include <gl/glut.h>
#include <gl/glu.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../include/camera.h"
#include "../include/shader.h"

#include <SOIL.h>
#include "../include/HalfEdge.h"

GLuint screenWidth = 800, screenHeight = 600;

// Camera
Camera camera(glm::vec3(-1.0f, 1.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
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

std::string models[2] = { "eight.uniform.obj", "bunny.obj" };
int currentModel = 0;
HalfEdgeModel mymodel;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mousebutton_callback(GLFWwindow* window, int button, int action, int mode);
void do_actions();

void nextModel();
void simp(double percentage);

int main(){

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "AssignmentB8", nullptr, nullptr);
	glfwMakeContextCurrent(window);

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

	mymodel = HalfEdgeModel(models[currentModel]);
	Shader shader("vertexShader.glsl", "fragmentShader.glsl");

	while (!glfwWindowShouldClose(window))
	{
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Check and call events
		glfwPollEvents();
		do_actions();

		glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		shader.Use();

		glUniform3f(glGetUniformLocation(shader.Program, "AmbientColor"), 0.2f, 0.2f, 0.2f);
		shader.SetCameraProperty(screenWidth, screenHeight, 0.1f, 1000.0f, camera);
		shader.SetDefaultLight();
		shader.SetSpotLight(0, camera.Position, camera.Front, glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, 0.09f, 0.032f, 40, 60);

		glUniform3f(glGetUniformLocation(shader.Program, "Albeido"), 0.0f, 0.0f, 0.0f);
		mymodel.Draw(shader);

		glUniform3f(glGetUniformLocation(shader.Program, "Albeido"), 1.0f, 0.0f, 0.0f);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		mymodel.Draw(shader);

		glfwSwapBuffers(window);

	}
	

	glfwTerminate();
	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode){
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (action == GLFW_PRESS)
		keys[key] = true;
	else if (action == GLFW_RELEASE)
		keys[key] = false;
}

void do_actions(){
	if (keys[GLFW_KEY_W])
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_S])
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_A])
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (keys[GLFW_KEY_D])
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (keys[GLFW_KEY_0]){
		simp(1);
	}
	if (keys[GLFW_KEY_1]){
		simp(0.1);
	}
	if (keys[GLFW_KEY_2]){
		simp(0.2);
	}
	if (keys[GLFW_KEY_3]){
		simp(0.3);
	}
	if (keys[GLFW_KEY_4]){
		simp(0.4);
	}
	if (keys[GLFW_KEY_5]){
		simp(0.5);
	}
	if (keys[GLFW_KEY_6]){
		simp(0.6);
	}
	if (keys[GLFW_KEY_7]){
		simp(0.7);
	}
	if (keys[GLFW_KEY_8]){
		simp(0.8);
	}
	if (keys[GLFW_KEY_9]){
		simp(0.9);
	}
	if (keys[GLFW_KEY_N]){
		nextModel();
		std::cout << "changing to model " << models[currentModel] << std::endl << std::endl;
		mymodel = HalfEdgeModel(models[currentModel]);
	}

	if (inViewRotate){
		float xoffset = -cursorPosX + prevPosX;
		float yoffset = -cursorPosY + prevPosY;
		camera.ProcessMouseRotate(xoffset, yoffset);
	}
	if (inViewPan){
		float xoffset = -cursorPosX + prevPosX;
		float yoffset = cursorPosY - prevPosY;
		camera.ProcessMousePan(xoffset, yoffset);
	}
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
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

void nextModel(){
	currentModel++;
	currentModel = currentModel % 2;
}

void simp(double percentage){
	mymodel = HalfEdgeModel(models[currentModel]);

	int originalFace = mymodel.meshes[0].faceSize();
	std::cout << "simplifing " << models[currentModel] << " to " << percentage * 100 << "% of original faces" << std::endl;
	time_t start = clock();
	mymodel.quadricSimplify(percentage);
	time_t end = clock();
	std::cout << "complete!" << std::endl;
	std::cout << "               original face number: " << originalFace << std::endl;
	std::cout << "   face number after simplification: " << mymodel.meshes[0].faceSize() << std::endl;
	std::cout << "                         time spend: " << (double)(end - start) / CLOCKS_PER_SEC << " s " << std::endl <<std::endl;

}