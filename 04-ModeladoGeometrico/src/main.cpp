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
#include "Headers/Texture.h"


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
Shader shaderTexture;

Sphere sphere1(20, 20);
Sphere sphere2(20, 20);
Sphere sphere3(20, 20);
Cylinder cylinder1(20, 20, 0.5, 0.5);
Box box1;

GLuint textureEsponja;

bool exitApp = false;
int lastMousePosX, offsetX = 0;
int lastMousePosY, offsetY = 0;


float HomIzArriba = 0.0, CodoIzArriba = 0.0, HomDerArriba = 0.0, CodoDerArriba = 0.0; //agrega movimiento a las articulaciones
float HomIzFrente = 0.0, CodoIzFrente = 0.0, HomDerFrente = 0.0, CodoDerFrente = 0.0 ; //MOVIMIENTOS EN EJE Z
float PelvisDerArriba = 0.0, PelvisDerFrente = 0.0, RodDerArriba = 0.0, RodDerFrente = 0.0;
float PelvisIzqArriba = 0.0, PelvisIzqFrente = 0.0, RodIzqArriba = 0.0, RodIzqFrente = 0.0;
float DA0 = 0.0, DA1 = 0.0, DA2 = 0.0, DB0 = 0.0, DB1 = 0.0, DB2 = 0.0, DC0 = 0.0, DC1 = 0.0, DC2 = 0.0;
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
	shaderTexture.initialize("../Shaders/texturizado.vs", "../Shaders/texturizado.fs");

	sphere1.init();
	sphere1.setShader(&shader);
	sphere1.setColor(glm::vec4(0.0, 0.0, 0.0, 1.0));

	sphere2.init();
	sphere2.setShader(&shader);
	sphere2.setColor(glm::vec4(255, 255, 255, 1.0));

	sphere3.init();
	sphere3.setShader(&shader);
	sphere3.setColor(glm::vec4(255, 255, 0, 1.0));

	cylinder1.init();
	cylinder1.setShader(&shader);
	cylinder1.setColor(glm::vec4(255, 255, 0.0, 1.0));

	box1.init();
	box1.setShader(&shaderTexture);
	box1.setColor(glm::vec4(255, 255, 0, 1.0));
	camera->setPosition(glm::vec3(2.0, 0.0, 4.0));

	// Definimos el tamanio de la imagen
	int imageWidth, imageHeight;
	Texture textureSponja("../Textures/sponge.jpg");
	FIBITMAP *bitmap = textureSponja.loadImage();
	unsigned char *data = textureSponja.convertToData(bitmap, imageWidth,
		imageHeight);
	glGenTextures(1, &textureEsponja);
	glBindTexture(GL_TEXTURE_2D, textureEsponja);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureSponja.freeImage(bitmap);


}

