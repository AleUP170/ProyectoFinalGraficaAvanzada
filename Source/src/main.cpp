#define _USE_MATH_DEFINES
#include <cmath>
//glew include
#include <GL/glew.h>

//std includes
#include <string>
#include <iostream>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

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
#include "Headers/ThirdPersonCamera.h"

//GLM include
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Headers/Texture.h"

// Include loader Model class
#include "Headers/Model.h"

// Include Terrain
#include "Headers/Terrain.h"

#include "Headers/AnimationUtils.h"

// Include Colision headers functions
#include "Headers/Colisiones.h"
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

enum Tipocolision { noColision, OBBCol, SBBCol };
class GameObject {
public:
	Model model;
	glm::mat4 transform = glm::mat4(1.0f);
	bool active = true;
	std::string modelLocation;
	std::string parentName;
	int animation_index = 0;
	glm::vec3 modelScale = glm::vec3(1.0f);
	Tipocolision colision = noColision;
	float rota = 0.0f;

	GameObject(std::string locationModel) {
		this->modelLocation = locationModel;
	}
	GameObject(std::string locationModel, int animIndex) {
		this->modelLocation = locationModel;
		this->animation_index = animIndex;
	}
	GameObject(std::string locationModel, int animIndex, glm::vec3 scale) {
		this->modelLocation = locationModel;
		this->animation_index = animIndex;
		this->modelScale = scale;
	}
	GameObject(std::string locationModel, glm::vec3 scale) {
		this->modelLocation = locationModel;
		this->modelScale = scale;
	}

	GameObject(std::string locationModel, Tipocolision col) {
		this->modelLocation = locationModel;
		this->colision = col;
	}
	GameObject(std::string locationModel, int animIndex, Tipocolision col) {
		this->modelLocation = locationModel;
		this->animation_index = animIndex;
		this->colision = col;
	}
	GameObject(std::string locationModel, int animIndex, glm::vec3 scale, Tipocolision col) {
		this->modelLocation = locationModel;
		this->animation_index = animIndex;
		this->modelScale = scale;
		this->colision = col;
	}
	GameObject(std::string locationModel, glm::vec3 scale, Tipocolision col) {
		this->modelLocation = locationModel;
		this->modelScale = scale;
		this->colision = col;
	}

};

class Controller {
public:
	int joystickR_X;
	int joystickR_Y;
	int joystickL_X;
	int joystickL_Y;
	int but_A;
	int but_B;
	Controller() {

	}
	Controller(int jR_X,int jR_Y,int jL_X,int jL_y,int butt_A, int butt_b) {
		this->joystickR_X = jR_X;
		this->joystickR_Y = jR_Y;
		this->joystickL_X = jL_X;
		this->joystickL_Y = jL_y;
		this->but_A = butt_A;
		this->but_B = butt_b;
	}
};

int screenWidth;
int screenHeight;

GLFWwindow *window;

Shader shader;
//Shader con skybox
Shader shaderSkybox;
//Shader con multiples luces
Shader shaderMulLighting;
//Shader para el terreno
Shader shaderTerrain;

//Mapeo controles
std::map<std::string, Controller> mapasControles{
	{ "PS4", Controller(2, 5, 0, 1, 1, 2) },
	{ "Xbox", Controller(2, 3, 0, 1, 1, 2) }
};

//	Modelos
std::map<std::string, GameObject> modelos {
	{"Raccoon",GameObject("../Assets/models/Racoon/Raccoon.fbx", glm::vec3(0.0005f,0.0005f,0.0005f),SBBCol)},
	{"Building",GameObject("../Assets/models/Building/Building.fbx", OBBCol)},
	{"Fountain",GameObject("../Assets/models/Fountain/Fountain.fbx", OBBCol)},
	{"Slide",GameObject("../Assets/models/Slide/Slide.fbx", OBBCol)},
	{"Swing",GameObject("../Assets/models/Swing/swing.fbx", OBBCol)}
};

//	Modelos que necesitaran colliders 
std::map<std::string, GameObject> modelosCollider{
	{"Bush1",GameObject("../Assets/Models/Bush/Bush.fbx", OBBCol)},
	{"Bush2",GameObject("../Assets/Models/Bush/Bush2.fbx", glm::vec3(1.0f,0.5f,0.5f), OBBCol)},
	{"BushBorder",GameObject("../Assets/Models/Bush/BushBorder.fbx", OBBCol)},
	{"Bush1Wall",GameObject("../Assets/Models/Bush/Bush1Wall.fbx", OBBCol)},
	{"Bush2Wall",GameObject("../Assets/Models/Bush/Bush2Wall.fbx", OBBCol)},
	{"Bench",GameObject("../Assets/Models/StoneBench/Bench.fbx", OBBCol)},
	{"Tree",GameObject("../Assets/Models/Trees/Tree.fbx", OBBCol)},
	{"Cherry",GameObject("../Assets/Models/frutas/Cherry.fbx", SBBCol)},
	{"Pear",GameObject("../Assets/Models/frutas/Pear.fbx", SBBCol)},
	{"Pineapple",GameObject("../Assets/Models/frutas/Pineapple.fbx", SBBCol)},
	{"Watermelon",GameObject("../Assets/Models/frutas/Watermelon.fbx", SBBCol)}
};

