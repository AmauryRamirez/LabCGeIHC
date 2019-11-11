#define _USE_MATH_DEFINES
#include <cmath>
#include <stdlib.h>
#include <time.h>


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
//Include loader model Class
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

//--------------------------CASA------------------------------------------------
Box cubo;
Box cuboAzul;
Cylinder tubo(20, 20, 0.5, 0.5);


Model modelChimenea;
Model modelSofaHolly;


//Models complex instances 
Model modelRock;
Model modelRailRoad;
Model modelAircraft_obj;
Model modelDinosaurio;
Model modelDoor1;
Model modelEscalera;
Model modelCocina;
Model modelVentana;
Model modelRefri;
Model modelCerca;
Model modelCarroEclip1;
Model modelHelico;


GLuint textureID1, textureID2, textureID3, textureID4, IDtextuPiso1, IDtextureMarmol;
GLuint IDtextuFachada, IDtextuMadera1, IDTextureTv, IDTextureCesped, IDtextureMetal, IDtextureCristal;
GLuint IDtextuCarretera, IDtextureAsfalto;
GLuint skyboxTextureID;

GLenum types[6] = {
GL_TEXTURE_CUBE_MAP_POSITIVE_X,
GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
GL_TEXTURE_CUBE_MAP_NEGATIVE_Z };

std::string fileNames[6] = { "../Textures/mp_druidcove/druidcove_ft.tga",
		"../Textures/mp_druidcove/druidcove_bk.tga",
		"../Textures/mp_druidcove/druidcove_up.tga",
		"../Textures/mp_druidcove/druidcove_dn.tga",
		"../Textures/mp_druidcove/druidcove_rt.tga",
		"../Textures/mp_druidcove/druidcove_lf.tga" };

bool exitApp = false;
int lastMousePosX, offsetX = 0;
int lastMousePosY, offsetY = 0;

float rot0 = 0.0, dz = 0.0;

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
	sphere1.setColor(glm::vec4(0.3, 0.3, 1.0, 1.0));

	// Inicializar los buffers VAO, VBO, EBO
	sphere2.init();
	// Método setter que colocar el apuntador al shader
	sphere2.setShader(&shaderColorLighting);
	//Setter para poner el color de la geometria
	sphere2.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));

	// Inicializar los buffers VAO, VBO, EBO
	sphereLamp.init();
	// Método setter que colocar el apuntador al shader
	sphereLamp.setShader(&shader);
	//Setter para poner el color de la geometria
	sphereLamp.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));

	cylinder1.init();
	cylinder1.setShader(&shaderColorLighting);
	cylinder1.setColor(glm::vec4(0.3, 0.3, 1.0, 1.0));

	cylinder2.init();
	cylinder2.setShader(&shaderTextureLighting);

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
	box1.setColor(glm::vec4(1.0, 1.0, 0.0, 1.0));

	box2.init();
	box2.setShader(&shaderMulLighting);

	sphere3.init();
	sphere3.setShader(&shaderTextureLighting);

	box3.init();
	box3.setShader(&shaderTextureLighting);

	cubo.init();								//PISO PLANTA BAJA
	cubo.setShader(&shaderMulLighting);
	cubo.setColor(glm::vec4(1.0, 1.0, 0.0, 1.0));

	cuboAzul.init();								//PISO PLANTA BAJA
	cuboAzul.setShader(&shaderMulLighting);
	cuboAzul.setColor(glm::vec4(0.0, 0.0, 1.0, 1.0));

	tubo.init();
	tubo.setShader(&shaderMulLighting);





	//INITIALIZACION MODELS
	modelRock.loadModel("../models/rock/rock.obj");
	modelRock.setShader(&shaderMulLighting);

	modelRailRoad.loadModel("../models/railroad/railroad_track.obj");
	modelRailRoad.setShader(&shaderMulLighting);

	modelAircraft_obj.loadModel("../models/Aircraft_obj/E 45 Aircraft_obj.obj");
	modelAircraft_obj.setShader(&shaderMulLighting);


	modelDinosaurio.loadModel("../models/Dinosaurio/Nightstand.obj");
	modelDinosaurio.setShader(&shaderMulLighting);

	modelDoor1.loadModel("../models/Doors/InteriorDoor.obj");
	modelDoor1.setShader(&shaderMulLighting);

	modelChimenea.loadModel("../models/Chimenea/kamin_06_obj.obj");
	modelChimenea.setShader(&shaderMulLighting);

	modelSofaHolly.loadModel("../models/Sofa/Sofa HollyWood OBJ.obj");
	modelSofaHolly.setShader(&shaderMulLighting);
	
	modelEscalera.loadModel("../models/Escalera/Stairs.obj");
	modelEscalera.setShader(&shaderMulLighting);

	modelCocina.loadModel("../models/cocina2/kitchen_fr.obj");
	modelCocina.setShader(&shaderMulLighting);

	modelVentana.loadModel("../models/Window_obj/Window.obj");
	modelVentana.setShader(&shaderMulLighting);

	modelRefri.loadModel("../models/Double_Door_Fridge/Fridge.obj");
	modelRefri.setShader(&shaderMulLighting);

	modelRefri.loadModel("../models/cerca/Fence_White.obj");
	modelRefri.setShader(&shaderMulLighting);

	modelCarroEclip1.loadModel("../models/Eclipse/2003eclipse.obj");
	modelCarroEclip1.setShader(&shaderMulLighting);

	modelHelico.loadModel("../models/Helicopter/Mi_24.obj");
	modelHelico.setShader(&shaderMulLighting);

	camera->setPosition(glm::vec3(0.0, 3.0, 17.0));

	// Descomentar
	// Definimos el tamanio de la imagen
	int imageWidth, imageHeight;
	// Definiendo la textura a utilizar
	Texture texture1("../Textures/sponge.jpg");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	FIBITMAP *bitmap = texture1.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	unsigned char *data = texture1.convertToData(bitmap, imageWidth,
			imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureID1);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureID1);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
		GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	} else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	texture1.freeImage(bitmap);

	// Definiendo la textura a utilizar
	Texture texture2("../Textures/water.jpg");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = texture2.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = texture2.convertToData(bitmap, imageWidth, imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureID2);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureID2);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
		GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	} else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	texture2.freeImage(bitmap);

	// Definiendo la textura a utilizar
	Texture texture3("../Textures/goku.png");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	// Voltear la imagen
	bitmap = texture3.loadImage(true);
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = texture3.convertToData(bitmap, imageWidth, imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureID3);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureID3);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
		GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	} else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	texture3.freeImage(bitmap);

	// Definiendo la textura a utilizar
	Texture texture4("../Textures/texturaLadrillos.jpg");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	// Voltear la imagen
	bitmap = texture4.loadImage(true);
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = texture4.convertToData(bitmap, imageWidth, imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureID4);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureID4);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
		GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	} else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	texture4.freeImage(bitmap);

	//----------------------------TEXTURA PISO 1--------------------------------------------------------------
	Texture textuPiso1("../Textures/Piso_1.jpg");
	bitmap = textuPiso1.loadImage(false);
	data = textuPiso1.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &IDtextuPiso1);
	glBindTexture(GL_TEXTURE_2D, IDtextuPiso1);
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
	textuPiso1.freeImage(bitmap);
	//--------------------------

	//----------------------------TEXTURA ASFALTO--------------------------------------------------------------
	Texture textuAsfalto("../Textures/asfalto.jpg");
	bitmap = textuAsfalto.loadImage(TRUE);
	data = textuAsfalto.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &IDtextureAsfalto);
	glBindTexture(GL_TEXTURE_2D, IDtextureAsfalto);
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
	textuAsfalto.freeImage(bitmap);
	//--------------------------


	//----------------------------TEXTURA ColumCentral--------------------------------------------------------------
	Texture textuMarmol("../Textures/marmol.jpg");
	bitmap = textuMarmol.loadImage(false);
	data = textuMarmol.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &IDtextureMarmol);
	glBindTexture(GL_TEXTURE_2D, IDtextureMarmol);
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
	textuMarmol.freeImage(bitmap);
	//--------------------------

	//----------------------------TEXTURA CARRETERA--------------------------------------------------------------
	Texture textuCarretera("../Textures/Carretera.jpg");
	bitmap = textuCarretera.loadImage(TRUE);
	data = textuCarretera.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &IDtextuCarretera);
	glBindTexture(GL_TEXTURE_2D, IDtextuCarretera);
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
	textuCarretera.freeImage(bitmap);
	//--------------------------


	//----------------------------TEXTURA fachada--------------------------------------------------------------
	Texture textuFachada("../Textures/fachada.jpg");
	bitmap = textuFachada.loadImage(false);
	data = textuFachada.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &IDtextuFachada);
	glBindTexture(GL_TEXTURE_2D, IDtextuFachada);
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
	textuFachada.freeImage(bitmap);
	//--------------------------

	//----------------------------TEXTURA MADERA1--------------------------------------------------------------
	Texture textuMadera1("../Textures/Madera_1.jpg");
	bitmap = textuMadera1.loadImage(false);
	data = textuMadera1.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &IDtextuMadera1);
	glBindTexture(GL_TEXTURE_2D, IDtextuMadera1);
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
	textuMadera1.freeImage(bitmap);
	//--------------------------


	//----------------------------TEXTURA MADERA1--------------------------------------------------------------
	Texture textuTv("../Textures/Screen1.jpg");
	bitmap = textuTv.loadImage(false);
	data = textuTv.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &IDTextureTv);
	glBindTexture(GL_TEXTURE_2D, IDTextureTv);
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
	textuTv.freeImage(bitmap);
	//--------------------------

	//----------------------------TEXTURA CESPED--------------------------------------------------------------
	Texture textuCesped("../Textures/cesped.jpg");
	bitmap = textuCesped.loadImage(false);
	data = textuCesped.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &IDTextureCesped);
	glBindTexture(GL_TEXTURE_2D, IDTextureCesped);
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
	//--------------------------

	//----------------------------TEXTURA Crystal--------------------------------------------------------------
	Texture textuCristal("../Textures/cristal.jpg");
	bitmap = textuCristal.loadImage(false);
	data = textuCristal.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &IDtextureCristal);
	glBindTexture(GL_TEXTURE_2D, IDtextureCristal);
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
	textuCristal.freeImage(bitmap);
	//--------------------------

	//----------------------------TEXTURA METAL--------------------------------------------------------------
	Texture textuMetal("../Textures/metal.jpg");
	bitmap = textuMetal.loadImage(false);
	data = textuMetal.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &IDtextureMetal);
	glBindTexture(GL_TEXTURE_2D, IDtextureMetal);
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
	textuMetal.freeImage(bitmap);
	//--------------------------


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
		} else
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
	cubo.destroy();
	cuboAzul.destroy();
	tubo.destroy();


	
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


	glfwPollEvents();
	return continueApplication;
}

