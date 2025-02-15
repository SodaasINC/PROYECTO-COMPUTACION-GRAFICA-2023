/*---------------------------------------------------------*/
/* ---------------------  Proyecto ------------------------*/
/*---------------------------------------------------------*/

#include <Windows.h>

#include <glad/glad.h>
#include <glfw3.h>	//main
#include <stdlib.h>		
#include <glm/glm.hpp>	//camera y model
#include <glm/gtc/matrix_transform.hpp>	//camera y model
#include <glm/gtc/type_ptr.hpp>
#include <time.h>


#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>	//Texture

#define SDL_MAIN_HANDLED
#include <SDL/SDL.h>

#include <shader_m.h>
#include <camera.h>
#include <modelAnim.h>
#include <model.h>
#include <Skybox.h>
#include <iostream>

//#pragma comment(lib, "winmm.lib")

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
//void my_input(GLFWwindow *window);
void my_input(GLFWwindow* window, int key, int scancode, int action, int mods);
void animate(void);

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;
GLFWmonitor *monitors;

void getResolution(void);

// camera
Camera camera(glm::vec3(0.0f, 10.0f, 90.0f));
float MovementSpeed = 0.1f;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
const int FPS = 60;
const int LOOP_TIME = 1000 / FPS; // = 16 milisec // 1000 millisec == 1 sec
double	deltaTime = 0.0f,
		lastFrame = 0.0f;

//Lighting
glm::vec3 lightPosition(0.0f, 4.0f, -10.0f);
glm::vec3 lightDirection(0.0f, -1.0f, 0.0f);
glm::vec3 lightBlue(0.0f, 0.0f, 0.0f);


glm::vec3 lightPower(0.0f, 0.0f, 0.0f);

float myVar = 0.0f;
float liPosZ = 0.0f;
float liPosY = 0.0f;
bool encender = false;
bool cambio = false;
int color_count = 0;
int fl = 0;


//// Light
glm::vec3 lightColor = glm::vec3(0.7f);
glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
glm::vec3 ambientColor = diffuseColor * glm::vec3(0.75f);

// posiciones
float	movAuto_x = 0.0f,
		movAuto_z = 0.0f,
		movAuto_y = 0.0f,
		orienta = 0.0f;
bool	animacion = false,
		recorrido1 = true,
		recorrido2 = false,
		recorrido3 = false,
		recorrido4 = false;

int circuitoAuto = 1;

int flag = 0;

//Keyframes (Manipulación y dibujo)
float	posX = 0.0f,
		posY = 0.0f,
		posZ = 0.0f,
		rotHead = 0.0f,
		rotRodIzq = 0.0f,
		rotRodDer = 0.0f,
		giroMonito = 0.0f,
		giroBraDer = 0.0f,
		giroBraIzq = 0.0f;
float	incX = 0.0f,
		incY = 0.0f,
		incZ = 0.0f,
		rotHeadInc = 0.0f,
		rotInc = 0.0f,
		rotDerInc = 0.0f,
		giroMonitoInc = 0.0f,
		giroBraDerInc = 0.0f,
		giroBraIzqInc = 0.0f;

#define MAX_FRAMES 19
int i_max_steps = 60;
int i_curr_steps = 0;
typedef struct _frame
{
	//Variables para GUARDAR Key Frames
	float posX;		//Variable para PosicionX
	float posY;		//Variable para PosicionY
	float posZ;		//Variable para PosicionZ
	float rotHead;
	float rotRodIzq;
	float rotRodDer;
	float giroMonito;
	float giroBraDer;
	float giroBraIzq;

}FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 0;			//introducir número en caso de tener Key guardados
bool play = false;
int playIndex = 0;

void saveFrame(void)
{
	//printf("frameindex %d\n", FrameIndex);
	std::cout << "Frame Index = " << FrameIndex << std::endl;

	KeyFrame[FrameIndex].posX = posX;
	KeyFrame[FrameIndex].posY = posY;
	KeyFrame[FrameIndex].posZ = posZ;

	KeyFrame[FrameIndex].rotHead = rotHead;

	KeyFrame[FrameIndex].rotRodIzq = rotRodIzq;
	KeyFrame[FrameIndex].rotRodDer = rotRodDer;
	KeyFrame[FrameIndex].giroMonito = giroMonito;

	KeyFrame[FrameIndex].giroBraDer = giroBraDer;
	KeyFrame[FrameIndex].giroBraIzq = giroBraIzq;

	FrameIndex++;
}

