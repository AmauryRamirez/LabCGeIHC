//glew include
#include <GL/glew.h>

//std includes
#include <string>
#include <iostream>

//glfw include
#include <GLFW/glfw3.h>

// program include
#include "Headers/TimeManager.h"

int screenWidth;
int screenHeight;

GLFWwindow * window; // permite crear las ventanas y mandar al contexto de OpenGL 

bool exitApp = false;
int lastMousePosX;
int lastMousePosY;

int colorStatus = 0;

double deltaTime;

// Se definen todos las funciones.
void reshapeCallback(GLFWwindow* Window, int widthRes, int heightRes);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void mouseButtonCallback(GLFWwindow* window, int button, int state, int mod);
void init(int width, int height, std::string strTitle, bool bFullScreen);
void destroy();
bool processInput(bool continueApplication = true);

// Implementacion de todas las funciones.
void init(int width, int height, std::string strTitle, bool bFullScreen) {
	
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		exit(-1);
	}

	screenWidth = width;
	screenHeight = height;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	if (bFullScreen)
		window = glfwCreateWindow(width, height, strTitle.c_str(),
			glfwGetPrimaryMonitor(), nullptr);
	else
		window = glfwCreateWindow(width, height, strTitle.c_str(), nullptr,
			nullptr);

	if (window == nullptr) {
		std::cerr
			<< "Error to create GLFW window, you can try download the last version of your video card that support OpenGL 3.3+"
			<< std::endl;
		destroy();
		exit(-1);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);

	glfwSetWindowSizeCallback(window, reshapeCallback); //Tamaño de la pantalla. para alguna redimensión 
	glfwSetKeyCallback(window, keyCallback);			//
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Init glew
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		std::cerr << "Failed to initialize glew" << std::endl;
		exit(-1);
	}

	glViewport(0, 0, screenWidth, screenHeight);
	glClearColor(1.0f, 0.0f, 0.0f, 0.0f); 
}

void destroy() {
	glfwDestroyWindow(window);
	glfwTerminate();
}

void reshapeCallback(GLFWwindow* Window, int widthRes, int heightRes) {
	screenWidth = widthRes;
	screenHeight = heightRes;
	glViewport(0, 0, widthRes, heightRes);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_ESCAPE:
			exitApp = true;
			break;
		case GLFW_KEY_R:		//CACHAMOS SI PRESIONAMOS ALGUNA TECLA, EN ESTE CASO ESCAPE,R,G,B. 
			colorStatus = 0;
			break;
		case GLFW_KEY_G:
			colorStatus = 1;
			break;
		case GLFW_KEY_B:
			colorStatus = 2;
			break;
		case GLFW_KEY_N:
			colorStatus = 3;
			break;
		case GLFW_KEY_A:
			colorStatus = 4;
			break;
		case GLFW_KEY_W:
			colorStatus = 5;
			break;

		}
	}
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	lastMousePosX = xpos;
	lastMousePosY = ypos;
}

void mouseButtonCallback(GLFWwindow* window, int button, int state, int mod) {
	if (state == GLFW_PRESS) {
		switch (button) {
		case GLFW_MOUSE_BUTTON_RIGHT:
			std::cout << "lastMousePos.y:" << lastMousePosY << std::endl;
			glClearColor(0.0, 1.0, 0.0, 0.0);
			//colorStatus++;
			break;
		case GLFW_MOUSE_BUTTON_LEFT:
			std::cout << "lastMousePos.x:" << lastMousePosX << std::endl;
			//colorStatus--;
			break;
		case GLFW_MOUSE_BUTTON_MIDDLE:
			std::cout << "lastMousePos.x:" << lastMousePosX << std::endl;
			std::cout << "lastMousePos.y:" << lastMousePosY << std::endl;
			break;
		}
	}
}

bool processInput(bool continueApplication){
	if (exitApp || glfwWindowShouldClose(window) != 0) {
		return false;
	}
	deltaTime = 1 / TimeManager::Instance().CalculateFrameRate(false);
	glfwPollEvents();
	return continueApplication;
}

void applicationLoop() {
	bool psi = true;
	while (psi) {
		psi = processInput(true);
		glClear(GL_COLOR_BUFFER_BIT);

		if (colorStatus == 0)
			glClearColor(1.0, 0.0f, 0.0, 0.0);
		else if (colorStatus == 1)
			glClearColor(0.0, 1.0, 0.0, 0.0);
		else if (colorStatus == 2)
			glClearColor(0.0, 0.0f, 1.0, 0.0);
		else if (colorStatus == 3)
			glClearColor(0.0, 0.0f, 0.0, 1.0);
		else if (colorStatus == 4)
			glClearColor(1.0, 1.0f, 0.0, 0.0);
		else if (colorStatus == 5)
			glClearColor(1.0, 1.0f, 1.0, 1.0);




		glfwSwapBuffers(window);
	}
}

int main(int argc, char ** argv) {
	init(1600, 900, "MI VENTANA",true);
	applicationLoop();
	destroy();
	return 1;
}