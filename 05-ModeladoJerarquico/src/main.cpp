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

Shader shader;
std::shared_ptr<FirstPersonCamera> camera(new FirstPersonCamera());

Sphere sphere1(20, 20);
Sphere sphere2(20, 20);
Cylinder cylinder1(20, 20, 0.5, 0.5);
Box box1;

bool exitApp = false;
int lastMousePosX, offsetX = 0;
int lastMousePosY, offsetY = 0;

float CabezaRota = 0.0, CabezaInfRota = 0.0; //agrega movimiento a las articulaciones
float LlantaIzq = 0.0, LlantaDer = 0.0;
bool sentido = true; 

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

	// Inicializar los buffers VAO, VBO, EBO
	sphere1.init();
	// Método setter que colocar el apuntador al shader
	sphere1.setShader(&shader);
	//Setter para poner el color de la geometria
	sphere1.setColor(glm::vec4(0.3, 0.3, 1.0, 1.0));

	// Inicializar los buffers VAO, VBO, EBO
	sphere2.init();
	// Método setter que colocar el apuntador al shader
	sphere2.setShader(&shader);
	//Setter para poner el color de la geometria
	sphere2.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));

	cylinder1.init();
	cylinder1.setShader(&shader);
	cylinder1.setColor(glm::vec4(0.3, 0.3, 1.0, 1.0));

	box1.init();
	box1.setShader(&shader);
	box1.setColor(glm::vec4(1.0, 1.0, 0.0, 1.0));
	camera->setPosition(glm::vec3(0.0, 0.0, 4.0));
}

void destroy() {
	glfwDestroyWindow(window);
	glfwTerminate();
	// --------- IMPORTANTE ----------
	// Eliminar los shader y buffers creados.

	// Destrucción de los VAO, EBO, VBO
	sphere1.destroy();
	cylinder1.destroy();
	box1.destroy();

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

	if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera->moveFrontCamera(true, deltaTime);
	if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera->moveFrontCamera(false, deltaTime);
	if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera->moveRightCamera(false, deltaTime);
	if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera->moveRightCamera(true, deltaTime);
	if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		camera->mouseMoveCamera(offsetX, offsetY, 0.01);
	offsetX = 0;
	offsetY = 0;

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		sentido = false;
	//CABEZA SUPERIOR
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS && sentido)
		CabezaRota += 0.001;
	else if(glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS && !sentido)
		CabezaRota -= 0.001;
	//CABEZA INFERIOR
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && sentido)
		CabezaInfRota += 0.001;
	else if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && !sentido)
		CabezaInfRota -= 0.001;
	//LLANTA IZQUIERDA ADELANTE
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && sentido)
		LlantaIzq += 0.001;
	else if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && !sentido)
		LlantaIzq -= 0.001;
	//LLANTA DERECHA ADELANTE
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && sentido)
		LlantaDer += 0.001;
	else if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && !sentido)
		LlantaDer -= 0.001;

	sentido = true;

	glfwPollEvents();
	return continueApplication;
}