void resetElements(void)
{
	posX = KeyFrame[0].posX;
	posY = KeyFrame[0].posY;
	posZ = KeyFrame[0].posZ;

	rotHead = KeyFrame[0].rotHead;

	rotRodIzq = KeyFrame[0].rotRodIzq;
	rotRodDer = KeyFrame[0].rotRodDer;
	giroMonito = KeyFrame[0].giroMonito;

	giroBraDer = KeyFrame[0].giroBraDer;
	giroBraIzq = KeyFrame[0].giroBraIzq;
}

void interpolation(void)
{
	incX = (KeyFrame[playIndex + 1].posX - KeyFrame[playIndex].posX) / i_max_steps;
	incY = (KeyFrame[playIndex + 1].posY - KeyFrame[playIndex].posY) / i_max_steps;
	incZ = (KeyFrame[playIndex + 1].posZ - KeyFrame[playIndex].posZ) / i_max_steps;

	rotHeadInc = (KeyFrame[playIndex + 1].rotHead - KeyFrame[playIndex].rotHead) / i_max_steps;

	rotInc = (KeyFrame[playIndex + 1].rotRodIzq - KeyFrame[playIndex].rotRodIzq) / i_max_steps;
	rotDerInc = (KeyFrame[playIndex + 1].rotRodDer - KeyFrame[playIndex].rotRodDer) / i_max_steps;
	giroMonitoInc = (KeyFrame[playIndex + 1].giroMonito - KeyFrame[playIndex].giroMonito) / i_max_steps;

	giroBraDerInc = (KeyFrame[playIndex + 1].giroBraDer - KeyFrame[playIndex].giroBraDer) / i_max_steps;
	giroBraIzqInc = (KeyFrame[playIndex + 1].giroBraIzq - KeyFrame[playIndex].giroBraIzq) / i_max_steps;
}

