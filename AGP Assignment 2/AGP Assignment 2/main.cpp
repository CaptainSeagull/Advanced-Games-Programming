// Windows specific: Uncomment the following line to open a console window for debug output
#if _DEBUG
#pragma comment(linker, "/subsystem:\"console\" /entry:\"WinMainCRTStartup\"")
#endif

#define checkGLError() checkOpenGLErr(__FILE__, __LINE__)

#include "rt3d.h"
#include "Pointlight.h"
#include "Spotlight.h"
#include "AmbientLight.h"
#include "UniformBufferObject.h"
#include "LightManager.h"
#include "Texture.h"
#include "FreeCamera.h"
#include "Model.h"
#include "snow.h"
#include "rain.h"
#include "BumpMap.h"
#include <stack>
#include <random>
#include <SDL_ttf.h>
#include <glm\gtc\matrix_transform.hpp>

bool running = true, zFailAlgorithm = false, silhouette = false, drawVolumes = false;
Shader *lightShader, *gouraudShader, *particlesShader, *phongShader, *currentShader, *silhouetteShader, *shadowVolume, *ambient, *phong_excludingAmbient;
Texture *rainTex, *snowTex;
Model *cube, *room, *bunny;
FreeCamera *camera;
LightManager *lightManager;
Material *whiteMaterial;

snow *snowArray;
rain *rainArray;
bool isSnowing = true;

UniformBufferObject *matricesUBO, *lightsUBO;

GLuint screenWidth = 800, screenHeight = 600;

const float movementSpeed = 10.0f;

std::vector<Model*> shadowedModels;


GLuint meshIndexCount = 0;
GLuint meshObjects[3];


GLuint normalMapProgram;


glm::vec3 eye(-1.0f, 1.0f, 4.0f);
glm::vec3 at(0.0f, 1.0f, -1.0f);
glm::vec3 up(0.0f, 1.0f, 0.0f);

std::stack<glm::mat4> mvStack;

// TEXTURE STUFF
GLuint textures[3];
GLuint skybox[5];
GLuint labels[5];

rt3d::lightStruct light0 = {
	{ 0.4f, 0.4f, 0.4f, 1.0f }, // ambient
	{ 1.0f, 1.0f, 1.0f, 1.0f }, // diffuse
	{ 1.0f, 1.0f, 1.0f, 1.0f }, // specular
	{ -5.0f, 2.0f, 2.0f, 1.0f }  // position
};
glm::vec4 lightPos(-5.0f, 2.0f, 2.0f, 1.0f); //light position

rt3d::materialStruct material0 = {
	{ 0.2f, 0.4f, 0.2f, 1.0f }, // ambient
	{ 0.5f, 1.0f, 0.5f, 1.0f }, // diffuse
	{ 0.0f, 0.1f, 0.0f, 1.0f }, // specular
	2.0f  // shininess
};
rt3d::materialStruct material1 = {
	{ 0.4f, 0.4f, 1.0f, 1.0f }, // ambient
	{ 0.8f, 0.8f, 1.0f, 1.0f }, // diffuse
	{ 0.0f, 0.1f, 0.0f, 1.0f }, // specular
	1.0f  // shininess
};

// light attenuation
float attConstant = 1.0f;
float attLinear = 0.0f;
float attQuadratic = 0.00f;


float theta = 0.0f;
bool  rotateToggle = true;

BumpMap *g_bumpMap = nullptr;

void checkOpenGLErr(const char* file, int line)
{
	GLint error;

	do
	{
		error = glGetError();

		if (error != GL_NO_ERROR)
			std::cout << "OpenGL error - " << file << " : " << line << " : ";

		switch (error)
		{
		case (GL_INVALID_ENUM) :
			std::cout << "Invalid Enum" << std::endl;
			break;
		case (GL_INVALID_VALUE) :
			std::cout << "Invalid Value" << std::endl;
			break;
		case (GL_INVALID_OPERATION) :
			std::cout << "Invalid Operation" << std::endl;
			break;
		case (GL_STACK_OVERFLOW) :
			std::cout << "Stack Overflow" << std::endl;
			break;
		case (GL_STACK_UNDERFLOW) :
			std::cout << "Stack Underflow" << std::endl;
			break;
		case (GL_OUT_OF_MEMORY) :
			std::cout << "Out of Memory" << std::endl;
			break;
		case (GL_INVALID_FRAMEBUFFER_OPERATION) :
			std::cout << "Invalid Framebuffer Operation" << std::endl;
			break;
		case (GL_TABLE_TOO_LARGE) :
			std::cout << "Table Too Large" << std::endl;
			break;
		}
	} while (error != GL_NO_ERROR);
}

