#define _USE_MATH_DEFINES
#include <cmath>
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

#include "Headers/Texture.h"

// Include loader Model class
#include "Headers/Model.h"

#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

int screenWidth;
int screenHeight;

GLFWwindow *window;

Shader shader;
//Shader de texturizado
Shader shaderTexture;
//Shader con iluminacion solo color
Shader shaderColorLighting;
//Shader con iluminacion y textura
Shader shaderTextureLighting;
// Descomentar
//Shader con materiales
Shader shaderMaterialLighting;
// Descomentar
//Shader con skybox
Shader shaderSkybox;
// Shader con multiples luces
Shader shaderMulLighting;

std::shared_ptr<FirstPersonCamera> camera(new FirstPersonCamera());

Sphere sphere1(20, 20);
Sphere sphere2(20, 20);
Sphere sphere3(20, 20);
Sphere sphereLamp(20, 20);
// Descomentar
Sphere skyboxSphere(20, 20);
Cylinder cylinder1(20, 20, 0.5, 0.5);
Cylinder cylinder2(20, 20, 0.5, 0.5);
Cylinder cylinderMaterials(20, 20, 0.5, 0.5);
Box boxMaterials;
Box box1;
Box box2;
Box box3;
Box boxCesped;
Box boxWall;
// Models complex instances
Model modelPuerta1, modelPuerta2;

GLuint IDtextureCarretera, IDtextuLuna, IDtextuVenRot, IDtextuParedTerror, IDtextuCesped, IDtextuCera, IDtextuReja;
GLuint IDtextuMantel, IDtextuCadePapel;
GLuint skyboxTextureID;

GLenum types[6] = {
GL_TEXTURE_CUBE_MAP_POSITIVE_X,
GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
GL_TEXTURE_CUBE_MAP_NEGATIVE_Z };

std::string fileNames[6] = { "../Textures/ame_nebula/purplenebula_bk.tga",
		"../Textures/ame_nebula/purplenebula_bk.tga",
		"../Textures/ame_nebula/purplenebula_up.tga",
		"../Textures/ame_nebula/purplenebula_dn.tga",
		"../Textures/ame_nebula/purplenebula_rt.tga",
		"../Textures/ame_nebula/purplenebula_lf.tga" };

bool exitApp = false;
int lastMousePosX, offsetX = 0;
int lastMousePosY, offsetY = 0;

float rot0 = 0.0;

float rot1 = 0.0, rot2 = 0.0, rot3 = 0.0, rot4 = 0.0;
bool sentido = true;

double deltaTime;

// Se definen todos las funciones.
void reshapeCallback(GLFWwindow *Window, int widthRes, int heightRes);
void keyCallback(GLFWwindow *window, int key, int scancode, int action,
	int mode);
void mouseCallback(GLFWwindow *window, double xpos, double ypos);
void mouseButtonCallback(GLFWwindow *window, int button, int state, int mod);
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
	shaderTexture.initialize("../Shaders/texturizado_res.vs",
		"../Shaders/texturizado_res.fs");
	shaderColorLighting.initialize("../Shaders/iluminacion_color_res.vs",
		"../Shaders/iluminacion_color_res.fs");
	shaderTextureLighting.initialize("../Shaders/iluminacion_texture_res.vs",
		"../Shaders/iluminacion_texture_res.fs");
	shaderMaterialLighting.initialize("../Shaders/iluminacion_material.vs",
		"../Shaders/iluminacion_material_res.fs");
	shaderSkybox.initialize("../Shaders/cubeTexture.vs",
		"../Shaders/cubeTexture.fs");
	shaderMulLighting.initialize("../Shaders/iluminacion_texture_res.vs",
		"../Shaders/multipleLights.fs");

	// Inicializar los buffers VAO, VBO, EBO
	sphere1.init();
	// Método setter que colocar el apuntador al shader
	sphere1.setShader(&shaderColorLighting);
	//Setter para poner el color de la geometria
	sphere1.setColor(glm::vec4(1.0, 1.0, 0.0, 1.0));

	// Inicializar los buffers VAO, VBO, EBO
	sphere2.init();
	// Método setter que colocar el apuntador al shader
	sphere2.setShader(&shaderColorLighting);
	//Setter para poner el color de la geometria
	sphere2.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));

	// Inicializar los buffers VAO, VBO, EBO
	sphereLamp.init();
	// Método setter que colocar el apuntador al shader
	sphereLamp.setShader(&shaderTexture);
	//Setter para poner el color de la geometria
	sphereLamp.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));

	cylinder1.init();
	cylinder1.setShader(&shaderMulLighting);
	//cylinder1.setColor(glm::vec4(0.3, 0.3, 1.0, 1.0));

	cylinder2.init();
	cylinder2.setShader(&shaderMulLighting);

	cylinderMaterials.init();
	cylinderMaterials.setShader(&shaderMaterialLighting);
	cylinderMaterials.setColor(glm::vec4(0.3, 0.3, 1.0, 1.0));

	boxMaterials.init();
	boxMaterials.setShader(&shaderMaterialLighting);

	// Inicializacion de la esfera del skybox
	skyboxSphere.init();
	skyboxSphere.setShader(&shaderSkybox);
	skyboxSphere.setScale(glm::vec3(20.0f, 20.0f, 20.0f));

	box1.init();
	// Settea el shader a utilizar
	box1.setShader(&shaderMulLighting);
	box1.setColor(glm::vec4(1.0, 1.0, 0.0, 0.2));

	box2.init();
	box2.setShader(&shaderMulLighting);
	box2.setColor(glm::vec4(1.0, 1.0, 0.0, 0.1));


	sphere3.init();
	sphere3.setShader(&shaderMulLighting);

	box3.init();
	box3.setShader(&shaderMulLighting);
	
	modelPuerta1.loadModel("../models/Door/Puerta1.obj");
	modelPuerta1.setShader(&shaderMulLighting);

	modelPuerta2.loadModel("../models/Door/Puerta2.obj");
	modelPuerta2.setShader(&shaderMulLighting);

	camera->setPosition(glm::vec3(0.0, 0.01, 0.0));

	int imageWidth, imageHeight;

	/**************************************************************************************************************
				TEXTURA - CARRETERA
	***************************************************************************************************************/
	Texture texture1("../Textures/Carretera.jpg");
	FIBITMAP *bitmap = texture1.loadImage(TRUE);
	unsigned char *data = texture1.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &IDtextureCarretera);
	glBindTexture(GL_TEXTURE_2D, IDtextureCarretera);
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
	texture1.freeImage(bitmap);


	/**************************************************************************************************************
				TEXTURA - LUNA
	***************************************************************************************************************/
	Texture textuLuna("../Textures/Luna.jpg");
	bitmap = textuLuna.loadImage(false);
	data = textuLuna.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &IDtextuLuna);
	glBindTexture(GL_TEXTURE_2D, IDtextuLuna);
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
	textuLuna.freeImage(bitmap);


	/**************************************************************************************************************
				TEXTURA - PARED - CASA - TERROR
	***************************************************************************************************************/
	Texture textuPTerror("../Textures/ParedTerror.jpg");
	bitmap = textuPTerror.loadImage(false);
	data = textuPTerror.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &IDtextuParedTerror);
	glBindTexture(GL_TEXTURE_2D, IDtextuParedTerror);
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
	textuPTerror.freeImage(bitmap);
	//--------------------------

	/**************************************************************************************************************
				TEXTURA - TENDEDERO
	***************************************************************************************************************/
	Texture textuCadePapel("../Textures/CadenaPapel.png");
	bitmap = textuCadePapel.loadImage(false);
	data = textuCadePapel.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &IDtextuCadePapel);
	glBindTexture(GL_TEXTURE_2D, IDtextuCadePapel);
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
	textuCadePapel.freeImage(bitmap);

	/**************************************************************************************************************
				TEXTURA - LUNA
	***************************************************************************************************************/
	Texture textuCesped("../Textures/Cesped2.jpg");
	bitmap = textuCesped.loadImage(false);
	data = textuCesped.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &IDtextuCesped);
	glBindTexture(GL_TEXTURE_2D, IDtextuCesped);
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
	textuCesped.freeImage(bitmap);

	/**************************************************************************************************************
				TEXTURA - TERROR - VENTANAS
	***************************************************************************************************************/
	Texture textuVenRot("../Textures/VentanaRota.jpg");
	bitmap = textuVenRot.loadImage(false);
	data = textuVenRot.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &IDtextuVenRot);
	glBindTexture(GL_TEXTURE_2D, IDtextuVenRot);
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
	textuVenRot.freeImage(bitmap);

	/**************************************************************************************************************
				TEXTURA - CERA
	***************************************************************************************************************/
	Texture textuCera("../Textures/Cera.jpg");
	bitmap = textuCera.loadImage(false);
	data = textuCera.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &IDtextuCera);
	glBindTexture(GL_TEXTURE_2D, IDtextuCera);
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
	textuCera.freeImage(bitmap);

	/**************************************************************************************************************
				TEXTURA - CERCA
	***************************************************************************************************************/
	Texture textuReja("../Textures/malla1.png");
	bitmap = textuReja.loadImage(false);
	data = textuReja.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &IDtextuReja);
	glBindTexture(GL_TEXTURE_2D, IDtextuReja);
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
	textuReja.freeImage(bitmap);

	/**************************************************************************************************************
				TEXTURA - MANTEL
	***************************************************************************************************************/
	Texture textuMantel("../Textures/Mantel.jpg");
	bitmap = textuMantel.loadImage(false);
	data = textuMantel.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &IDtextuMantel);
	glBindTexture(GL_TEXTURE_2D, IDtextuMantel);
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
	textuMantel.freeImage(bitmap);


	// Carga de texturas para el skybox
	Texture skyboxTexture = Texture("");
	glGenTextures(1, &skyboxTextureID);
	// Tipo de textura CUBE MAP
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTextureID);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	for (int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(types); i++) {
		skyboxTexture = Texture(fileNames[i]);
		FIBITMAP *bitmap = skyboxTexture.loadImage(true);
		unsigned char *data = skyboxTexture.convertToData(bitmap, imageWidth,
			imageHeight);
		if (data) {
			glTexImage2D(types[i], 0, GL_RGBA, imageWidth, imageHeight, 0,
				GL_BGRA, GL_UNSIGNED_BYTE, data);
		}
		else
			std::cout << "Failed to load texture" << std::endl;
		skyboxTexture.freeImage(bitmap);
	}

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