void applicationLoop() {
	bool psi = true;
	while (psi) {
		psi = processInput(true);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float) screenWidth / (float) screenHeight, 0.01f, 100.0f);
		glm::mat4 view = camera->getViewMatrix();

		shader.setMatrix4("projection", 1, false, glm::value_ptr(projection));
		shader.setMatrix4("view", 1, false, glm::value_ptr(view));

		glm::mat4 model = glm::mat4(1.0f);

		//box1.enableWireMode();
		//box1.render(glm::scale(model, glm::vec3(1.0, 1.0, 0.1)));

		//CUERPO
		glm::mat4 Cu = glm::translate(model, glm::vec3(0.0, 0.0, 0.0));
		Cu = glm::rotate(Cu, glm::radians(30.0f), glm::vec3(0, 0, 1));
		cylinder1.enableWireMode();
		cylinder1.render(glm::scale(Cu, glm::vec3(1, 1, 1)));

		//CABEZA INTERNA UNION A 30 GRADOS DEL EJE Z
		glm::mat4 Cai = glm::translate(Cu, glm::vec3(0.0, 0.5, 0.0));
		sphere1.enableWireMode();
		sphere1.render(glm::scale(Cai, glm::vec3(0.1, 0.1, 0.1)));
		Cai = glm::rotate(Cai, CabezaRota, glm::vec3(0.0, 1.0, 0.0));

		//CABEZA SUPERIOR
		glm::mat4 CaS = glm::translate(Cai, glm::vec3(0.0, 0.0, 0.0));
		sphere1.enableWireMode();
		sphere1.render(glm::scale(CaS, glm::vec3(1.0, 1.0, 1.0)));

		//PARTESITAS INFERIORES
		glm::mat4 CaIi = glm::translate(Cu, glm::vec3(0.0, -0.5, 0.0));
		sphere1.enableWireMode();
		sphere1.render(glm::scale(CaIi, glm::vec3(0.1, 0.1, 0.1)));
		CaIi = glm::rotate(CaIi, CabezaInfRota, glm::vec3(0.0, 1.0, 0.0));



		//CABEZA INFERIOR
		glm::mat4 CaI = glm::translate(CaIi, glm::vec3(0.0, 0.0, 0.0));
		sphere1.enableWireMode();
		sphere1.render(glm::scale(CaI, glm::vec3(1.0, 1.0, 1.0)));

		//HOMBRO IZQUIERDO
		glm::mat4 Hi0 = glm::translate(Cu, glm::vec3(0.0, 0.25, -0.25));
		sphere1.enableWireMode();
		sphere1.render(glm::scale(Hi0, glm::vec3(0.2, 0.2, 0.2)));

		//HOMBRO HUESO IZQ
		glm::mat4 Hi = glm::translate(Hi0, glm::vec3(0.0, 0.0, -0.25));
		Hi = glm::rotate(Hi, glm::radians(90.0f), glm::vec3(1, 0, 0));
		cylinder1.enableWireMode();
		cylinder1.render(glm::scale(Hi, glm::vec3(0.2, 0.5, 0.2)));

		//HOMBRO EXTERIOR IZQ
		glm::mat4 Hi1 = glm::translate(Hi0, glm::vec3(0.0, 0.0, -0.5));
		sphere1.enableWireMode();
		sphere1.render(glm::scale(Hi1, glm::vec3(0.2, 0.2, 0.2)));

		//BRAZO IZQUIERDO
		glm::mat4 Hi2 = glm::rotate(Hi1, glm::radians(-40.0f), glm::vec3(0, 0, 1)); 
		Hi2 = glm::translate(Hi2, glm::vec3(0.0, -0.5, 0.0));
		cylinder1.enableWireMode();
		cylinder1.render(glm::scale(Hi2, glm::vec3(0.5, 1.0, 0.5)));

		//ENVOLTURA BRAZO IZQUIERDO HOMBRO
		glm::mat4 HiE = glm::translate(Hi1, glm::vec3(0, 0, 0));
		sphere1.enableWireMode();
		sphere1.render(glm::scale(HiE, glm::vec3(0.5, 0.5, 0.5)));

		//MANO BRAZO IZQUIERDO
		glm::mat4 Hi3 = glm::rotate(Hi1, glm::radians(-40.0f), glm::vec3(0, 0, 1));
		Hi3 = glm::translate(Hi3, glm::vec3(0.0, -1.0, 0.0));
		sphere1.enableWireMode();
		sphere1.render(glm::scale(Hi3, glm::vec3(0.5, 0.5, 0.5)));



		//HOMBRO DERECHO INTERNO
		glm::mat4 Hd0 = glm::translate(Cu, glm::vec3(0.0, 0.25, 0.25));
		sphere1.enableWireMode();
		sphere1.render(glm::scale(Hd0, glm::vec3(0.2, 0.2, 0.2)));

		//HOMBRO HUESO DER
		glm::mat4 Hd = glm::translate(Hd0, glm::vec3(0.0, 0.0, 0.25));
		Hd = glm::rotate(Hd, glm::radians(90.0f), glm::vec3(1, 0, 0));
		cylinder1.enableWireMode();
		cylinder1.render(glm::scale(Hd, glm::vec3(0.2, 0.5, 0.2)));
		
		//HOMBRO EXTERIOR DER
		glm::mat4 Hd1 = glm::translate(Hd0, glm::vec3(0.0, 0.0, 0.5));
		sphere1.enableWireMode();
		sphere1.render(glm::scale(Hd1, glm::vec3(0.2, 0.2, 0.2)));
		
		//BRAZO IZQUIERDO
		glm::mat4 Hd2 = glm::rotate(Hd1, glm::radians(-40.0f), glm::vec3(0, 0, 1));
		Hd2 = glm::translate(Hd2, glm::vec3(0.0, -0.5, 0.0));
		cylinder1.enableWireMode();
		cylinder1.render(glm::scale(Hd2, glm::vec3(0.5, 1.0, 0.5)));
		
		//ENVOLTURA BRAZO IZQUIERDO HOMBRO
		glm::mat4 HdE = glm::translate(Hd1, glm::vec3(0, 0, 0));
		sphere1.enableWireMode();
		sphere1.render(glm::scale(HdE, glm::vec3(0.5, 0.5, 0.5)));
		
		//MANO BRAZO IZQUIERDO
		glm::mat4 Hd3 = glm::rotate(Hd1, glm::radians(-40.0f), glm::vec3(0, 0, 1));
		Hd3 = glm::translate(Hd3, glm::vec3(0.0, -1.0, 0.0));
		sphere1.enableWireMode();
		sphere1.render(glm::scale(Hd3, glm::vec3(0.5, 0.5, 0.5)));
		

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