// Set up rendering context
SDL_Window * setupRC(SDL_GLContext &context) {
	SDL_Window * window;
	if (SDL_Init(SDL_INIT_VIDEO) < 0) // Initialize video
		rt3d::exitFatalError("Unable to initialize SDL");

	// Request an OpenGL 3.3 context.

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);  // double buffering on
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8); // 8 bit alpha buffering
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4); // Turn on x4 multisampling anti-aliasing (MSAA)
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);

	// Create window
	window = SDL_CreateWindow("Stuff", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		screenWidth, screenHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if (!window) // Check window was created OK
		rt3d::exitFatalError("Unable to create window");

	context = SDL_GL_CreateContext(window); // Create opengl context and attach to window
	SDL_GL_SetSwapInterval(1); // set swap buffers to sync with monitor's vertical refresh rate
	return window;
}

void init(void)
{
	glm::mat4 projection = glm::perspective(60.0f, (float)screenWidth / screenHeight, 1.0f, 10000.0f);

	camera = new FreeCamera();
	camera->setPosition(glm::vec3(0.0f, 0.0f, 300.0f));

	lightShader = new Shader("Shaders/light.vert", "Shaders/light.frag", "Light");
	gouraudShader = new Shader("Shaders/gouraud.vert", "Shaders/gouraud.frag", "Gouraud");
	phongShader = new Shader("Shaders/phong.vert", "Shaders/phong.frag", "Phong");
	particlesShader = new Shader("Shaders/particles.vert", "Shaders/particles.frag", "Particles");
	silhouetteShader = new Shader("Shaders/Silhouette.vert", "Shaders/Silhouette.frag", "Shaders/Silhouette.geom", "Silhouette");
	shadowVolume = new Shader("Shaders/ShadowVolume.vert", "Shaders/ShadowVolume.frag", "Shaders/ShadowVolume.geom", "Shadow Volume");
	shadowVolume->setUniform("zFail", zFailAlgorithm);
	ambient = new Shader("Shaders/ambient.vert", "Shaders/ambient.frag", "Ambient");
	phong_excludingAmbient = new Shader("Shaders/phong_excAmbient.vert", "Shaders/phong_excAmbient.frag", "Phong - Excluding Ambient");
	currentShader = phongShader;

	rainTex = new Texture();
	rainTex->load("Textures/drop.bmp");
	snowTex = new Texture();
	snowTex->load("Textures/smoke1.bmp");

	whiteMaterial = new Material(
		glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
		glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 
		glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 
		1.0f);

	cube = new Model();
	cube->load("Models/cube.obj", false);
	cube->setShader(gouraudShader);

	room = new Model();
	room->load("Models/invertedCube.dae", true);
	room->setShader(gouraudShader);
	room->setMaterial(whiteMaterial);
	room->setPosition(glm::vec3(10.0f, 0.0f, -10.0f));
	room->setScale(glm::vec3(1000.0f, 500.0f, 1000.0f));
	shadowedModels.push_back(room);

	bunny = new Model();
	bunny->load("Models/bunny-5000.obj", true);
	bunny->setShader(gouraudShader);
	bunny->setMaterial(whiteMaterial);
	bunny->setPosition(glm::vec3(15.0f, 0.0f, 0.0f));
	bunny->setScale(glm::vec3(500.0f, 500.0f, 500.0f));
	shadowedModels.push_back(bunny);

	const GLchar * matriceNames[] =
	{
		"modelMat",
		"viewMat",
		"modelViewMat",
		"normalMat",
		"projectionMat",
		"mvpMat",
		"eyePos"
	};
	matricesUBO = new UniformBufferObject("MatrixBlock", matriceNames, 7, gouraudShader, 0, GL_DYNAMIC_DRAW);
	matricesUBO->setUniform("projectionMat", projection);

	const GLchar * lightsNames[] =
	{
		"numLights",
		"position",
		"direction",
		"ambient",
		"diffuse",
		"specular",
		"attenuation",
		"cosCutoffAngle",
		"secondaryCutoffAngle"
	};
	lightsUBO = new UniformBufferObject("LightBlock", lightsNames, 9, gouraudShader, 1, GL_DYNAMIC_DRAW);
	lightsUBO->setUniform("numLights", 0);

	lightManager = new LightManager(lightsUBO);

	AmbientLight *ambientLight = new AmbientLight(glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));
	lightManager->addLight(ambientLight);
	ambientLight->setTag("Ambient Light");

	//Create a default random number generator and seed it
	std::default_random_engine generator(7);
	//Create two distributions for the generator, one for the colour (float between 0 and 1), and one for the velocity(float between -0.1 and 0.1).
	std::uniform_real_distribution<float> lightDistribution(0.0f, 0.1f);

	//int i = 0;
	for (int row = 0; row < 2; row++)
	{
		for (int column = 0; column < 2; column++)
		{
			//i++;
			//if (i == 3) break;
			Pointlight *light = new Pointlight(
				glm::vec4((row+1)*100.0f - 200.0f, 50.0f, (column+1)*100 - 70.0f, 1.0f),
				glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
				//glm::vec4(lightDistribution(generator), lightDistribution(generator), lightDistribution(generator), 1.0f),
				//glm::vec4(lightDistribution(generator), lightDistribution(generator), lightDistribution(generator), 1.0f));
				glm::vec4(0.1f, 0.1f, 0.01f, 1.0f),
				glm::vec4(0.2f, 0.1f, 0.1f, 1.0f));
			light->setMesh(cube, lightShader);
			lightManager->addLight(light);
			light->setTag("Pointlight");
		}
	}

	//Initialise Snow + Rain
	snowArray = new snow(1000);
	snowArray->setTexture(snowTex);
	snowArray->init(particlesShader);
	rainArray = new rain(2000);
	rainArray->setTexture(rainTex);
	rainArray->init(particlesShader);





	// BUMP MAPPING
	std::cout << '\n' << "Started Bump Mapping!";

	g_bumpMap = new BumpMap();

	std::vector<GLfloat> tangents;
	std::vector<GLfloat> verts;
	std::vector<GLfloat> norms;
	std::vector<GLfloat> tex_coords;
	std::vector<GLuint> indices;
	rt3d::loadObj("cube.obj", verts, norms, tex_coords, indices);
	std::cout << '\n' << "Cube Loaded!";

	meshIndexCount = indices.size();

	std::cout << '\n' << "Calculating Tangents";
	g_bumpMap->calculateTangents(tangents, verts, norms, tex_coords, indices);


	std::cout << '\n' << "Creating Mesh";
	meshObjects[0] = rt3d::createMesh(verts.size() / 3, verts.data(), nullptr, norms.data(), tex_coords.data(), meshIndexCount, indices.data());
	// And add the tangent data

	glBindVertexArray(meshObjects[0]);
	GLuint VBO;
	glGenBuffers(1, &VBO);

	// VBO for tangents
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, tangents.size()*sizeof(GLfloat), tangents.data(), GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)5, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(5);
	glBindVertexArray(0);

	std::cout << '\n' << "Loading Bitmap";
	textures[2] = g_bumpMap->loadBitmap("fabric.bmp");
	textures[1] = g_bumpMap->loadBitmap("hobgoblin2.bmp");
	textures[0] = g_bumpMap->loadBitmap("fabric.bmp");
	textures[3] = g_bumpMap->loadBitmap("hhh.bmp");


	normalMapProgram = rt3d::initShaders("normalmap.vert", "normalmap.frag");

	glUseProgram(normalMapProgram);
	rt3d::setLight(normalMapProgram, light0);
	rt3d::setMaterial(normalMapProgram, material1);
	std::cout << '\n' << "Bump Mapping Done!";
	// BUMP MAPPING











	glEnable(GL_DEPTH_TEST);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_BLEND); 
	//glEnable(GL_PROGRAM_POINT_SIZE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearStencil(0); 
	glStencilMask(~0);
	glViewport(0, 0, screenWidth, screenHeight);
}

