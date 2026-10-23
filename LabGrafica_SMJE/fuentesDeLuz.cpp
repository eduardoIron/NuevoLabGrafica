#include <iostream>
#include <cmath>

// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// Other Libs
#include "stb_image.h"

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Load Models
#include "SOIL2/SOIL2.h"

// Other includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"

// Function prototypes
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow* window, double xPos, double yPos);
void DoMovement();

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Camera
Camera  camera(glm::vec3(0.0f, 0.0f, 3.0f));
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool keys[1024];
bool firstMouse = true;

// Light attributes
glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
bool active;
float dayFactor = 0.0f;

// Positions of the point lights
glm::vec3 pointLightPositions[] = {
	glm::vec3(2.5f, 0.2f,  2.5f),
	glm::vec3(2.5f, 0.2f, -2.5f),
	glm::vec3(-2.5f, 0.2f,  2.5f),
	glm::vec3(-2.5f, 0.2f, -2.5f)
};

float vertices[] = {
	 -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	   -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	   -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

	   -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	   -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	   -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

	   -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	   -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	   -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	   -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	   -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	   -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

	   -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	   -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	   -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

	   -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	   -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	   -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
};

glm::vec3 Light1 = glm::vec3(0);

// Deltatime
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;   // Time of last frame

int main()
{
	// Init GLFW
	glfwInit();

	// Create a GLFWwindow object
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Fuentes de luz", nullptr, nullptr);
	if (nullptr == window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);
	glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

	// Set the required callback functions
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, MouseCallback);

	// Initialize GLEW
	glewExperimental = GL_TRUE;
	if (GLEW_OK != glewInit())
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}

	// Define the viewport dimensions
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	Shader lightingShader("Shader/lighting.vs", "Shader/lighting.frag");
	Shader lampShader("Shader/lamp.vs", "Shader/lamp.frag");

	Model Ball((char*)"Models/ball.obj");
	Model Piso((char*)"Models/piso.obj");

	// Setup VAO and VBO for the light cubes
	GLuint VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// Normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	lightingShader.Use();
	glUniform1i(glGetUniformLocation(lightingShader.Program, "Material.difuse"), 0);
	glUniform1i(glGetUniformLocation(lightingShader.Program, "Material.specular"), 1);

	glm::mat4 projection = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 100.0f);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glfwPollEvents();
		DoMovement();

		glClearColor(dayFactor * 0.5f, dayFactor * 0.7f, dayFactor * 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		lightingShader.Use();
		glUniform1i(glGetUniformLocation(lightingShader.Program, "diffuse"), 0);

		GLint viewPosLoc = glGetUniformLocation(lightingShader.Program, "viewPos");
		glUniform3f(viewPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);

		//
		// Directional light
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.ambient"), dayFactor * 0.5f, dayFactor * 0.5f, dayFactor * 0.5f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.diffuse"), dayFactor * 0.8f, dayFactor * 0.8f, dayFactor * 0.8f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.specular"), dayFactor * 0.8f, dayFactor * 0.8f, dayFactor * 0.8f);

		// Point light 1 rojo
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].ambient"), 0.05f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].diffuse"), 1.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].specular"), 1.0f, 0.2f, 0.2f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].linear"), 0.045f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].quadratic"), 0.075f);

		// Point light 2 verde
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].ambient"), 0.0f, 0.05f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].diffuse"), 0.0f, 1.0f, 0.2f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].specular"), 0.5f, 1.0f, 0.5f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].linear"), 0.045f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].quadratic"), 0.075f);

		// Point light 3 amarillo
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].ambient"), 0.05f, 0.05f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].diffuse"), 1.0f, 0.8f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].specular"), 1.0f, 1.0f, 0.5f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].linear"), 0.045f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].quadratic"), 0.075f);

		// Point light 4 azul
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].position"), pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].ambient"), 0.0f, 0.0f, 0.05f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].diffuse"), 0.0f, 0.0f, 1.0f); //aqui
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].specular"), 0.5f, 0.5f, 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].linear"), 0.045f); //aqui
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].quadratic"), 0.075f); //aqui

		// SpotLight
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.position"), camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.direction"), camera.GetFront().x, camera.GetFront().y, camera.GetFront().z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.ambient"), 0.1f, 0.1f, 0.8f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.diffuse"), 0.1f, 0.1f, 0.8f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.specular"), 0.0f, 0.0f, 0.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.linear"), 0.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.quadratic"), 0.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.cutOff"), glm::cos(glm::radians(12.0f)));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.outerCutOff"), glm::cos(glm::radians(18.0f)));

		// Set material properties
		glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 16.0f);

		// Create camera transformations
		glm::mat4 view = camera.GetViewMatrix();
		GLint modelLoc = glGetUniformLocation(lightingShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(lightingShader.Program, "view");
		GLint projLoc = glGetUniformLocation(lightingShader.Program, "projection");

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		// Draw Piso
		glm::mat4 model = glm::mat4(1);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Piso.Draw(lightingShader);

		// Draw Ball
		model = glm::mat4(1);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 1);
		Ball.Draw(lightingShader);
		glDisable(GL_BLEND);
		glBindVertexArray(0);

		// Draw Light Cubes
		lampShader.Use();
		modelLoc = glGetUniformLocation(lampShader.Program, "model");
		viewLoc = glGetUniformLocation(lampShader.Program, "view");
		projLoc = glGetUniformLocation(lampShader.Program, "projection");

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		for (GLuint i = 0; i < 4; i++)
		{
			model = glm::mat4(1);
			model = glm::translate(model, pointLightPositions[i]);
			model = glm::scale(model, glm::vec3(0.2f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}

void DoMovement()
{
	if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT])
		camera.ProcessKeyboard(RIGHT, deltaTime);

	//TECLA CAMBIO
	if (keys[GLFW_KEY_C])
	{
		dayFactor += deltaTime * 0.5f;
		if (dayFactor > 1.0f) dayFactor = 1.0f;
	}

	if (keys[GLFW_KEY_T]) pointLightPositions[0].x += 0.01f;
	if (keys[GLFW_KEY_G]) pointLightPositions[0].x -= 0.01f;
	if (keys[GLFW_KEY_Y]) pointLightPositions[0].y += 0.01f;
	if (keys[GLFW_KEY_H]) pointLightPositions[0].y -= 0.01f;
	if (keys[GLFW_KEY_U]) pointLightPositions[0].z -= 0.1f;
	if (keys[GLFW_KEY_J]) pointLightPositions[0].z += 0.01f;
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS) keys[key] = true;
		else if (action == GLFW_RELEASE) keys[key] = false;
	}

	if (keys[GLFW_KEY_SPACE])
	{
		active = !active;
		if (active) Light1 = glm::vec3(1.0f, 1.0f, 0.0f);
		else Light1 = glm::vec3(0);
	}
}

void MouseCallback(GLFWwindow* window, double xPos, double yPos)
{
	if (firstMouse)
	{
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	GLfloat xOffset = xPos - lastX;
	GLfloat yOffset = lastY - yPos;
	lastX = xPos;
	lastY = yPos;

	camera.ProcessMouseMovement(xOffset, yOffset);
}