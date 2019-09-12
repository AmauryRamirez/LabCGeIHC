//glew include
#include <GL/glew.h>

//std includes
#include <string>
#include <iostream>

//glfw include
#include <GLFW/glfw3.h>

// program include
#include "Headers/TimeManager.h"

// Shader include
#include "Headers/Shader.h"

// Model geometric includes
#include "Headers/Sphere.h"
#include "Headers/Cylinder.h"
#include "Headers/Box.h"
#include "Headers/FirstPersonCamera.h"


//GLM include
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

int screenWidth;
int screenHeight;

GLFWwindow * window;
std::shared_ptr<FirstPersonCamera> camera(new FirstPersonCamera());
Shader shader;

Sphere sphere1(20, 20);
Sphere sphere2(20, 20);
Sphere sphere3(20, 20);
Cylinder cylinder1(20, 20, 0.5, 0.5);
Box box1;


bool exitApp = false;
int lastMousePosX, offsetX = 0;
int lastMousePosY, offsetY = 0;


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

	glfwSetWindowSizeCallback(window, reshapeCallback);
	glfwSetKeyCallback(window, keyCallback);
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
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	shader.initialize("../Shaders/colorShader.vs", "../Shaders/colorShader.fs");

	// INICIALIZAR LOS BUFFERS VAO, VBO, EBO
	sphere1.init();
	// MÉTODO SETTER QUE COLOCA EL APUNTADOR AL SHADER QUE VAMOS A  OCUPAR. 
	sphere1.setShader(&shader);
	//sETTER PARA PONER EL COLOR DE LA GEOMETRÍA
	sphere1.setColor(glm::vec4(0.0, 0.0, 0.0, 1.0));

	// INICIALIZAR LOS BUFFERS VAO, VBO, EBO ESFERA 2
	sphere2.init();
	// MÉTODO SETTER QUE COLOCA EL APUNTADOR AL SHADER QUE VAMOS A  OCUPAR. 
	sphere2.setShader(&shader);
	//sETTER PARA PONER EL COLOR DE LA GEOMETRÍA
	sphere2.setColor(glm::vec4(255, 255, 255, 1.0));

	// INICIALIZAR LOS BUFFERS VAO, VBO, EBO ESFERA 2
	sphere3.init();
	// MÉTODO SETTER QUE COLOCA EL APUNTADOR AL SHADER QUE VAMOS A  OCUPAR. 
	sphere3.setShader(&shader);
	//sETTER PARA PONER EL COLOR DE LA GEOMETRÍA
	sphere3.setColor(glm::vec4(255, 255, 0, 1.0));


	//
	cylinder1.init();
	cylinder1.setShader(&shader);
	cylinder1.setColor(glm::vec4(255, 255, 0.0, 1.0));

	box1.init();
	box1.setShader(&shader);
	box1.setColor(glm::vec4(255, 255, 0, 1.0));
	camera->setPosition(glm::vec3(2.0, 0.0, 4.0));
}

void destroy() {
	glfwDestroyWindow(window);
	glfwTerminate();
	// --------- IMPORTANTE ----------
	// Eliminar los shader y buffers creados.

	sphere1.destroy();
	cylinder1.destroy(); //MANDA A EJECUTAR TODOS LOS DESTRUCTORES DE LOS VAO, EBO Y VBO
	box1.destroy();		//ESTO PARA LIBERAR LA MEMORIA

	shader.destroy();
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
		}
	}
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	offsetX = xpos - lastMousePosX;
	offsetY = ypos - lastMousePosY;
	lastMousePosX = xpos;
	lastMousePosY = ypos;
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		camera->mouseMoveCamera(offsetX, offsetY, deltaTime);
}