void update(float deltaTime) {
	camera->update();

	snowArray->update();
	rainArray->update();

	lightManager->update();
}

void handleInput()
{
	SDL_Event sdlEvent;
	while (SDL_PollEvent(&sdlEvent))
	{
		if (sdlEvent.type == SDL_KEYUP)
		{
			switch (sdlEvent.key.keysym.sym)
			{
			case SDLK_p:
				lightManager->flipSwitches("Pointlight");
				break;
			case SDLK_o:
				lightManager->flipSwitches("Torch");
				break;
			case SDLK_z:
				zFailAlgorithm = !zFailAlgorithm;
				shadowVolume->setUniform("zFail", zFailAlgorithm);
				break;
			case SDLK_t:
				drawVolumes = !drawVolumes;
				break;
			case SDLK_y:
				silhouette = !silhouette;
				break;
			case SDLK_i:
				lightManager->flipSwitches("Ambient Light");
				break;
			case SDLK_ESCAPE:
				running = false;
				break;
			}
		}
		if (sdlEvent.type == SDL_QUIT)
			running = false;
	}

	const Uint8 *keys = SDL_GetKeyboardState(NULL);
	if (keys[SDL_SCANCODE_W]) camera->moveForward(movementSpeed);
	if (keys[SDL_SCANCODE_S]) camera->moveForward(-movementSpeed);
	if (keys[SDL_SCANCODE_A]) camera->moveRight(-movementSpeed);
	if (keys[SDL_SCANCODE_D]) camera->moveRight(movementSpeed);
	if (keys[SDL_SCANCODE_R]) camera->moveUp(movementSpeed);
	if (keys[SDL_SCANCODE_F]) camera->moveUp(-movementSpeed);

	if (keys[SDL_SCANCODE_5]) isSnowing = false; //SWITCH TO RAIN	
	if (keys[SDL_SCANCODE_6]) isSnowing = true; //SWITCH TO SNOW

	if (keys[SDL_SCANCODE_1]) currentShader = gouraudShader; //Switch to Gouraud shader
	if (keys[SDL_SCANCODE_2]) currentShader = phongShader; //Switch to Phong shader

	if (keys[SDL_SCANCODE_COMMA]) camera->rotateY(-2.0f);
	if (keys[SDL_SCANCODE_PERIOD]) camera->rotateY(2.0f);
}