void reshapeCallback(GLFWwindow *Window, int widthRes, int heightRes) {
	screenWidth = widthRes;
	screenHeight = heightRes;
	glViewport(0, 0, widthRes, heightRes);
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action,
	int mode) {
	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_ESCAPE:
			exitApp = true;
			break;
		}
	}
}

void mouseCallback(GLFWwindow *window, double xpos, double ypos) {
	offsetX = xpos - lastMousePosX;
	offsetY = ypos - lastMousePosY;
	lastMousePosX = xpos;
	lastMousePosY = ypos;
}

void mouseButtonCallback(GLFWwindow *window, int button, int state, int mod) {
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

bool processInput(bool continueApplication) {
	if (exitApp || glfwWindowShouldClose(window) != 0) {
		return false;
	}

	TimeManager::Instance().CalculateFrameRate(false);
	deltaTime = TimeManager::Instance().DeltaTime;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera->moveFrontCamera(true, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera->moveFrontCamera(false, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera->moveRightCamera(false, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera->moveRightCamera(true, deltaTime);
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		camera->mouseMoveCamera(offsetX, offsetY, 0.01);
	offsetX = 0;
	offsetY = 0;

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS &&
		glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		rot1 += 0.001;
	else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE &&
		glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		rot1 -= 0.001;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS &&
		glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		rot2 += 0.001;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE &&
		glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		rot2 -= 0.001;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS &&
		glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
		rot3 += 0.001;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE &&
		glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
		rot3 -= 0.001;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS &&
		glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
		rot4 += 0.001;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE &&
		glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
		rot4 -= 0.001;

	sentido = true;

	glfwPollEvents();
	return continueApplication;
}

void applicationLoop() {
	bool psi = true;

	float offX = 0.0;
	float angle = 0.0;
	float ratio = 100.0;

	bool Pos0 = false;


	while (psi) {
		psi = processInput(true);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 projection = glm::perspective(glm::radians(45.0f),
			(float)screenWidth / (float)screenHeight, 0.01f, 100.0f);
		glm::mat4 view = camera->getViewMatrix();

		// Settea la matriz de vista y projection al shader con solo color
		shader.setMatrix4("projection", 1, false, glm::value_ptr(projection));
		shader.setMatrix4("view", 1, false, glm::value_ptr(view));
		// Settea la matriz de vista y projection al shader con solo textura
		shaderTexture.setMatrix4("projection", 1, false,
			glm::value_ptr(projection));
		shaderTexture.setMatrix4("view", 1, false, glm::value_ptr(view));

		// Settea la matriz de vista y projection al shader con iluminacion solo color
		shaderColorLighting.setMatrix4("projection", 1, false,
			glm::value_ptr(projection));
		shaderColorLighting.setMatrix4("view", 1, false, glm::value_ptr(view));

		// Settea la matriz de vista y projection al shader con iluminacion con textura
		shaderTextureLighting.setMatrix4("projection", 1, false,
			glm::value_ptr(projection));
		shaderTextureLighting.setMatrix4("view", 1, false,
			glm::value_ptr(view));

		// Settea la matriz de vista y projection al shader con iluminacion con material
		shaderMaterialLighting.setMatrix4("projection", 1, false,
			glm::value_ptr(projection));
		shaderMaterialLighting.setMatrix4("view", 1, false,
			glm::value_ptr(view));

		// Settea la matriz de vista y projection al shader con skybox
		shaderSkybox.setMatrix4("projection", 1, false,
			glm::value_ptr(projection));
		shaderSkybox.setMatrix4("view", 1, false,
			glm::value_ptr(glm::mat4(glm::mat3(view))));
		// Settea la matriz de vista y projection al shader con multiples luces
		shaderMulLighting.setMatrix4("projection", 1, false,
			glm::value_ptr(projection));
		shaderMulLighting.setMatrix4("view", 1, false,
			glm::value_ptr(view));

		// Propiedades de la luz para objetos con color
		shaderColorLighting.setVectorFloat3("viewPos",
			glm::value_ptr(camera->getPosition()));
		shaderColorLighting.setVectorFloat3("light.ambient",
			glm::value_ptr(glm::vec3(0.3, 0.3, 0.3)));
		shaderColorLighting.setVectorFloat3("light.diffuse",
			glm::value_ptr(glm::vec3(0.3, 0.3, 0.3)));
		shaderColorLighting.setVectorFloat3("light.specular",
			glm::value_ptr(glm::vec3(0.9, 0.0, 0.0)));

		// Propiedades de la luz para objetos con textura
		shaderTextureLighting.setVectorFloat3("viewPos",
			glm::value_ptr(camera->getPosition()));
		shaderTextureLighting.setVectorFloat3("light.ambient",
			glm::value_ptr(glm::vec3(0.3, 0.3, 0.3)));
		shaderTextureLighting.setVectorFloat3("light.diffuse",
			glm::value_ptr(glm::vec3(0.3, 0.3, 0.3)));
		shaderTextureLighting.setVectorFloat3("light.specular",
			glm::value_ptr(glm::vec3(0.9, 0.0, 0.0)));

		// Propiedades de la luz para objetos con textura
		shaderMaterialLighting.setVectorFloat3("viewPos", glm::value_ptr(camera->getPosition()));
		shaderMaterialLighting.setVectorFloat3("light.ambient", glm::value_ptr(glm::vec3(0.3, 0.3, 0.3)));
		shaderMaterialLighting.setVectorFloat3("light.diffuse", glm::value_ptr(glm::vec3(0.5, 0.5, 0.5)));
		shaderMaterialLighting.setVectorFloat3("light.specular", glm::value_ptr(glm::vec3(0.9, 0.9, 0.9)));

		// Propiedades de la luz para objetos con multiples luces
		shaderMulLighting.setVectorFloat3("viewPos", glm::value_ptr(camera->getPosition()));
		shaderMulLighting.setVectorFloat3("directionalLight.light.ambient", glm::value_ptr(glm::vec3(0.3, 0.3, 0.3)));
		shaderMulLighting.setVectorFloat3("directionalLight.light.diffuse", glm::value_ptr(glm::vec3(0.4, 0.4, 0.4)));
		shaderMulLighting.setVectorFloat3("directionalLight.light.specular", glm::value_ptr(glm::vec3(0.7, 0.7, 0.7)));
		shaderMulLighting.setVectorFloat3("directionalLight.direction", glm::value_ptr(glm::vec3(-1.0, 0.0, 0.0)));

		// Esto es para la luz spotlight
		//shaderMulLighting.setInt("spotLightCount", 1);
		//shaderMulLighting.setVectorFloat3("spotLights[0].position", glm::value_ptr(camera->getPosition()));
		//shaderMulLighting.setVectorFloat3("spotLights[0].direction", glm::value_ptr(camera->getFront()));
		//Propiedades de la luz verde
		//shaderMulLighting.setVectorFloat3("spotLights[0].light.ambient", glm::value_ptr(glm::vec3(0.01, 0.01, 0.01)));
		//shaderMulLighting.setVectorFloat3("spotLights[0].light.diffuse", glm::value_ptr(glm::vec3(0.4, 0.4, 0.4)));
		//shaderMulLighting.setVectorFloat3("spotLights[0].light.specular", glm::value_ptr(glm::vec3(0.6, 0.6, 0.6)));
		//shaderMulLighting.setFloat("spotLights[0].cutOff", cos(glm::radians(12.5)));
		//shaderMulLighting.setFloat("spotLights[0].outerCutOff", cos(glm::radians(15.0)));
		//shaderMulLighting.setFloat("spotLights[0].constant", 1.0); //
		//shaderMulLighting.setFloat("spotLights[0].linear", 0.1);
		//shaderMulLighting.setFloat("spotLights[0].quadratic", 0.001);
		
		// Esto es para la luces pointlights
		//numero de luces a utilizar de tipo pointlihts = 3
		shaderMulLighting.setInt("pointLightCount",20); //agregar numero de luces sise ponen 5 tambien ir a multipleShader.fs en const int MAX_POINT_LIGHTS = 5;

		// posicion de la luz con indice 0
		shaderMulLighting.setVectorFloat3("pointLights[0].position", glm::value_ptr((glm::vec3(-0.25, 0.0045, -0.075)))); // debe ser igual a sphereLamp.setPosition(glm::vec3(-5.1, 4.5, -3.5));
		//propiedades de la luz 0
		shaderMulLighting.setVectorFloat3("pointLights[0].light.ambient", glm::value_ptr(glm::vec3(0.001, 0.001, 0.001)));
		shaderMulLighting.setVectorFloat3("pointLights[0].light.diffuse", glm::value_ptr(glm::vec3(0.01, 0.01, 0.0)));
		shaderMulLighting.setVectorFloat3("pointLights[0].light.specular", glm::value_ptr(glm::vec3(0.1, 0.1, 0.0)));
		shaderMulLighting.setFloat("pointLights[0].constant", 2.0);
		shaderMulLighting.setFloat("pointLights[0].linear", 0.01);
		shaderMulLighting.setFloat("pointLights[0].quadratic", 0.001);

		shaderMulLighting.setVectorFloat3("pointLights[1].position", glm::value_ptr((glm::vec3(-0.25, 0.0045, 0.075)))); // debe ser igual a sphereLamp.setPosition(glm::vec3(-5.1, 4.5, -3.5));
		shaderMulLighting.setVectorFloat3("pointLights[1].light.ambient", glm::value_ptr(glm::vec3(0.001, 0.001, 0.001)));
		shaderMulLighting.setVectorFloat3("pointLights[1].light.diffuse", glm::value_ptr(glm::vec3(0.01, 0.01, 0.0)));
		shaderMulLighting.setVectorFloat3("pointLights[1].light.specular", glm::value_ptr(glm::vec3(0.1, 0.1, 0.0)));
		shaderMulLighting.setFloat("pointLights[1].constant", 2.0);
		shaderMulLighting.setFloat("pointLights[1].linear", 0.01);
		shaderMulLighting.setFloat("pointLights[1].quadratic", 0.001);

		shaderMulLighting.setVectorFloat3("pointLights[2].position", glm::value_ptr((glm::vec3(-0.35, 0.0045, -0.075)))); // debe ser igual a sphereLamp.setPosition(glm::vec3(-5.1, 4.5, -3.5));
		shaderMulLighting.setVectorFloat3("pointLights[2].light.ambient", glm::value_ptr(glm::vec3(0.001, 0.001, 0.001)));
		shaderMulLighting.setVectorFloat3("pointLights[2].light.diffuse", glm::value_ptr(glm::vec3(0.01, 0.01, 0.0)));
		shaderMulLighting.setVectorFloat3("pointLights[2].light.specular", glm::value_ptr(glm::vec3(0.1, 0.1, 0.0)));
		shaderMulLighting.setFloat("pointLights[2].constant", 2.0);
		shaderMulLighting.setFloat("pointLights[2].linear", 0.01);
		shaderMulLighting.setFloat("pointLights[2].quadratic", 0.001);

		shaderMulLighting.setVectorFloat3("pointLights[3].position", glm::value_ptr((glm::vec3(-0.35, 0.0045, 0.075)))); // debe ser igual a sphereLamp.setPosition(glm::vec3(-5.1, 4.5, -3.5));
		shaderMulLighting.setVectorFloat3("pointLights[3].light.ambient", glm::value_ptr(glm::vec3(0.001, 0.001, 0.001)));
		shaderMulLighting.setVectorFloat3("pointLights[3].light.diffuse", glm::value_ptr(glm::vec3(0.01, 0.01, 0.0)));
		shaderMulLighting.setVectorFloat3("pointLights[3].light.specular", glm::value_ptr(glm::vec3(0.1, 0.1, 0.0)));
		shaderMulLighting.setFloat("pointLights[3].constant", 2.0);
		shaderMulLighting.setFloat("pointLights[3].linear", 0.01);
		shaderMulLighting.setFloat("pointLights[3].quadratic", 0.001);

		shaderMulLighting.setVectorFloat3("pointLights[4].position", glm::value_ptr((glm::vec3(-0.45, 0.0015, -0.075)))); // debe ser igual a sphereLamp.setPosition(glm::vec3(-5.1, 4.5, -3.5));
		shaderMulLighting.setVectorFloat3("pointLights[4].light.ambient", glm::value_ptr(glm::vec3(0.001, 0.001, 0.001)));
		shaderMulLighting.setVectorFloat3("pointLights[4].light.diffuse", glm::value_ptr(glm::vec3(0.01, 0.01, 0.0)));
		shaderMulLighting.setVectorFloat3("pointLights[4].light.specular", glm::value_ptr(glm::vec3(0.1, 0.1, 0.0)));
		shaderMulLighting.setFloat("pointLights[4].constant", 2.0);
		shaderMulLighting.setFloat("pointLights[4].linear", 0.01);
		shaderMulLighting.setFloat("pointLights[4].quadratic", 0.001);

		shaderMulLighting.setVectorFloat3("pointLights[5].position", glm::value_ptr((glm::vec3(-0.45, 0.0045, 0.075)))); // debe ser igual a sphereLamp.setPosition(glm::vec3(-5.1, 4.5, -3.5));
		shaderMulLighting.setVectorFloat3("pointLights[5].light.ambient", glm::value_ptr(glm::vec3(0.001, 0.001, 0.001)));
		shaderMulLighting.setVectorFloat3("pointLights[5].light.diffuse", glm::value_ptr(glm::vec3(0.01, 0.01, 0.0)));
		shaderMulLighting.setVectorFloat3("pointLights[5].light.specular", glm::value_ptr(glm::vec3(0.1, 0.1, 0.0)));
		shaderMulLighting.setFloat("pointLights[5].constant", 2.0);
		shaderMulLighting.setFloat("pointLights[5].linear", 0.01);
		shaderMulLighting.setFloat("pointLights[5].quadratic", 0.001);


		shaderMulLighting.setVectorFloat3("pointLights[6].position", glm::value_ptr((glm::vec3(-0.55, 0.0045, -0.075)))); // debe ser igual a sphereLamp.setPosition(glm::vec3(-5.1, 4.5, -3.5));
		shaderMulLighting.setVectorFloat3("pointLights[6].light.ambient", glm::value_ptr(glm::vec3(0.001, 0.001, 0.001)));
		shaderMulLighting.setVectorFloat3("pointLights[6].light.diffuse", glm::value_ptr(glm::vec3(0.01, 0.01, 0.0)));
		shaderMulLighting.setVectorFloat3("pointLights[6].light.specular", glm::value_ptr(glm::vec3(0.1, 0.1, 0.0)));
		shaderMulLighting.setFloat("pointLights[6].constant", 2.0);
		shaderMulLighting.setFloat("pointLights[6].linear", 0.01);
		shaderMulLighting.setFloat("pointLights[6].quadratic", 0.001);

		shaderMulLighting.setVectorFloat3("pointLights[7].position", glm::value_ptr((glm::vec3(-0.55, 0.0045, 0.075)))); // debe ser igual a sphereLamp.setPosition(glm::vec3(-5.1, 4.5, -3.5));
		shaderMulLighting.setVectorFloat3("pointLights[7].light.ambient", glm::value_ptr(glm::vec3(0.001, 0.001, 0.001)));
		shaderMulLighting.setVectorFloat3("pointLights[7].light.diffuse", glm::value_ptr(glm::vec3(0.01, 0.01, 0.0)));
		shaderMulLighting.setVectorFloat3("pointLights[7].light.specular", glm::value_ptr(glm::vec3(0.1, 0.1, 0.0)));
		shaderMulLighting.setFloat("pointLights[7].constant", 2.0);
		shaderMulLighting.setFloat("pointLights[7].linear", 0.01);
		shaderMulLighting.setFloat("pointLights[7].quadratic", 0.001);
	
		shaderMulLighting.setVectorFloat3("pointLights[8].position", glm::value_ptr((glm::vec3(-0.65, 0.0015, -0.075)))); // debe ser igual a sphereLamp.setPosition(glm::vec3(-5.1, 4.5, -3.5));
		shaderMulLighting.setVectorFloat3("pointLights[8].light.ambient", glm::value_ptr(glm::vec3(0.001, 0.001, 0.001)));
		shaderMulLighting.setVectorFloat3("pointLights[8].light.diffuse", glm::value_ptr(glm::vec3(0.01, 0.01, 0.0)));
		shaderMulLighting.setVectorFloat3("pointLights[8].light.specular", glm::value_ptr(glm::vec3(0.1, 0.1, 0.0)));
		shaderMulLighting.setFloat("pointLights[8].constant", 2.0);
		shaderMulLighting.setFloat("pointLights[8].linear", 0.01);
		shaderMulLighting.setFloat("pointLights[8].quadratic", 0.001);

		shaderMulLighting.setVectorFloat3("pointLights[9].position", glm::value_ptr((glm::vec3(-0.65, 0.0045, 0.075)))); // debe ser igual a sphereLamp.setPosition(glm::vec3(-5.1, 4.5, -3.5));
		shaderMulLighting.setVectorFloat3("pointLights[9].light.ambient", glm::value_ptr(glm::vec3(0.001, 0.001, 0.001)));
		shaderMulLighting.setVectorFloat3("pointLights[9].light.diffuse", glm::value_ptr(glm::vec3(0.01, 0.01, 0.0)));
		shaderMulLighting.setVectorFloat3("pointLights[9].light.specular", glm::value_ptr(glm::vec3(0.1, 0.1, 0.0)));
		shaderMulLighting.setFloat("pointLights[9].constant", 2.0);
		shaderMulLighting.setFloat("pointLights[9].linear", 0.01);
		shaderMulLighting.setFloat("pointLights[9].quadratic", 0.001);

		shaderMulLighting.setVectorFloat3("pointLights[10].position", glm::value_ptr((glm::vec3(-0.75, 0.0045, -0.075)))); // debe ser igual a sphereLamp.setPosition(glm::vec3(-5.1, 4.5, -3.5));
		shaderMulLighting.setVectorFloat3("pointLights[10].light.ambient", glm::value_ptr(glm::vec3(0.001, 0.001, 0.001)));
		shaderMulLighting.setVectorFloat3("pointLights[10].light.diffuse", glm::value_ptr(glm::vec3(0.01, 0.01, 0.0)));
		shaderMulLighting.setVectorFloat3("pointLights[10].light.specular", glm::value_ptr(glm::vec3(0.1, 0.1, 0.0)));
		shaderMulLighting.setFloat("pointLights[10].constant", 2.0);
		shaderMulLighting.setFloat("pointLights[10].linear", 0.01);
		shaderMulLighting.setFloat("pointLights[10].quadratic", 0.001);

		shaderMulLighting.setVectorFloat3("pointLights[11].position", glm::value_ptr((glm::vec3(-0.75, 0.0045, 0.075)))); // debe ser igual a sphereLamp.setPosition(glm::vec3(-5.1, 4.5, -3.5));
		shaderMulLighting.setVectorFloat3("pointLights[11].light.ambient", glm::value_ptr(glm::vec3(0.001, 0.001, 0.001)));
		shaderMulLighting.setVectorFloat3("pointLights[11].light.diffuse", glm::value_ptr(glm::vec3(0.01, 0.01, 0.0)));
		shaderMulLighting.setVectorFloat3("pointLights[11].light.specular", glm::value_ptr(glm::vec3(0.1, 0.1, 0.0)));
		shaderMulLighting.setFloat("pointLights[11].constant", 2.0);
		shaderMulLighting.setFloat("pointLights[11].linear", 0.01);
		shaderMulLighting.setFloat("pointLights[11].quadratic", 0.001);

		shaderMulLighting.setVectorFloat3("pointLights[12].position", glm::value_ptr((glm::vec3(-0.85, 0.0045, -0.075)))); // debe ser igual a sphereLamp.setPosition(glm::vec3(-5.1, 4.5, -3.5));
		shaderMulLighting.setVectorFloat3("pointLights[12].light.ambient", glm::value_ptr(glm::vec3(0.001, 0.001, 0.001)));
		shaderMulLighting.setVectorFloat3("pointLights[12].light.diffuse", glm::value_ptr(glm::vec3(0.01, 0.01, 0.0)));
		shaderMulLighting.setVectorFloat3("pointLights[12].light.specular", glm::value_ptr(glm::vec3(0.1, 0.1, 0.0)));
		shaderMulLighting.setFloat("pointLights[12].constant", 2.0);
		shaderMulLighting.setFloat("pointLights[12].linear", 0.01);
		shaderMulLighting.setFloat("pointLights[12].quadratic", 0.001);

		shaderMulLighting.setVectorFloat3("pointLights[13].position", glm::value_ptr((glm::vec3(-0.85, 0.0045, 0.075)))); // debe ser igual a sphereLamp.setPosition(glm::vec3(-5.1, 4.5, -3.5));
		shaderMulLighting.setVectorFloat3("pointLights[13].light.ambient", glm::value_ptr(glm::vec3(0.001, 0.001, 0.001)));
		shaderMulLighting.setVectorFloat3("pointLights[13].light.diffuse", glm::value_ptr(glm::vec3(0.01, 0.01, 0.0)));
		shaderMulLighting.setVectorFloat3("pointLights[13].light.specular", glm::value_ptr(glm::vec3(0.1, 0.1, 0.0)));
		shaderMulLighting.setFloat("pointLights[13].constant", 2.0);
		shaderMulLighting.setFloat("pointLights[13].linear", 0.01);
		shaderMulLighting.setFloat("pointLights[13].quadratic", 0.001);

		shaderMulLighting.setVectorFloat3("pointLights[14].position", glm::value_ptr((glm::vec3(-0.95, 0.0045, -0.075)))); // debe ser igual a sphereLamp.setPosition(glm::vec3(-5.1, 4.5, -3.5));
		shaderMulLighting.setVectorFloat3("pointLights[14].light.ambient", glm::value_ptr(glm::vec3(0.001, 0.001, 0.001)));
		shaderMulLighting.setVectorFloat3("pointLights[14].light.diffuse", glm::value_ptr(glm::vec3(0.01, 0.01, 0.0)));
		shaderMulLighting.setVectorFloat3("pointLights[14].light.specular", glm::value_ptr(glm::vec3(0.1, 0.1, 0.0)));
		shaderMulLighting.setFloat("pointLights[14].constant", 2.0);
		shaderMulLighting.setFloat("pointLights[14].linear", 0.01);
		shaderMulLighting.setFloat("pointLights[14].quadratic", 0.001);

		shaderMulLighting.setVectorFloat3("pointLights[15].position", glm::value_ptr((glm::vec3(-0.95, 0.0045, 0.075)))); // debe ser igual a sphereLamp.setPosition(glm::vec3(-5.1, 4.5, -3.5));
		shaderMulLighting.setVectorFloat3("pointLights[15].light.ambient", glm::value_ptr(glm::vec3(0.001, 0.001, 0.001)));
		shaderMulLighting.setVectorFloat3("pointLights[15].light.diffuse", glm::value_ptr(glm::vec3(0.01, 0.01, 0.0)));
		shaderMulLighting.setVectorFloat3("pointLights[15].light.specular", glm::value_ptr(glm::vec3(0.1, 0.1, 0.0)));
		shaderMulLighting.setFloat("pointLights[15].constant", 2.0);
		shaderMulLighting.setFloat("pointLights[15].linear", 0.01);
		shaderMulLighting.setFloat("pointLights[15].quadratic", 0.001);

		shaderMulLighting.setVectorFloat3("pointLights[16].position", glm::value_ptr((glm::vec3(-1.05, 0.0045, -0.075)))); // debe ser igual a sphereLamp.setPosition(glm::vec3(-5.1, 4.5, -3.5));
		shaderMulLighting.setVectorFloat3("pointLights[16].light.ambient", glm::value_ptr(glm::vec3(0.001, 0.001, 0.001)));
		shaderMulLighting.setVectorFloat3("pointLights[16].light.diffuse", glm::value_ptr(glm::vec3(0.01, 0.01, 0.0)));
		shaderMulLighting.setVectorFloat3("pointLights[16].light.specular", glm::value_ptr(glm::vec3(0.1, 0.1, 0.0)));
		shaderMulLighting.setFloat("pointLights[16].constant", 2.0);
		shaderMulLighting.setFloat("pointLights[16].linear", 0.01);
		shaderMulLighting.setFloat("pointLights[16].quadratic", 0.001);

		shaderMulLighting.setVectorFloat3("pointLights[17].position", glm::value_ptr((glm::vec3(-1.05, 0.0045, 0.075)))); // debe ser igual a sphereLamp.setPosition(glm::vec3(-5.1, 4.5, -3.5));
		shaderMulLighting.setVectorFloat3("pointLights[17].light.ambient", glm::value_ptr(glm::vec3(0.001, 0.001, 0.001)));
		shaderMulLighting.setVectorFloat3("pointLights[17].light.diffuse", glm::value_ptr(glm::vec3(0.01, 0.01, 0.0)));
		shaderMulLighting.setVectorFloat3("pointLights[17].light.specular", glm::value_ptr(glm::vec3(0.1, 0.1, 0.0)));
		shaderMulLighting.setFloat("pointLights[17].constant", 2.0);
		shaderMulLighting.setFloat("pointLights[17].linear", 0.01);
		shaderMulLighting.setFloat("pointLights[17].quadratic", 0.001);

		shaderMulLighting.setVectorFloat3("pointLights[18].position", glm::value_ptr((glm::vec3(-1.15, 0.0045, -0.075)))); // debe ser igual a sphereLamp.setPosition(glm::vec3(-5.1, 4.5, -3.5));
		shaderMulLighting.setVectorFloat3("pointLights[18].light.ambient", glm::value_ptr(glm::vec3(0.001, 0.001, 0.001)));
		shaderMulLighting.setVectorFloat3("pointLights[18].light.diffuse", glm::value_ptr(glm::vec3(0.01, 0.01, 0.0)));
		shaderMulLighting.setVectorFloat3("pointLights[18].light.specular", glm::value_ptr(glm::vec3(0.1, 0.1, 0.0)));
		shaderMulLighting.setFloat("pointLights[18].constant", 2.0);
		shaderMulLighting.setFloat("pointLights[18].linear", 0.05);
		shaderMulLighting.setFloat("pointLights[18].quadratic", 0.005);

		shaderMulLighting.setVectorFloat3("pointLights[19].position", glm::value_ptr((glm::vec3(-1.15, 0.0045, 0.075)))); // debe ser igual a sphereLamp.setPosition(glm::vec3(-5.1, 4.5, -3.5));
		shaderMulLighting.setVectorFloat3("pointLights[19].light.ambient", glm::value_ptr(glm::vec3(0.001, 0.001, 0.001)));
		shaderMulLighting.setVectorFloat3("pointLights[19].light.diffuse", glm::value_ptr(glm::vec3(0.01, 0.01, 0.0)));
		shaderMulLighting.setVectorFloat3("pointLights[19].light.specular", glm::value_ptr(glm::vec3(0.1, 0.1, 0.0)));
		shaderMulLighting.setFloat("pointLights[19].constant", 2.0);
		shaderMulLighting.setFloat("pointLights[19].linear", 0.05);
		shaderMulLighting.setFloat("pointLights[19].quadratic", 0.005);


		//ESTO ES PARA COLOCAR LAS ESFERAS DE LAS LUCES
		/*sphereLamp.setScale(glm::vec3(0.0007, 0.0015, 0.0007));
		sphereLamp.setPosition(glm::vec3(-0.25, 0.0045, -0.075));
		sphereLamp.setColor(glm::vec4(0.01, 0.01, 0.0, 1.0));
		sphereLamp.render();*/


		glm::mat4 lightModelmatrix = glm::rotate(glm::mat4(1.0f), angle,
			glm::vec3(1.0f, 0.0f, 0.0f));
		lightModelmatrix = glm::translate(lightModelmatrix,
			glm::vec3(0.0f, 0.0f, -ratio));
		// Posicion luz para objetos con color
		shaderColorLighting.setVectorFloat3("light.position",
			glm::value_ptr(
				glm::vec4(
					lightModelmatrix
					* glm::vec4(0.0, 0.0, 0.0, 1.0))));

		// Posicion luz para objetos con textura
		shaderTextureLighting.setVectorFloat3("light.position",
			glm::value_ptr(
				glm::vec4(
					lightModelmatrix
					* glm::vec4(0.0, 0.0, 0.0, 1.0))));

		// Posicion luz para objetos con materiales
		shaderMaterialLighting.setVectorFloat3("light.position",
			glm::value_ptr(
				glm::vec4(
					lightModelmatrix
					* glm::vec4(0.0, 0.0, 0.0, 1.0))));
		sphereLamp.setScale(glm::vec3(5, 5, 5));
		sphereLamp.setPosition(glm::vec3(0, 0, 0));
		sphereLamp.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));
		glBindTexture(GL_TEXTURE_2D, IDtextuLuna);
		sphereLamp.render(lightModelmatrix);
		glBindTexture(GL_TEXTURE_2D, 0);

		/*****************************************************************************************
			Carretera
		******************************************************************************************/
		glm::mat4 matCalle = glm::mat4(1.0);
		matCalle = glm::translate(matCalle, glm::vec3(0.0, 0.0, 0.0));
		matCalle = glm::scale(matCalle, glm::vec3(0.2, 0.0025, 2.0));
		glBindTexture(GL_TEXTURE_2D, IDtextureCarretera);
		shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(1.0, 1.0)));
		box1.render(matCalle);
		glBindTexture(GL_TEXTURE_2D, 0);

		/*****************************************************************************************
			CASA - TERROR
		******************************************************************************************/
		//PAREDES FACHADA
			glm::mat4 ParedTorreC1 = glm::mat4(1.0);
			ParedTorreC1 = glm::translate(ParedTorreC1, glm::vec3(-1.205, 0.3, 0.2)); //-1.1, 7.5, 0.35
			ParedTorreC1 = glm::scale(ParedTorreC1, glm::vec3(0.01, 0.6, 0.1)); //0.3, 0.75, 0.3
			glBindTexture(GL_TEXTURE_2D, IDtextuParedTerror);
			box1.render(ParedTorreC1);
			glBindTexture(GL_TEXTURE_2D, 0);

			glm::mat4 ParedTorreC2 = glm::mat4(1.0);
			ParedTorreC2 = glm::translate(ParedTorreC2, glm::vec3(-1.25, 0.3, 0.245)); //-1.1, 7.5, 0.35
			ParedTorreC2 = glm::scale(ParedTorreC2, glm::vec3(0.08, 0.6, 0.01)); //0.3, 0.75, 0.3
			glBindTexture(GL_TEXTURE_2D, IDtextuParedTerror);
			box1.render(ParedTorreC2);
			glBindTexture(GL_TEXTURE_2D, 0);

			glm::mat4 ParedTorreC3 = glm::mat4(1.0);
			ParedTorreC3 = glm::translate(ParedTorreC3, glm::vec3(-1.295, 0.3, 0.2)); //-1.1, 7.5, 0.35
			ParedTorreC3 = glm::scale(ParedTorreC3, glm::vec3(0.01, 0.6, 0.1)); //0.3, 0.75, 0.3
			glBindTexture(GL_TEXTURE_2D, IDtextuParedTerror);
			box1.render(ParedTorreC3);
			glBindTexture(GL_TEXTURE_2D, 0);

			glm::mat4 ParedTorreC4 = glm::mat4(1.0);
			ParedTorreC4 = glm::translate(ParedTorreC4, glm::vec3(-1.25, 0.3, 0.155)); //-1.1, 7.5, 0.35
			ParedTorreC4 = glm::scale(ParedTorreC4, glm::vec3(0.08, 0.6, 0.01)); //0.3, 0.75, 0.3
			glBindTexture(GL_TEXTURE_2D, IDtextuParedTerror);
			box1.render(ParedTorreC4);
			glBindTexture(GL_TEXTURE_2D, 0);

			glm::mat4 ParedCuboTorre1 = glm::mat4(1.0);
			ParedCuboTorre1 = glm::translate(ParedCuboTorre1, glm::vec3(-1.185,0.66,0.2));
			ParedCuboTorre1 = glm::scale(ParedCuboTorre1, glm::vec3(0.01,0.12,0.14));
			glBindTexture(GL_TEXTURE_2D, IDtextuParedTerror);
			box1.render(ParedCuboTorre1);
			glBindTexture(GL_TEXTURE_2D, 0);

			glm::mat4 ParedCuboTorre2 = glm::mat4(1.0);
			ParedCuboTorre2 = glm::translate(ParedCuboTorre2, glm::vec3(-1.25, 0.66, 0.135));
			ParedCuboTorre2 = glm::scale(ParedCuboTorre2, glm::vec3(0.12, 0.12, 0.01));
			glBindTexture(GL_TEXTURE_2D, IDtextuParedTerror);
			box1.render(ParedCuboTorre2);
			glBindTexture(GL_TEXTURE_2D, 0);

			glm::mat4 ParedCuboTorre3 = glm::mat4(1.0);
			ParedCuboTorre3 = glm::translate(ParedCuboTorre3, glm::vec3(-1.315, 0.66, 0.2));
			ParedCuboTorre3 = glm::scale(ParedCuboTorre3, glm::vec3(0.01, 0.12, 0.14));
			glBindTexture(GL_TEXTURE_2D, IDtextuParedTerror);
			box1.render(ParedCuboTorre3);
			glBindTexture(GL_TEXTURE_2D, 0);

			glm::mat4 ParedCuboTorre4 = glm::mat4(1.0);
			ParedCuboTorre4 = glm::translate(ParedCuboTorre4, glm::vec3(-1.25, 0.66, 0.265));
			ParedCuboTorre4 = glm::scale(ParedCuboTorre4, glm::vec3(0.12, 0.12, 0.01));
			glBindTexture(GL_TEXTURE_2D, IDtextuParedTerror);
			box1.render(ParedCuboTorre4);
			glBindTexture(GL_TEXTURE_2D, 0);

			glm::mat4 ParedCuboTorre5 = glm::mat4(1.0);
			ParedCuboTorre5 = glm::translate(ParedCuboTorre5, glm::vec3(-1.25, 0.715, 0.2));
			ParedCuboTorre5 = glm::scale(ParedCuboTorre5, glm::vec3(0.12, 0.01, 0.12));
			glBindTexture(GL_TEXTURE_2D, IDtextuParedTerror);
			box1.render(ParedCuboTorre5);
			glBindTexture(GL_TEXTURE_2D, 0);

			glm::mat4 ParedCuboTorre6 = glm::mat4(1.0);
			ParedCuboTorre6 = glm::translate(ParedCuboTorre6, glm::vec3(-1.25, 0.6, 0.145));
			ParedCuboTorre6 = glm::scale(ParedCuboTorre6, glm::vec3(0.12, 0.01, 0.01));
			glBindTexture(GL_TEXTURE_2D, IDtextuParedTerror);
			box1.render(ParedCuboTorre6);
			glBindTexture(GL_TEXTURE_2D, 0);
			
			glm::mat4 ParedCuboTorre7 = glm::mat4(1.0);
			ParedCuboTorre7 = glm::translate(ParedCuboTorre7, glm::vec3(-1.25, 0.6, 0.255));
			ParedCuboTorre7 = glm::scale(ParedCuboTorre7, glm::vec3(0.12, 0.01, 0.01));
			glBindTexture(GL_TEXTURE_2D, IDtextuParedTerror);
			box1.render(ParedCuboTorre7);
			glBindTexture(GL_TEXTURE_2D, 0);

			glm::mat4 ParedCuboTorre8 = glm::mat4(1.0);
			ParedCuboTorre8 = glm::translate(ParedCuboTorre8, glm::vec3(-1.195, 0.605, 0.2));
			ParedCuboTorre8 = glm::scale(ParedCuboTorre8, glm::vec3(0.01, 0.01, 0.12));
			glBindTexture(GL_TEXTURE_2D, IDtextuParedTerror);
			box1.render(ParedCuboTorre8);
			glBindTexture(GL_TEXTURE_2D, 0);

			glm::mat4 ParedCuboTorre9 = glm::mat4(1.0);
			ParedCuboTorre9 = glm::translate(ParedCuboTorre9, glm::vec3(-1.305, 0.605, 0.2));
			ParedCuboTorre9 = glm::scale(ParedCuboTorre9, glm::vec3(0.01, 0.01, 0.12));
			glBindTexture(GL_TEXTURE_2D, IDtextuParedTerror);
			box1.render(ParedCuboTorre9);
			glBindTexture(GL_TEXTURE_2D, 0);

		//PAREDES CASA GRANDE
		glm::mat4 ParedCasaT0 = glm::mat4(1.0);
		ParedCasaT0 = glm::translate(ParedCasaT0, glm::vec3(-1.255, 0.2, 0.115)); //-1.1, 7.5, 0.35
		ParedCasaT0 = glm::scale(ParedCasaT0, glm::vec3(0.01, 0.4, 0.07)); //0.3, 0.75, 0.3
		glBindTexture(GL_TEXTURE_2D, IDtextuParedTerror);
		box1.render(ParedCasaT0);
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 ParedCasaT1 = glm::mat4(1.0);
		ParedCasaT1 = glm::translate(ParedCasaT1, glm::vec3(-1.255, 0.25, 0.0)); //-1.1, 7.5, 0.35
		ParedCasaT1 = glm::scale(ParedCasaT1, glm::vec3(0.01, 0.3, 0.16)); //0.3, 0.75, 0.3
		glBindTexture(GL_TEXTURE_2D, IDtextuParedTerror);
		box1.render(ParedCasaT1);
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 ParedCasaT2 = glm::mat4(1.0);
		ParedCasaT2 = glm::translate(ParedCasaT2, glm::vec3(-1.255, 0.2, -0.115)); //-1.1, 7.5, 0.35
		ParedCasaT2 = glm::scale(ParedCasaT2, glm::vec3(0.01, 0.4, 0.07)); //0.3, 0.75, 0.3
		glBindTexture(GL_TEXTURE_2D, IDtextuParedTerror);
		box1.render(ParedCasaT2);
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 ParedCasaT4 = glm::mat4(1.0);
		ParedCasaT4 = glm::translate(ParedCasaT4, glm::vec3(-1.7, 0.05, -0.145)); //-1.1, 7.5, 0.35
		ParedCasaT4 = glm::scale(ParedCasaT4, glm::vec3(0.9, 0.1, 0.01)); //0.3, 0.75, 0.3
		glBindTexture(GL_TEXTURE_2D, IDtextuParedTerror);
		box1.render(ParedCasaT4);
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 ParedCasaT5 = glm::mat4(1.0);
		ParedCasaT5 = glm::translate(ParedCasaT5, glm::vec3(-1.7, 0.35, -0.145)); //-1.1, 7.5, 0.35
		ParedCasaT5 = glm::scale(ParedCasaT5, glm::vec3(0.9, 0.1, 0.01)); //0.3, 0.75, 0.3
		glBindTexture(GL_TEXTURE_2D, IDtextuParedTerror);
		box1.render(ParedCasaT5);
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 ParedCasaT6 = glm::mat4(1.0);
		ParedCasaT6 = glm::translate(ParedCasaT6, glm::vec3(-1.3, 0.2, -0.145)); //-1.1, 7.5, 0.35
		ParedCasaT6 = glm::scale(ParedCasaT6, glm::vec3(0.1, 0.2, 0.01)); //0.3, 0.75, 0.3
		glBindTexture(GL_TEXTURE_2D, IDtextuParedTerror);
		box1.render(ParedCasaT6);
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 ParedCasaT7 = glm::mat4(1.0);
		ParedCasaT7 = glm::translate(ParedCasaT7, glm::vec3(-1.5, 0.2, -0.145)); //-1.1, 7.5, 0.35
		ParedCasaT7 = glm::scale(ParedCasaT7, glm::vec3(0.1, 0.2, 0.01)); //0.3, 0.75, 0.3
		glBindTexture(GL_TEXTURE_2D, IDtextuParedTerror);
		box1.render(ParedCasaT7);
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 ParedCasaT8 = glm::mat4(1.0);
		ParedCasaT8 = glm::translate(ParedCasaT8, glm::vec3(-1.7, 0.2, -0.145)); //-1.1, 7.5, 0.35
		ParedCasaT8 = glm::scale(ParedCasaT8, glm::vec3(0.1, 0.2, 0.01)); //0.3, 0.75, 0.3
		glBindTexture(GL_TEXTURE_2D, IDtextuParedTerror);
		box1.render(ParedCasaT8);
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 ParedCasaT9 = glm::mat4(1.0);
		ParedCasaT9 = glm::translate(ParedCasaT9, glm::vec3(-1.9, 0.2, -0.145)); //-1.1, 7.5, 0.35
		ParedCasaT9 = glm::scale(ParedCasaT9, glm::vec3(0.1, 0.2, 0.01)); //0.3, 0.75, 0.3
		glBindTexture(GL_TEXTURE_2D, IDtextuParedTerror);
		box1.render(ParedCasaT9);
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 ParedCasaT10 = glm::mat4(1.0);
		ParedCasaT10 = glm::translate(ParedCasaT10, glm::vec3(-2.1, 0.2, -0.145)); //-1.1, 7.5, 0.35
		ParedCasaT10 = glm::scale(ParedCasaT10, glm::vec3(0.1, 0.2, 0.01)); //0.3, 0.75, 0.3
		glBindTexture(GL_TEXTURE_2D, IDtextuParedTerror);
		box1.render(ParedCasaT10);
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 ParedCasaT11 = glm::mat4(1.0);
		ParedCasaT11 = glm::translate(ParedCasaT11, glm::vec3(-2.155, 0.2, 0.0)); //-1.1, 7.5, 0.35
		ParedCasaT11 = glm::scale(ParedCasaT11, glm::vec3(0.01, 0.4, 0.3)); //0.3, 0.75, 0.3
		glBindTexture(GL_TEXTURE_2D, IDtextuParedTerror);
		box1.render(ParedCasaT11);
		glBindTexture(GL_TEXTURE_2D, 0);


		glm::mat4 ParedCasaTA4 = glm::mat4(1.0);
		ParedCasaTA4 = glm::translate(ParedCasaTA4, glm::vec3(-1.7, 0.05, 0.145)); //-1.1, 7.5, 0.35
		ParedCasaTA4 = glm::scale(ParedCasaTA4, glm::vec3(0.9, 0.1, 0.01)); //0.3, 0.75, 0.3
		glBindTexture(GL_TEXTURE_2D, IDtextuParedTerror);
		box1.render(ParedCasaTA4);
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 ParedCasaTA5 = glm::mat4(1.0);
		ParedCasaTA5 = glm::translate(ParedCasaTA5, glm::vec3(-1.7, 0.35, 0.145)); //-1.1, 7.5, 0.35
		ParedCasaTA5 = glm::scale(ParedCasaTA5, glm::vec3(0.9, 0.1, 0.01)); //0.3, 0.75, 0.3
		glBindTexture(GL_TEXTURE_2D, IDtextuParedTerror);
		box1.render(ParedCasaTA5);
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 ParedCasaTA6 = glm::mat4(1.0);
		ParedCasaTA6 = glm::translate(ParedCasaTA6, glm::vec3(-1.3, 0.2, 0.145)); //-1.1, 7.5, 0.35
		ParedCasaTA6 = glm::scale(ParedCasaTA6, glm::vec3(0.1, 0.2, 0.01)); //0.3, 0.75, 0.3
		glBindTexture(GL_TEXTURE_2D, IDtextuParedTerror);
		box1.render(ParedCasaTA6);
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 ParedCasaTA7 = glm::mat4(1.0);
		ParedCasaTA7 = glm::translate(ParedCasaTA7, glm::vec3(-1.5, 0.2, 0.145)); //-1.1, 7.5, 0.35
		ParedCasaTA7 = glm::scale(ParedCasaTA7, glm::vec3(0.1, 0.2, 0.01)); //0.3, 0.75, 0.3
		glBindTexture(GL_TEXTURE_2D, IDtextuParedTerror);
		box1.render(ParedCasaTA7);
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 ParedCasaTA8 = glm::mat4(1.0);
		ParedCasaTA8 = glm::translate(ParedCasaTA8, glm::vec3(-1.7, 0.2, 0.145)); //-1.1, 7.5, 0.35
		ParedCasaTA8 = glm::scale(ParedCasaTA8, glm::vec3(0.1, 0.2, 0.01)); //0.3, 0.75, 0.3
		glBindTexture(GL_TEXTURE_2D, IDtextuParedTerror);
		box1.render(ParedCasaTA8);
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 ParedCasaTA9 = glm::mat4(1.0);
		ParedCasaTA9 = glm::translate(ParedCasaTA9, glm::vec3(-1.9, 0.2, 0.145)); //-1.1, 7.5, 0.35
		ParedCasaTA9 = glm::scale(ParedCasaTA9, glm::vec3(0.1, 0.2, 0.01)); //0.3, 0.75, 0.3
		glBindTexture(GL_TEXTURE_2D, IDtextuParedTerror);
		box1.render(ParedCasaTA9);
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 ParedCasaTA10 = glm::mat4(1.0);
		ParedCasaTA10 = glm::translate(ParedCasaTA10, glm::vec3(-2.1, 0.2, 0.145)); //-1.1, 7.5, 0.35
		ParedCasaTA10 = glm::scale(ParedCasaTA10, glm::vec3(0.1, 0.2, 0.01)); //0.3, 0.75, 0.3
		glBindTexture(GL_TEXTURE_2D, IDtextuParedTerror);
		box1.render(ParedCasaTA10);
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 ParedCasaTA11 = glm::mat4(1.0);
		ParedCasaTA11 = glm::translate(ParedCasaTA11, glm::vec3(-1.7, 0.4025, 0.0));
		ParedCasaTA11 = glm::scale(ParedCasaTA11, glm::vec3(0.9, 0.005, 0.3));
		glBindTexture(GL_TEXTURE_2D, IDtextuParedTerror);
		box1.render(ParedCasaTA11);
		glBindTexture(GL_TEXTURE_2D, 0);

		//OFRENDA
			glm::mat4 matMesa = glm::mat4(1.0);
			matMesa = glm::translate(matMesa, glm::vec3(-2.075, 0.0225, 0.0)); //-2.75, 0.045, 0.0
			matMesa = glm::scale(matMesa, glm::vec3(0.15, 0.005, 0.2));
			glBindTexture(GL_TEXTURE_2D, IDtextuMantel);
			box1.render(matMesa);
			glBindTexture(GL_TEXTURE_2D, 0);

			glm::mat4 matPata0 = glm::mat4(1.0);
			matPata0 = glm::translate(matPata0, glm::vec3(-2.075, 0.01, 0.075));
			matPata0 = glm::scale(matPata0, glm::vec3(0.14, 0.02, 0.006));
			glBindTexture(GL_TEXTURE_2D, IDtextuMantel);
			box1.render(matPata0);
			glBindTexture(GL_TEXTURE_2D, 0);

			glm::mat4 matPata1 = glm::mat4(1.0);
			matPata1 = glm::translate(matPata1, glm::vec3(-2.075, 0.01, -0.075));
			matPata1 = glm::scale(matPata1, glm::vec3(0.14, 0.02, 0.006));
			glBindTexture(GL_TEXTURE_2D, IDtextuMantel);
			box1.render(matPata1);
			glBindTexture(GL_TEXTURE_2D, 0);

			glm::mat4 matPata2 = glm::mat4(1.0);
			matPata2 = glm::translate(matPata2, glm::vec3(-2.015, 0.01, 0.0));
			matPata2 = glm::scale(matPata2, glm::vec3(0.006, 0.02, 0.16));
			glBindTexture(GL_TEXTURE_2D, IDtextuMantel);
			box1.render(matPata2);
			glBindTexture(GL_TEXTURE_2D, 0);


		//TENDEDEROS PICADOS
			glm::mat4 Tendedero0 = glm::mat4(1.0);
			Tendedero0 = glm::translate(Tendedero0, glm::vec3(0.0, 0.1, 0.0));
			Tendedero0 = glm::scale(Tendedero0, glm::vec3(0.2, 0.2, 0.2));
			glBindTexture(GL_TEXTURE_2D, IDtextuCadePapel); 
			box1.render(Tendedero0);
			glBindTexture(GL_TEXTURE_2D, 0);

		//VENTANAS
		glm::mat4 matVenta1 = glm::mat4(1.0);
		matVenta1 = glm::translate(matVenta1, glm::vec3(-1.405, 0.2, -0.145));
		matVenta1 = glm::scale(matVenta1, glm::vec3(0.12, 0.2, 0.005));
		glBindTexture(GL_TEXTURE_2D, IDtextuVenRot);
		box1.render(matVenta1);
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 matVenta2 = glm::mat4(1.0);
		matVenta2 = glm::translate(matVenta2, glm::vec3(-1.6, 0.2, -0.145));
		matVenta2 = glm::scale(matVenta2, glm::vec3(0.12, 0.2, 0.005));
		glBindTexture(GL_TEXTURE_2D, IDtextuVenRot);
		box1.render(matVenta2);
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 matVenta3 = glm::mat4(1.0);
		matVenta3 = glm::translate(matVenta3, glm::vec3(-1.8, 0.2, -0.145));
		matVenta3 = glm::scale(matVenta3, glm::vec3(0.12, 0.2, 0.005));
		glBindTexture(GL_TEXTURE_2D, IDtextuVenRot);
		box1.render(matVenta3);
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 matVenta4 = glm::mat4(1.0);
		matVenta4 = glm::translate(matVenta4, glm::vec3(-2.0, 0.2, -0.145));
		matVenta4 = glm::scale(matVenta4, glm::vec3(0.12, 0.2, 0.005));
		glBindTexture(GL_TEXTURE_2D, IDtextuVenRot);
		box1.render(matVenta4);
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 matVenta5 = glm::mat4(1.0);
		matVenta5 = glm::translate(matVenta5, glm::vec3(-1.405, 0.2, 0.145));
		matVenta5 = glm::scale(matVenta5, glm::vec3(0.12, 0.2, 0.005));
		glBindTexture(GL_TEXTURE_2D, IDtextuVenRot);
		box1.render(matVenta5);
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 matVenta6 = glm::mat4(1.0);
		matVenta6 = glm::translate(matVenta6, glm::vec3(-1.6, 0.2, 0.145));
		matVenta6 = glm::scale(matVenta6, glm::vec3(0.12, 0.2, 0.005));
		glBindTexture(GL_TEXTURE_2D, IDtextuVenRot);
		box1.render(matVenta6);
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 matVenta7 = glm::mat4(1.0);
		matVenta7 = glm::translate(matVenta7, glm::vec3(-1.8, 0.2, 0.145));
		matVenta7 = glm::scale(matVenta7, glm::vec3(0.12, 0.2, 0.005));
		glBindTexture(GL_TEXTURE_2D, IDtextuVenRot);
		box1.render(matVenta7);
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 matVenta8 = glm::mat4(1.0);
		matVenta8 = glm::translate(matVenta8, glm::vec3(-2.0, 0.2, 0.145));
		matVenta8 = glm::scale(matVenta8, glm::vec3(0.12, 0.2, 0.005));
		glBindTexture(GL_TEXTURE_2D, IDtextuVenRot);
		box1.render(matVenta8);
		glBindTexture(GL_TEXTURE_2D, 0);


		//PUERTA
				glm::mat4 matDoor1 = glm::mat4(1.0);
				matDoor1 = glm::rotate(matDoor1, glm::radians(90.0f),glm::vec3(0.0,1.0,0.0));
				matDoor1 = glm::translate(matDoor1, glm::vec3(0.0, 0.0, -1.255));
				matDoor1 = glm::scale(matDoor1, glm::vec3(0.48, 0.39, 0.4));
				modelPuerta1.render(matDoor1);
				glActiveTexture(GL_TEXTURE0);

				glm::mat4 matDoor2 = glm::mat4(1.0);
				matDoor2 = glm::rotate(matDoor2, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
				matDoor2 = glm::translate(matDoor2, glm::vec3(0.0, 0.0, -1.255));
				matDoor2 = glm::scale(matDoor2, glm::vec3(0.48, 0.39, 0.4));
				modelPuerta2.render(matDoor2);
				glActiveTexture(GL_TEXTURE0);

		//CESPED
		glm::mat4 matCesped = glm::mat4(1.0);
		matCesped = glm::translate(matCesped, glm::vec3(0.0, -0.005, 0.0));
		matCesped = glm::scale(matCesped, glm::vec3(10.0, 0.01, 10.0));
		shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(30.0, 10.0)));
		glBindTexture(GL_TEXTURE_2D, IDtextuCesped);
		box1.render(matCesped);
		shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(1.0, 1.0)));
		glBindTexture(GL_TEXTURE_2D, 0);

		/*****************************************************************************************
			CASA - TERROR - VELAS
		******************************************************************************************/

					glm::mat4 matVela = glm::mat4(1.0);
					matVela = glm::translate(matVela, glm::vec3(-0.25, 0.0015, -0.075)); //-0.25, 0.0, -0.075
					matVela = glm::scale(matVela, glm::vec3(0.002, 0.005, 0.002));
					glBindTexture(GL_TEXTURE_2D, IDtextuCera);
					cylinder1.render(matVela);
					glBindTexture(GL_TEXTURE_2D, 0);

						glm::mat4 llama = glm::mat4(1.0);
						llama = glm::translate(llama, glm::vec3(-0.25, 0.0045, -0.075));
						llama = glm::scale(llama, glm::vec3(0.0007, 0.0015, 0.0007));
						sphere1.render(llama);

					glm::mat4 matVela1 = glm::mat4(1.0);
					matVela1 = glm::translate(matVela1, glm::vec3(-0.25, 0.0015, 0.075)); //-0.25, 0.0, -0.075
					matVela1 = glm::scale(matVela1, glm::vec3(0.002, 0.005, 0.002));
					glBindTexture(GL_TEXTURE_2D, IDtextuCera);
					cylinder1.render(matVela1);
					glBindTexture(GL_TEXTURE_2D, 0);

						glm::mat4 llama1 = glm::mat4(1.0);
						llama1 = glm::translate(llama1, glm::vec3(-0.25, 0.0045, 0.075));
						llama1 = glm::scale(llama1, glm::vec3(0.0007, 0.0015, 0.0007));
						sphere1.render(llama1);

					glm::mat4 matVela2 = glm::mat4(1.0);
					matVela2 = glm::translate(matVela2, glm::vec3(-0.35, 0.0015, 0.075));
					matVela2 = glm::scale(matVela2, glm::vec3(0.002, 0.005, 0.002));
					glBindTexture(GL_TEXTURE_2D, IDtextuCera);
					cylinder1.render(matVela2);
					glBindTexture(GL_TEXTURE_2D, 0);

						glm::mat4 llama2 = glm::mat4(1.0);
						llama2 = glm::translate(llama2, glm::vec3(-0.35, 0.0045, 0.075));
						llama2 = glm::scale(llama2, glm::vec3(0.0007, 0.0015, 0.0007));
						sphere1.render(llama2);


					glm::mat4 matVela3 = glm::mat4(1.0);
					matVela3 = glm::translate(matVela3, glm::vec3(-0.35, 0.0015, -0.075));
					matVela3 = glm::scale(matVela3, glm::vec3(0.002, 0.005, 0.002));
					glBindTexture(GL_TEXTURE_2D, IDtextuCera);
					cylinder1.render(matVela3);
					glBindTexture(GL_TEXTURE_2D, 0);

						glm::mat4 llama3 = glm::mat4(1.0);
						llama3 = glm::translate(llama3, glm::vec3(-0.35, 0.0045, -0.075));
						llama3 = glm::scale(llama3, glm::vec3(0.0007, 0.0015, 0.0007));
						sphere1.render(llama3);

					glm::mat4 matVela4 = glm::mat4(1.0);
					matVela4 = glm::translate(matVela4, glm::vec3(-0.45, 0.0015, -0.075));
					matVela4 = glm::scale(matVela4, glm::vec3(0.002, 0.005, 0.002));
					glBindTexture(GL_TEXTURE_2D, IDtextuCera);
					cylinder1.render(matVela4);
					glBindTexture(GL_TEXTURE_2D, 0);

						glm::mat4 llama4 = glm::mat4(1.0);
						llama4 = glm::translate(llama4, glm::vec3(-0.45, 0.0045, -0.075));
						llama4 = glm::scale(llama4, glm::vec3(0.0007, 0.0015, 0.0007));
						sphere1.render(llama4);

					glm::mat4 matVela5 = glm::mat4(1.0);
					matVela5 = glm::translate(matVela5, glm::vec3(-0.45, 0.0015, 0.075));
					matVela5 = glm::scale(matVela5, glm::vec3(0.002, 0.005, 0.002));
					glBindTexture(GL_TEXTURE_2D, IDtextuCera);
					cylinder1.render(matVela5);
					glBindTexture(GL_TEXTURE_2D, 0);

						glm::mat4 llama5 = glm::mat4(1.0);
						llama5 = glm::translate(llama5, glm::vec3(-0.45, 0.0045, 0.075));
						llama5 = glm::scale(llama5, glm::vec3(0.0007, 0.0015, 0.0007));
						sphere1.render(llama5);

					glm::mat4 matVela6 = glm::mat4(1.0);
					matVela6 = glm::translate(matVela6, glm::vec3(-0.55, 0.0015, 0.075));
					matVela6 = glm::scale(matVela6, glm::vec3(0.002, 0.005, 0.002));
					glBindTexture(GL_TEXTURE_2D, IDtextuCera);
					cylinder1.render(matVela6);
					glBindTexture(GL_TEXTURE_2D, 0);
					
						glm::mat4 llama6 = glm::mat4(1.0);
						llama6 = glm::translate(llama6, glm::vec3(-0.55, 0.0045, 0.075));
						llama6 = glm::scale(llama6, glm::vec3(0.0007, 0.0015, 0.0007));
						sphere1.render(llama6);

					glm::mat4 matVela7 = glm::mat4(1.0);
					matVela7 = glm::translate(matVela7, glm::vec3(-0.55, 0.0015, -0.075));
					matVela7 = glm::scale(matVela7, glm::vec3(0.002, 0.005, 0.002));
					glBindTexture(GL_TEXTURE_2D, IDtextuCera);
					cylinder1.render(matVela7);
					glBindTexture(GL_TEXTURE_2D, 0);

						glm::mat4 llama7 = glm::mat4(1.0);
						llama7 = glm::translate(llama7, glm::vec3(-0.55, 0.0045, -0.075));
						llama7 = glm::scale(llama7, glm::vec3(0.0007, 0.0015, 0.0007));
						sphere1.render(llama7);

					glm::mat4 matVela8 = glm::mat4(1.0);
					matVela8 = glm::translate(matVela8, glm::vec3(-0.65, 0.0015, -0.075));
					matVela8 = glm::scale(matVela8, glm::vec3(0.002, 0.005, 0.002));
					glBindTexture(GL_TEXTURE_2D, IDtextuCera);
					cylinder1.render(matVela8);
					glBindTexture(GL_TEXTURE_2D, 0);

						glm::mat4 llama8 = glm::mat4(1.0);
						llama8 = glm::translate(llama8, glm::vec3(-0.65, 0.0045, -0.075));
						llama8 = glm::scale(llama8, glm::vec3(0.0007, 0.0015, 0.0007));
						sphere1.render(llama8);

					glm::mat4 matVela9 = glm::mat4(1.0);
					matVela9 = glm::translate(matVela9, glm::vec3(-0.65, 0.0015, 0.075));
					matVela9 = glm::scale(matVela9, glm::vec3(0.002, 0.005, 0.002));
					glBindTexture(GL_TEXTURE_2D, IDtextuCera);
					cylinder1.render(matVela9);
					glBindTexture(GL_TEXTURE_2D, 0);

						glm::mat4 llama9 = glm::mat4(1.0);
						llama9 = glm::translate(llama9, glm::vec3(-0.65, 0.0045, 0.075));
						llama9 = glm::scale(llama9, glm::vec3(0.0007, 0.0015, 0.0007));
						sphere1.render(llama9);

					glm::mat4 matVela10 = glm::mat4(1.0);
					matVela10 = glm::translate(matVela10, glm::vec3(-0.75, 0.0015, 0.075));
					matVela10 = glm::scale(matVela10, glm::vec3(0.002, 0.005, 0.002));
					glBindTexture(GL_TEXTURE_2D, IDtextuCera);
					cylinder1.render(matVela10);
					glBindTexture(GL_TEXTURE_2D, 0);

						glm::mat4 llama10 = glm::mat4(1.0);
						llama10 = glm::translate(llama10, glm::vec3(-0.75, 0.0045, 0.075));
						llama10 = glm::scale(llama10, glm::vec3(0.0007, 0.0015, 0.0007));
						sphere1.render(llama10);

					glm::mat4 matVela11 = glm::mat4(1.0);
					matVela11 = glm::translate(matVela11, glm::vec3(-0.75, 0.0015, -0.075));
					matVela11 = glm::scale(matVela11, glm::vec3(0.002, 0.005, 0.002));
					glBindTexture(GL_TEXTURE_2D, IDtextuCera);
					cylinder1.render(matVela11);
					glBindTexture(GL_TEXTURE_2D, 0);

						glm::mat4 llama11 = glm::mat4(1.0);
						llama11 = glm::translate(llama11, glm::vec3(-0.75, 0.0045, -0.075));
						llama11 = glm::scale(llama11, glm::vec3(0.0007, 0.0015, 0.0007));
						sphere1.render(llama11);

					glm::mat4 matVela12 = glm::mat4(1.0);
					matVela12 = glm::translate(matVela12, glm::vec3(-0.85, 0.0015, -0.075));
					matVela12 = glm::scale(matVela12, glm::vec3(0.002, 0.005, 0.002));
					glBindTexture(GL_TEXTURE_2D, IDtextuCera);
					cylinder1.render(matVela12);
					glBindTexture(GL_TEXTURE_2D, 0);

						glm::mat4 llama12 = glm::mat4(1.0);
						llama12 = glm::translate(llama12, glm::vec3(-0.85, 0.0045, -0.075));
						llama12 = glm::scale(llama12, glm::vec3(0.0007, 0.0015, 0.0007));
						sphere1.render(llama12);

					glm::mat4 matVela13 = glm::mat4(1.0);
					matVela13 = glm::translate(matVela13, glm::vec3(-0.85, 0.0015, 0.075));
					matVela13 = glm::scale(matVela13, glm::vec3(0.002, 0.005, 0.002));
					glBindTexture(GL_TEXTURE_2D, IDtextuCera);
					cylinder1.render(matVela13);
					glBindTexture(GL_TEXTURE_2D, 0);

						glm::mat4 llama13 = glm::mat4(1.0);
						llama13 = glm::translate(llama13, glm::vec3(-0.85, 0.0045, 0.075));
						llama13 = glm::scale(llama13, glm::vec3(0.0007, 0.0015, 0.0007));
						sphere1.render(llama13);

					glm::mat4 matVela14 = glm::mat4(1.0);
					matVela14 = glm::translate(matVela14, glm::vec3(-0.95, 0.0015, 0.075));
					matVela14 = glm::scale(matVela14, glm::vec3(0.002, 0.005, 0.002));
					glBindTexture(GL_TEXTURE_2D, IDtextuCera);
					cylinder1.render(matVela14);
					glBindTexture(GL_TEXTURE_2D, 0);

						glm::mat4 llama14 = glm::mat4(1.0);
						llama14 = glm::translate(llama14, glm::vec3(-0.95, 0.0045, 0.075));
						llama14 = glm::scale(llama14, glm::vec3(0.0007, 0.0015, 0.0007));
						sphere1.render(llama14);


					glm::mat4 matVela15 = glm::mat4(1.0);
					matVela15 = glm::translate(matVela15, glm::vec3(-0.95, 0.0015, -0.075));
					matVela15 = glm::scale(matVela15, glm::vec3(0.002, 0.005, 0.002));
					glBindTexture(GL_TEXTURE_2D, IDtextuCera);
					cylinder1.render(matVela15);
					glBindTexture(GL_TEXTURE_2D, 0);

						glm::mat4 llama15 = glm::mat4(1.0);
						llama15 = glm::translate(llama15, glm::vec3(-0.95, 0.0045, -0.075));
						llama15 = glm::scale(llama15, glm::vec3(0.0007, 0.0015, 0.0007));
						sphere1.render(llama15);


					glm::mat4 matVela16 = glm::mat4(1.0);
					matVela16 = glm::translate(matVela16, glm::vec3(-1.05, 0.0015, -0.075));
					matVela16 = glm::scale(matVela16, glm::vec3(0.002, 0.005, 0.002));
					glBindTexture(GL_TEXTURE_2D, IDtextuCera);
					cylinder1.render(matVela16);
					glBindTexture(GL_TEXTURE_2D, 0);

						glm::mat4 llama16 = glm::mat4(1.0);
						llama16 = glm::translate(llama16, glm::vec3(-1.05, 0.0045, -0.075));
						llama16 = glm::scale(llama16, glm::vec3(0.0007, 0.0015, 0.0007));
						sphere1.render(llama16);


					glm::mat4 matVela17 = glm::mat4(1.0);
					matVela17 = glm::translate(matVela17, glm::vec3(-1.05, 0.0015, 0.075));
					matVela17 = glm::scale(matVela17, glm::vec3(0.002, 0.005, 0.002));
					glBindTexture(GL_TEXTURE_2D, IDtextuCera);
					cylinder1.render(matVela17);
					glBindTexture(GL_TEXTURE_2D, 0);

						glm::mat4 llama17 = glm::mat4(1.0);
						llama17 = glm::translate(llama17, glm::vec3(-1.05, 0.0045, 0.075));
						llama17 = glm::scale(llama17, glm::vec3(0.0007, 0.0015, 0.0007));
						sphere1.render(llama17);

					glm::mat4 matVela18 = glm::mat4(1.0);
					matVela18 = glm::translate(matVela18, glm::vec3(-1.15, 0.0015, 0.075));
					matVela18 = glm::scale(matVela18, glm::vec3(0.002, 0.005, 0.002));
					glBindTexture(GL_TEXTURE_2D, IDtextuCera);
					cylinder1.render(matVela18);
					glBindTexture(GL_TEXTURE_2D, 0);

						glm::mat4 llama18 = glm::mat4(1.0);
						llama18 = glm::translate(llama18, glm::vec3(-1.15, 0.0045, 0.075));
						llama18 = glm::scale(llama18, glm::vec3(0.0007, 0.0015, 0.0007));
						sphere1.render(llama18);

					glm::mat4 matVela19 = glm::mat4(1.0);
					matVela19 = glm::translate(matVela19, glm::vec3(-1.15, 0.0015, -0.075));
					matVela19 = glm::scale(matVela19, glm::vec3(0.002, 0.005, 0.002));
					glBindTexture(GL_TEXTURE_2D, IDtextuCera);
					cylinder1.render(matVela19);
					glBindTexture(GL_TEXTURE_2D, 0);

						glm::mat4 llama19 = glm::mat4(1.0);
						llama19 = glm::translate(llama19, glm::vec3(-1.15, 0.0045, -0.075));
						llama19 = glm::scale(llama19, glm::vec3(0.0007, 0.0015, 0.0007));
						sphere1.render(llama19);

			/*****************************************************************************************
				CASA - TERROR - CERCA
			******************************************************************************************/

					glm::mat4 Cerca = glm::mat4(1.0);
					Cerca = glm::translate(Cerca, glm::vec3(-0.2, 0.01, -1.08));
					Cerca = glm::scale(Cerca, glm::vec3(0.0001, 0.04, 2.0));
					glBindTexture(GL_TEXTURE_2D, IDtextuReja);
					shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(40.0, 1.0)));
					box2.render(Cerca);
					glBindTexture(GL_TEXTURE_2D, 0);

					glm::mat4 Cerca2 = glm::mat4(1.0);
					Cerca2 = glm::translate(Cerca2, glm::vec3(-0.2, 0.01, 1.08));
					Cerca2 = glm::scale(Cerca2, glm::vec3(0.0001, 0.04, 2.0));
					glBindTexture(GL_TEXTURE_2D, IDtextuReja);
					shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(40.0, 1.0)));
					box2.render(Cerca2);
					glBindTexture(GL_TEXTURE_2D, 0);

					glm::mat4 Cerca3 = glm::mat4(1.0);
					Cerca3 = glm::translate(Cerca3, glm::vec3(-1.2, 0.01, 2.08));
					Cerca3 = glm::scale(Cerca3, glm::vec3(2.0, 0.04, 0.0001));
					glBindTexture(GL_TEXTURE_2D, IDtextuReja);
					shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(40.0, 1.0)));
					box2.render(Cerca3);
					glBindTexture(GL_TEXTURE_2D, 0);

					glm::mat4 Cerca4 = glm::mat4(1.0);
					Cerca4 = glm::translate(Cerca4, glm::vec3(-1.2, 0.01, -2.08));
					Cerca4 = glm::scale(Cerca4, glm::vec3(2.0, 0.04, 0.0001));
					glBindTexture(GL_TEXTURE_2D, IDtextuReja);
					shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(40.0, 1.0)));
					box2.render(Cerca4);
					glBindTexture(GL_TEXTURE_2D, 0);

					glm::mat4 Cerca5 = glm::mat4(1.0);
					Cerca5 = glm::translate(Cerca5, glm::vec3(-2.2, 0.01, 0.0));
					Cerca5 = glm::scale(Cerca5, glm::vec3(0.0001, 0.04, 4.1));
					glBindTexture(GL_TEXTURE_2D, IDtextuReja);
					shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(40.0, 1.0)));
					box2.render(Cerca5);
					glBindTexture(GL_TEXTURE_2D, 0);



		if (angle > 2 * M_PI)
			angle = 0.0;
		else
			angle += 0.0001;

		// Se Dibuja el Skybox
		GLint oldCullFaceMode;
		GLint oldDepthFuncMode;
		// deshabilita el modo del recorte de caras ocultas para ver las esfera desde adentro
		glGetIntegerv(GL_CULL_FACE_MODE, &oldCullFaceMode);
		glGetIntegerv(GL_DEPTH_FUNC, &oldDepthFuncMode);
		shaderSkybox.setFloat("skybox", 0);
		glCullFace(GL_FRONT);
		glDepthFunc(GL_LEQUAL);
		skyboxSphere.render();
		glCullFace(oldCullFaceMode);
		glDepthFunc(oldDepthFuncMode);

		rot0 = 0;
		glfwSwapBuffers(window);
	}
}

int main(int argc, char **argv) {
	init(800, 700, "Window GLFW", false);
	applicationLoop();
	destroy();
	return 1;
}