void mouseButtonCallback(GLFWwindow* window, int button, int state, int mod) {
	if (state == GLFW_PRESS) {
		switch (button) {
		case GLFW_MOUSE_BUTTON_RIGHT:
			std::cout << "lastMousePos.y:" << lastMousePosY << std::endl;
			break;
		case GLFW_MOUSE_BUTTON_LEFT:
			std::cout << "lastMousePos.x:" << lastMousePosX << std::endl;
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

	TimeManager::Instance().CalculateFrameRate(false);
	deltaTime = TimeManager::Instance().DeltaTime;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) //NOS AYUDAN PARA SI PRESIONAMOS LA TECLA W AVANCEMOS
		camera->moveFrontCamera(true, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) //NOS AYUDAN PARA SI PRESIONAMOS LA TECLA S RETROCEDER
		camera->moveFrontCamera(false, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) //NOS AYUDAN PARA SI PRESIONAMOS LA TECLA A IZQ
		camera->moveRightCamera(false, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) //NOS AYUDAN PARA SI PRESIONAMOS LA TECLA D DER
		camera->moveRightCamera(true, deltaTime);
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		camera->mouseMoveCamera(offsetX, offsetY, 0.0001);

	glfwPollEvents();
	return continueApplication;
}

void applicationLoop() {
	bool psi = true;
	while (psi) {
		psi = processInput(true);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float) screenWidth / (float) screenHeight, 0.01f, 100.0f);
		glm::mat4 view = camera->getViewMatrix(); //glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));


		shader.turnOn();

		shader.setMatrix4("projection", 1, false, glm::value_ptr(projection));
		shader.setMatrix4("view", 1, false, glm::value_ptr(view));

		glm::mat4 model = glm::mat4(1.0f);


		/*sphere1.enableWireMode(); //VISUALIZAR CON LINEAS LA ESFERA
		//HACE EL DIBUJADO DE LA GEOMETRÍA Y RECIBE LA MATRIZ DE TRANSFORMACIÓN
		sphere1.render(model); 
		/*cylinder1.render(model);
		cylinder1.enableWireMode();*/
		
		//CAJITA------------------
		//box1.enableWireMode();
		box1.render(scale(model, glm::vec3(1.0, 1.0, 0.1)));
		//----------------------
		//ARTICULACIÓN 1-----------------------------------------------
		glm::mat4 j1 = glm::translate(model, glm::vec3(0.5f, 0.0f, 0.0f));
		sphere2.enableWireMode();
		sphere2.render(glm::scale(j1, glm::vec3(0.1, 0.1, 0.1)));
		//--------------------------------------------------------------------
		//HUESO1
		glm::mat4 L1 = glm::translate(j1, glm::vec3(0.25, 0.0, 0.0));
		L1 = glm::rotate(L1, glm::radians(90.0f), glm::vec3(0, 0, 1.0));
		cylinder1.enableWireMode();
		cylinder1.render(glm::scale(L1, glm::vec3(0.1, 0.5, 0.1)));
		//--------------------------------------------------------------------
		//ARTICULACIÓN 2
		glm::mat4 j2 = glm::translate(j1, glm::vec3(0.25, 0.0f, 0.0f)); //j2 va a depender de j1
		sphere3.enableWireMode();
		sphere3.render(glm::scale(j2, glm::vec3(0.1, 0.1, 0.1)));
		//------------------------------------------------------
		//PUPILA OJOS
		glm::mat4 pupila1 = glm::translate(model, glm::vec3(0.25, 0.25, 0.065f)); //ojo se va a mover con el modelo, depende del modelo.
		//sphere1.enableWireMode();						//HACE QUE SE VEA CON LINEAS
		sphere1.render(glm::scale(pupila1, glm::vec3(0.1, 0.1, 0.1)));	//MUESTRA L FIGURA PERO LA DEBE ESCALAR CON SCALE
		//PUPILA2----------------------------------------------
		glm::mat4 pupila2 = glm::translate(model, glm::vec3(-0.25, 0.25, 0.065f)); //ojo se va a mover con el modelo, depende del modelo.
		//sphere1.enableWireMode();						//HACE QUE SE VEA CON LINEAS
		sphere1.render(glm::scale(pupila2, glm::vec3(0.1, 0.1, 0.1)));	//MUESTRA L FIGURA PERO LA DEBE ESCALAR CON SCALE

																		
		//OJO dDERECHO
		glm::mat4 ojo = glm::translate(model, glm::vec3(0.25, 0.25, 0.05f)); //ojo se va a mover con el modelo, depende del modelo.
		//sphere2.enableWireMode();						//HACE QUE SE VEA CON LINEAS
		sphere2.render(glm::scale(ojo, glm::vec3(0.2, 0.2, 0.1)));	//MUESTRA L FIGURA PERO LA DEBE ESCALAR CON SCALE
		//OJO IZQUIERDO------------------------
		glm::mat4 ojo2 = glm::translate(model, glm::vec3(-0.25, 0.25, 0.05f)); //ojo se va a mover con el modelo, depende del modelo.
		//phere2.enableWireMode();						//HACE QUE SE VEA CON LINEAS
		sphere2.render(glm::scale(ojo2, glm::vec3(0.2, 0.2, 0.1)));	//MUESTRA L FIGURA PERO LA DEBE ESCALAR CON SCALE
		//ARTICULACIÓN LADO DERECHO-----------------------------------------------
		glm::mat4 j3 = glm::translate(model, glm::vec3(-0.5f, 0.0f, 0.0f));
		sphere2.enableWireMode();
		sphere2.render(glm::scale(j3, glm::vec3(0.1, 0.1, 0.1)));
		//--------------------------------------------------------------------
		//HUSEO 2 BRAZO DERECHO-----------
		glm::mat4 L2 = glm::translate(j3, glm::vec3(-0.215, -0.1223, 0.0));
		L2 = glm::rotate(L2, glm::radians(120.0f), glm::vec3(0, 0, 1.0));
		cylinder1.enableWireMode();
		cylinder1.render(glm::scale(L2, glm::vec3(0.1, 0.5, 0.1)));
		//--------------------------------------------------------------------
		//ARTICULACIÓN 2 CODO DERECHO
		glm::mat4 j4 = glm::translate(j3, glm::vec3(-0.432, -0.248, 0.0f)); //j2 va a depender de j1
		sphere3.enableWireMode();
		sphere3.render(glm::scale(j4, glm::vec3(0.1, 0.1, 0.1)));
		//------------------------------------------------------
		///HUESO2
		glm::mat4 L3 = glm::translate(j4, glm::vec3(-0.166, 0.164, 0.0));
		L3 = glm::rotate(L3, glm::radians(45.0f), glm::vec3(0, 0, 1.0));
		cylinder1.enableWireMode();
		cylinder1.render(glm::scale(L3, glm::vec3(0.1, 0.5, 0.1)));
		//--------------------------------------------------------------------
		//ARTICULACIÓN PIE IZQ ALTO-----------------------------------------------
		glm::mat4 j5 = glm::translate(model, glm::vec3(0.25f, -0.5f, 0.0f));
		sphere1.enableWireMode();
		sphere1.render(glm::scale(j5, glm::vec3(0.1, 0.1, 0.1)));
		//--------------------------------------------------------------------
		//HUESO5
		glm::mat4 L4 = glm::translate(j5, glm::vec3(0.0, -0.25, 0.0));
		L4 = glm::rotate(L4, glm::radians(0.0f), glm::vec3(0, 0, 1.0));
		cylinder1.enableWireMode();
		cylinder1.render(glm::scale(L4, glm::vec3(0.1, 0.5, 0.1)));
		//--------------------------------------------------------------------
		//ARTICULACIÓN PIE DER-----------------------------------------------
		glm::mat4 j6 = glm::translate(model, glm::vec3(-0.25f, -0.5f, 0.0f));
		sphere1.enableWireMode();
		sphere1.render(glm::scale(j6, glm::vec3(0.1, 0.1, 0.1)));
		//--------------------------------------------------------------------
		//HUESO5
		glm::mat4 L5 = glm::translate(j6, glm::vec3(0.0, -0.25, 0.0));
		L5 = glm::rotate(L5, glm::radians(0.0f), glm::vec3(0, 0, 1.0));
		cylinder1.enableWireMode();
		cylinder1.render(glm::scale(L5, glm::vec3(0.1, 0.5, 0.1)));
		//--------------------------------------------------------------------
		//ARTICULACIÓN PIE IZQ RODILLA-----------------------------------------------
		glm::mat4 j7 = glm::translate(model, glm::vec3(0.25f, -0.75f, 0.0f));
		sphere3.enableWireMode();
		sphere3.render(glm::scale(j7, glm::vec3(0.1, 0.1, 0.1)));
		//--------------------------------------------------------------------
		//ARTICULACIÓN PIE DER RODILLA-----------------------------------------------
		glm::mat4 j8 = glm::translate(model, glm::vec3(-0.25f, -0.75f, 0.0f));
		sphere3.enableWireMode();
		sphere3.render(glm::scale(j8, glm::vec3(0.1, 0.1, 0.1)));
		//--------------------------------------------------------------------
		//ARTICULACIÓN ZAPATO DERECHO-----------------------------------------------
		glm::mat4 j9 = glm::translate(model, glm::vec3(-0.25f, -1.0f, 0.0f));
		sphere1.enableWireMode();
		sphere1.render(glm::scale(j9, glm::vec3(0.1, 0.1, 0.1)));
		//--------------------------------------------------------------------
		//ARTICULACIÓN ZAPATO DERECHO-----------------------------------------------
		glm::mat4 j10 = glm::translate(model, glm::vec3(0.25f, -1.0f, 0.0f));
		sphere1.enableWireMode();
		sphere1.render(glm::scale(j10, glm::vec3(0.1, 0.1, 0.1)));
		//--------------------------------------------------------------------

		

		
		
		shader.turnOff();
		glfwSwapBuffers(window);
	}
}

int main(int argc, char ** argv) {
	init(800, 700, "Window GLFW", false);
	applicationLoop();
	destroy();
	return 1;
}