void drawAmbient(glm::mat4 viewMat, UniformBufferObject *matrixUBO)
{
	glPolygonOffset(0.0f, 0.0f);
	glDisable(GL_STENCIL_BUFFER);
	glClear(GL_DEPTH_BUFFER_BIT); 
	glDrawBuffer(GL_BACK);
	//Draw all models using only the ambient lighting, also writes to the depth buffer, which is necessary for the stencil test.
	for (auto model : shadowedModels)
	{
		model->setShader(ambient);
		model->draw(viewMat, matrixUBO);
	}
}

void drawToStencilBuffer(glm::mat4 viewMat, UniformBufferObject *matrixUBO)
{
	//Enable stencil test
	glEnable(GL_STENCIL_TEST);
	//Stop drawing to the colour buffer
	if (!drawVolumes)
		glDrawBuffer(GL_NONE);
	//Stop drawing to the depth buffer
	glDepthMask(GL_FALSE);
	//Disables culling polygons based on their winding order
	glDisable(GL_CULL_FACE);

	//Let the stencil test always pass.
	glStencilFunc(GL_ALWAYS, 0, 0x0);

	if (zFailAlgorithm)
	{
		glEnable(GL_DEPTH_CLAMP);
		glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
		glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);
	}
	else
	{
		glStencilOpSeparate(GL_BACK, GL_KEEP, GL_KEEP, GL_DECR_WRAP);
		glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_KEEP, GL_INCR_WRAP);
	}
	
	for (auto model : shadowedModels)
	{
		model->setShader(shadowVolume);
		model->draw(viewMat, matrixUBO);
	}

	//Restore original state.
	glDisable(GL_DEPTH_CLAMP);
	glEnable(GL_CULL_FACE);
	glDepthMask(GL_TRUE);
	glDrawBuffer(GL_BACK);
	glDisable(GL_STENCIL_TEST);
}