void destroy() {
	glfwDestroyWindow(window);
	glfwTerminate();
	// --------- IMPORTANTE ----------
	// Eliminar los shader y buffers creados.

	sphere1.destroy();
	sphere2.destroy();
	sphere3.destroy();
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

	offsetX = 0;
	offsetY = 0;

		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			sentido = false;

	//MOVIMIENTO HOMBRO IZQUIERDO 1- EJE XY  2- EJE XZ
		if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && sentido)
			HomIzArriba += 0.001;
		else if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && !sentido)
			HomIzArriba -= 0.001;
		if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && sentido)
			HomIzFrente -= 0.001;
		else if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && !sentido)
			HomIzFrente += 0.001;
	//MOVIMINETO CODO IZQUIERDO
		if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && sentido)
			CodoIzArriba += 0.001;
		else if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && !sentido)
			CodoIzArriba -= 0.001;
		if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && sentido)
			CodoIzFrente -= 0.001;
		else if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && !sentido)
			CodoIzFrente += 0.001;
	//MOVIMIENTO HOMBRO DERECHO 
		if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && sentido)
			HomDerArriba -= 0.001;
		else if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && !sentido)
			HomDerArriba += 0.001;
		if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && sentido)
			HomDerFrente -= 0.001;
		else if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && !sentido)
			HomDerFrente += 0.001;
	//MOVIMIENTO CODO DERECHO
		if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && sentido)
			CodoDerArriba -= 0.001;
		else if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && !sentido)
			CodoDerArriba += 0.001;
		if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && sentido)
			CodoDerFrente += 0.001;
		else if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && !sentido)
			CodoDerFrente -= 0.001;
	
	//MOVIMIENTO PIE DERECHO
		if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && sentido)
			PelvisDerArriba -= 0.001;
		else if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && !sentido)
			PelvisDerArriba += 0.001;
		if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && sentido)
			PelvisDerFrente -= 0.001;
		else if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && !sentido)
			PelvisDerFrente += 0.001;
	//MOVIMIENTO RODILLA DERECHA
		if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && sentido)
			RodDerArriba -= 0.001;
		else if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && !sentido)
			RodDerArriba += 0.001;
		if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && sentido)
			RodDerFrente -= 0.001;
		else if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && !sentido)
			RodDerFrente += 0.001;

	//MOVIMIENTO PIE IZQUIERDO
		if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && sentido)
			PelvisIzqArriba -= 0.001;
		else if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && !sentido)
			PelvisIzqArriba += 0.001;
		if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && sentido)
			PelvisIzqFrente -= 0.001;
		else if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && !sentido)
			PelvisIzqFrente += 0.001;
	//MOVIMIENTO RODILLA IZQUIERD
		if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && sentido)
			RodIzqArriba -= 0.001;
		else if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && !sentido)
			RodIzqArriba += 0.001;
		if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && sentido)
			RodIzqFrente -= 0.001;
		else if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && !sentido)
			RodIzqFrente += 0.001;
	//MOIMIENTO DEDO1
		if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && sentido)
			DA0 += 0.001;
		else if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && !sentido)
			DA0 -= 0.001;
		if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && sentido)
			DA1 += 0.001;
		else if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && !sentido)
			DA1 -= 0.001;
		if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS && sentido)
			DA2 += 0.001;
		else if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS && !sentido)
			DA2 -= 0.001;



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
				glBindTexture(GL_TEXTURE_2D, textureEsponja);
				box1.render(scale(model, glm::vec3(1.0, 1.0, 0.1)));
				


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



		//HOMBRO IAZQUIERO-----------------------------------------------
				glm::mat4 j1 = glm::translate(model, glm::vec3(0.5f, 0.0f, 0.0f));
				sphere2.enableWireMode();
				sphere2.render(glm::scale(j1, glm::vec3(0.1, 0.1, 0.1)));
				j1 = glm::rotate(j1, HomIzArriba,glm::vec3(0.0, 0.0, 1));
				j1 = glm::rotate(j1, HomIzFrente, glm::vec3(0.0, 1.0, 0.0));
		
		//ANTEBRAZO IZQ
				glm::mat4 L1 = glm::translate(j1, glm::vec3(0.25, 0.0, 0.0));
				L1 = glm::rotate(L1, glm::radians(90.0f), glm::vec3(0, 0, 1.0));
				cylinder1.enableWireMode();
				cylinder1.render(glm::scale(L1, glm::vec3(0.1, 0.5, 0.1)));
		
		//CODO IZQ
				glm::mat4 j2 = glm::translate(j1, glm::vec3(0.5, 0.0f, 0.0f)); //j2 va a depender de j1
				sphere3.enableWireMode();
				sphere3.render(glm::scale(j2, glm::vec3(0.1, 0.1, 0.1)));
				j2 = glm::rotate(j2, CodoIzArriba ,glm::vec3(0.0, 0.0, 1));
				j2 = glm::rotate(j2, CodoIzFrente ,glm::vec3(0.0, 1.0, 0.0));
		
		//BRAZO IZQUIERDO
				glm::mat4 L2 = glm::translate(j2, glm::vec3(0.25, 0.0, 0.0));
				L2 = glm::rotate(L2, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
				cylinder1.enableWireMode();
				cylinder1.render(glm::scale(L2, glm::vec3(0.1, 0.5, 0.1)));
			
		

		//HOMBRO DERECHO
				glm::mat4 j3 = glm::translate(model, glm::vec3(-0.5, 0.0, 0.0));
				sphere3.enableWireMode();
				sphere3.render(glm::scale(j3, glm::vec3(0.1, 0.1, 0.1)));
				j3 = glm::rotate(j3, HomDerArriba, glm::vec3(0.0, 0.0, 1.0));
				j3 = glm::rotate(j3, HomDerFrente, glm::vec3(0.0, 1.0, 0.0));

		//ANTEBRAZO DERECHO
				glm::mat4 L3 = glm::translate(j3, glm::vec3(-0.25, 0.0, 0.0));
				L3 = glm::rotate(L3, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
				cylinder1.enableWireMode();
				cylinder1.render(glm::scale(L3, glm::vec3(0.1, 0.5, 0.1)));

		//CODO DERECHO
				glm::mat4 j4 = glm::translate(j3, glm::vec3(-0.5, 0.0, 0.0));
				sphere3.enableWireMode();
				sphere3.render(glm::scale(j4, glm::vec3(0.1, 0.1, 0.1)));
				j4 = glm::rotate(j4, CodoDerArriba, glm::vec3(0.0, 0.0, 1.0));
				j4 = glm::rotate(j4, CodoDerFrente, glm::vec3(0.0, 1.0, 0.0));

		//BRAZO DERECHO
				glm::mat4 L4 = glm::translate(j4, glm::vec3(-0.25, 0.0, 0.0));
				L4 = glm::rotate(L4, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
				cylinder1.enableWireMode();
				cylinder1.render(glm::scale(L4, glm::vec3(0.1, 0.5, 0.1)));

		//MANO DERECHA
				glm::mat4 MD = glm::translate(j4, glm::vec3(-0.5, 0.0, 0.0));
				sphere3.enableWireMode();
				sphere3.render(glm::scale(MD, glm::vec3(0.1, 0.1, 0.1)));

//-----------------------------------------------------------------------------------------------------

		//PELVIS PIE DERECHO
				glm::mat4 j5 = glm::translate(model, glm::vec3(-0.25, -0.5, 0.0));
				sphere3.enableWireMode();
				sphere3.render(glm::scale(j5, glm::vec3(0.1, 0.1, 0.1)));
				j5 = glm::rotate(j5, PelvisDerArriba, glm::vec3(1.0, 0.0, 0.0));
				j5 = glm::rotate(j5, PelvisDerFrente, glm::vec3(0.0, 0.1, 0.0));
	
		//PIERNA DERECHA
				glm::mat4 L5 = glm::translate(j5, glm::vec3(0.0, -0.25, 0.0));
				cylinder1.enableWireMode();
				cylinder1.render(glm::scale(L5, glm::vec3(0.1, 0.5, 0.1)));

		//RODILLA DERECHA
				glm::mat4 j7 = glm::translate(j5, glm::vec3(0.0, -0.5, 0.0));
				sphere3.enableWireMode();
				sphere3.render(glm::scale(j7, glm::vec3(0.1, 0.1, 0.1)));
				j7 = glm::rotate(j7, RodDerArriba, glm::vec3(1.0, 0.0, 0.0));
				j7 = glm::rotate(j7, RodDerFrente, glm::vec3(0.0, 1.0, 0.0));

		//CHAMORRO DERECHO
				glm::mat4 L6 = glm::translate(j7, glm::vec3(0.0, -0.25, 0.0));
				cylinder1.enableWireMode();
				cylinder1.render(glm::scale(L6, glm::vec3(0.1, 0.5, 0.1)));

		

		//PELVIS IZQUIERDA
				glm::mat4 j6 = glm::translate(model, glm::vec3(0.25, -0.5, 0.0));
				sphere3.enableWireMode();
				sphere3.render(glm::scale(j6, glm::vec3(0.1, 0.1, 0.1)));
				j6 = glm::rotate(j6, PelvisIzqArriba, glm::vec3(1.0, 0.0, 0.0));
				j6 = glm::rotate(j6, PelvisIzqFrente, glm::vec3(0.0, 1.0, 0.0));

		//PIERNA DERECHA
				glm::mat4 L7 = glm::translate(j6, glm::vec3(0.0, -0.25, 0.0));
				cylinder1.enableWireMode();
				cylinder1.render(glm::scale(L7, glm::vec3(0.1, 0.5, 0.1)));

		//RODILLA DERECHA
				glm::mat4 j8 = glm::translate(j6, glm::vec3(0.0, -0.5, 0.0));
				sphere3.enableWireMode();
				sphere3.render(glm::scale(j8, glm::vec3(0.1, 0.1, 0.1)));
				j8 = glm::rotate(j8, RodIzqArriba, glm::vec3(1.0, 0.0, 0.0));
				j8 = glm::rotate(j8, RodIzqFrente, glm::vec3(0.0, 1.0, 0.0));

		//CHAMORRO DERECHO
				glm::mat4 L8 = glm::translate(j8, glm::vec3(0.0, -0.25, 0.0));
				cylinder1.enableWireMode();
				cylinder1.render(glm::scale(L8, glm::vec3(0.1, 0.5, 0.1)));
				
		
				
		//MANO DERECHA DEDO 1
				glm::mat4 D1 = glm::translate(MD, glm::vec3(-0.05, 0.05, 0.0));
				D1 = glm::rotate(D1, glm::radians(45.0f), glm::vec3(0.0, 0.0, 1.0));
				cylinder1.enableWireMode();
				cylinder1.render(glm::scale(D1, glm::vec3(0.02, 0.04, 0.02)));
		
		//DEDO 1 HUESO UNION
				glm::mat4 jd1 = glm::rotate(MD, glm::radians(45.0f), glm::vec3(0.0, 0.0, 0.1));
				jd1 = glm::translate(jd1, glm::vec3(0.0, 0.09, 0.0));
				sphere3.enableWireMode();
				sphere3.render(glm::scale(jd1, glm::vec3(0.02, 0.02, 0.02)));
				jd1 = glm::rotate(jd1, DA0, glm::vec3(1.0, 0.0, 0.0));
				
		//DEDO DERECHO SEGUNDA PARTE
				glm::mat4 D1b;
				D1b = glm::translate(jd1, glm::vec3(0.0, 0.02, 0.0));
				cylinder1.enableWireMode();
				cylinder1.render(glm::scale(D1b, glm::vec3(0.02, 0.04, 0.02)));
		
		//DEDO DERECHO FIN
				glm::mat4 jd2;
				jd2 = glm::translate(jd1, glm::vec3(0.0, 0.04, 0.0));
				sphere3.enableWireMode();
				sphere3.render(glm::scale(jd2, glm::vec3(0.02, 0.02, 0.02)));

		//MANO DERECHA DEDO 2
				glm::mat4 D2 = glm::translate(MD, glm::vec3(-0.05, 0.0, -0.05));
				D2 = glm::rotate(D2, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
				D2 = glm::rotate(D2, glm::radians(-45.0f), glm::vec3(1.0, 0.0, 0.0));
				cylinder1.enableWireMode();
				cylinder1.render(glm::scale(D2, glm::vec3(0.02, 0.04, 0.02)));

		//MANO DERECHA 2 HUESO UNION
				glm::mat4 jdb = glm::rotate(MD, glm::radians(-45.0f), glm::vec3(0.0, 1.0, 0.0));
				jdb = glm::translate(jdb, glm::vec3(-0.09, 0.0, 0.0));
				sphere3.enableWireMode();
				sphere3.render(glm::scale(jdb, glm::vec3(0.02, 0.02, 0.02)));
				jdb = glm::rotate(jdb, DA1, glm::vec3(0.0, 1.0, 0.0));

		//MANO DERECHA SEGUNDA PARTE
				glm::mat4 D2b = glm::translate(jdb, glm::vec3(-0.02, 0.0, 0.0));
				D2b = glm::rotate(D2b, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
				cylinder1.enableWireMode();
				cylinder1.render(glm::scale(D2b, glm::vec3(0.02, 0.04, 0.02)));

		//MANO DERECHA FIN
				glm::mat4 jdb2 = glm::translate(jdb, glm::vec3(-0.04, 0.0, 0.0));
				sphere3.enableWireMode();
				sphere3.render(glm::scale(jdb2, glm::vec3(0.02, 0.02, 0.02)));



		//MANO DERECHA DEDO 3
				glm::mat4 D3 = glm::translate(MD, glm::vec3(-0.05, -0.05, 0.0));
				D3 = glm::rotate(D3, glm::radians(135.0f), glm::vec3(0.0, 0.0, 1.0));	
				cylinder1.enableWireMode();
				cylinder1.render(glm::scale(D3, glm::vec3(0.02, 0.04, 0.02)));

		//MANO DERECHA HUESO UNION
				glm::mat4 jdc = glm::rotate(MD, glm::radians(45.0f), glm::vec3(0.0, 0.0, 0.1));
				jdc = glm::translate(jdc, glm::vec3(-0.09, 0.0, 0.0));
				sphere3.enableWireMode();
				sphere3.render(glm::scale(jdc, glm::vec3(0.02, 0.02, 0.02)));
				jdc = glm::rotate(jdc, DA2, glm::vec3(0.0, 1.0, 0.0));

		//MANO DERECHA SEGUNDA PARTE
				glm::mat4 D3b = glm::translate(jdc, glm::vec3(-0.02, 0.0, 0.0));
				D3b = glm::rotate(D3b, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
				cylinder1.enableWireMode();
				cylinder1.render(glm::scale(D3b, glm::vec3(0.02, 0.04, 0.02)));
			
		//MANO DERECHA FIN
				glm::mat4 jdc2 = glm::translate(jdc, glm::vec3(-0.04, 0.0, 0.0));
				sphere3.enableWireMode();
				sphere3.render(glm::scale(jdc2, glm::vec3(0.02, 0.02, 0.02)));



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