void animate(void)
{
	liPosZ = sin(myVar);
	liPosY = cos(myVar);
	lightPosition.x = 2200.0f * sin(myVar);
	lightPosition.y = 2200.0f * cos(myVar);
	myVar += 0.006f;

	if(liPosY > 0)
		lightDirection = glm::vec3(-1.0f, 1.0f, 0.0f);
	if(liPosY < 0)
		lightDirection = glm::vec3(-liPosZ, -liPosY, 0.0f);

	if (cambio) {
		if (fl == 0) {
			lightBlue = glm::vec3(0.0f, 0.0f, 0.0f);
			fl = 1;
			cambio = false;
		}
		else {
			lightBlue = glm::vec3(0.0f, 0.0f, 1.0f);
			fl = 0;
			cambio = false;
		}
	}

	if (encender) {
		if (color_count < 15)
			lightPower = glm::vec3(1.0f, 0.0f, 0.0f);
		if (color_count >= 15 && color_count < 30)
			lightPower = glm::vec3(0.0f, 1.0f, 0.0f);
		if (color_count >= 30 && color_count < 45)
			lightPower = glm::vec3(0.0f, 0.0f, 1.0f);
		if (color_count >= 45 && color_count < 60)
			lightPower = glm::vec3(1.0f, 0.0f, 1.0f);
		if (color_count >= 60 && color_count < 75)
			lightPower = glm::vec3(1.0f, 1.0f, 0.0f);
		if (color_count >= 75 && color_count < 90)
			lightPower = glm::vec3(0.0f, 1.0f, 1.0f);
		if (color_count >= 90 && color_count < 95)
			color_count = 0;
		
		color_count += 1;



	}
	
	

	if (play)
	{
		
		if (i_curr_steps >= i_max_steps) //end of animation between frames?
		{
			playIndex++;
			if (playIndex > FrameIndex - 2)	//end of total animation?
			{
				std::cout << "Animation ended" << std::endl;
				//printf("termina anim\n");
				playIndex = 0;
				play = false;
			}
			else //Next frame interpolations
			{
				i_curr_steps = 0; //Reset counter
								  //Interpolation
				interpolation();
			}
		}
		else
		{
			//Draw animation
			posX += incX;
			posY += incY;
			posZ += incZ;

			rotHead += rotHeadInc;

			rotRodIzq += rotInc;
			rotRodDer += rotDerInc;
			giroMonito += giroMonitoInc;

			giroBraDer += giroBraDerInc;
			giroBraIzq += giroBraIzqInc;

			i_curr_steps++;
		}
	}

	//Vehículo
	if (animacion)
	{	
		if (circuitoAuto == 1) {
			movAuto_z += 3.5f;
			orienta = 0.0f;
			if (movAuto_z >= 200.0f)
				circuitoAuto = 6;
		}
		if (circuitoAuto == 2) {
			orienta = -90.0f;
			movAuto_x -= 3.0f;
			if (movAuto_x <= -250.0f)
				circuitoAuto = 3;			
		}
		if (circuitoAuto == 3) {
			orienta = -180.0f;
			movAuto_z -= 3.0f;
			if (movAuto_z <= -200.0f)
				circuitoAuto = 4;
		}
		if (circuitoAuto == 4) {
			orienta = -270.0f;
			movAuto_x += 3.0f;
			if (movAuto_x >= 0.0f)
				circuitoAuto = 5;
		}

		if (circuitoAuto == 5) {
			orienta = 33.0f;
			movAuto_x += 1.3f;
			movAuto_z += 2.0f;
			if (movAuto_x >= 65.0f && movAuto_z >= -100)
				circuitoAuto = 7;
		}

		if (circuitoAuto == 7) {
			orienta = -33.0f;
			movAuto_x -= 1.3f;
			movAuto_z += 2.0f;
			if (movAuto_x <= 0.0f && movAuto_z >= 0.0f)
				circuitoAuto = 1;
		}

		if (circuitoAuto == 6) {
			orienta = -148.0f;
			movAuto_x -= 1.875f;
			movAuto_z -= 3.0f;
			if (movAuto_x <= -250.0f && movAuto_z <= -200.0f)
				circuitoAuto = 4;
		
		}
		/*
		if (flag == 0) {//Cuando el coche llega a la posicion
			movAuto_z -= 2.0f;//controla la velocidad incrementando  el valor y poniendolo negativo cuando se mueve al lado opuesto
			if (movAuto_z <= -170.0f)
				flag = 1;
		}

		if (flag == 1){
			movAuto_y += 2.0f;
			if (movAuto_y >= 200.0f) {
				movAuto_y += 0.5f;
				flag = 2;
			}
		}

		if (flag == 2) {
			movAuto_z += 2.0f;
			if (movAuto_z >= 240.0f) {
				movAuto_z += 0.5f;
				flag = 3;
			}
		}

		if (flag == 3) {
			movAuto_y -= 2.0f;
			if (movAuto_y <= 0.5f) {
				movAuto_y = 0.0f;
				flag = 4;
			}
		}

		if (flag == 4) {
			movAuto_z += 2.0f;
			if (movAuto_z >= 540.0f) {
				movAuto_z += 0.5f;
				animacion = false;
			}
		}*/

	}
}

void getResolution()
{
	const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	SCR_WIDTH = mode->width;
	SCR_HEIGHT = (mode->height) - 80;
}