void applicationLoop() {
	bool psi = true;

	//glm::mat4 model = glm::mat4(1.0f);
	float offX = 0.0;
	float angle = 0.0;
	float ratio = 50.0;

	int aleatorio = 0, aleatorio2 = 0;

	

	float offsetCarAdvance = 0.0;
	float offsetCarRot = 0.0;
	int stateCar = 0;
	
	float offsetHeliAdvance = 0.0;
	float offsetHeliDown = 0.0;
	float offsetHeliRot = 0.0;
	int stateHeli = 0;
	
	glm::mat4 ObjCarroEclipse = glm::mat4(1.0);
	ObjCarroEclipse = glm::translate(ObjCarroEclipse, glm::vec3(0.0, -0.8, 21.0));
	ObjCarroEclipse = glm::scale(ObjCarroEclipse, glm::vec3(0.2, 0.2, 0.2));
	ObjCarroEclipse = glm::rotate(ObjCarroEclipse, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
	
	glm::mat4 matHelico = glm::mat4(1.0);
	matHelico = glm::translate(matHelico, glm::vec3(-3.5, 20.0, -100.0));
	matHelico = glm::rotate(matHelico, glm::radians(45.0f), glm::vec3(1.0, 0.0, 0.0));
	

	while (psi) {

		srand(time(NULL));
		aleatorio = rand() % 10;

		if (aleatorio <= 5) {
			aleatorio = 5;
			aleatorio2 = 10;
		}
		else
		{
			aleatorio = 10;
			aleatorio2 = 5;
		}

		psi = processInput(true);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 projection = glm::perspective(glm::radians(45.0f),
				(float) screenWidth / (float) screenHeight, 0.01f, 100.0f);
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
		shaderMulLighting.setVectorFloat3("directionalLight.light.diffuse", glm::value_ptr(glm::vec3(0.5, 0.5, 0.5)));
		shaderMulLighting.setVectorFloat3("directionalLight.light.specular", glm::value_ptr(glm::vec3(0.9, 0.9, 0.9)));
		shaderMulLighting.setVectorFloat3("directionalLight.direction", glm::value_ptr(glm::vec3(0.0, 0.0, 1.0)));

		//ESTO ES PARA LA LUZ SPOTLIGHT
		shaderMulLighting.setInt("spotLightCount", 1);
		shaderMulLighting.setVectorFloat3("spotLights[0].position",glm::value_ptr(camera->getPosition()));
		shaderMulLighting.setVectorFloat3("spotLights[0].direction", glm::value_ptr(camera->getFront()));
		shaderMulLighting.setVectorFloat3("spotLights[0].light.ambient", glm::value_ptr(glm::vec3(0.1, 0.1, 0.1)));
		shaderMulLighting.setVectorFloat3("spotLights[0].light.diffuse", glm::value_ptr(glm::vec3(0.4, 0.4, 0.4))); 
		shaderMulLighting.setVectorFloat3("spotLights[0].light.specular", glm::value_ptr(glm::vec3(0.6, 0.6, 0.6)));
		shaderMulLighting.setFloat("spotLights[0].cutOff", cos(glm::radians(12.5)));
		shaderMulLighting.setFloat("spotLights[0].outerCutOff", cos(glm::radians(17.0)));
		shaderMulLighting.setFloat("spotLights[0].constant", 1.0);
		shaderMulLighting.setFloat("spotLights[0].linear", 0.1);
		shaderMulLighting.setFloat("spotLights[0].quadratic", 0.05);
		
		
		// Esto es para la luces pointlights
		//numero de luces a utilizar de tipo pointlihts = 3
		shaderMulLighting.setInt("pointLightCount", 20); //agregar numero de luces sise ponen 5 tambien ir a multipleShader.fs en const int MAX_POINT_LIGHTS = 5;

		// posicion de la luz con indice 0
		shaderMulLighting.setVectorFloat3("pointLights[0].position", glm::value_ptr((glm::vec3(-6.6, 4.0, 7.1)))); // debe ser igual a sphereLamp.setPosition(glm::vec3(-5.1, 4.5, -3.5));
		shaderMulLighting.setVectorFloat3("pointLights[0].light.ambient", glm::value_ptr(glm::vec3(0.001, 0.001, 0.001)));
		shaderMulLighting.setVectorFloat3("pointLights[0].light.diffuse", glm::value_ptr(glm::vec3(0.0, 0.0, 1.0)));
		shaderMulLighting.setVectorFloat3("pointLights[0].light.specular", glm::value_ptr(glm::vec3(0.0, 0.0, 0.6)));
		shaderMulLighting.setFloat("pointLights[0].constant", aleatorio);
		shaderMulLighting.setFloat("pointLights[0].linear", 0.0004);
		shaderMulLighting.setFloat("pointLights[0].quadratic", 0.00004);

		shaderMulLighting.setVectorFloat3("pointLights[1].position", glm::value_ptr((glm::vec3(-6.5, 3.8, 7.1)))); // debe ser igual a sphereLamp.setPosition(glm::vec3(-5.1, 4.5, -3.5));
		shaderMulLighting.setVectorFloat3("pointLights[1].light.ambient", glm::value_ptr(glm::vec3(0.001, 0.001, 0.001)));
		shaderMulLighting.setVectorFloat3("pointLights[1].light.diffuse", glm::value_ptr(glm::vec3(1.0, 0.0, 0.0)));
		shaderMulLighting.setVectorFloat3("pointLights[1].light.specular", glm::value_ptr(glm::vec3(0.6, 0.0, 0.0)));
		shaderMulLighting.setFloat("pointLights[1].constant", aleatorio2);
		shaderMulLighting.setFloat("pointLights[1].linear", 0.0004);
		shaderMulLighting.setFloat("pointLights[1].quadratic", 0.00004);

		shaderMulLighting.setVectorFloat3("pointLights[2].position", glm::value_ptr((glm::vec3(-6.4, 3.6, 7.1)))); // debe ser igual a sphereLamp.setPosition(glm::vec3(-5.1, 4.5, -3.5));
		shaderMulLighting.setVectorFloat3("pointLights[2].light.ambient", glm::value_ptr(glm::vec3(0.001, 0.001, 0.001)));
		shaderMulLighting.setVectorFloat3("pointLights[2].light.diffuse", glm::value_ptr(glm::vec3(0.0, 0.0, 1.0)));
		shaderMulLighting.setVectorFloat3("pointLights[2].light.specular", glm::value_ptr(glm::vec3(0.0, 0.0, 0.6)));
		shaderMulLighting.setFloat("pointLights[2].constant", aleatorio);
		shaderMulLighting.setFloat("pointLights[2].linear", 0.0004);
		shaderMulLighting.setFloat("pointLights[2].quadratic", 0.00004);

		shaderMulLighting.setVectorFloat3("pointLights[3].position", glm::value_ptr((glm::vec3(-6.3, 3.4, 7.1)))); // debe ser igual a sphereLamp.setPosition(glm::vec3(-5.1, 4.5, -3.5));
		shaderMulLighting.setVectorFloat3("pointLights[3].light.ambient", glm::value_ptr(glm::vec3(0.001, 0.001, 0.001)));
		shaderMulLighting.setVectorFloat3("pointLights[3].light.diffuse", glm::value_ptr(glm::vec3(1.0, 0.0, 1.0)));
		shaderMulLighting.setVectorFloat3("pointLights[3].light.specular", glm::value_ptr(glm::vec3(0.6, 0.0, 0.6)));
		shaderMulLighting.setFloat("pointLights[3].constant", aleatorio2);
		shaderMulLighting.setFloat("pointLights[3].linear", 0.0004);
		shaderMulLighting.setFloat("pointLights[3].quadratic", 0.00004);

		shaderMulLighting.setVectorFloat3("pointLights[4].position", glm::value_ptr((glm::vec3(-6.2, 3.2, 7.1)))); // debe ser igual a sphereLamp.setPosition(glm::vec3(-5.1, 4.5, -3.5));
		shaderMulLighting.setVectorFloat3("pointLights[4].light.ambient", glm::value_ptr(glm::vec3(0.001, 0.001, 0.001)));
		shaderMulLighting.setVectorFloat3("pointLights[4].light.diffuse", glm::value_ptr(glm::vec3(0.5, 0.5, 1.0)));
		shaderMulLighting.setVectorFloat3("pointLights[4].light.specular", glm::value_ptr(glm::vec3(0.6, 0.6, 0.6)));
		shaderMulLighting.setFloat("pointLights[4].constant", aleatorio);
		shaderMulLighting.setFloat("pointLights[4].linear", 0.0004);
		shaderMulLighting.setFloat("pointLights[4].quadratic", 0.00004);



		shaderMulLighting.setVectorFloat3("pointLights[5].position", glm::value_ptr((glm::vec3(-6.1, 3.0, 7.1)))); // debe ser igual a sphereLamp.setPosition(glm::vec3(-5.1, 4.5, -3.5));
		shaderMulLighting.setVectorFloat3("pointLights[5].light.ambient", glm::value_ptr(glm::vec3(0.001, 0.001, 0.001)));
		shaderMulLighting.setVectorFloat3("pointLights[5].light.diffuse", glm::value_ptr(glm::vec3(0.0, 0.0, 1.0)));
		shaderMulLighting.setVectorFloat3("pointLights[5].light.specular", glm::value_ptr(glm::vec3(0.0, 0.0, 0.6)));
		shaderMulLighting.setFloat("pointLights[5].constant", aleatorio2);
		shaderMulLighting.setFloat("pointLights[5].linear", 0.0004);
		shaderMulLighting.setFloat("pointLights[5].quadratic", 0.00004);

		shaderMulLighting.setVectorFloat3("pointLights[6].position", glm::value_ptr((glm::vec3(-6.0, 3.2, 7.1)))); // debe ser igual a sphereLamp.setPosition(glm::vec3(-5.1, 4.5, -3.5));
		shaderMulLighting.setVectorFloat3("pointLights[6].light.ambient", glm::value_ptr(glm::vec3(0.001, 0.001, 0.001)));
		shaderMulLighting.setVectorFloat3("pointLights[6].light.diffuse", glm::value_ptr(glm::vec3(1.0, 0.0, 0.0)));
		shaderMulLighting.setVectorFloat3("pointLights[6].light.specular", glm::value_ptr(glm::vec3(0.6, 0.0, 0.0)));
		shaderMulLighting.setFloat("pointLights[6].constant", aleatorio);
		shaderMulLighting.setFloat("pointLights[6].linear", 0.0004);
		shaderMulLighting.setFloat("pointLights[6].quadratic", 0.00004);

		shaderMulLighting.setVectorFloat3("pointLights[7].position", glm::value_ptr((glm::vec3(-5.9, 3.4, 7.1)))); // debe ser igual a sphereLamp.setPosition(glm::vec3(-5.1, 4.5, -3.5));
		shaderMulLighting.setVectorFloat3("pointLights[7].light.ambient", glm::value_ptr(glm::vec3(0.001, 0.001, 0.001)));
		shaderMulLighting.setVectorFloat3("pointLights[7].light.diffuse", glm::value_ptr(glm::vec3(0.0, 0.0, 1.0)));
		shaderMulLighting.setVectorFloat3("pointLights[7].light.specular", glm::value_ptr(glm::vec3(0.0, 0.0, 0.6)));
		shaderMulLighting.setFloat("pointLights[7].constant", aleatorio2);
		shaderMulLighting.setFloat("pointLights[7].linear", 0.0004);
		shaderMulLighting.setFloat("pointLights[7].quadratic", 0.00004);

		shaderMulLighting.setVectorFloat3("pointLights[8].position", glm::value_ptr((glm::vec3(-5.8, 3.6, 7.1)))); // debe ser igual a sphereLamp.setPosition(glm::vec3(-5.1, 4.5, -3.5));
		shaderMulLighting.setVectorFloat3("pointLights[8].light.ambient", glm::value_ptr(glm::vec3(0.001, 0.001, 0.001)));
		shaderMulLighting.setVectorFloat3("pointLights[8].light.diffuse", glm::value_ptr(glm::vec3(1.0, 0.0, 1.0)));
		shaderMulLighting.setVectorFloat3("pointLights[8].light.specular", glm::value_ptr(glm::vec3(0.6, 0.0, 0.6)));
		shaderMulLighting.setFloat("pointLights[8].constant", aleatorio);
		shaderMulLighting.setFloat("pointLights[8].linear", 0.0004);
		shaderMulLighting.setFloat("pointLights[8].quadratic", 0.00004);

		shaderMulLighting.setVectorFloat3("pointLights[9].position", glm::value_ptr((glm::vec3(-5.7, 3.8, 7.1)))); // debe ser igual a sphereLamp.setPosition(glm::vec3(-5.1, 4.5, -3.5));
		shaderMulLighting.setVectorFloat3("pointLights[9].light.ambient", glm::value_ptr(glm::vec3(0.001, 0.001, 0.001)));
		shaderMulLighting.setVectorFloat3("pointLights[9].light.diffuse", glm::value_ptr(glm::vec3(0.5, 0.5, 1.0)));
		shaderMulLighting.setVectorFloat3("pointLights[9].light.specular", glm::value_ptr(glm::vec3(0.6, 0.0, 0.6)));
		shaderMulLighting.setFloat("pointLights[9].constant", aleatorio2);
		shaderMulLighting.setFloat("pointLights[9].linear", 0.0004);
		shaderMulLighting.setFloat("pointLights[9].quadratic", 0.00004);




		shaderMulLighting.setVectorFloat3("pointLights[10].position", glm::value_ptr((glm::vec3(-5.6, 4.0, 7.1)))); // debe ser igual a sphereLamp.setPosition(glm::vec3(-5.1, 4.5, -3.5));
		shaderMulLighting.setVectorFloat3("pointLights[10].light.ambient", glm::value_ptr(glm::vec3(0.001, 0.001, 0.001)));
		shaderMulLighting.setVectorFloat3("pointLights[10].light.diffuse", glm::value_ptr(glm::vec3(0.5, 0.5, 1.0)));
		shaderMulLighting.setVectorFloat3("pointLights[10].light.specular", glm::value_ptr(glm::vec3(0.6, 0.0, 0.6)));
		shaderMulLighting.setFloat("pointLights[10].constant", aleatorio);
		shaderMulLighting.setFloat("pointLights[10].linear", 0.0004);
		shaderMulLighting.setFloat("pointLights[10].quadratic", 0.00004);

		shaderMulLighting.setVectorFloat3("pointLights[11].position", glm::value_ptr((glm::vec3(-5.5, 3.8, 7.1)))); // debe ser igual a sphereLamp.setPosition(glm::vec3(-5.1, 4.5, -3.5));
		shaderMulLighting.setVectorFloat3("pointLights[11].light.ambient", glm::value_ptr(glm::vec3(0.001, 0.001, 0.001)));
		shaderMulLighting.setVectorFloat3("pointLights[11].light.diffuse", glm::value_ptr(glm::vec3(0.5, 0.5, 1.0)));
		shaderMulLighting.setVectorFloat3("pointLights[11].light.specular", glm::value_ptr(glm::vec3(0.6, 0.0, 0.6)));
		shaderMulLighting.setFloat("pointLights[11].constant", aleatorio2);
		shaderMulLighting.setFloat("pointLights[11].linear", 0.0004);
		shaderMulLighting.setFloat("pointLights[11].quadratic", 0.00004);

		shaderMulLighting.setVectorFloat3("pointLights[12].position", glm::value_ptr((glm::vec3(-5.4, 3.6, 7.1)))); // debe ser igual a sphereLamp.setPosition(glm::vec3(-5.1, 4.5, -3.5));
		shaderMulLighting.setVectorFloat3("pointLights[12].light.ambient", glm::value_ptr(glm::vec3(0.001, 0.001, 0.001)));
		shaderMulLighting.setVectorFloat3("pointLights[12].light.diffuse", glm::value_ptr(glm::vec3(0.5, 0.5, 1.0)));
		shaderMulLighting.setVectorFloat3("pointLights[12].light.specular", glm::value_ptr(glm::vec3(0.6, 0.0, 0.6)));
		shaderMulLighting.setFloat("pointLights[12].constant", aleatorio);
		shaderMulLighting.setFloat("pointLights[12].linear", 0.0004);
		shaderMulLighting.setFloat("pointLights[12].quadratic", 0.00004);

		shaderMulLighting.setVectorFloat3("pointLights[13].position", glm::value_ptr((glm::vec3(-5.3, 3.4, 7.1)))); // debe ser igual a sphereLamp.setPosition(glm::vec3(-5.1, 4.5, -3.5));
		shaderMulLighting.setVectorFloat3("pointLights[13].light.ambient", glm::value_ptr(glm::vec3(0.001, 0.001, 0.001)));
		shaderMulLighting.setVectorFloat3("pointLights[13].light.diffuse", glm::value_ptr(glm::vec3(0.5, 0.5, 1.0)));
		shaderMulLighting.setVectorFloat3("pointLights[13].light.specular", glm::value_ptr(glm::vec3(0.6, 0.0, 0.6)));
		shaderMulLighting.setFloat("pointLights[13].constant", aleatorio2);
		shaderMulLighting.setFloat("pointLights[13].linear", 0.0004);
		shaderMulLighting.setFloat("pointLights[13].quadratic", 0.00004);

		shaderMulLighting.setVectorFloat3("pointLights[14].position", glm::value_ptr((glm::vec3(-5.2, 3.2, 7.1)))); // debe ser igual a sphereLamp.setPosition(glm::vec3(-5.1, 4.5, -3.5));
		shaderMulLighting.setVectorFloat3("pointLights[14].light.ambient", glm::value_ptr(glm::vec3(0.001, 0.001, 0.001)));
		shaderMulLighting.setVectorFloat3("pointLights[14].light.diffuse", glm::value_ptr(glm::vec3(0.5, 0.5, 1.0)));
		shaderMulLighting.setVectorFloat3("pointLights[14].light.specular", glm::value_ptr(glm::vec3(0.6, 0.0, 0.6)));
		shaderMulLighting.setFloat("pointLights[14].constant", aleatorio);
		shaderMulLighting.setFloat("pointLights[14].linear", 0.0004);
		shaderMulLighting.setFloat("pointLights[14].quadratic", 0.00004);

		shaderMulLighting.setVectorFloat3("pointLights[15].position", glm::value_ptr((glm::vec3(-5.1, 3.0, 7.1)))); // debe ser igual a sphereLamp.setPosition(glm::vec3(-5.1, 4.5, -3.5));
		shaderMulLighting.setVectorFloat3("pointLights[15].light.ambient", glm::value_ptr(glm::vec3(0.001, 0.001, 0.001)));
		shaderMulLighting.setVectorFloat3("pointLights[15].light.diffuse", glm::value_ptr(glm::vec3(0.5, 0.5, 1.0)));
		shaderMulLighting.setVectorFloat3("pointLights[15].light.specular", glm::value_ptr(glm::vec3(0.6, 0.0, 0.6)));
		shaderMulLighting.setFloat("pointLights[15].constant", aleatorio2);
		shaderMulLighting.setFloat("pointLights[15].linear", 0.0004);
		shaderMulLighting.setFloat("pointLights[15].quadratic", 0.00004);

		shaderMulLighting.setVectorFloat3("pointLights[16].position", glm::value_ptr((glm::vec3(-5.0, 3.2, 7.1)))); // debe ser igual a sphereLamp.setPosition(glm::vec3(-5.1, 4.5, -3.5));
		shaderMulLighting.setVectorFloat3("pointLights[16].light.ambient", glm::value_ptr(glm::vec3(0.001, 0.001, 0.001)));
		shaderMulLighting.setVectorFloat3("pointLights[16].light.diffuse", glm::value_ptr(glm::vec3(0.5, 0.5, 1.0)));
		shaderMulLighting.setVectorFloat3("pointLights[16].light.specular", glm::value_ptr(glm::vec3(0.6, 0.0, 0.6)));
		shaderMulLighting.setFloat("pointLights[16].constant", aleatorio);
		shaderMulLighting.setFloat("pointLights[16].linear", 0.0004);
		shaderMulLighting.setFloat("pointLights[16].quadratic", 0.00004);

		shaderMulLighting.setVectorFloat3("pointLights[17].position", glm::value_ptr((glm::vec3(-4.9, 3.4, 7.1)))); // debe ser igual a sphereLamp.setPosition(glm::vec3(-5.1, 4.5, -3.5));
		shaderMulLighting.setVectorFloat3("pointLights[17].light.ambient", glm::value_ptr(glm::vec3(0.001, 0.001, 0.001)));
		shaderMulLighting.setVectorFloat3("pointLights[17].light.diffuse", glm::value_ptr(glm::vec3(0.5, 0.5, 1.0)));
		shaderMulLighting.setVectorFloat3("pointLights[17].light.specular", glm::value_ptr(glm::vec3(0.6, 0.0, 0.6)));
		shaderMulLighting.setFloat("pointLights[17].constant", aleatorio2);
		shaderMulLighting.setFloat("pointLights[17].linear", 0.0004);
		shaderMulLighting.setFloat("pointLights[17].quadratic", 0.00004);

		shaderMulLighting.setVectorFloat3("pointLights[18].position", glm::value_ptr((glm::vec3(-4.8, 3.6, 7.1)))); // debe ser igual a sphereLamp.setPosition(glm::vec3(-5.1, 4.5, -3.5));
		shaderMulLighting.setVectorFloat3("pointLights[18].light.ambient", glm::value_ptr(glm::vec3(0.001, 0.001, 0.001)));
		shaderMulLighting.setVectorFloat3("pointLights[18].light.diffuse", glm::value_ptr(glm::vec3(0.5, 0.5, 1.0)));
		shaderMulLighting.setVectorFloat3("pointLights[18].light.specular", glm::value_ptr(glm::vec3(0.6, 0.0, 0.6)));
		shaderMulLighting.setFloat("pointLights[18].constant", aleatorio);
		shaderMulLighting.setFloat("pointLights[18].linear", 0.0004);
		shaderMulLighting.setFloat("pointLights[18].quadratic", 0.00004);

		shaderMulLighting.setVectorFloat3("pointLights[19].position", glm::value_ptr((glm::vec3(-4.7, 3.8, 7.1)))); // debe ser igual a sphereLamp.setPosition(glm::vec3(-5.1, 4.5, -3.5));
		shaderMulLighting.setVectorFloat3("pointLights[19].light.ambient", glm::value_ptr(glm::vec3(0.001, 0.001, 0.001)));
		shaderMulLighting.setVectorFloat3("pointLights[19].light.diffuse", glm::value_ptr(glm::vec3(0.5, 0.5, 1.0)));
		shaderMulLighting.setVectorFloat3("pointLights[19].light.specular", glm::value_ptr(glm::vec3(0.6, 0.0, 0.6)));
		shaderMulLighting.setFloat("pointLights[19].constant", aleatorio2);
		shaderMulLighting.setFloat("pointLights[19].linear", 0.0004);
		shaderMulLighting.setFloat("pointLights[19].quadratic", 0.00004);


		sphereLamp.setScale(glm::vec3(0.1, 0.1, 0.1));
		sphereLamp.setPosition(glm::vec3(-6.6, 4.0, 7.2));
		sphereLamp.setColor(glm::vec4(0.0, 0.0, 1.0, 1.0));
		sphereLamp.render();

		sphereLamp.setScale(glm::vec3(0.1, 0.1, 0.1));
		sphereLamp.setPosition(glm::vec3(-6.5, 3.8, 7.1));
		sphereLamp.setColor(glm::vec4(1.0, 0.0, 0.0, 1.0));
		sphereLamp.render();

		sphereLamp.setScale(glm::vec3(0.1, 0.1, 0.1));
		sphereLamp.setPosition(glm::vec3(-6.4, 3.6, 7.1));
		sphereLamp.setColor(glm::vec4(0.0, 0.0, 1.0, 1.0));
		sphereLamp.render();

		sphereLamp.setScale(glm::vec3(0.1, 0.1, 0.1));
		sphereLamp.setPosition(glm::vec3(-6.3, 3.4, 7.1));
		sphereLamp.setColor(glm::vec4(1.0, 0.0, 1.0, 1.0));
		sphereLamp.render();

		sphereLamp.setScale(glm::vec3(0.1, 0.1, 0.1));
		sphereLamp.setPosition(glm::vec3(-6.2, 3.2, 7.1));
		sphereLamp.setColor(glm::vec4(0.5, 0.5, 1.0, 1.0));
		sphereLamp.render();


		sphereLamp.setScale(glm::vec3(0.1, 0.1, 0.1));
		sphereLamp.setPosition(glm::vec3(-6.1, 3.0, 7.1));
		sphereLamp.setColor(glm::vec4(0.0, 0.0, 1.0, 1.0));
		sphereLamp.render();

		sphereLamp.setScale(glm::vec3(0.1, 0.1, 0.1));
		sphereLamp.setPosition(glm::vec3(-6.0, 3.2, 7.1));
		sphereLamp.setColor(glm::vec4(1.0, 0.0, 0.0, 1.0));
		sphereLamp.render();

		sphereLamp.setScale(glm::vec3(0.1, 0.1, 0.1));
		sphereLamp.setPosition(glm::vec3(-5.9, 3.4, 7.1));
		sphereLamp.setColor(glm::vec4(0.0, 0.0, 1.0, 1.0));
		sphereLamp.render();

		sphereLamp.setScale(glm::vec3(0.1, 0.1, 0.1));
		sphereLamp.setPosition(glm::vec3(-5.8, 3.6, 7.1));
		sphereLamp.setColor(glm::vec4(1.0, 0.0, 1.0, 1.0));
		sphereLamp.render();

		sphereLamp.setScale(glm::vec3(0.1, 0.1, 0.1));
		sphereLamp.setPosition(glm::vec3(-5.7, 3.8, 7.1));
		sphereLamp.setColor(glm::vec4(0.5, 0.5, 1.0, 1.0));
		sphereLamp.render();



		sphereLamp.setScale(glm::vec3(0.1, 0.1, 0.1));
		sphereLamp.setPosition(glm::vec3(-5.6, 4.0, 7.2));
		sphereLamp.setColor(glm::vec4(0.0, 0.0, 1.0, 1.0));
		sphereLamp.render();

		sphereLamp.setScale(glm::vec3(0.1, 0.1, 0.1));
		sphereLamp.setPosition(glm::vec3(5.5, 3.8, 7.1));
		sphereLamp.setColor(glm::vec4(1.0, 0.0, 0.0, 1.0));
		sphereLamp.render();

		sphereLamp.setScale(glm::vec3(0.1, 0.1, 0.1));
		sphereLamp.setPosition(glm::vec3(-5.4, 3.6, 7.1));
		sphereLamp.setColor(glm::vec4(0.0, 0.0, 1.0, 1.0));
		sphereLamp.render();

		sphereLamp.setScale(glm::vec3(0.1, 0.1, 0.1));
		sphereLamp.setPosition(glm::vec3(-5.3, 3.4, 7.1));
		sphereLamp.setColor(glm::vec4(1.0, 0.0, 1.0, 1.0));
		sphereLamp.render();

		sphereLamp.setScale(glm::vec3(0.1, 0.1, 0.1));
		sphereLamp.setPosition(glm::vec3(-5.2, 3.2, 7.1));
		sphereLamp.setColor(glm::vec4(0.5, 0.5, 1.0, 1.0));
		sphereLamp.render();


		sphereLamp.setScale(glm::vec3(0.1, 0.1, 0.1));
		sphereLamp.setPosition(glm::vec3(-5.1, 3.0, 7.1));
		sphereLamp.setColor(glm::vec4(0.0, 0.0, 1.0, 1.0));
		sphereLamp.render();

		sphereLamp.setScale(glm::vec3(0.1, 0.1, 0.1));
		sphereLamp.setPosition(glm::vec3(-5.0, 3.2, 7.1));
		sphereLamp.setColor(glm::vec4(1.0, 0.0, 0.0, 1.0));
		sphereLamp.render();

		sphereLamp.setScale(glm::vec3(0.1, 0.1, 0.1));
		sphereLamp.setPosition(glm::vec3(-4.9, 3.4, 7.1));
		sphereLamp.setColor(glm::vec4(0.0, 0.0, 1.0, 1.0));
		sphereLamp.render();

		sphereLamp.setScale(glm::vec3(0.1, 0.1, 0.1));
		sphereLamp.setPosition(glm::vec3(-4.8, 3.6, 7.1));
		sphereLamp.setColor(glm::vec4(1.0, 0.0, 1.0, 1.0));
		sphereLamp.render();

		sphereLamp.setScale(glm::vec3(0.1, 0.1, 0.1));
		sphereLamp.setPosition(glm::vec3(-4.7, 3.8, 7.1));
		sphereLamp.setColor(glm::vec4(0.5, 0.5, 1.0, 1.0));
		sphereLamp.render();



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
		sphereLamp.render(lightModelmatrix);//-----------------------------------------------------DESCOMENTAR LAMPARA SOL

		
		// PISO INTERIOR

			glm::mat4 modelCasa = glm::mat4(1.0f);

			//ZOTANO	
			
				glm::mat4 Zotano = glm::translate(modelCasa, glm::vec3(0.0, -3.0, 0.0));
			
				glm::mat4 ZotanoPiso = glm::translate(modelCasa, glm::vec3(0.0, -3.25, 0.0));
				glBindTexture(GL_TEXTURE_2D, IDtextuPiso1);
				shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(20.0, 20.0)));
				cubo.render(glm::scale(ZotanoPiso, glm::vec3(14.0, 0.1, 14.0)));
				glBindTexture(GL_TEXTURE_2D, 0);

			//PISOS
				glm::mat4 ObjPiso0A = glm::translate(modelCasa, glm::vec3(-1.5, 0.0, 0.0));
				glBindTexture(GL_TEXTURE_2D, IDtextuPiso1);
				shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(20.0, 20.0)));
				cubo.render(glm::scale(ObjPiso0A, glm::vec3(3.0, 0.01, 10.0)));
				glBindTexture(GL_TEXTURE_2D, 0);

					glm::mat4 ObjPiso0B = glm::translate(modelCasa, glm::vec3(-4.0, 0.0, 4.0));
					glBindTexture(GL_TEXTURE_2D, IDtextuPiso1);
					shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(20.0, 20.0)));
					cubo.render(glm::scale(ObjPiso0B, glm::vec3(2.0, 0.01, 2.0)));
					glBindTexture(GL_TEXTURE_2D, 0);

					glm::mat4 ObjPiso0C = glm::translate(modelCasa, glm::vec3(-4.0, 0.0, -3.5));
					glBindTexture(GL_TEXTURE_2D, IDtextuPiso1);
					shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(20.0, 20.0)));
					cubo.render(glm::scale(ObjPiso0C, glm::vec3(2.0, 0.01, 3.0)));
					glBindTexture(GL_TEXTURE_2D, 0);

					glm::mat4 ObjPiso0D = glm::translate(modelCasa, glm::vec3(-4.0, 0.0, -0.5));
					glBindTexture(GL_TEXTURE_2D, IDtextuPiso1);
					shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(20.0, 20.0)));
					cubo.render(glm::scale(ObjPiso0D, glm::vec3(2.0, 0.01, 3.0)));
					glBindTexture(GL_TEXTURE_2D, 0);

				glm::mat4 ObjPiso1 = glm::translate(modelCasa, glm::vec3(2.5, 0.0, -2.25));
				glBindTexture(GL_TEXTURE_2D, IDtextuPiso1);
				shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(20.0, 20.0)));
				cubo.render(glm::scale(ObjPiso1, glm::vec3(5.0, 0.01, 5.5)));
				glBindTexture(GL_TEXTURE_2D, 0);
		
				glm::mat4 ObjPiso2 = glm::translate(modelCasa,glm::vec3(2.5, -0.125, 2.75));
				glBindTexture(GL_TEXTURE_2D, IDtextuPiso1);
				shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(20.0, 20.0)));
				cubo.render(glm::scale(ObjPiso2, glm::vec3(5.0, 0.001, 4.5)));
				glBindTexture(GL_TEXTURE_2D, 0);

				glm::mat4 ObjEscA1 = glm::translate(modelCasa, glm::vec3(0.0, -0.0625, 2.5));
				glBindTexture(GL_TEXTURE_2D, IDtextuPiso1);
				shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(4.0, 1.0)));
				cubo.render(glm::scale(ObjEscA1, glm::vec3(0.0, 0.125, 5.0)));
				glBindTexture(GL_TEXTURE_2D, 0);
		
				glm::mat4 ObjEscB1 = glm::translate(modelCasa, glm::vec3(2.5, -0.0625, 0.5));
				glBindTexture(GL_TEXTURE_2D, IDtextuPiso1);
				shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(4.0, 1.0)));
				cubo.render(glm::scale(ObjEscB1, glm::vec3(5.0, 0.125, 0.0)));
				glBindTexture(GL_TEXTURE_2D, 0);

			//PAREDES PIso 1
				glm::mat4 ObjPared1 = glm::translate(modelCasa, glm::vec3(-4.9, 1.375, 0.0));
				glBindTexture(GL_TEXTURE_2D, IDtextuFachada);
				shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(10.0, 7.0)));
				cubo.render(glm::scale(ObjPared1, glm::vec3(0.2, 3.25, 10.0)));
				glBindTexture(GL_TEXTURE_2D, 0);

				glm::mat4 ObjPared2 = glm::translate(modelCasa, glm::vec3(0.0, 1.375, -4.9));
				glBindTexture(GL_TEXTURE_2D, IDtextuFachada);
				shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(10.0, 7.0)));
				cubo.render(glm::scale(ObjPared2, glm::vec3(10.0, 3.25, 0.2)));
				glBindTexture(GL_TEXTURE_2D, 0);

				glm::mat4 ObjPared3 = glm::translate(modelCasa, glm::vec3(4.9, 1.375, -2.25));
				glBindTexture(GL_TEXTURE_2D, IDtextuFachada);
				shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(10.0, 7.0)));
				cubo.render(glm::scale(ObjPared3, glm::vec3(0.2, 3.25, 5.5)));
				glBindTexture(GL_TEXTURE_2D, 0);

				glm::mat4 ObjPared3B = glm::translate(modelCasa, glm::vec3(4.9, 1.375, 2.75));
				glBindTexture(GL_TEXTURE_2D, IDtextuFachada);
				shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(10.0, 7.0)));
				cubo.render(glm::scale(ObjPared3B, glm::vec3(0.2, 3.25, 4.5)));
				glBindTexture(GL_TEXTURE_2D, 0);

				glm::mat4 ObjPared4A = glm::translate(modelCasa, glm::vec3(2.5, 1.375, 4.9));
				glBindTexture(GL_TEXTURE_2D, IDtextuFachada);
				shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(10.0, 7.0)));
				cubo.render(glm::scale(ObjPared4A, glm::vec3(5.0, 3.25, 0.2)));
				glBindTexture(GL_TEXTURE_2D, 0);

				glm::mat4 ObjPared4B = glm::translate(modelCasa, glm::vec3(-1.0, 1.5, 4.9));
				glBindTexture(GL_TEXTURE_2D, IDtextuFachada);
				shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(10.0, 7.0)));
				cubo.render(glm::scale(ObjPared4B, glm::vec3(2.0, 3.0, 0.2)));
				glBindTexture(GL_TEXTURE_2D, 0);

				glm::mat4 ObjPared4C = glm::translate(modelCasa, glm::vec3(-4.0, 1.5, 4.9));
				glBindTexture(GL_TEXTURE_2D, IDtextuFachada);
				shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(10.0, 7.0)));
				cubo.render(glm::scale(ObjPared4C, glm::vec3(2.0, 3.0, 0.2)));
				glBindTexture(GL_TEXTURE_2D, 0);

				glm::mat4 ObjPared4D = glm::translate(modelCasa, glm::vec3(-2.5, 2.43, 4.9));
				glBindTexture(GL_TEXTURE_2D, IDtextuFachada);
				shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(2.0, 3.0)));
				cubo.render(glm::scale(ObjPared4D, glm::vec3(1.0, 1.15, 0.2)));
				glBindTexture(GL_TEXTURE_2D, 0);

				glm::mat4 ObjParedBano1 = glm::translate(modelCasa, glm::vec3(-3.0, 1.5, -4.1));
				glBindTexture(GL_TEXTURE_2D, IDtextuFachada);
				shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(2.0, 3.0)));
				cubo.render(glm::scale(ObjParedBano1, glm::vec3(0.2, 3.0, 1.8)));
				glBindTexture(GL_TEXTURE_2D, 0);

				glm::mat4 ObjParedBano2 = glm::translate(modelCasa, glm::vec3(-3.95, 1.5, -2.1));
				glBindTexture(GL_TEXTURE_2D, IDtextuFachada);
				shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(2.0, 3.0)));
				cubo.render(glm::scale(ObjParedBano2, glm::vec3(2.1, 3.0, 0.2)));
				glBindTexture(GL_TEXTURE_2D, 0);

				glm::mat4 ObjParedBano3 = glm::translate(modelCasa, glm::vec3(-3.0, 2.43, -2.7));
				glBindTexture(GL_TEXTURE_2D, IDtextuFachada);
				shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(2.0, 3.0)));
				cubo.render(glm::scale(ObjParedBano3, glm::vec3(0.2, 1.15, 1.2)));
				glBindTexture(GL_TEXTURE_2D, 0);

				glm::mat4 ObjParedEscaA = glm::translate(modelCasa, glm::vec3(-3.1, 1.5, 0.25));
				glBindTexture(GL_TEXTURE_2D, IDtextuFachada);
				shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(2.0, 3.0)));
				cubo.render(glm::scale(ObjParedEscaA, glm::vec3(0.2, 3.0, 1.5)));
				glBindTexture(GL_TEXTURE_2D, 0);



		// COUMNA CENTRAL
			glm::mat4 ObjColumCen = glm::translate(modelCasa, glm::vec3(0.0, -0.125, 0.5));
			glBindTexture(GL_TEXTURE_2D, IDtextureMarmol);
			shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(1.0, 1.0)));
			cubo.render(glm::scale(ObjColumCen, glm::vec3(0.5, 6.25, 0.5)));
			glBindTexture(GL_TEXTURE_2D, 0); 

		//TECHO 
			/*glm::mat4 ObjTecho = glm::translate(modelCasa, glm::vec3(0-0, 3.0, 0.0));
			glBindTexture(GL_TEXTURE_2D, IDtextureMarmol);
			shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(2.0, 2.0)));
			cubo.render(glm::scale(ObjTecho, glm::vec3(10.0, 0.05, 10.0)));
			glBindTexture(GL_TEXTURE_2D, 0); */


		//PARTE EXTERIOR DE LA CASA
			glm::mat4 ObjPisoFrente = glm::translate(modelCasa, glm::vec3(0.0, -0.5, 6.0));
			glBindTexture(GL_TEXTURE_2D, IDtextuMadera1);
			shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(8.0, 8.0)));
			cubo.render(glm::scale(ObjPisoFrente, glm::vec3(10.0, 1.0, 2.0)));
			glBindTexture(GL_TEXTURE_2D, 0); 

			glm::mat4 ObjPisoDer = glm::translate(modelCasa, glm::vec3(-6.0, -0.5, 0.0));
			glBindTexture(GL_TEXTURE_2D, IDtextuMadera1);
			shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(8.0, 8.0)));
			cubo.render(glm::scale(ObjPisoDer, glm::vec3(2.0, 1.0, 14.0)));
			glBindTexture(GL_TEXTURE_2D, 0);

			glm::mat4 ObjPisoAtras = glm::translate(modelCasa, glm::vec3(0.0, -0.5, -6.0));
			glBindTexture(GL_TEXTURE_2D, IDtextuMadera1);
			shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(8.0, 8.0)));
			cubo.render(glm::scale(ObjPisoAtras, glm::vec3(10.0, 1.0, 2.0)));
			glBindTexture(GL_TEXTURE_2D, 0);

			glm::mat4 ObjPisoIzq = glm::translate(modelCasa, glm::vec3(6.0, -0.5, 0.0));
			glBindTexture(GL_TEXTURE_2D, IDtextuMadera1);
			shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(8.0, 8.0)));
			cubo.render(glm::scale(ObjPisoIzq, glm::vec3(2.0, 1.0, 14.0)));
			glBindTexture(GL_TEXTURE_2D, 0);

			glm::mat4 ObjPillar1 = glm::translate(modelCasa,glm::vec3(-6.8, 1.5,6.8));
			glBindTexture(GL_TEXTURE_2D, IDtextureMarmol);
			shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(2.0, 2.0)));
			cubo.render(glm::scale(ObjPillar1, glm::vec3(0.2, 3.0, 0.2)));

			glm::mat4 ObjPillar2 = glm::translate(modelCasa, glm::vec3(-6.8, 1.5, -6.8));
			glBindTexture(GL_TEXTURE_2D, IDtextureMarmol);
			shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(2.0, 2.0)));
			cubo.render(glm::scale(ObjPillar2, glm::vec3(0.2, 3.0, 0.2)));

			glm::mat4 ObjPillar3 = glm::translate(modelCasa, glm::vec3(6.8, 1.5, -6.8));
			glBindTexture(GL_TEXTURE_2D, IDtextureMarmol);
			shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(2.0, 2.0)));
			cubo.render(glm::scale(ObjPillar3, glm::vec3(0.2, 3.0, 0.2)));

			glm::mat4 ObjPillar4 = glm::translate(modelCasa, glm::vec3(6.8, 1.5, 6.8));
			glBindTexture(GL_TEXTURE_2D, IDtextureMarmol);
			shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(2.0, 2.0)));
			cubo.render(glm::scale(ObjPillar4, glm::vec3(0.2, 3.0, 0.2)));

				//2 PISO

			glm::mat4 Obj2PisoSueloA = glm::translate(modelCasa, glm::vec3(3.5, 3.125, 0.0));
			cubo.render(glm::scale(Obj2PisoSueloA, glm::vec3(7.0, 0.25, 13.95)));

			glm::mat4 Obj2PisoSueloB = glm::translate(modelCasa, glm::vec3(-1.7, 3.125, 0.0));
			cubo.render(glm::scale(Obj2PisoSueloB, glm::vec3(3.4, 0.25, 13.95)));

			glm::mat4 Obj2PisoSueloC = glm::translate(modelCasa, glm::vec3(-5.2, 3.125, 4.25));
			cubo.render(glm::scale(Obj2PisoSueloC, glm::vec3(3.59, 0.25, 4.95)));

			glm::mat4 Obj2PisoSueloD = glm::translate(modelCasa, glm::vec3(-5.0, 3.125, -4.5));
			cubo.render(glm::scale(Obj2PisoSueloD, glm::vec3(3.95, 0.25, 5.0)));

			glm::mat4 Obj2PisoSueloE = glm::translate(modelCasa, glm::vec3(-5.9, 3.125, 0.0));
			cubo.render(glm::scale(Obj2PisoSueloE, glm::vec3(2.19, 0.25, 4.0)));

			glm::mat4 SegundoPiso = glm::translate(modelCasa, glm::vec3(0.0, 3.0, 0.0));

			glm::mat4 Obj2PisoPared1 = glm::translate(SegundoPiso, glm::vec3(-6.9, 1.5, 0.0));
			glBindTexture(GL_TEXTURE_2D, IDtextuFachada);
			shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(10.0, 7.0)));
			cubo.render(glm::scale(Obj2PisoPared1, glm::vec3(0.2, 3.0, 14.0)));
			glBindTexture(GL_TEXTURE_2D, 0);

			//PARED 2 PISO 2
			glm::mat4 Obj2PisoPared2 = glm::translate(SegundoPiso, glm::vec3(0.0, 1.5, -6.9));
			glBindTexture(GL_TEXTURE_2D, IDtextuFachada);
			shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(10.0, 7.0)));
			cubo.render(glm::scale(Obj2PisoPared2, glm::vec3(14.0, 3.0, 0.2)));
			glBindTexture(GL_TEXTURE_2D, 0);

			glm::mat4 Obj2PisoPared3 = glm::translate(SegundoPiso, glm::vec3(6.9, 0.6, 0.0));
			glBindTexture(GL_TEXTURE_2D, IDtextuFachada);
			shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(10.0, 7.0)));
			cubo.render(glm::scale(Obj2PisoPared3, glm::vec3(0.2, 1.2, 14.0)));
			glBindTexture(GL_TEXTURE_2D, 0);
			
			//PARED 4 2 PISO
				glm::mat4 Obj2PisoPared4A = glm::translate(SegundoPiso, glm::vec3(0.0, 0.5, 6.9));
				glBindTexture(GL_TEXTURE_2D, IDtextuFachada);
				shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(10.0, 7.0)));
				cubo.render(glm::scale(Obj2PisoPared4A, glm::vec3(14.0, 1.0, 0.2)));
				glBindTexture(GL_TEXTURE_2D, 0);
			
				glm::mat4 Obj2PisoPared4B = glm::translate(SegundoPiso, glm::vec3(-6.8, 2.0, 6.9));
				glBindTexture(GL_TEXTURE_2D, IDtextuFachada);
				shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(10.0, 7.0)));
				cubo.render(glm::scale(Obj2PisoPared4B, glm::vec3(0.4, 2.0, 0.2)));
				glBindTexture(GL_TEXTURE_2D, 0);

				glm::mat4 Obj2PisoPared4C = glm::translate(SegundoPiso, glm::vec3(-3.5, 2.7, 6.9));
				glBindTexture(GL_TEXTURE_2D, IDtextuFachada);
				shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(10.0, 7.0)));
				cubo.render(glm::scale(Obj2PisoPared4C, glm::vec3(7.0, 0.6, 0.2)));
				glBindTexture(GL_TEXTURE_2D, 0);

				glm::mat4 Obj2PisoPared4D = glm::translate(SegundoPiso, glm::vec3(0.0, 1.7, 6.9));
				glBindTexture(GL_TEXTURE_2D, IDtextuFachada);
				shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(10.0, 7.0)));
				cubo.render(glm::scale(Obj2PisoPared4D, glm::vec3(4.0, 1.4, 0.2)));
				glBindTexture(GL_TEXTURE_2D, 0);

				glm::mat4 Obj2PisoPared4E = glm::translate(SegundoPiso, glm::vec3(3.0, 1.5, 6.9));
				glBindTexture(GL_TEXTURE_2D, IDtextuFachada);
				shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(10.0, 7.0)));
				cubo.render(glm::scale(Obj2PisoPared4E, glm::vec3(2.0, 1.0, 0.2)));
				glBindTexture(GL_TEXTURE_2D, 0);

				glm::mat4 Obj2PisoPared4F = glm::translate(SegundoPiso, glm::vec3(5.5, 1.1, 6.9));
				glBindTexture(GL_TEXTURE_2D, IDtextuFachada);
				shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(10.0, 7.0)));
				cubo.render(glm::scale(Obj2PisoPared4F, glm::vec3(3.0, 0.2, 0.2)));
				glBindTexture(GL_TEXTURE_2D, 0);

				glm::mat4 Obj2PisoPared4G = glm::translate(SegundoPiso, glm::vec3(3.39, 1.7, 6.9));
				Obj2PisoPared4G = glm::rotate(Obj2PisoPared4G, glm::radians(-15.0f), glm::vec3(0.0, 0.0, 1.0));
				glBindTexture(GL_TEXTURE_2D, IDtextuFachada);
				shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(10.0, 7.0)));
				cubo.render(glm::scale(Obj2PisoPared4G, glm::vec3(7.25, 0.9, 0.2)));
				glBindTexture(GL_TEXTURE_2D, 0);

			//Pared 5  MODIFICAR
				glm::mat4 Obj2PisoPared5 = glm::translate(SegundoPiso, glm::vec3(6.9, 1.5, -6.0));
				glBindTexture(GL_TEXTURE_2D, IDtextuFachada);
				shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(10.0, 7.0)));
				cubo.render(glm::scale(Obj2PisoPared5, glm::vec3(0.2, 3.0, 2.0)));
				glBindTexture(GL_TEXTURE_2D, 0);

				glm::mat4 Obj2PisoParedInte1 = glm::translate(SegundoPiso, glm::vec3(3.5, 1.5, -5.0));
				glBindTexture(GL_TEXTURE_2D, IDtextuFachada);
				shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(10.0, 7.0)));
				cubo.render(glm::scale(Obj2PisoParedInte1, glm::vec3(7.0, 3.0, 0.1)));
				glBindTexture(GL_TEXTURE_2D, 0);

		//TERRAZA
					
				glm::mat4 Terraza = glm::translate(SegundoPiso, glm::vec3(-7.0, 3.0, -7.0));
				
				glm::mat4 ObjTerraSuelo = glm::translate(Terraza, glm::vec3(3.5, 0.0, 7.0));
				glBindTexture(GL_TEXTURE_2D, IDtextureMarmol);
				shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(10.0, 7.0)));
				cubo.render(glm::scale(ObjTerraSuelo, glm::vec3(7.0, 0.05, 14.0)));
				glBindTexture(GL_TEXTURE_2D, 0);

				glm::mat4 ObjTerraSuelo2 = glm::translate(Terraza, glm::vec3(10.5, 0.0, 1.0));
				glBindTexture(GL_TEXTURE_2D, IDtextureMarmol);
				shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(10.0, 7.0)));
				cubo.render(glm::scale(ObjTerraSuelo2, glm::vec3(7.0, 0.05, 2.0)));
				glBindTexture(GL_TEXTURE_2D, 0);

				glm::mat4 Obj2TerraSuelo3 = glm::translate(SegundoPiso, glm::vec3(3.39, 1.7, 4.25));
				Obj2TerraSuelo3 = glm::rotate(Obj2TerraSuelo3, glm::radians(-15.0f), glm::vec3(0.0, 0.0, 1.0));
				glBindTexture(GL_TEXTURE_2D, IDtextuFachada);
				Obj2TerraSuelo3 = glm::translate(Obj2TerraSuelo3, glm::vec3(0.0, 0.4, 0.0));
				shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(10.0, 7.0)));
				cubo.render(glm::scale(Obj2TerraSuelo3, glm::vec3(7.25, 0.1, 5.5)));
				glBindTexture(GL_TEXTURE_2D, 0);

				glm::mat4 Obj2TerraSuelo4 = glm::translate(Obj2TerraSuelo3, glm::vec3(-2.75, 0.0, -6.0));
				glBindTexture(GL_TEXTURE_2D, IDtextuFachada);
				//Obj2TerraSuelo4 = glm::translate(Obj2TerraSuelo4, glm::vec3(-2.75, 0.4, -6.0));
				shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(10.0, 7.0)));
				cubo.render(glm::scale(Obj2TerraSuelo4, glm::vec3(1.8, 0.1, 6.5)));
				glBindTexture(GL_TEXTURE_2D, 0);

				glm::mat4 Obj2TerraSuelo5 = glm::translate(Obj2TerraSuelo4, glm::vec3(5.5, 0.0, 0.0));
				glBindTexture(GL_TEXTURE_2D, IDtextuFachada);
				//Obj2TerraSuelo4 = glm::translate(Obj2TerraSuelo4, glm::vec3(-2.75, 0.4, -6.0));
				shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(10.0, 7.0)));
				cubo.render(glm::scale(Obj2TerraSuelo5, glm::vec3(1.8, 0.1, 6.5)));
				glBindTexture(GL_TEXTURE_2D, 0);

				glm::mat4 VentanaTerraza = glm::translate(Obj2TerraSuelo3, glm::vec3(0.0, 0.0, -6.0));
				cuboAzul.render(glm::scale(VentanaTerraza, glm::vec3(3.8, 0.08, 7.0)));
	
				glm::mat4 ObjTerraTubo1 = glm::translate(ObjTerraSuelo, glm::vec3(-3.25, 0.5, -6.75));
				glBindTexture(GL_TEXTURE_2D, IDtextureMetal);
				shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(1.0, 1.0)));
				tubo.render(glm::scale(ObjTerraTubo1, glm::vec3(0.25, 1.0, 0.25)));
				glBindTexture(GL_TEXTURE_2D, 0);

				glm::mat4 ObjTerraTubo2 = glm::translate(ObjTerraSuelo, glm::vec3(3.25, 0.5, -6.75));
				glBindTexture(GL_TEXTURE_2D, IDtextureMetal);
				shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(1.0, 1.0)));
				tubo.render(glm::scale(ObjTerraTubo2, glm::vec3(0.25, 1.0, 0.25)));
				glBindTexture(GL_TEXTURE_2D, 0);

				glm::mat4 ObjTerraTubo3 = glm::translate(ObjTerraSuelo, glm::vec3(-3.25, 0.5, 6.75));
				glBindTexture(GL_TEXTURE_2D, IDtextureMetal);
				shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(1.0, 1.0)));
				tubo.render(glm::scale(ObjTerraTubo3, glm::vec3(0.25, 1.0, 0.25)));
				glBindTexture(GL_TEXTURE_2D, 0);

				glm::mat4 ObjTerraTubo4 = glm::translate(ObjTerraSuelo, glm::vec3(3.25, 0.5, 6.75));
				glBindTexture(GL_TEXTURE_2D, IDtextureMetal);
				shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(1.0, 1.0)));
				tubo.render(glm::scale(ObjTerraTubo4, glm::vec3(0.25, 1.0, 0.25)));
				glBindTexture(GL_TEXTURE_2D, 0);

				glm::mat4 ObjTerraTubo5 = glm::translate(ObjTerraSuelo, glm::vec3(3.25, 0.5, -4.75));
				glBindTexture(GL_TEXTURE_2D, IDtextureMetal);
				shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(1.0, 1.0)));
				tubo.render(glm::scale(ObjTerraTubo5, glm::vec3(0.25, 1.0, 0.25)));
				glBindTexture(GL_TEXTURE_2D, 0);

				glm::mat4 ObjTerraCriz1 = glm::translate(ObjTerraTubo1, glm::vec3(3.25, 0.0, 0.0));
				glBindTexture(GL_TEXTURE_2D, IDtextureCristal);
				shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(1.0, 1.0)));
				cubo.render(glm::scale(ObjTerraCriz1, glm::vec3(6.5, 0.7, 0.2)));
				glBindTexture(GL_TEXTURE_2D, 0);

				glm::mat4 ObjTerraCriz2 = glm::translate(ObjTerraTubo1, glm::vec3(0.0, 0.0, 6.75));
				glBindTexture(GL_TEXTURE_2D, IDtextureCristal);
				shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(1.0, 1.0)));
				cubo.render(glm::scale(ObjTerraCriz2, glm::vec3(0.2, 0.7, 13.5)));
				glBindTexture(GL_TEXTURE_2D, 0);

				glm::mat4 ObjTerraCriz3 = glm::translate(ObjTerraTubo3, glm::vec3(3.25, 0.0, 0.0));
				glBindTexture(GL_TEXTURE_2D, IDtextureCristal);
				shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(1.0, 1.0)));
				cubo.render(glm::scale(ObjTerraCriz3, glm::vec3(6.5, 0.7, 0.2)));
				glBindTexture(GL_TEXTURE_2D, 0);

				glm::mat4 ObjTerraCriz4 = glm::translate(ObjTerraTubo2, glm::vec3(0.0, 0.0, 1.0));
				glBindTexture(GL_TEXTURE_2D, IDtextureCristal);
				shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(1.0, 1.0)));
				cubo.render(glm::scale(ObjTerraCriz4, glm::vec3(0.2, 0.7, 2.0)));
				glBindTexture(GL_TEXTURE_2D, 0);

				glm::mat4 ObjTerraCriz5 = glm::translate(ObjTerraTubo5, glm::vec3(0.0, 0.0, 5.75));
				glBindTexture(GL_TEXTURE_2D, IDtextureCristal);
				shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(1.0, 1.0)));
				cubo.render(glm::scale(ObjTerraCriz5, glm::vec3(0.2, 0.7, 12.0)));
				glBindTexture(GL_TEXTURE_2D, 0);


				//CARRETERA
				
				glm::mat4 matCarretera = glm::mat4(1.0);
				matCarretera = glm::translate(matCarretera, glm::vec3(0.0, -0.8, 20.0));
				glBindTexture(GL_TEXTURE_2D, IDtextuCarretera);
				shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(1.0, 1.0)));
				cubo.render(glm::scale(matCarretera, glm::vec3(20.0, 0.01, 5.0)));
				glBindTexture(GL_TEXTURE_2D, 0);

				glm::mat4 matCarretera0 = glm::mat4(1.0);
				matCarretera0 = glm::translate(matCarretera, glm::vec3(-20.0, 0.0, 0.0));
				glBindTexture(GL_TEXTURE_2D, IDtextuCarretera);
				shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(1.0, 1.0)));
				cubo.render(glm::scale(matCarretera0, glm::vec3(20.0, 0.01, 5.0)));
				glBindTexture(GL_TEXTURE_2D, 0);
				
				glm::mat4 matCarreteraCruce0 = glm::mat4(1.0);
				matCarreteraCruce0 = glm::translate(matCarretera0, glm::vec3(-12.5, 0.0, 0.0));
				glBindTexture(GL_TEXTURE_2D, IDtextureAsfalto);
				shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(1.0, 1.0)));
				cubo.render(glm::scale(matCarreteraCruce0, glm::vec3(5.0, 0.01, 5.0)));
				glBindTexture(GL_TEXTURE_2D, 0);


				glm::mat4 matCarretera2 = glm::mat4(1.0);
				matCarretera2 = glm::translate(matCarretera, glm::vec3(20.0, 0.0, 0.0));
				glBindTexture(GL_TEXTURE_2D, IDtextuCarretera);
				shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(1.0, 1.0)));
				cubo.render(glm::scale(matCarretera2, glm::vec3(20.0, 0.01, 5.0)));
				glBindTexture(GL_TEXTURE_2D, 0);

				glm::mat4 matCarreteraCruce = glm::mat4(1.0);
				matCarreteraCruce = glm::translate(matCarretera2, glm::vec3(12.5, 0.0, 0.0));
				glBindTexture(GL_TEXTURE_2D, IDtextureAsfalto);
				shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(1.0, 1.0)));
				cubo.render(glm::scale(matCarreteraCruce, glm::vec3(5.0, 0.01, 5.0)));
				glBindTexture(GL_TEXTURE_2D, 0);

				glm::mat4 matCarretera3 = glm::mat4(1.0);
				matCarretera3 = glm::rotate(matCarreteraCruce, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
				matCarretera3 = glm::translate(matCarretera3, glm::vec3(12.5, 0.0, 0.0));
				glBindTexture(GL_TEXTURE_2D, IDtextuCarretera);
				shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(1.0, 1.0)));
				cubo.render(glm::scale(matCarretera3, glm::vec3(20.0, 0.01, 5.0)));
				glBindTexture(GL_TEXTURE_2D, 0);

				glm::mat4 matCarretera4 = glm::mat4(1.0);
				matCarretera4 = glm::translate(matCarretera3, glm::vec3(20.0, 0.0, 0.0));
				glBindTexture(GL_TEXTURE_2D, IDtextuCarretera);
				shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(1.0, 1.0)));
				cubo.render(glm::scale(matCarretera4, glm::vec3(20.0, 0.01, 5.0)));
				glBindTexture(GL_TEXTURE_2D, 0);

				glm::mat4 matCarreteraCruce2 = glm::mat4(1.0);
				matCarreteraCruce2 = glm::translate(matCarretera4, glm::vec3(12.5, 0.0, 0.0));
				glBindTexture(GL_TEXTURE_2D, IDtextureAsfalto);
				shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(1.0, 1.0)));
				cubo.render(glm::scale(matCarreteraCruce2, glm::vec3(5.0, 0.01, 5.0)));
				glBindTexture(GL_TEXTURE_2D, 0);

				glm::mat4 matCarretera5 = glm::mat4(1.0);
				matCarretera5 = glm::rotate(matCarreteraCruce2, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
				matCarretera5 = glm::translate(matCarretera5, glm::vec3(12.5, 0.0, 0.0));
				glBindTexture(GL_TEXTURE_2D, IDtextuCarretera);
				shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(1.0, 1.0)));
				cubo.render(glm::scale(matCarretera5, glm::vec3(20.0, 0.01, 5.0)));
				glBindTexture(GL_TEXTURE_2D, 0);

				glm::mat4 matCarretera6 = glm::mat4(1.0);
				matCarretera6 = glm::translate(matCarretera5, glm::vec3(20.0, 0.0, 0.0));
				glBindTexture(GL_TEXTURE_2D, IDtextuCarretera);
				shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(1.0, 1.0)));
				cubo.render(glm::scale(matCarretera6, glm::vec3(20.0, 0.01, 5.0)));
				glBindTexture(GL_TEXTURE_2D, 0);
				
				glm::mat4 matCarretera7 = glm::mat4(1.0);
				matCarretera7 = glm::translate(matCarretera6, glm::vec3(20.0, 0.0, 0.0));
				glBindTexture(GL_TEXTURE_2D, IDtextuCarretera);
				shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(1.0, 1.0)));
				cubo.render(glm::scale(matCarretera7, glm::vec3(20.0, 0.01, 5.0)));
				glBindTexture(GL_TEXTURE_2D, 0);

				glm::mat4 matCarreteraCruce3 = glm::mat4(1.0);
				matCarreteraCruce3 = glm::translate(matCarretera7, glm::vec3(12.5, 0.0, 0.0));
				glBindTexture(GL_TEXTURE_2D, IDtextureAsfalto);
				shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(1.0, 1.0)));
				cubo.render(glm::scale(matCarreteraCruce3, glm::vec3(5.0, 0.01, 5.0)));
				glBindTexture(GL_TEXTURE_2D, 0);

				glm::mat4 matCarretera8 = glm::mat4(1.0);
				matCarretera8 = glm::rotate(matCarreteraCruce3, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
				matCarretera8 = glm::translate(matCarretera8, glm::vec3(12.5, 0.0, 0.0));
				glBindTexture(GL_TEXTURE_2D, IDtextuCarretera);
				shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(1.0, 1.0)));
				cubo.render(glm::scale(matCarretera8, glm::vec3(20.0, 0.01, 5.0)));
				glBindTexture(GL_TEXTURE_2D, 0);

				glm::mat4 matCarretera9 = glm::mat4(1.0);
				matCarretera9 = glm::translate(matCarretera8, glm::vec3(20.0, 0.0, 0.0));
				glBindTexture(GL_TEXTURE_2D, IDtextuCarretera);
				shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(1.0, 1.0)));
				cubo.render(glm::scale(matCarretera9, glm::vec3(20.0, 0.01, 5.0)));
				glBindTexture(GL_TEXTURE_2D, 0);

		// ACCESORIOS

			glm::mat4 ObjRefri = glm::mat4(1.0);
			ObjRefri = glm::translate(ObjRefri, glm::vec3(0.0, 0.0, -4.7));
			ObjRefri = glm::scale(ObjRefri, glm::vec3(0.01, 0.01, 0.01));
			shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(1, 1)));
			modelRefri.render(ObjRefri);
			//FORCE TO ENABLE THE UNIT TEXTURE TO 0 ALWAYS .............. IMPORTANT
			glActiveTexture(GL_TEXTURE0);
				
			modelCarroEclip1.render(ObjCarroEclipse);
			//FORCE TO ENABLE THE UNIT TEXTURE TO 0 ALWAYS .............. IMPORTANT
			glActiveTexture(GL_TEXTURE0);

			modelHelico.render(matHelico);
			//FORCE TO ENABLE THE UNIT TEXTURE TO 0 ALWAYS .............. IMPORTANT
			glActiveTexture(GL_TEXTURE0);

			glm::mat4 matDino = glm::mat4(1.0);
			matDino = glm::rotate(matDino, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
			matDino = glm::translate(matDino, glm::vec3(-2.6, 0.0, -2.6));
			matDino = glm::scale(matDino, glm::vec3(1.5, 1.0, 1.0));
			shaderMulLighting.setFloat("offsetX", 0);
			shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(1,1)));
			modelDinosaurio.render(matDino);
			//FORCE TO ENABLE THE UNIT TEXTURE TO 0 ALWAYS .............. IMPORTANT
			glActiveTexture(GL_TEXTURE0);

			glm::mat4 ObjDoor1 = glm::mat4(1.0);
			ObjDoor1 = glm::translate(ObjDoor1, glm::vec3(-2.5, 0.0, 4.9));
			ObjDoor1 = glm::scale(ObjDoor1, glm::vec3(0.02, 0.02, 0.04));
			shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(1, 1)));
			modelDoor1.render(ObjDoor1);
			//FORCE TO ENABLE THE UNIT TEXTURE TO 0 ALWAYS .............. IMPORTANT
			glActiveTexture(GL_TEXTURE0);

			//Baño planta baja
			glm::mat4 ObjDoor2 = glm::mat4(1.0);
			ObjDoor2 = glm::rotate(ObjDoor2, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
			ObjDoor2 = glm::translate(ObjDoor2, glm::vec3(2.7, 0.0, -3.0));
			ObjDoor2 = glm::scale(ObjDoor2, glm::vec3(0.02, 0.02, 0.04));
			shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(1, 1)));
			modelDoor1.render(ObjDoor2);
			//FORCE TO ENABLE THE UNIT TEXTURE TO 0 ALWAYS .............. IMPORTANT
			glActiveTexture(GL_TEXTURE0);

			glm::mat4 matModelChimenea = glm::mat4(1.0);
			matModelChimenea = glm::translate(modelCasa, glm::vec3(0, 0, 0)); //2.5, 4.8, -0.25
			matModelChimenea = glm::scale(matModelChimenea, glm::vec3(1.0, 1.0, 1.0));
			modelChimenea.render(matModelChimenea);
			//FORCE TO ENABLE THE UNIT TEXTURE TO 0 ALWAYS .............. IMPORTANT
			glActiveTexture(GL_TEXTURE0);

			glm::mat4 matSofaH = glm::mat4(1.0);
			matSofaH = glm::rotate(matSofaH, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
			matSofaH = glm::translate(matSofaH, glm::vec3(0.9, -3.5, -0.125)); //2.5, 4.8, -0.25
			matSofaH = glm::scale(matSofaH, glm::vec3(0.0009, 0.0009, 0.0009));
			modelSofaHolly.render(matSofaH);
			//FORCE TO ENABLE THE UNIT TEXTURE TO 0 ALWAYS .............. IMPORTANT
			glActiveTexture(GL_TEXTURE0);

			glm::mat4 matEscalera = glm::mat4(1.0);
			matEscalera = glm::rotate(matEscalera, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
			matEscalera = glm::translate(matEscalera, glm::vec3(-0.0, 0.0, -3.0));
			matEscalera = glm::scale(matEscalera, glm::vec3(0.6, 0.6, 0.6));
			modelEscalera.render(matEscalera);
			//FORCE TO ENABLE THE UNIT TEXTURE TO 0 ALWAYS .............. IMPORTANT
			glActiveTexture(GL_TEXTURE0);

			glm::mat4 matEscaleraZotano = glm::mat4(1.0);
			matEscaleraZotano = glm::rotate(matEscaleraZotano, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
			matEscaleraZotano = glm::translate(matEscaleraZotano, glm::vec3(-1.25, -3.25, -3.0));
			matEscaleraZotano = glm::scale(matEscaleraZotano, glm::vec3(0.6, 0.6, 0.6));
			modelEscalera.render(matEscaleraZotano);
			//FORCE TO ENABLE THE UNIT TEXTURE TO 0 ALWAYS .............. IMPORTANT
			glActiveTexture(GL_TEXTURE0);

			/*glm::mat4 matCoci = glm::mat4(1.0);
			//matCocina = glm::rotate(matCocina, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
			matCoci = glm::translate(matCoci, glm::vec3(0.0, 0.0, 0.0));
			matCoci = glm::scale(matCoci, glm::vec3(0.0001, 0.0001, 0.0001));
			modelCocina.render(matCoci);
			//FORCE TO ENABLE THE UNIT TEXTURE TO 0 ALWAYS .............. IMPORTANT
			glActiveTexture(GL_TEXTURE0);*/

			glm::mat4 matTv1 = glm::mat4(1.0);
			matTv1 = glm::translate(matTv1, glm::vec3(2.5, 1.6, 4.8));
			matTv1 = glm::scale(matTv1, glm::vec3(2.5, 1.25, 0.05));
			glBindTexture(GL_TEXTURE_2D, IDTextureTv);
			shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(1.0, 1.0)));
			cubo.render(matTv1);
			//FORCE TO ENABLE THE UNIT TEXTURE TO 0 ALWAYS .............. IMPORTANT
			glActiveTexture(GL_TEXTURE0);

			glm::mat4 matCesped1 = glm::mat4(1.0);
			matCesped1 = glm::translate(matCesped1, glm::vec3(0.0, -1.0, -17.0));
			matCesped1 = glm::scale(matCesped1, glm::vec3(40.0, 0.25, 20.0));
			glBindTexture(GL_TEXTURE_2D, IDTextureCesped);
			shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(2.0, 2.0)));
			cubo.render(matCesped1);
			//FORCE TO ENABLE THE UNIT TEXTURE TO 0 ALWAYS .............. IMPORTANT
			glActiveTexture(GL_TEXTURE0);

			glm::mat4 matCesped2 = glm::mat4(1.0);
			matCesped2 = glm::translate(matCesped2, glm::vec3(13.5, -1.0, 0.0));
			matCesped2 = glm::scale(matCesped2, glm::vec3(13.0, 0.25, 14.0));
			glBindTexture(GL_TEXTURE_2D, IDTextureCesped);
			shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(1.0, 1.0)));
			cubo.render(matCesped2);
			//FORCE TO ENABLE THE UNIT TEXTURE TO 0 ALWAYS .............. IMPORTANT
			glActiveTexture(GL_TEXTURE0);

			glm::mat4 matCesped3 = glm::mat4(1.0);
			matCesped3 = glm::translate(matCesped3, glm::vec3(-13.5, -1.0, 0.0));
			matCesped3 = glm::scale(matCesped3, glm::vec3(13.0, 0.25, 14.0));
			glBindTexture(GL_TEXTURE_2D, IDTextureCesped);
			shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(1.0, 1.0)));
			cubo.render(matCesped3);
			//FORCE TO ENABLE THE UNIT TEXTURE TO 0 ALWAYS .............. IMPORTANT
			glActiveTexture(GL_TEXTURE0);

			glm::mat4 matCesped4 = glm::mat4(1.0);
			matCesped4 = glm::translate(matCesped4, glm::vec3(0.0, -1.0, 17.0));
			matCesped4 = glm::scale(matCesped4, glm::vec3(40.0, 0.25, 20.0));
			glBindTexture(GL_TEXTURE_2D, IDTextureCesped);
			shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(2.0, 2.0)));
			cubo.render(matCesped4);
			//FORCE TO ENABLE THE UNIT TEXTURE TO 0 ALWAYS .............. IMPORTANT
			glActiveTexture(GL_TEXTURE0);

			glm::mat4 matVentana = glm::mat4(1.0);
			matVentana = glm::translate(matVentana, glm::vec3(-4.3, 4.0, 6.9));
			matVentana = glm::scale(matVentana, glm::vec3(0.065, 0.065, 0.065));
			shaderMulLighting.setFloat("offsetX", 0);
			shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(1, 1)));
			modelVentana.render(matVentana);
			//FORCE TO ENABLE THE UNIT TEXTURE TO 0 ALWAYS .............. IMPORTANT
			glActiveTexture(GL_TEXTURE0);



		
		/*model = glm::translate(model, glm::vec3(0, 0, dz));
		model = glm::rotate(model, rot0, glm::vec3(0, 1, 0));
		//box1.enableWireMode();
		// Usamos la textura ID 1
		glBindTexture(GL_TEXTURE_2D, textureID1);
		box1.render(glm::scale(model, glm::vec3(1.0, 1.0, 0.1)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);*/

		/*// Articulacion 1
		glm::mat4 j1 = glm::translate(model, glm::vec3(0.5f, 0.0f, 0.0f));
		j1 = glm::rotate(j1, rot1, glm::vec3(0, 0, 1));
		j1 = glm::rotate(j1, rot2, glm::vec3(0, 1, 0));
		sphere1.enableWireMode();
		sphere1.render(glm::scale(j1, glm::vec3(0.1, 0.1, 0.1)));*/

		/*// Hueso 1
		glm::mat4 l1 = glm::translate(j1, glm::vec3(0.25f, 0.0, 0.0));
		l1 = glm::rotate(l1, glm::radians(90.0f), glm::vec3(0, 0, 1.0));
		//cylinder1.enableWireMode();
		cylinder1.render(glm::scale(l1, glm::vec3(0.1, 0.5, 0.1)));*/

		// Articulacion 2
		/*glm::mat4 j2 = glm::translate(j1, glm::vec3(0.5, 0.0f, 0.0f));
		j2 = glm::rotate(j2, rot3, glm::vec3(0.0, 0.0, 1.0));
		j2 = glm::rotate(j2, rot4, glm::vec3(1.0, 0.0, 0.0));
		sphere1.enableWireMode();
		sphere1.render(glm::scale(j2, glm::vec3(0.1, 0.1, 0.1)));*/
		
		// Hueso 2
		/*glm::mat4 l2 = glm::translate(j2, glm::vec3(0.25, 0.0, 0.0));
		l2 = glm::rotate(l2, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
		//cylinder1.enableWireMode();
		cylinder1.render(glm::scale(l2, glm::vec3(0.1, 0.5, 0.1)));*/

		// Ojo
		//glm::mat4 ojo = glm::translate(model, glm::vec3(0.25, 0.25, 0.05));
		//sphere1.enableWireMode();
		//sphere1.render(glm::scale(ojo, glm::vec3(0.2, 0.2, 0.1)));

		//glm::mat4 ojo2 = glm::translate(model, glm::vec3(-0.25, 0.25, 0.05));
		//sphere2.enableWireMode();
		//sphere2.render(glm::scale(ojo2, glm::vec3(0.2, 0.2, 0.1)));

		/*glm::mat4 modelAgua = glm::mat4(1.0);
		modelAgua = glm::translate(modelAgua, glm::vec3(0.0, 0.0, 0.0));
		modelAgua = glm::scale(modelAgua, glm::vec3(10.0, 0.01, 10.0));
		// Se activa la textura del agua
		glBindTexture(GL_TEXTURE_2D, textureID2);
		shaderMulLighting.setFloat("offsetX", offX);
		box2.render(modelAgua);
		glBindTexture(GL_TEXTURE_2D, 0);
		shaderTexture.setFloat("offsetX", 0);*/

		/*glm::mat4 modelSphere = glm::mat4(1.0);
		modelSphere = glm::translate(modelSphere, glm::vec3(3.0, 0.0, 0.0));
		glBindTexture(GL_TEXTURE_2D, textureID3);
		sphere3.render(modelSphere);
		glBindTexture(GL_TEXTURE_2D, 0);*/

		/*glm::mat4 modelCylinder = glm::mat4(1.0);
		modelCylinder = glm::translate(modelCylinder,
				glm::vec3(-3.0, 0.0, 0.0));
		// Envolvente desde el indice 0, el tamanio es 20 * 20 * 6
		// Se usa la textura 1 ( Bon sponja)
		glBindTexture(GL_TEXTURE_2D, textureID1);
		cylinder2.render(0, cylinder2.getSlices() * cylinder2.getStacks() * 6,
				modelCylinder);
		// Tapa Superior desde el indice : 20 * 20 * 6, el tamanio de indices es 20 * 3
		// Se usa la textura 2 ( Agua )
		glBindTexture(GL_TEXTURE_2D, textureID2);
		cylinder2.render(cylinder2.getSlices() * cylinder2.getStacks() * 6,
				cylinder2.getSlices() * 3, modelCylinder);
		// Tapa inferior desde el indice : 20 * 20 * 6 + 20 * 3, el tamanio de indices es 20 * 3
		// Se usa la textura 3 ( Goku )
		glBindTexture(GL_TEXTURE_2D, textureID3);
		cylinder2.render(
				cylinder2.getSlices() * cylinder2.getStacks() * 6
						+ cylinder2.getSlices() * 3, cylinder2.getSlices() * 3,
				modelCylinder);
		glBindTexture(GL_TEXTURE_2D, 0);

		// Render del cubo con textura de ladrillos y con repeticion en x
		glm::mat4 cubeTextureModel = glm::mat4(1.0);
		cubeTextureModel = glm::translate(cubeTextureModel,
				glm::vec3(3.0, 2.0, 3.0));
		glBindTexture(GL_TEXTURE_2D, textureID4);
		shaderTexture.setVectorFloat2("scaleUV",
				glm::value_ptr(glm::vec2(2.0, 1.0)));
		box3.render(cubeTextureModel);
		glBindTexture(GL_TEXTURE_2D, 0);*/

		// Render del cyindro con materiales
		/*glm::mat4 cylinderMaterialModel = glm::mat4(1.0);
		cylinderMaterialModel = glm::translate(cylinderMaterialModel,  glm::vec3(3.0, 2.0, -3.0));
		shaderMaterialLighting.setVectorFloat3("material.ambient", glm::value_ptr(glm::vec3(0.61424f, 0.04136f, 0.04136f)));
		shaderMaterialLighting.setVectorFloat3("material.diffuse", glm::value_ptr(glm::vec3(0.61424f, 0.04136f, 0.04136f)));
		shaderMaterialLighting.setVectorFloat3("material.specular", glm::value_ptr(glm::vec3(0.727811f, 0.626959f, 0.626959f)));
		shaderMaterialLighting.setFloat("material.shininess", 76.8f);
		cylinderMaterials.render(cylinderMaterialModel);*/

		/*glm::mat4 boxMaterialModel = glm::mat4(1.0f);
		boxMaterialModel = glm::translate(boxMaterialModel, glm::vec3(-3.0, 2.0, -3.0));
		shaderMaterialLighting.setVectorFloat3("material.ambient", glm::value_ptr(glm::vec3(0.61424f, 0.04136f, 0.04136f)));
		shaderMaterialLighting.setVectorFloat3("material.diffuse", glm::value_ptr(glm::vec3(0.61424f, 0.04136f, 0.04136f)));
		shaderMaterialLighting.setVectorFloat3("material.specular", glm::value_ptr(glm::vec3(0.727811f, 0.626959f, 0.626959f)));
		shaderMaterialLighting.setFloat("material.shininess", 76.8f);
		boxMaterials.render(boxMaterialModel);*/


		//MODEL COMPLEX RENDER
		/*glm::mat4 matModelRock = glm::mat4(1.0);
		matModelRock = glm::translate(matModelRock, glm::vec3(-3.0, -1.0, 4.0));
		shaderMulLighting.setFloat("offsetX", 0);
		modelRock.render(matModelRock);
		//FORCE TO ENABLE THE UNIT TEXTURE TO 0 ALWAYS .............. IMPORTANT
		glActiveTexture(GL_TEXTURE0);*/
		/*
		//MODEL VIAS DEL TREN
		glm::mat4 matrailroad = glm::mat4(1.0);
		matrailroad = glm::translate(matrailroad, glm::vec3(3.0, 0.0, 4.0));
		shaderMulLighting.setFloat("offsetX", 0);
		modelRailRoad.render(matrailroad);
		//FORCE TO ENABLE THE UNIT TEXTURE TO 0 ALWAYS .............. IMPORTANT
		glActiveTexture(GL_TEXTURE0);*/

		/*//MODEL Aircraft_obj
		glm::mat4 matAircraft_obj = glm::mat4(1.0);
		matAircraft_obj = glm::translate(matAircraft_obj, glm::vec3(3.0, 4.0, 4.0));
		shaderMulLighting.setFloat("offsetX", 0);
		modelAircraft_obj.render(matAircraft_obj);
		//FORCE TO ENABLE THE UNIT TEXTURE TO 0 ALWAYS .............. IMPORTANT
		glActiveTexture(GL_TEXTURE0);*/


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

		dz = 0;
		rot0 = 0;
		offX += 0.001;

		switch (stateCar) {
		case 0:
			std::cout << "Advance" << std::endl;
			//0.005 debe ser igual 
			ObjCarroEclipse = glm::translate(ObjCarroEclipse, glm::vec3(0.0, 0.0, 0.9));
			offsetCarAdvance += 0.9;
			if (offsetCarAdvance > 150.0) {
				offsetCarAdvance = 0.0;
				stateCar = 1;
			}
			break;

		case 1:
			std::cout << "Turn" << std::endl;
			ObjCarroEclipse = glm::translate(ObjCarroEclipse, glm::vec3(0, 0, 0.16));
			ObjCarroEclipse = glm::rotate(ObjCarroEclipse, glm::radians(0.5f), glm::vec3(0.0, 1.0, 0.0));
			offsetCarRot += 0.5; //igual a los grados
			if (offsetCarRot > 90.0) {
				offsetCarRot = 0.0;
				stateCar = 2;
			}
			break;

		case 2:
			std::cout << "Advance" << std::endl; 
			ObjCarroEclipse = glm::translate(ObjCarroEclipse, glm::vec3(0.0, 0.0, 0.9));
			offsetCarAdvance += 0.9;
			if (offsetCarAdvance > 200.0) {
				offsetCarAdvance = 0.0;
				stateCar = 3;
			}
			break;

		case 3:
			std::cout << "Turn" << std::endl;
			ObjCarroEclipse = glm::translate(ObjCarroEclipse, glm::vec3(0, 0, 0.16));
			ObjCarroEclipse = glm::rotate(ObjCarroEclipse, glm::radians(0.5f), glm::vec3(0.0, 1.0, 0.0));
			offsetCarRot += 0.5; //igual a los grados
			if (offsetCarRot > 89.0) {
				offsetCarRot = 0.0;
				stateCar = 4;
			}
			break;

		case 4:
			std::cout << "Advance" << std::endl;
			//0.005 debe ser igual 
			ObjCarroEclipse = glm::translate(ObjCarroEclipse, glm::vec3(0.0, 0.0, 0.9));
			offsetCarAdvance += 0.9;
			if (offsetCarAdvance > 150.0) {
				offsetCarAdvance = 0.0;
				stateCar = 0;
			}
			break;
		}

		switch (stateHeli) {
		case 0: 
			matHelico = glm::translate(matHelico, glm::vec3(0.0, 0.15, 0.15));
			offsetHeliAdvance += 0.15;
			if (offsetHeliAdvance > 56.0) {
				offsetHeliAdvance = 0.0;
				stateHeli = 1;
			}
			break;

		case 1:
			matHelico = glm::translate(matHelico, glm::vec3(0.0, 0.15, 0.15));
			matHelico = glm::translate(matHelico, glm::vec3(0.0, -0.15, 0.0));
			matHelico = glm::rotate(matHelico, glm::radians(-0.5f), glm::vec3(1.0, 0.0, 0.0));
			offsetHeliRot += 0.5; //igual a los grados
			if (offsetHeliRot > 45.0) {
				offsetHeliRot = 0.0;
				stateHeli = 2;
			}
			break;
		
		case 2:	
			matHelico = glm::translate(matHelico, glm::vec3(0.0, -0.15, 0.15));
			offsetHeliAdvance += 0.15;
			if (offsetHeliAdvance > 9.0) {
				offsetHeliAdvance = 0.0;
				stateHeli = 3;
			}
			break;
		case 3:
			offsetHeliAdvance += 0.1;
			if (offsetHeliAdvance > 9.0) {
				offsetHeliAdvance = 0.0;
				stateHeli = 0;
			}

			break;
		}

		glfwSwapBuffers(window);
	}
}

int main(int argc, char **argv) {
	init(800, 700, "Window GLFW", false);
	applicationLoop();
	destroy();
	return 1;
}