//variables player
float speed = 0.3f;
bool isJumping = false;
float heightTerrainJump = 0.0f;
float verticalSpeedJump = 10.0f;
double timeJump = 0.0f;
float gravity = 9.81f;
Controller currentController;

// Posiciones arbustos 1
std::vector<glm::vec3> bush1Positions = {
		glm::vec3(93.0f, 0.0f, -13.0f),
		glm::vec3(7.0f, 0.0f, 64.0f),
		glm::vec3(92.0f, 0.0f, 142.0f),
		glm::vec3(18.0f, 0.0f, -26.0f),
		glm::vec3(-2.0f, 0.0f, -108.0f)
};
std::vector<float> bush1Orientations = {
		0, 0, 0, 0, 0
};

// Posiciones arbustos 2
std::vector<glm::vec3> bush2Positions = {
		glm::vec3(144.0f, 0, -29.0f),
		glm::vec3(140.0f, 0, 82.0f),
		glm::vec3(125.0f, 0, 183.0f),
		glm::vec3(17.0f, 0, 177.0f),
		glm::vec3(-6.0f, 0, 126.0f)
};
std::vector<float> bush2Orientations = {
		0, 0, 0, 0, 0
};

// Posiciones arbustos borde
std::vector<glm::vec3> bushBorderPositions = {
		glm::vec3(0, 0, -200),
		glm::vec3(0, 0, 200),
		glm::vec3(-200, 0, 0),
		glm::vec3(200, 0, 0)
};
std::vector<float> bushBorderOrientations = {
		0,
		180,
		90,
		270
};

// Posiciones paredes arbustos pequeños
std::vector<glm::vec3> bush1WallPositions = {
		glm::vec3(-28.0f, 0.0f, -14.0f),
		glm::vec3(-28.0f, 0.0f, -92.0f),
		glm::vec3(-78.0f, 0.0f, -14.0f),
		glm::vec3(-78.0f, 0.0f, -92.0f),
		glm::vec3(-6.0f, 0.0f, -21.0f),
		glm::vec3(-84.0f, 0.0f, -21.0f),
		glm::vec3(-6.0f, 0.0f, -71.0f),
		glm::vec3(-84.0f, 0.0f, -71.0f)
};
std::vector<float> bush1WallOrientations = {
		0, 0, 0, 0,
		90, 90, 90, 90
};

// Posiciones paredes arbustos grandes
std::vector<glm::vec3> bush2WallPositions = {
		// Borde central
		glm::vec3(9.0f, 0.0f, -5.0f),
		glm::vec3(-102.0f, 0.0f, -5.0f),
		glm::vec3(9.0f, 0.0f, -70.0f),
		glm::vec3(-102.0f, 0.0f, -70.0f),
		glm::vec3(-28.0f, 0.0f, 2.0f),
		glm::vec3(-28.0f, 0.0f, -110.0f),
		glm::vec3(-95.0f, 0.0f, 2.0f),
		glm::vec3(-95.0f, 0.0f, -110.0f),

		glm::vec3(-31.0f, 0.0f, -110.0f),

		// Pasillo
		glm::vec3(-31.0f, 0.0f, 42.0f),
		glm::vec3(-31.0f, 0.0f, 69.0f),
		glm::vec3(-31.0f, 0.0f, 106.0f),
		glm::vec3(-31.0f, 0.0f, 133.0f),
		glm::vec3(-31.0f, 0.0f, 153.0f),

		glm::vec3(-59.0f, 0.0f, 42.0f),
		glm::vec3(-59.0f, 0.0f, 50.0f),
		glm::vec3(-59.0f, 0.0f, 106.0f),
		glm::vec3(-59.0f, 0.0f, 133.0f),
		glm::vec3(-59.0f, 0.0f, 163.0f),

		// Cancha
		glm::vec3(-66.0f, 0.0f, 50.0f),
		glm::vec3(-66.0f, 0.0f, 127.0f),
		glm::vec3(-66.0f, 0.0f, 105.0f),

		glm::vec3(-137.0f, 0.0f, 50.0f),
		glm::vec3(-137.0f, 0.0f, 127.0f),
		glm::vec3(-137.0f, 0.0f, 105.0f),

		glm::vec3(-135.0f, 0.0f, 20.0f),
		glm::vec3(-97.0f, 0.0f, 20.0f),
		glm::vec3(-135.0f, 0.0f, 131.0f),
		glm::vec3(-97.0f, 0.0f, 131.0f),

		// Edificio
		glm::vec3(12.0f, 0.0f, -65.0f),
		glm::vec3(12.0f, 0.0f, -40.0f),
		glm::vec3(42.0f, 0.0f, -40.0f),
		glm::vec3(72.0f, 0.0f, -40.0f),

		glm::vec3(83.0f, 0.0f, -161.0f),
		glm::vec3(118.0f, 0.0f, -161.0f),

		glm::vec3(82.0f, 0.0f, -71.0f),
		glm::vec3(82.0f, 0.0f, -116.0f),

		glm::vec3(152.0f, 0.0f, -71.0f),
		glm::vec3(152.0f, 0.0f, -116.0f)

};
std::vector<float> bush2WallOrientations = {
		90, 90, 90, 90,
		0, 0, 0, 0,

		90,

		90, 90, 90, 90, 90,
		90, 90, 90, 90, 90,

		90, 90, 90,
		90, 90, 90,
		0, 0, 0, 0,

		0, 0, 0, 0,
		0, 0,
		90, 90, 90, 90
};