int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	/*glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);*/

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	// --------------------
	monitors = glfwGetPrimaryMonitor();
	getResolution();

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "CGeIHC 20241", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwSetWindowPos(window, 0, 30);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, my_input);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile shaders
	// -------------------------
	Shader staticShader("Shaders/shader_Lights.vs", "Shaders/shader_Lights_mod.fs");
	Shader skyboxShader("Shaders/skybox.vs", "Shaders/skybox.fs");
	Shader animShader("Shaders/anim.vs", "Shaders/anim.fs");

	vector<std::string> faces
	{
		"resources/skybox/right.jpg",
		"resources/skybox/left.jpg",
		"resources/skybox/top.jpg",
		"resources/skybox/bottom.jpg",
		"resources/skybox/front.jpg",
		"resources/skybox/back.jpg"
	};

	Skybox skybox = Skybox(faces);

	// Shader configuration
	// --------------------
	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);

	// load models
	// -----------
	Model piso("resources/objects/piso/piso.obj");
	

	ModelAnim ninja("resources/objects/ZombieWalk/ZombieWalk.dae");
	ninja.initShaders(animShader.ID);


	//Inicialización de KeyFrames VALORES YA PROGRAMADOS DE ANIMACION
	for (int i = 0; i < MAX_FRAMES; i++)
	{
		KeyFrame[i].posX = 0;
		KeyFrame[i].posY = 0;
		KeyFrame[i].posZ = 0;
		KeyFrame[i].rotHead = 0;
		KeyFrame[i].rotRodIzq = 0;
		KeyFrame[i].rotRodDer = 0;
		KeyFrame[i].giroMonito = 0;
		KeyFrame[i].giroBraDer = 0;
		KeyFrame[i].giroBraIzq = 0;
	}

	// draw in wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		skyboxShader.setInt("skybox", 0);

		// per-frame time logic
		// --------------------
		lastFrame = SDL_GetTicks();

		// input
		// -----
		//my_input(window);
		animate();

		// render
		// ------
		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// don't forget to enable shader before setting uniforms
		staticShader.use();
		//SOL
		//Setup Advanced Lights
		//staticShader.setVec3("viewPos", camera.Position);
		//staticShader.setVec3("dirLight.direction", lightDirection);
		staticShader.setVec3("viewPos", lightPosition);
		staticShader.setVec3("dirLight.direction", lightDirection);
		staticShader.setVec3("dirLight.ambient", glm::vec3(0.39f, 0.37f, 0.37f));
		staticShader.setVec3("dirLight.diffuse", glm::vec3(0.39f, 0.37f, 0.37f));
		staticShader.setVec3("dirLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
		//0.39f, 0.37f, 0.37f
		//0.80f, 0.37f, 0.80f

		staticShader.setVec3("pointLight[0].position", glm::vec3(-50.0, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[0].ambient", lightBlue);
		staticShader.setVec3("pointLight[0].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[0].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("pointLight[0].constant", 0.0008f);
		staticShader.setFloat("pointLight[0].linear", 0.0009f);
		staticShader.setFloat("pointLight[0].quadratic", 0.00032f);

		//ejercicio
		staticShader.setVec3("pointLight[1].position", glm::vec3(50.0, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[1].ambient", lightPower);
		staticShader.setVec3("pointLight[1].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[1].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("pointLight[1].constant", 0.0008f);
		staticShader.setFloat("pointLight[1].linear", 0.0009f);
		staticShader.setFloat("pointLight[1].quadratic", 0.00032f);

		staticShader.setVec3("pointLight[2].position", glm::vec3(-80.0, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[2].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[2].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[2].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("pointLight[2].constant", 0.001f);
		staticShader.setFloat("pointLight[2].linear", 0.0009f);
		staticShader.setFloat("pointLight[2].quadratic", 0.0032f);

		//staticShader.setVec3("spotLight[0].position", glm::vec3(camera.Position.x, camera.Position.y, camera.Position.z));
		//staticShader.setVec3("spotLight[0].direction", glm::vec3(camera.Front.x, camera.Front.y, camera.Front.z));
		staticShader.setVec3("spotLight[0].position", lightPosition);
		staticShader.setVec3("spotLight[0].direction", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("spotLight[0].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("spotLight[0].diffuse", glm::vec3(0.0f, 0.0f, 0.9f));
		staticShader.setVec3("spotLight[0].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("spotLight[0].cutOff", glm::cos(glm::radians(10.0f)));
		staticShader.setFloat("spotLight[0].outerCutOff", glm::cos(glm::radians(15.0f)));
		staticShader.setFloat("spotLight[0].constant", 0.001f);
		staticShader.setFloat("spotLight[0].linear", 0.009f);
		staticShader.setFloat("spotLight[0].quadratic", 0.005f);

		staticShader.setFloat("material_shininess", 32.0f);

		glm::mat4 model = glm::mat4(1.0f);	
		glm::mat4 tmp = glm::mat4(1.0f);
		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
		glm::mat4 view = camera.GetViewMatrix();
		staticShader.setMat4("projection", projection);
		staticShader.setMat4("view", view);

		// -------------------------------------------------------------------------------------------------------------------------
		// Personaje Animacion
		// -------------------------------------------------------------------------------------------------------------------------
		//Remember to activate the shader with the animation
		
		animShader.use();
		animShader.setMat4("projection", projection);
		animShader.setMat4("view", view);

		animShader.setVec3("material.specular", glm::vec3(0.5f));
		animShader.setFloat("material.shininess", 32.0f);
		animShader.setVec3("light.ambient", ambientColor);
		animShader.setVec3("light.diffuse", diffuseColor);
		animShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
		animShader.setVec3("light.direction", lightDirection);
		animShader.setVec3("viewPos", camera.Position);

		// -------------------------------------------------------------------------------------------------------------------------
		// Segundo Personaje Animacion
		// -------------------------------------------------------------------------------------------------------------------------

		model = glm::translate(glm::mat4(1.0f), glm::vec3(40.3f, 1.75f, 0.3f)); // translate it down so it's at the center of the scene
		model = glm::scale(model, glm::vec3(0.5f));	// it's a bit too big for our scene, so scale it down
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		animShader.setMat4("model", model);
		ninja.Draw(animShader);

		// -------------------------------------------------------------------------------------------------------------------------
		// Escenario
		// -------------------------------------------------------------------------------------------------------------------------
		staticShader.use();
		staticShader.setMat4("projection", projection);
		staticShader.setMat4("view", view);

		//model = glm::translate(glm::mat4(1.0f), glm::vec3(250.0f, 0.0f, -10.0f));
		
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.0f, 0.5f));
		staticShader.setMat4("model", model);
		piso.Draw(staticShader);
		

		// -------------------------------------------------------------------------------------------------------------------------
		// Caja Transparente --- Siguiente Práctica
		// -------------------------------------------------------------------------------------------------------------------------
		/*glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -70.0f));
		model = glm::scale(model, glm::vec3(5.0f));
		staticShader.setMat4("model", model);
		cubo.Draw(staticShader);
		glEnable(GL_BLEND);*/
		// -------------------------------------------------------------------------------------------------------------------------
		// Termina Escenario
		// -------------------------------------------------------------------------------------------------------------------------

		//-------------------------------------------------------------------------------------
		// draw skybox as last
		// -------------------
		skyboxShader.use();
		skybox.Draw(skyboxShader, view, projection, camera);

		// Limitar el framerate a 60
		deltaTime = SDL_GetTicks() - lastFrame; // time for full 1 loop
		//std::cout <<"frame time = " << frameTime << " milli sec"<< std::endl;
		if (deltaTime < LOOP_TIME)
		{
			SDL_Delay((int)(LOOP_TIME - deltaTime));
		}

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	skybox.Terminate();

	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void my_input(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, (float)deltaTime);
	//To Configure Model
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
		posZ++;
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
		posZ--;
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
		posX--;
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		posX++;
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
		posY--;
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
		posY++;
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS){
		rotRodIzq--;
		rotRodDer++;
	}
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS){
		rotRodIzq++;
		rotRodDer--;
	}
	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
		giroMonito--;
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
		giroMonito++;
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
		lightPosition.x++;
	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
		lightPosition.x--;
	if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)
		giroBraDer += 3.0f;
	if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS)
		giroBraDer -= 3.0f;
	if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS)
		giroBraIzq += 3.0f;
	if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS)
		giroBraIzq -= 3.0f;
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		rotHead += 3.0f;
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		rotHead -= 3.0f;
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
		lightPower.x = (0.5f);
		lightPower.y = (0.5f);
		lightPower.z = (0.5f);
		encender = true;
	}

	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
		lightPower.x = (0.0f);
		lightPower.y = (0.0f);
		lightPower.z = (0.0f);
		encender = false;
	}

	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
		cambio = true;
	}
	

	//Car animation
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
		animacion ^= true;
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
		movAuto_z = 0.0f;
		movAuto_y = 0.0f;
		animacion = false;
		flag = 0;
	}

	//To play KeyFrame animation 
	if (key == GLFW_KEY_P && action == GLFW_PRESS)
	{
		if (play == false && (FrameIndex > 1))
		{
			std::cout << "Play animation" << std::endl;
			resetElements();
			//First Interpolation				
			interpolation();

			play = true;
			playIndex = 0;
			i_curr_steps = 0;
		}
		else
		{
			play = false;
			std::cout << "Not enough Key Frames" << std::endl;
		}
	}

	//To Save a KeyFrame
	if (key == GLFW_KEY_L && action == GLFW_PRESS)
	{
		if (FrameIndex < MAX_FRAMES)
		{
			saveFrame();
		}
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}
// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}
