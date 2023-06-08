/*---------------------------------------------------------*/
/* ----------------  Proyecto final                  -----------*/
/*-----------------    2023-2   ---------------------------*/
/*------------- Alumno:Moreno Duran Jaime---------------*/
/*------------- No. Cuenta: 316064378---------------*/
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
glm::vec3 lightDirection(0.0f, -1.0f, -1.0f);



//Keyframes (Manipulación y dibujo)
float	posX = 0.0f,
		posY = 0.0f,
		posZ = 0.0f,
		alaDer=0.0f,
		alaIzq=0.0f,
		pataDer=0.0f,
		pataIzq=0.0f,
		cabeza = 0.0f,
		giroMonito = 0.0f;
float	incX = 0.0f,
		incY = 0.0f,
		incZ = 0.0f,
		alaDerInc = 0.0f,
		alaIzqInc = 0.0f,
		pataDerInc = 0.0f,
		pataIzqInc = 0.0f,
		cabezaInc = 0.0f,
		giroMonitoInc = 0.0f;

#define MAX_FRAMES 20
int i_max_steps = 60;
int i_curr_steps = 0;
typedef struct _frame
{
	//Variables para GUARDAR Key Frames
	float posX;		//Variable para PosicionX
	float posY;		//Variable para PosicionY
	float posZ;		//Variable para PosicionZ
	
	float	alaDer;
	float	alaIzq;
	float	pataDer;
	float	pataIzq;
	float   cabeza;
	float giroMonito;

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
	KeyFrame[FrameIndex].alaDer = alaDer;
	KeyFrame[FrameIndex].alaIzq = alaIzq;
	KeyFrame[FrameIndex].pataDer = pataDer;
	KeyFrame[FrameIndex].cabeza = cabeza;
	KeyFrame[FrameIndex].pataIzq = pataIzq;

	FrameIndex++;
}

void resetElements(void)
{
	posX = KeyFrame[0].posX;
	posY = KeyFrame[0].posY;
	posZ = KeyFrame[0].posZ;
	alaDer = KeyFrame[0].alaDer;
	alaIzq = KeyFrame[0].alaIzq;
	pataDer = KeyFrame[0].pataDer;
	pataIzq = KeyFrame[0].pataIzq;
	cabeza = KeyFrame[0].cabeza;
	giroMonito = KeyFrame[0].giroMonito;

}

void interpolation(void)
{
	incX = (KeyFrame[playIndex + 1].posX - KeyFrame[playIndex].posX) / i_max_steps;
	incY = (KeyFrame[playIndex + 1].posY - KeyFrame[playIndex].posY) / i_max_steps;
	incZ = (KeyFrame[playIndex + 1].posZ - KeyFrame[playIndex].posZ) / i_max_steps;

	alaDerInc = (KeyFrame[playIndex + 1].alaDer - KeyFrame[playIndex].alaDer) / i_max_steps;
	alaIzqInc = (KeyFrame[playIndex + 1].alaIzq - KeyFrame[playIndex].alaIzq) / i_max_steps;
	pataDerInc = (KeyFrame[playIndex + 1].pataDer - KeyFrame[playIndex].pataDer) / i_max_steps;
	pataIzqInc = (KeyFrame[playIndex + 1].pataIzq - KeyFrame[playIndex].pataIzq) / i_max_steps;
	cabezaInc = (KeyFrame[playIndex + 1].cabeza - KeyFrame[playIndex].cabeza) / i_max_steps;

	giroMonitoInc = (KeyFrame[playIndex + 1].giroMonito - KeyFrame[playIndex].giroMonito) / i_max_steps;

}