// Posiciones bancas
std::vector<glm::vec3> benchPositions = {
		glm::vec3(-21.0f, 0, -31.0f),
		glm::vec3(-69.0f, 0, -31.0f),
		glm::vec3(-69.0f, 0, -77.0f),
		glm::vec3(-21.0f, 0, -77.0f),
		glm::vec3(-117.0f, 0, -113.0f),
		glm::vec3(-106.0f, 0, -124.0f),

		glm::vec3(-81.0f, 0, 165.0f),
		glm::vec3(-114.0f, 0, 155.0f),

		glm::vec3(-80.0f, 0, 113.0f),
		glm::vec3(-100.0f, 0, 113.0f),
		glm::vec3(-120.0f, 0, 113.0f)
};
std::vector<float> benchOrientations = {
		45, 135, 45, 135,
		0, 90,
		0, -35,
		0, 0, 0
};

// Posiciones arboles
std::vector<glm::vec3> treePositions = {
		glm::vec3(-73.0f, 0, -80.0f),
		glm::vec3(-24.0f, 0, -88.0f),
		glm::vec3(-11.0f, 0, -71.0f),
		glm::vec3(-21.0f, 0, -21.0f),
		glm::vec3(-64.0f, 0, -20.0f),
		glm::vec3(-78.0f, 0, -31.0f),
		glm::vec3(-131.0f, 0, -139.0f),
		glm::vec3(-137.0f, 0, -33.0f),
		glm::vec3(-115.0f, 0, -37.0f),
		glm::vec3(-66.0f, 0, 160.0f),
		glm::vec3(-93.0f, 0, 142.0f),
		glm::vec3(-127.0f, 0, 146.0f),
		glm::vec3(-133.0f, 0, 170.0f),
		glm::vec3(-143.0f, 0, 70.0f),
		glm::vec3(-168.0f, 0, 44.0f),
		glm::vec3(-140.0f, 0, 2.0f),
		glm::vec3(-169.0f, 0, -85.0f),
		glm::vec3(-170.0f, 0, -151.0f),
		glm::vec3(-135.0f, 0, -180.0f),
		glm::vec3(-100.0f, 0, -176.0f),
		glm::vec3(45.0f, 0, -74.0f),
		glm::vec3(17.0f, 0, -89.0f),
		glm::vec3(-22.0f, 0, -126.0f),
		glm::vec3(69.0f, 0, -154.0f),
		glm::vec3(-11.0f, 0, 15.0f),
		glm::vec3(-1.0f, 0, 131.0f),
		glm::vec3(97.0f, 0, 1.0f),
		glm::vec3(73.0f, 0, 37.0f),
		glm::vec3(137.0f, 0, 79.0f),
		glm::vec3(44.0f, 0, 137.0f),
		glm::vec3(139.0f, 0, 34.0f),
		glm::vec3(178.0f, 0, 35.0f)
};

//*** Posiciones de frutas ***//
//	CHERRY
std::vector<glm::vec3> cherryPosition = { glm::vec3(1, 0.0, 0) };
//	PERA
std::vector<glm::vec3> peraPosition = { glm::vec3(1, 0.0, 1) };
//	PIÑA
std::vector<glm::vec3> pinAppPosition = { glm::vec3(-1, 0.0, -1) };
//	SANDIA
std::vector<glm::vec3> sandiaPosition = { glm::vec3(0, 0.0, 0) };

std::shared_ptr<Camera> camera(new ThirdPersonCamera());

Sphere skyboxSphere(20, 20);

// Terrain model instance
Terrain terrain(-1, -1, 400, 8, "../Assets/Textures/heightmap.png");

GLuint textureTerrainBackgroundID, textureTerrainRID, textureTerrainGID, textureTerrainBID, textureTerrainBlendMapID;
GLuint skyboxTextureID;

GLenum types[6] = {
GL_TEXTURE_CUBE_MAP_POSITIVE_X,
GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
GL_TEXTURE_CUBE_MAP_NEGATIVE_Z };

std::string fileNames[6] = { "../Assets/Textures/interstellar/xpos.png",
		"../Assets/Textures/interstellar/xneg.png",
		"../Assets/Textures/interstellar/ypos.png",
		"../Assets/Textures/interstellar/yneg.png",
		"../Assets/Textures/interstellar/zpos.png",
		"../Assets/Textures/interstellar/zneg.png" };


bool exitApp = false;
int lastMousePosX, offsetX = 0;
int lastMousePosY, offsetY = 0;



double deltaTime;
double currTime, lastTime;

float distanceFromTarget = 10.0;

// Colliders
std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> > collidersOBB;
std::map<std::string, std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> > collidersSBB;

//Render Colliders
Box boxCollider;
Sphere sphereCollider(10, 10);

// Se definen todos las funciones.
void reshapeCallback(GLFWwindow *Window, int widthRes, int heightRes);
void keyCallback(GLFWwindow *window, int key, int scancode, int action,
	int mode);
void GamePadLogic();
void mouseCallback(GLFWwindow *window, double xpos, double ypos);
void mouseButtonCallback(GLFWwindow *window, int button, int state, int mod);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void init(int width, int height, std::string strTitle, bool bFullScreen);
void destroy();
bool processInput(bool continueApplication = true);