void drawScene(glm::mat4 viewMat, UniformBufferObject *matrixUBO)
{
	glPolygonOffset(-1.0f, -1.0f);
	//Enable stencil test
	glEnable(GL_STENCIL_TEST);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE);
	//Make the stencil test for stencil values that are equal to 0;
	glStencilFunc(GL_EQUAL, 0, ~0);

	//Do nothing to the stencil values. 
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	for (auto model : shadowedModels)
	{
		model->setShader(phong_excludingAmbient);
		model->draw(viewMat, matrixUBO);
	}

	glDisable(GL_STENCIL_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void drawSilhouettes(glm::mat4 viewMat, UniformBufferObject *matrixUBO)
{
	glLineWidth(5.0f);

	for (auto model : shadowedModels)
	{
		model->setShader(silhouetteShader);
		model->draw(viewMat, matrixUBO);
	}
}



#define DEG_TO_RADIAN 0.017453293
glm::vec3 moveForward(glm::vec3 pos, GLfloat angle, GLfloat d) {
	return glm::vec3(pos.x + d*std::sin(0.0f*DEG_TO_RADIAN), pos.y, pos.z - d*std::cos(0.0f*DEG_TO_RADIAN));
}

void draw(SDL_Window * window)
{
	// clear the screen
	glDisable(GL_STENCIL_BUFFER);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glDepthMask(GL_TRUE);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 viewMat = camera->getViewMat();
	glm::vec3 viewPos = camera->getPosition();
	matricesUBO->setUniform("viewMat", viewMat);
	matricesUBO->setUniform("eyePos", viewPos);

	lightManager->updateUniforms();
	lightManager->draw(viewMat, matricesUBO);

	drawAmbient(viewMat, matricesUBO);
	for (int i = 0; i < lightManager->lightCount(); i++)
	{
		glClear(GL_STENCIL_BUFFER_BIT);
		shadowVolume->setUniform("lightIndex", i);
		drawToStencilBuffer(viewMat, matricesUBO);
		phong_excludingAmbient->setUniform("lightIndex", i);
		drawScene(viewMat, matricesUBO);
	}

	if (silhouette)
		drawSilhouettes(viewMat, matricesUBO);

	if (isSnowing) //Particles Drawing
		snowArray->draw(10);
	else
		rainArray->draw(5);

	//BUMP MAPPING
	glm::mat4 projection(1.0);
	projection = glm::perspective(60.0f, 800.0f / 600.0f, 1.0f, 150.0f);


	glm::mat4 modelview(1.0); // set base position for scene
	mvStack.push(modelview);

	at = moveForward(eye, 0.0f, 1.0f);
	mvStack.top() = glm::lookAt(eye, at, up);
	glm::mat4 cameraMatrix = glm::lookAt(eye, at, up);


	glm::vec4 tmp = mvStack.top()*lightPos;
	light0.position[0] = tmp.x;
	light0.position[1] = tmp.y;
	light0.position[2] = tmp.z;

	glUseProgram(normalMapProgram);
	rt3d::setUniformMatrix4fv(normalMapProgram, "projection", glm::value_ptr(projection));
	rt3d::setLightPos(normalMapProgram, glm::value_ptr(tmp));
	g_bumpMap->render(textures, normalMapProgram, mvStack, theta, eye, up, at, meshObjects, meshIndexCount);


	SDL_GL_SwapWindow(window); // swap buffers
}

// Program entry point - SDL manages the actual WinMain entry point for us
int main(int argc, char *argv[]) {
	SDL_Window * hWindow; // window handle
	SDL_GLContext glContext; // OpenGL context handle
	hWindow = setupRC(glContext); // Create window and render context 

	// Required on Windows *only* init GLEW to access OpenGL beyond 1.1
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err) { // glewInit failed, something is seriously wrong
		std::cout << "glewInit failed, aborting." << std::endl;
		exit(1);
	}
	std::cout << glGetString(GL_VERSION) << std::endl;

	init();


	while (running)
	{	// the event loop
		handleInput();
		update(1.0f);
		draw(hWindow); // call the draw function
	}

	SDL_GL_DeleteContext(glContext);
	SDL_DestroyWindow(hWindow);
	SDL_Quit();
	return 0;
}