void animate(void)
{
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
			alaDer += alaDerInc;
			alaIzq += alaIzqInc;
			pataDer += pataDerInc;
			pataIzq += pataIzqInc;
			cabeza += cabezaInc;
			giroMonito += giroMonitoInc;

			i_curr_steps++;
		}
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

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "CGeIHC", NULL, NULL);
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
	//Model piso("resources/objects/pasto/pastos.obj");
	Model cabezaP("resources/objects/PatoCabeza/PatoCabeza.obj");
	Model alaizqP("resources/objects/PatoAlaIzq/PatoAlaIzq.obj");
	Model aladerP("resources/objects/PatoAlaDer/PatoAlaDer.obj");
	Model pataizqP("resources/objects/PatoPataIzq/PatoPataIzq.obj");
	Model pataderP("resources/objects/PatoPataDer/PatoPataDer.obj");
	Model cuerpoP("resources/objects/PatoCuerpo/PatoCuerpo.obj");



	Model piso("resources/objects/piso/piso.obj");
	Model Escenario("resources/objects/Paredes/EscenarioListo.obj");
	Model Elefantes("resources/objects/ZonaElefantesTexturizado/ZonaEledantes.obj");
	Model Lobos("resources/objects/ZonaLobosTexturizado/Lobos.obj");
	Model Patos("resources/objects/ZonaPatosTexturizado/Patos.obj");
	Model Monos("resources/objects/ZonaMonosTexturizado/ZonaMonos.obj");
	Model Pandas("resources/objects/ZonaPandasTexturizado/ZonaPandas.obj");
	Model Tigres("resources/objects/ZonaTigreTexturizada/ZonaTigres.obj");
	Model Leones("resources/objects/ZonaLeoneTexturizado/ZonaLeonNuevo.obj");
	Model TiendaR("resources/objects/TiendaT/TiendaCT.obj");
	Model Estacionamiento("resources/objects/Estacionamiento/EstacionamintoCT.obj");


	//Granjero

	ModelAnim Elf("resources/objects/PersonaAN1/Person.fbx");
	Elf.initShaders(animShader.ID);

	//Oso Granjero

	ModelAnim oso("resources/objects/PersonaAN1/oso.fbx");
	oso.initShaders(animShader.ID);

	// Panda

	ModelAnim Panda("resources/objects/PersonaAN2/PandaAnim.fbx");
	Panda.initShaders(animShader.ID);

	//Mono

	ModelAnim Mono("resources/objects/PersonaAN3/MonoHuesos.fbx");
	Mono.initShaders(animShader.ID);

	//Elefante

	ModelAnim ElefanteA("resources/objects/PersonaAN4/ElefanteA.fbx");
	ElefanteA.initShaders(animShader.ID);



	//MODELOS PROYECTO

	//ninja.initShaders(animShader.ID);

	//Inicialización de KeyFrames
	for (int i = 0; i < MAX_FRAMES; i++)
	{
		KeyFrame[i].posX = 0;
		KeyFrame[i].posY = 0;
		KeyFrame[i].posZ = 0;

		KeyFrame[i].alaDer = 0;
		KeyFrame[i].alaIzq = 0;
		KeyFrame[i].pataDer = 0;
		KeyFrame[i].pataIzq = 0;
		KeyFrame[i].cabeza = 0;

		KeyFrame[i].giroMonito = 0;
	}
	
	FrameIndex = 7;

	KeyFrame[0].posX = 0.000000;
	KeyFrame[0].posY = 0.000000;
	KeyFrame[0].posZ = 0.000000;
	KeyFrame[0].alaDer = 0.000000;
	KeyFrame[0].alaIzq = 0.000000;
	KeyFrame[0].pataDer = 0.000000;
	KeyFrame[0].pataIzq = 0.000000;
	KeyFrame[0].cabeza = 0.000000;
	KeyFrame[0].giroMonito = 0.000000;
	KeyFrame[1].posX = 0.000000;
	KeyFrame[1].posY = 0.000000;
	KeyFrame[1].posZ = 17.000000;
	KeyFrame[1].alaDer = -7.000000;
	KeyFrame[1].alaIzq = 2.000000;
	KeyFrame[1].pataDer = -83.000000;
	KeyFrame[1].pataIzq = 0.000000;
	KeyFrame[1].cabeza = 0.000000;
	KeyFrame[1].giroMonito = 0.000000;
	KeyFrame[2].posX = 0.000000;
	KeyFrame[2].posY = 0.000000;
	KeyFrame[2].posZ = 22.000010;
	KeyFrame[2].alaDer = 42.000000;
	KeyFrame[2].alaIzq = -43.000000;
	KeyFrame[2].pataDer = 3.999985;
	KeyFrame[2].pataIzq = -86.000000;
	KeyFrame[2].cabeza = 0.000000;
	KeyFrame[2].giroMonito = 0.000000;
	KeyFrame[3].posX = 6.000000;
	KeyFrame[3].posY = 0.000000;
	KeyFrame[3].posZ = 56.000046;
	KeyFrame[3].alaDer = -16.000000;
	KeyFrame[3].alaIzq = 13.000000;
	KeyFrame[3].pataDer = -89.000053;
	KeyFrame[3].pataIzq = 10.999977;
	KeyFrame[3].cabeza = 0.000000;
	KeyFrame[3].giroMonito = 0.000000;
	KeyFrame[4].posX = 25.999996;
	KeyFrame[4].posY = 0.000000;
	KeyFrame[4].posZ = 75.000015;
	KeyFrame[4].alaDer = 42.999992;
	KeyFrame[4].alaIzq = -48.000004;
	KeyFrame[4].pataDer = 2.999886;
	KeyFrame[4].pataIzq = -80.000031;
	KeyFrame[4].cabeza = 0.000000;
	KeyFrame[4].giroMonito = 0.000000;
	KeyFrame[5].posX = 28.000010;
	KeyFrame[5].posY = 0.000000;
	KeyFrame[5].posZ = 86.999962;
	KeyFrame[5].alaDer = -16.000000;
	KeyFrame[5].alaIzq = 10.000004;
	KeyFrame[5].pataDer = -80.000153;
	KeyFrame[5].pataIzq = 1.999931;
	KeyFrame[5].cabeza = 25.000000;
	KeyFrame[5].giroMonito = 0.000000;
	KeyFrame[6].posX = 32.999977;
	KeyFrame[6].posY = 0.000000;
	KeyFrame[6].posZ = 127.999779;
	KeyFrame[6].alaDer = 39.999992;
	KeyFrame[6].alaIzq = -46.999973;
	KeyFrame[6].pataDer = 14.999825;
	KeyFrame[6].pataIzq = 14.999901;
	KeyFrame[6].cabeza = -40.000008;
	KeyFrame[6].giroMonito = 0.000000;

	
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
		//Setup Advanced Lights
		staticShader.setVec3("viewPos", camera.Position);
		staticShader.setVec3("dirLight.direction", lightDirection);
		staticShader.setVec3("dirLight.ambient", glm::vec3(0.5f, 0.5f, 0.5f));
		staticShader.setVec3("dirLight.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
		staticShader.setVec3("dirLight.specular", glm::vec3(0.0f, 0.0f, 0.0f));

		staticShader.setVec3("pointLight[0].position", lightPosition);
		staticShader.setVec3("pointLight[0].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[0].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[0].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("pointLight[0].constant", 0.08f);
		staticShader.setFloat("pointLight[0].linear", 0.009f);
		staticShader.setFloat("pointLight[0].quadratic", 0.032f);

		staticShader.setVec3("pointLight[1].position", glm::vec3(-80.0, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[1].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[1].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[1].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("pointLight[1].constant", 1.0f);
		staticShader.setFloat("pointLight[1].linear", 0.009f);
		staticShader.setFloat("pointLight[1].quadratic", 0.032f);

		staticShader.setVec3("spotLight[0].position", glm::vec3(0.0f, 20.0f, 10.0f));
		staticShader.setVec3("spotLight[0].direction", glm::vec3(0.0f, -1.0f, 0.0f));
		staticShader.setVec3("spotLight[0].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("spotLight[0].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("spotLight[0].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("spotLight[0].cutOff", glm::cos(glm::radians(10.0f)));
		staticShader.setFloat("spotLight[0].outerCutOff", glm::cos(glm::radians(60.0f)));
		staticShader.setFloat("spotLight[0].constant", 1.0f);
		staticShader.setFloat("spotLight[0].linear", 0.0009f);
		staticShader.setFloat("spotLight[0].quadratic", 0.0005f);

		staticShader.setFloat("material_shininess", 32.0f);

		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 tmp = glm::mat4(1.0f);

		glm::mat4 model1 = glm::mat4(1.0f);
		glm::mat4 tmp1 = glm::mat4(1.0f);

		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
		glm::mat4 view = camera.GetViewMatrix();
		staticShader.setMat4("projection", projection);
		staticShader.setMat4("view", view);

		//// Light
		glm::vec3 lightColor = glm::vec3(0.6f);
		glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
		glm::vec3 ambientColor = diffuseColor * glm::vec3(0.75f);


		// -------------------------------------------------------------------------------------------------------------------------
		// Personaje Animacion
		// -------------------------------------------------------------------------------------------------------------------------
		//Remember to activate the shader with the animation
	

		animShader.use();
		animShader.setMat4("projection", projection);
		animShader.setMat4("view", view);

		animShader.setVec3("material.specular", glm::vec3(0.5f));
		animShader.setFloat("material.shininess", 32.0f);
		animShader.setVec3("light.ambient", lightColor);
		animShader.setVec3("light.diffuse", diffuseColor);
		animShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
		animShader.setVec3("light.direction", lightDirection);
		animShader.setVec3("viewPos", camera.Position);


		// -------------------------------------------------------------------------------------------------------------------------
		// Segundo Personaje Animacion
		// -------------------------------------------------------------------------------------------------------------------------
		//Granjero

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-110.0f, 2.0f, -150.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		animShader.setMat4("model", model);
		Elf.Draw(animShader);

		//Osito

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-325.0f, 25.0f, -140.0f));
		model = glm::scale(model, glm::vec3(0.3f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		animShader.setMat4("model", model);
		oso.Draw(animShader);

		//Panda

		model = glm::translate(tmp, glm::vec3(365.0f, 2.5f, -170.0f));
		model = glm::scale(model, glm::vec3(0.05f));
		animShader.setMat4("model", model);
		Panda.Draw(animShader);

		//Mono

		model = glm::translate(tmp, glm::vec3(357.0f, 2.5f, 18.0f));
		model = glm::scale(model, glm::vec3(0.03f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		animShader.setMat4("model", model);
		Mono.Draw(animShader);
		
		// Elefante

		model = glm::translate(tmp, glm::vec3(235.0f, 2.5f, 150.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.03f));
		animShader.setMat4("model", model);
		ElefanteA.Draw(animShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(40.3f, 1.75f, 0.3f)); // translate it down so it's at the center of the scene
		model = glm::scale(model, glm::vec3(0.5f));	// it's a bit too big for our scene, so scale it down
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		//animShader.setMat4("model", model);
		//ninja.Draw(animShader);
		

		// -------------------------------------------------------------------------------------------------------------------------
		// Escenario
		// -------------------------------------------------------------------------------------------------------------------------
		staticShader.use();
		staticShader.setMat4("projection", projection);
		staticShader.setMat4("view", view);


		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		staticShader.setMat4("model", model);
		piso.Draw(staticShader);

		


		// -------------------------------------------------------------------------------------------------------------------------
		// Escenario
		// -------------------------------------------------------------------------------------------------------------------------
		staticShader.use();
		staticShader.setMat4("projection", projection);
		staticShader.setMat4("view", view);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(250.0f, 0.0f, -10.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		//casaDoll.Draw(staticShader);

		

		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -70.0f));
		model = glm::scale(model, glm::vec3(5.0f));
		staticShader.setMat4("model", model);
		//casaVieja.Draw(staticShader);

		// -------------------------------------------------------------------------------------------------------------------------
		// Carro
		// -------------------------------------------------------------------------------------------------------------------------
		

		model = glm::translate(tmp, glm::vec3(8.5f, 2.5f, 12.9f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		staticShader.setMat4("model", model);
		//llanta.Draw(staticShader);	//Izq delantera

		model = glm::translate(tmp, glm::vec3(-8.5f, 2.5f, 12.9f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		//llanta.Draw(staticShader);	//Der delantera

		model = glm::translate(tmp, glm::vec3(-8.5f, 2.5f, -14.5f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		//llanta.Draw(staticShader);	//Der trasera

		model = glm::translate(tmp, glm::vec3(8.5f, 2.5f, -14.5f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		staticShader.setMat4("model", model);
		//llanta.Draw(staticShader);	//Izq trase
		// -------------------------------------------------------------------------------------------------------------------------
		// Personaje
		// -------------------------------------------------------------------------------------------------------------------------
		model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
		model = glm::translate(model, glm::vec3(posX, posY, posZ));
		tmp = model = glm::rotate(model, glm::radians(giroMonito), glm::vec3(0.0f, 1.0f, 0.0));
		staticShader.setMat4("model", model);
		//torso.Draw(staticShader);

		//Pierna Der
	

		//Pie Der
		model = glm::translate(model, glm::vec3(0, -0.9f, -0.2f));
		staticShader.setMat4("model", model);
		//botaDer.Draw(staticShader);

		//Pierna Izq
		model = glm::translate(tmp, glm::vec3(0.5f, 0.0f, -0.1f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		//piernaIzq.Draw(staticShader);

		//Pie Iz
		model = glm::translate(model, glm::vec3(0, -0.9f, -0.2f));
		staticShader.setMat4("model", model);
		//botaDer.Draw(staticShader);	//Izq trase

		//Brazo derecho
		model = glm::translate(tmp, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(-0.75f, 2.5f, 0));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		//brazoDer.Draw(staticShader);

		//Brazo izquierdo
		model = glm::translate(tmp, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(0.75f, 2.5f, 0));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", model);
		//brazoIzq.Draw(staticShader);

		//Cabeza
		model = glm::translate(tmp, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::translate(model, glm::vec3(0.0f, 2.5f, 0));
		staticShader.setMat4("model", model);



		//Proyecto Importaciones:
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		staticShader.setMat4("model", model);
		piso.Draw(staticShader);

		//Escenario

		model = glm::translate(tmp1, glm::vec3(160.5f, 2.5f, 12.9f));
		model = glm::scale(model, glm::vec3(4.5f, 4.5f, 5.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		Escenario.Draw(staticShader);

		//Zona Elefante

		model = glm::translate(tmp1, glm::vec3(250.0f, 2.5f, 140.0f));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		Elefantes.Draw(staticShader);


		//Zona de Lobos

		model = glm::translate(tmp1, glm::vec3(110.0f, 2.5f, 260.0f));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		Lobos.Draw(staticShader);

		//Zona Patos

		model = glm::translate(tmp1, glm::vec3(190.0f, 2.5f, -20.0f));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		Patos.Draw(staticShader);

		//Zona Monos

		model = glm::translate(tmp1, glm::vec3(360.0f, 2.5f, 20.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		Monos.Draw(staticShader);

		//Zona Pandas

		model = glm::translate(tmp1, glm::vec3(375.0f, 2.5f, -170.0f));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		Pandas.Draw(staticShader);

		//Zona Leones

		model = glm::translate(tmp1, glm::vec3(190.0f, 2.5f, -180.0f));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		Leones.Draw(staticShader);

		//Zona Tigres
		model = glm::translate(tmp1, glm::vec3(420.0f, 2.5f, 240.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		Tigres.Draw(staticShader);

		//Tienda De regalos

		model = glm::translate(tmp1, glm::vec3(-350.0f, 2.5f, -100.0f));
		model = glm::scale(model, glm::vec3(6.0f, 6.0f, 6.0f));
		model = glm::rotate(model, glm::radians(360.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		TiendaR.Draw(staticShader);

		//Estacionamiento

		model = glm::translate(tmp1, glm::vec3(-200.0f, 2.5f, 200.0f));
		model = glm::scale(model, glm::vec3(0.35f, 0.35f, 0.35f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		Estacionamiento.Draw(staticShader);



		// -------------------------------------------------------------------------------------------------------------------------
		// Personaje
		// -------------------------------------------------------------------------------------------------------------------------
		model = glm::translate(glm::mat4(1.0f), glm::vec3(100.0f, 3.0f, 0.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(posX, posY, posZ));
		tmp = model = glm::rotate(model, glm::radians(giroMonito), glm::vec3(0.0f, 1.0f, 0.0));
		staticShader.setMat4("model", model);
		cuerpoP.Draw(staticShader);

		//Ala Der
		model = glm::translate(tmp, glm::vec3(-2.0f, 8.0f, 00.0f));
		model = glm::rotate(model, glm::radians(alaDer), glm::vec3(0.0f, 0.0f, 1.0));
		staticShader.setMat4("model", model);
		aladerP.Draw(staticShader);

		//Pata Der
		model = glm::translate(tmp, glm::vec3(-1.5f, 3.0f, 0.0f));
		model = glm::rotate(model, glm::radians(pataDer), glm::vec3(1.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", model);
		pataderP.Draw(staticShader);

		//Pata Izq
		model = glm::translate(tmp, glm::vec3(1.5f, 3.0f, 0.0f));
		model = glm::rotate(model, glm::radians(pataIzq), glm::vec3(1.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", model);
		pataizqP.Draw(staticShader);

		//Ala Izq
		model = glm::translate(tmp, glm::vec3(2.3f, 8.0f, 0.0f));
		model = glm::rotate(model, glm::radians(alaIzq), glm::vec3(0.0f, 0.0f, 1.0f));
		staticShader.setMat4("model", model);
		alaizqP.Draw(staticShader);

		//Cabeza
		model = glm::translate(tmp, glm::vec3(0.0f, 9.5f, 0.0f));
		model = glm::rotate(model, glm::radians(cabeza), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		cabezaP.Draw(staticShader);




		//cabeza.Draw(staticShader);
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
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
		pataDer++;
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
		pataDer--;
	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
		giroMonito--;
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
		giroMonito++;
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
		pataIzq++;
	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
		pataIzq--;
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		alaDer++;
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
		alaDer--;
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
		alaIzq++;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		alaIzq--;
	if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
		cabeza++;
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
		cabeza--;

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		posY++;
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		posY--;
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
		for (int i = 0; i < FrameIndex; i++)
		{
			printf("KeyFrame[i].posX = %f;\n", KeyFrame[i].posX);
			printf("KeyFrame[i].posY = %f;\n", KeyFrame[i].posY);
			printf("KeyFrame[i].posZ = %f;\n", KeyFrame[i].posZ);
			printf("KeyFrame[i].alaDer = %f;\n", KeyFrame[i].alaDer);
			printf("KeyFrame[i].alaIzq= %f;\n", KeyFrame[i].alaIzq);
			printf("KeyFrame[i].pataDer =%f;\n", KeyFrame[i].pataDer);
			printf("KeyFrame[i].pataIzq =%f;\n", KeyFrame[i].pataIzq);
			printf("KeyFrame[i].cabeza =%f;\n", KeyFrame[i].cabeza);
			printf("KeyFrame[i].giroMonito =%f;\n", KeyFrame[i].giroMonito);
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