void LoadModels() {
	for (std::map<std::string, GameObject>::iterator it = modelos.begin(); it != modelos.end(); ++it)
	{
		it->second.model.loadModel(it->second.modelLocation);
		it->second.model.setShader(&shaderMulLighting);
		it->second.model.setOrientation(glm::vec3(1.0f));

	}
	//Colliders Render
	boxCollider.init();
	boxCollider.setShader(&shader);
	boxCollider.setColor(glm::vec4(1.0f));
	sphereCollider.init();
	sphereCollider.setShader(&shader);
	sphereCollider.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));

}

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
	glfwSetScrollCallback(window, scrollCallback);
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

	// Inicialización de los shaders
	shader.initialize("../Shaders/colorShader.vs", "../Shaders/colorShader.fs");
	shaderSkybox.initialize("../Shaders/skyBox.vs", "../Shaders/skyBox.fs");
	shaderMulLighting.initialize("../Shaders/iluminacion_textura_animation.vs", "../Shaders/multipleLights.fs");
	shaderTerrain.initialize("../Shaders/terrain.vs", "../Shaders/terrain.fs");

	// Inicializacion de los objetos.
	skyboxSphere.init();
	skyboxSphere.setShader(&shaderSkybox);
	skyboxSphere.setScale(glm::vec3(20.0f, 20.0f, 20.0f));

	LoadModels();

	terrain.init();
	terrain.setShader(&shaderTerrain);
	terrain.setPosition(glm::vec3(200, 0, 200));

	camera->setSensitivity(1.0);
	camera->setDistanceFromTarget(distanceFromTarget);
	camera->setCameraTarget(modelos.at("Raccoon").transform[3]);
	camera->updateCamera();

	// Definimos el tamanio de la imagen
	int imageWidth, imageHeight;
	FIBITMAP *bitmap;
	unsigned char *data;

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
	// Definiendo la textura a utilizar
	Texture textureTerrainBackground("../Assets/Textures/Grass.png");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureTerrainBackground.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureTerrainBackground.convertToData(bitmap, imageWidth,
		imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureTerrainBackgroundID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureTerrainBackgroundID);
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
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureTerrainBackground.freeImage(bitmap);

	// Definiendo la textura a utilizar
	Texture textureTerrainR("../Assets/Textures/Path.png");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureTerrainR.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureTerrainR.convertToData(bitmap, imageWidth,
		imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureTerrainRID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureTerrainRID);
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
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureTerrainR.freeImage(bitmap);

	// Definiendo la textura a utilizar
	Texture textureTerrainG("../Assets/Textures/Dirt.png");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureTerrainG.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureTerrainG.convertToData(bitmap, imageWidth,
		imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureTerrainGID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureTerrainGID);
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
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureTerrainG.freeImage(bitmap);

	// Definiendo la textura a utilizar
	Texture textureTerrainB("../Assets/Textures/Cork.png");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureTerrainB.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureTerrainB.convertToData(bitmap, imageWidth,
		imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureTerrainBID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureTerrainBID);
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
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureTerrainB.freeImage(bitmap);

	// Definiendo la textura a utilizar
	Texture textureTerrainBlendMap("../Assets/Textures/blendMap.png");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureTerrainBlendMap.loadImage(true);
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureTerrainBlendMap.convertToData(bitmap, imageWidth,
		imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureTerrainBlendMapID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureTerrainBlendMapID);
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
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureTerrainBlendMap.freeImage(bitmap);

}
void DestroyModels() {
	for (std::map<std::string, GameObject>::iterator it = modelos.begin(); it != modelos.end(); ++it)
	{
		it->second.model.destroy();
	}
	//Destroy Render Colliders
	boxCollider.destroy();
	sphereCollider.destroy();

}

void destroy() {
	glfwDestroyWindow(window);
	glfwTerminate();
	// --------- IMPORTANTE ----------
	// Eliminar los shader y buffers creados.

	// Shaders Delete
	shader.destroy();
	shaderMulLighting.destroy();
	shaderSkybox.destroy();
	shaderTerrain.destroy();

	// Basic objects Delete
	skyboxSphere.destroy();

	// Terrains objects Delete
	terrain.destroy();

	// Textures Delete
	glBindTexture(GL_TEXTURE_2D, 0);
	glDeleteTextures(1, &textureTerrainBackgroundID);
	glDeleteTextures(1, &textureTerrainRID);
	glDeleteTextures(1, &textureTerrainGID);
	glDeleteTextures(1, &textureTerrainBID);
	glDeleteTextures(1, &textureTerrainBlendMapID);

	DestroyModels();
	// Cube Maps Delete
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glDeleteTextures(1, &skyboxTextureID);
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

void SetJumpVariables() {
	std::cout << "Set jump Variables" << std::endl;
	std::cout << "time Jump = " << TimeManager::Instance().GetTime() << std::endl;
	heightTerrainJump = terrain.getHeightTerrain(modelos.at("Raccoon").transform[3][0] , modelos.at("Raccoon").transform[3][2]);
	timeJump = TimeManager::Instance().GetTime();
	isJumping = true;
	modelos.at("Raccoon").animation_index = 3;
}
void GamePadLogic() {
	int present = glfwJoystickPresent(GLFW_JOYSTICK_1);
	if (present == 1) {
		int axisCount;
		const float *axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axisCount);
		int buttonCount;
		const unsigned char *buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonCount);

		
		if (glfwGetJoystickName(GLFW_JOYSTICK_1) == "Wireless Controller" && buttonCount == 18) {
			currentController = mapasControles.at("PS4");
			//std::cout << "PS4 " << std::endl;
		}
		else {
			currentController = mapasControles.at("Xbox");
			//std::cout << "xbox " << std::endl;
		}
		

		modelos.at("Raccoon").animation_index = 1;
		
		
		//Left stick Y
		if (axes[currentController.joystickL_Y] >= 0.1 || axes[currentController.joystickL_Y] <= -0.1) {

			glm::mat4 cameraTransform = glm::mat4(1.0f);
			cameraTransform[3] = glm::vec4(camera->getPosition(), 1);

			glm::mat4 inv = glm::inverse(cameraTransform);
			inv = glm::rotate(inv, camera->getAngleAroundTarget(), glm::vec3(0.0f, 1.0f, 0.0f));
			glm::vec3 cameraForward = glm::normalize(glm::vec3(inv[2]));
			glm::vec3 x_axis = glm::cross(cameraForward, glm::vec3(0, 1, 0));
			//modelos.at("Raccoon").rota = camera->getAngleAroundTarget() + glm::radians(90.0f - 90.0f*signbit(axes[currentController.joystickL_Y]));
			modelos.at("Raccoon").transform = glm::translate(modelos.at("Raccoon").transform, cameraForward * axes[currentController.joystickL_Y] * speed);
			modelos.at("Raccoon").animation_index = 4;

		}

		//Left stick X
		if (axes[currentController.joystickL_X] >= 0.1 || axes[currentController.joystickL_X] <= -0.1) {
			glm::mat4 cameraTransform = glm::mat4(1.0f);
			cameraTransform[3] = glm::vec4(camera->getPosition(), 1);

			glm::mat4 inv = glm::inverse(cameraTransform);
			inv = glm::rotate(inv, camera->getAngleAroundTarget(), glm::vec3(0.0f, 1.0f, 0.0f));
			glm::vec3 cameraForward = glm::normalize(glm::vec3(inv[2]));
			glm::vec3 x_axis = glm::cross(cameraForward, glm::vec3(0, 1, 0));
			modelos.at("Raccoon").transform = glm::translate(modelos.at("Raccoon").transform, x_axis * axes[currentController.joystickL_X] * speed);
			modelos.at("Raccoon").rota = camera->getAngleAroundTarget() + glm::radians(90.0f * (axes[currentController.joystickL_X] * -1.0f));
			modelos.at("Raccoon").animation_index = 4;

		}
		//Right stick X
		if (axes[currentController.joystickR_X] >= 0.1 || axes[currentController.joystickR_X] <= -0.1) {
			camera->mouseMoveCamera(axes[currentController.joystickR_X] * -1.5f , 0, deltaTime);

		}
		//Right stick Y
		if (axes[currentController.joystickR_Y] >= 0.1 || axes[currentController.joystickR_Y] <= -0.1) {
			camera->mouseMoveCamera(0,axes[currentController.joystickR_Y] * 1.5f , deltaTime);
		}
		//L2
		if (axes[3] != -1) {

		}
		//R2
		if (axes[4] != -1) {

		}
		if (GLFW_PRESS == buttons[currentController.but_A]) {
			if(!isJumping)
				SetJumpVariables();
			//std::cout << "X button pressed" << buttons[1] << std::endl;
		}
		else if (GLFW_RELEASE == buttons[1]) {
			//std::cout << "X button released" << buttons[1] << std::endl;
		}
		if (GLFW_PRESS == buttons[0]) {
			//std::cout << "Square button pressed" << buttons[0] << std::endl;
		}
		else if (GLFW_RELEASE == buttons[0]) {
			//std::cout << "Square button released" << buttons[0] << std::endl;
		}
		if (GLFW_PRESS == buttons[3]) {
			//std::cout << "Triangle button pressed" << buttons[3] << std::endl;
		}
		else if (GLFW_RELEASE == buttons[3]) {
			//std::cout << "Triangle button released" << buttons[3] << std::endl;
		}
		if (GLFW_PRESS == buttons[2]) {
			//std::cout << "circle button pressed" << buttons[2] << std::endl;
		}
		else if (GLFW_RELEASE == buttons[2]) {
			//std::cout << "circle button released" << buttons[2] << std::endl;
		}

		if (GLFW_PRESS == buttons[4]) {
			//std::cout << "L! button pressed" << buttons[4] << std::endl;
		}
		else if (GLFW_RELEASE == buttons[4]) {
			//std::cout << "L! button released" << buttons[4] << std::endl;
		}
		if (GLFW_PRESS == buttons[5]) {
			//std::cout << "R1 button pressed" << buttons[5] << std::endl;
		}
		else if (GLFW_RELEASE == buttons[5]) {
			//std::cout << "R1 button released" << buttons[5] << std::endl;
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
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	
}
bool processInput(bool continueApplication) {
	if (exitApp || glfwWindowShouldClose(window) != 0) {
		return false;
	}
	offsetX = 0;
	offsetY = 0;

	glfwPollEvents();
	return continueApplication;
}

float tiroParabolico(float currentTerrainHeight) {
	if (isJumping) {
		double t = TimeManager::Instance().GetTime() - timeJump;
		float yPos;
		modelos.at("Raccoon").animation_index = 3;			
		yPos = heightTerrainJump + (verticalSpeedJump * (float)t) - (0.5f * gravity *(float)t * (float)t);
		if (yPos > currentTerrainHeight) {
			return yPos;
		}
		modelos.at("Raccoon").animation_index = 1;
		isJumping = false;

	}
	return currentTerrainHeight;
}
void SetAnimationIndex() {
	std::map<std::string, GameObject>::iterator it;
	for (it = modelos.begin(); it != modelos.end(); it++) {
		it->second.model.setAnimationIndex(it->second.animation_index);
	}
}
void DrawModels() {

	// Mapache
	SetAnimationIndex();
	float terrainHeight = terrain.getHeightTerrain(modelos.at("Raccoon").transform[3][0], modelos.at("Raccoon").transform[3][2]);
	modelos.at("Raccoon").transform[3][1] = tiroParabolico(terrainHeight);
	printf("%f,%f,%f\n", modelos.at("Raccoon").transform[3][0], modelos.at("Raccoon").transform[3][1], modelos.at("Raccoon").transform[3][2]);
	modelos.at("Raccoon").model.setAnimationIndex(modelos.at("Raccoon").animation_index);
	glm::mat4 matrixRac = glm::scale(modelos.at("Raccoon").transform,modelos.at("Raccoon").modelScale);
	matrixRac = glm::rotate(matrixRac,modelos.at("Raccoon").rota, glm::vec3(0, 1, 0));
	
	matrixRac = glm::rotate(matrixRac, glm::radians(180.0f), glm::vec3(0, 0, 1));
	matrixRac = glm::rotate(matrixRac, glm::radians(180.0f), glm::vec3(0, 1, 0));
	//std::cout << glm::to_string(matrixRac) << std::endl;

	modelos.at("Raccoon").model.render(matrixRac);

	// Edificio
	glm::mat4 posBuild = glm::translate(modelos.at("Building").transform, glm::vec3(125.0f, 0.0f, -75.0f));
	modelos.at("Building").model.render(posBuild);

	// Fuente
	glm::mat4 posFount = glm::translate(modelos.at("Fountain").transform, glm::vec3(-45.0f, 0.0f, -53.0f));
	modelos.at("Fountain").model.render(posFount);
	
	// Resbaladilla
	glm::mat4 posSlide = glm::translate(modelos.at("Slide").transform, glm::vec3(36.0f, 0.0f, -102.0f));
	modelos.at("Slide").model.render(posSlide);

	// Columpio
	glm::mat4 posSwing = glm::translate(modelos.at("Swing").transform, glm::vec3(2.0f, 0.0f, -136.0f));
	modelos.at("Swing").model.render(posSwing);

	// Contorno Arbustos
	for (int i = 0; i < bushBorderPositions.size(); i++) {
		modelosCollider.at("BushBorder").model.setPosition(bushBorderPositions[i]);
		modelosCollider.at("BushBorder").model.setOrientation(glm::vec3(0,bushBorderOrientations[i],0));
		modelosCollider.at("BushBorder").model.render();
	}

	// Paredes arbustos 1
	/*for (int i = 0; i < bush1WallPositions.size(); i++) {
		modelosCollider.at("Bush1Wall").model.setPosition(bush1WallPositions[i]);
		modelosCollider.at("Bush1Wall").model.setOrientation(glm::vec3(0, bush1WallOrientations[i], 0));
		modelosCollider.at("Bush1Wall").model.render();
	}

	// Paredes arbustos 2
	for (int i = 0; i < bush2WallPositions.size(); i++) {
		modelosCollider.at("Bush2Wall").model.setPosition(bush2WallPositions[i]);
		modelosCollider.at("Bush2Wall").model.setOrientation(glm::vec3(0, bush2WallOrientations[i], 0));
		modelosCollider.at("Bush2Wall").model.render();
	}

	// Arbustos 1
	for (int i = 0; i < bush1Positions.size(); i++) {
		bush1Positions[i].y = terrain.getHeightTerrain(bush1Positions[i].x, bush1Positions[i].z);
		modelosCollider.at("Bush1").model.setPosition(bush1Positions[i]);
		modelosCollider.at("Bush1").model.setOrientation(glm::vec3(0, bush1Orientations[i], 0));
		modelosCollider.at("Bush1").model.render();
	}

	// Arbustos 2
	for (int i = 0; i < bush2Positions.size(); i++) {
		bush1Positions[i].y = terrain.getHeightTerrain(bush2Positions[i].x, bush2Positions[i].z);
		modelosCollider.at("Bush2").model.setPosition(bush2Positions[i]);
		modelosCollider.at("Bush2").model.setOrientation(glm::vec3(0, bush2Orientations[i], 0));
		modelosCollider.at("Bush2").model.render();
	}

	// Bancas
	for (int i = 0; i < benchPositions.size(); i++) {
		benchPositions[i].y = terrain.getHeightTerrain(benchPositions[i].x, benchPositions[i].z);
		modelosCollider.at("Bench").model.setPosition(benchPositions[i]);
		modelosCollider.at("Bench").model.setOrientation(glm::vec3(0, benchOrientations[i], 0));
		modelosCollider.at("Bench").model.render();
	}

	// Arboles
	for (int i = 0; i < treePositions.size(); i++) {
		treePositions[i].y = terrain.getHeightTerrain(treePositions[i].x, treePositions[i].z);
		modelosCollider.at("Tree").model.setPosition(treePositions[i]);
		modelosCollider.at("Tree").model.setScale(modelos.at("Tree").modelScale);
		modelosCollider.at("Tree").model.render();
	}*/
}

void SetUpColisionMeshes() {
	std::map<std::string, GameObject>::iterator it;
	glm::mat4 matrix;
	AbstractModel::OBB obbCollider;
	AbstractModel::SBB sbbCollider;
	// Agregar los arreglos de los modelos que necesitan colliders
	std::vector<std::vector<glm::vec3>> colisiones = { bush1Positions, bush2Positions, bushBorderPositions, bush1WallPositions, bush2WallPositions,
		benchPositions,	treePositions,  }; //cherryPosition, peraPosition, pinAppPosition, sandiaPosition
	int jt; // iterador del vector con las posiisones de los modelos (colisiones)
	for (it = modelos.begin(); it != modelos.end(); it++) {
		if (it->second.active) {
			switch (it->second.colision) {
			case OBBCol:
				std::cout << "Setting OBB collider for " << it->first << std::endl;
				matrix = it->second.transform;
				matrix = glm::scale(matrix, it->second.modelScale);
				obbCollider.u = glm::quat_cast(it->second.transform);
				matrix = glm::translate(matrix, it->second.model.getObb().c);
				obbCollider.c = glm::vec3(matrix[3]);
				obbCollider.e = it->second.model.getObb().e * it->second.modelScale * 100.0f;
				addOrUpdateColliders(collidersOBB, it->first, obbCollider, it->second.transform);
				break;
			case SBBCol:
				std::cout << "Setting SBB collider for " << it->first << std::endl;
				matrix = it->second.transform;
				matrix = glm::scale(matrix, it->second.modelScale);
				matrix = glm::translate(matrix, glm::vec3(it->second.model.getSbb().c));
				sbbCollider.c = glm::vec3(matrix[3]);
				sbbCollider.ratio = it->second.model.getSbb().ratio * it->second.modelScale.x * 50.0f;
				addOrUpdateColliders(collidersSBB, it->first, sbbCollider, it->second.transform);
				break;
			case noColision:
				break;
			}
		}
	}
	for (it = modelosCollider.begin(), jt = 0; it != modelosCollider.end() && jt < colisiones.size(); it++, jt++) {
		if (it->second.active) {
			switch (it->second.colision) {
			case OBBCol:
				for (int i = 0; i < colisiones[jt].size(); i++) {
					std::cout << "Setting OBB collider for " << it->first + std::to_string(i) << std::endl;
					matrix = it->second.transform;
					matrix = glm::translate(matrix, colisiones[jt][i]);
					obbCollider.u = glm::quat_cast(it->second.transform);
					matrix = glm::scale(matrix, it->second.modelScale);
					matrix = glm::translate(matrix, it->second.model.getObb().c);
					obbCollider.c = glm::vec3(matrix[3]);
					obbCollider.e = it->second.model.getObb().e * it->second.modelScale;
					addOrUpdateColliders(collidersOBB, it->first + std::to_string(i), obbCollider, it->second.transform);
				}
				break;
			case SBBCol:
				for (int i = 0; i < colisiones[jt].size(); i++) {
					std::cout << "Setting SBB collider for " << it->first + std::to_string(i) << std::endl;
					matrix = it->second.transform;
					matrix = glm::translate(matrix, colisiones[jt][i]);
					matrix = glm::scale(matrix, it->second.modelScale);
					matrix = glm::translate(matrix, it->second.model.getSbb().c);
					sbbCollider.c = glm::vec3(matrix[3]);
					sbbCollider.ratio = it->second.model.getSbb().ratio * 1.0;
					addOrUpdateColliders(collidersSBB, it->first + std::to_string(i), sbbCollider, it->second.transform);
				}
				break;
			}
		}
	}
}
void RenderColliders() {
	for (std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator it =
		collidersOBB.begin(); it != collidersOBB.end(); it++) {
		//std::cout << "Rendering collider OBB for " << it->first << std::endl;
		glm::mat4 matrixCollider = glm::mat4(1.0);
		matrixCollider = glm::translate(matrixCollider, std::get<0>(it->second).c);
		matrixCollider = matrixCollider * glm::mat4(std::get<0>(it->second).u);
		matrixCollider = glm::scale(matrixCollider, std::get<0>(it->second).e * 2.0f);
		boxCollider.setColor(glm::vec4(1.0, 0.0, 0.0, 1.0));
		boxCollider.enableWireMode();
		boxCollider.render(matrixCollider);
	}
	for (std::map<std::string, std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> >::iterator it =
		collidersSBB.begin(); it != collidersSBB.end(); it++) {
		//std::cout << "Rendering collider SBB for " << it->first << std::endl;
		glm::mat4 matrixCollider = glm::mat4(1.0);
		matrixCollider = glm::translate(matrixCollider, std::get<0>(it->second).c);
		matrixCollider = glm::scale(matrixCollider, glm::vec3(std::get<0>(it->second).ratio * 2.0f));
		sphereCollider.setColor(glm::vec4(0.0, 1.0, 0.0, 1.0));
		sphereCollider.enableWireMode();
		sphereCollider.render(matrixCollider);
	}
}
void applicationLoop() {
	
	bool psi = true;

	glm::mat4 view;
	glm::vec3 axis;
	glm::vec3 target;
	float angleTarget;

	lastTime = TimeManager::Instance().GetTime();
	while (psi) {
		currTime = TimeManager::Instance().GetTime();
		if (currTime - lastTime < 0.016666667) {
			glfwPollEvents();
			continue;
		}
		lastTime = currTime;
		TimeManager::Instance().CalculateFrameRate(true);
		deltaTime = TimeManager::Instance().DeltaTime;
		psi = processInput(true);
		GamePadLogic();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 projection = glm::perspective(glm::radians(45.0f),
			(float)screenWidth / (float)screenHeight, 0.01f, 100.0f);

		camera->setCameraTarget(modelos.at("Raccoon").transform[3]);
		camera->updateCamera();
		view = camera->getViewMatrix();


		// Settea la matriz de vista y projection al shader con solo color
		shader.setMatrix4("projection", 1, false, glm::value_ptr(projection));
		shader.setMatrix4("view", 1, false, glm::value_ptr(view));

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
		// Settea la matriz de vista y projection al shader con multiples luces
		shaderTerrain.setMatrix4("projection", 1, false,
			glm::value_ptr(projection));
		shaderTerrain.setMatrix4("view", 1, false,
			glm::value_ptr(view));

		

		/*******************************************
		 * Propiedades Luz direccional
		 *******************************************/
		shaderMulLighting.setVectorFloat3("viewPos", glm::value_ptr(camera->getPosition()));
		shaderMulLighting.setVectorFloat3("directionalLight.light.ambient", glm::value_ptr(glm::vec3(1, 1, 1)));
		shaderMulLighting.setVectorFloat3("directionalLight.light.diffuse", glm::value_ptr(glm::vec3(1, 1, 1)));
		shaderMulLighting.setVectorFloat3("directionalLight.light.specular", glm::value_ptr(glm::vec3(1, 1, 1)));
		shaderMulLighting.setVectorFloat3("directionalLight.direction", glm::value_ptr(glm::vec3(0, -1.0, 0.0)));
		/*******************************************
		 * Propiedades Luz direccional Terrain
		 *******************************************/
		shaderTerrain.setVectorFloat3("viewPos", glm::value_ptr(camera->getPosition()));
		shaderTerrain.setVectorFloat3("directionalLight.light.ambient", glm::value_ptr(glm::vec3(1, 1, 1)));
		shaderTerrain.setVectorFloat3("directionalLight.light.diffuse", glm::value_ptr(glm::vec3(1, 1, 1)));
		shaderTerrain.setVectorFloat3("directionalLight.light.specular", glm::value_ptr(glm::vec3(1, 1, 1)));
		shaderTerrain.setVectorFloat3("directionalLight.direction", glm::value_ptr(glm::vec3(0, -1.0, 0.0)));
		
		/*******************************************
		 * Propiedades SpotLights
		 *******************************************/
		shaderMulLighting.setInt("spotLightCount", 0);
		shaderTerrain.setInt("spotLightCount", 0);
		
		/*******************************************
		 * Propiedades PointLights
		 *******************************************/
		shaderMulLighting.setInt("pointLightCount", 0);
		shaderTerrain.setInt("pointLightCount",0);


		/*******************************************
		 * Terrain Cesped
		 *******************************************/
		glm::mat4 modelCesped = glm::mat4(1.0);
		modelCesped = glm::translate(modelCesped, glm::vec3(0.0, 0.0, 0.0));
		modelCesped = glm::scale(modelCesped, glm::vec3(200.0, 0.001, 200.0));
		// Se activa la textura del background
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureTerrainBackgroundID);
		shaderTerrain.setInt("backgroundTexture", 0);
		// Se activa la textura de tierra
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textureTerrainRID);
		shaderTerrain.setInt("rTexture", 1);
		// Se activa la textura de hierba
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, textureTerrainGID);
		shaderTerrain.setInt("gTexture", 2);
		// Se activa la textura del camino
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, textureTerrainBID);
		shaderTerrain.setInt("bTexture", 3);
		// Se activa la textura del blend map
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, textureTerrainBlendMapID);
		shaderTerrain.setInt("blendMapTexture", 4);
		shaderTerrain.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(40, 40)));
		terrain.render();
		shaderTerrain.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(0, 0)));
		glBindTexture(GL_TEXTURE_2D, 0);

		// Forze to enable the unit texture to 0 always ----------------- IMPORTANT
		glActiveTexture(GL_TEXTURE0);
		/*******************************************
		 * Skybox
		 *******************************************/
		GLint oldCullFaceMode;
		GLint oldDepthFuncMode;
		// deshabilita el modo del recorte de caras ocultas para ver las esfera desde adentro
		glGetIntegerv(GL_CULL_FACE_MODE, &oldCullFaceMode);
		glGetIntegerv(GL_DEPTH_FUNC, &oldDepthFuncMode);
		shaderSkybox.setFloat("skybox", 0);
		glCullFace(GL_FRONT);
		glDepthFunc(GL_LEQUAL);
		glActiveTexture(GL_TEXTURE0);
		skyboxSphere.render();
		glCullFace(oldCullFaceMode);
		glDepthFunc(oldDepthFuncMode);


		DrawModels();
		SetUpColisionMeshes();
		//RenderColliders();
		glfwSwapBuffers(window);
	}
}
int main(int argc, char **argv) {
	init(800, 700, "Window GLFW", false);
	applicationLoop();
	destroy();
	return 1;
}