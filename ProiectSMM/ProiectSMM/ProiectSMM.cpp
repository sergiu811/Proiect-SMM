#include <stdlib.h> // necesare pentru citirea shaderStencilTesting-elor
#include <stdio.h>
#include <math.h> 
#include <GL/glew.h>

#define GLM_FORCE_CTOR_INIT 
#include <GLM.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include <glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "OBJ_Loader.h"
#pragma comment (lib, "glfw3dll.lib")
#pragma comment (lib, "glew32.lib")
#pragma comment (lib, "OpenGL32.lib")

#include "Camera.h"
#include "Shader.h"

// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;
objl::Loader Loader;


bool rot = false;

Camera* pCamera = nullptr;

unsigned int CreateTexture(const std::string& strTexturePath)
{
	unsigned int textureId = -1;

	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	unsigned char* data = stbi_load(strTexturePath.c_str(), &width, &height, &nrChannels, 0);
	if (data) {
		GLenum format;
		if (nrChannels == 1)
			format = GL_RED;
		else if (nrChannels == 3)
			format = GL_RGB;
		else if (nrChannels == 4)
			format = GL_RGBA;

		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else {
		std::cout << "Failed to load texture: " << strTexturePath << std::endl;
	}
	stbi_image_free(data);

	return textureId;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

//textures
void renderFloor(const Shader& shader);
void renderWallRoom(const Shader& shader);
void renderBackground1(const Shader& shader);
void renderBackground2(const Shader& shader);
void renderDino1(const Shader& shader);
void renderTree(const Shader& shader);
void renderHawk(const Shader& shader);
void renderStatue1(const Shader& shader);
void renderStatue2(const Shader& shader);
void renderGrass(const Shader& shader);
void renderStork(const Shader& shader);


//objects
void renderFloor();
void renderBackground();
void renderDino1();
void renderTree();
void renderHawk();
void renderStatue1();
void renderStatue2();
void renderGrass();
void renderStork();

//room
void renderWall1();
void renderWall2();
void renderWall3();
void renderWall4();
void renderWall5();

void renderWall6();
void renderWall7();
void renderWall8();
void renderWall9();
void renderWall10();


// timing
double deltaTime = 0.0f;    // time between current frame and last frame
double lastFrame = 0.0f;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_L && action == GLFW_PRESS)
	{
		rot = true;
	}
	if (key == GLFW_KEY_S && action == GLFW_PRESS)
	{
		rot = false;
	}

}

int main(int argc, char** argv)
{
	std::string strFullExeFileName = argv[0];
	std::string strExePath;
	const size_t last_slash_idx = strFullExeFileName.rfind('\\');
	if (std::string::npos != last_slash_idx) {
		strExePath = strFullExeFileName.substr(0, last_slash_idx);
	}

	// glfw: initialize and configure
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// glfw window creation
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Proiect SMM", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, key_callback);
	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glewInit();



	// Create camera
	pCamera = new Camera(SCR_WIDTH, SCR_HEIGHT, glm::vec3(0.0, 1.0, 3.0));

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile shaders
	// -------------------------
	Shader shadowMappingShader("..\\Shaders\\ShadowMapping.vs", "..\\Shaders\\ShadowMapping.fs");
	Shader shadowMappingDepthShader("..\\Shaders\\ShadowMappingDepth.vs", "..\\Shaders\\ShadowMappingDepth.fs");

	// load textures

	unsigned int floorTexture = CreateTexture(strExePath + "\\floor.jpg");
	unsigned int wallTexture = CreateTexture(strExePath + "\\wall.jpg");
	unsigned int dino1 = CreateTexture(strExePath + "\\tietu.jpg");
	unsigned int treeTexture = CreateTexture(strExePath + "\\tree.jpg");
	unsigned int hawkTexture = CreateTexture(strExePath + "\\10025_Hawk_v1_Diffuse.jpg");
	unsigned int statue1Texture = CreateTexture(strExePath + "\\DavidFixedDiff.jpg");
	unsigned int statue2Texture = CreateTexture(strExePath + "\\statue.jpg");
	unsigned int GrassTexture = CreateTexture(strExePath + "\\greenGrass.jpg");
	unsigned int storkTexture = CreateTexture(strExePath + "\\stork.jpg");
	// configure depth map FBO
	// -----------------------
	const unsigned int SHADOW_WIDTH = 4098, SHADOW_HEIGHT = 4098;
	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	// create depth texture
	unsigned int depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	// shader configuration
	// --------------------
	shadowMappingShader.Use();
	shadowMappingShader.SetInt("diffuseTexture", 0);
	shadowMappingShader.SetInt("shadowMap", 1);

	// lighting info
	// -------------
	glm::vec3 lightPos(-25.0f, 6.0f, -1.0f);

	glEnable(GL_CULL_FACE);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		if (rot)
		{
			lightPos.x = 3.0 * sin(currentFrame);
			lightPos.z = 3.0 * cos(currentFrame);

		}

		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



		// 1. render depth of scene to texture (from light's perspective)
		glm::mat4 lightProjection, lightView;
		glm::mat4 lightSpaceMatrix;
		float near_plane = 1.0f, far_plane = 7.5f;
		lightProjection = glm::ortho(10.0f, -10.0f, -10.0f, 10.0f, near_plane, far_plane);
		lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;

		// render scene from light's point of view
		shadowMappingDepthShader.Use();
		shadowMappingDepthShader.SetMat4("lightSpaceMatrix", lightSpaceMatrix);

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, floorTexture);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		renderFloor(shadowMappingDepthShader);
		glCullFace(GL_BACK);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, wallTexture);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		renderWallRoom(shadowMappingDepthShader);
		glCullFace(GL_BACK);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, dino1);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		renderDino1(shadowMappingDepthShader);
		glCullFace(GL_BACK);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, treeTexture);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		renderTree(shadowMappingDepthShader);
		glCullFace(GL_BACK);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, hawkTexture);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		renderHawk(shadowMappingDepthShader);
		glCullFace(GL_BACK);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, statue1Texture);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		renderStatue1(shadowMappingDepthShader);
		glCullFace(GL_BACK);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, statue2Texture);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		renderStatue2(shadowMappingDepthShader);
		glCullFace(GL_BACK);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, GrassTexture);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		renderGrass(shadowMappingDepthShader);
		glCullFace(GL_BACK);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, storkTexture);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		renderStork(shadowMappingDepthShader);
		glCullFace(GL_BACK);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		// reset viewport
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 2. render scene as normal using the generated depth/shadow map 
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shadowMappingShader.Use();
		glm::mat4 projection = pCamera->GetProjectionMatrix();
		glm::mat4 view = pCamera->GetViewMatrix();
		shadowMappingShader.SetMat4("projection", projection);
		shadowMappingShader.SetMat4("view", view);
		// set light uniforms
		shadowMappingShader.SetVec3("viewPos", pCamera->GetPosition());
		shadowMappingShader.SetVec3("lightPos", lightPos);
		shadowMappingShader.SetMat4("lightSpaceMatrix", lightSpaceMatrix);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, floorTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glDisable(GL_CULL_FACE);
		renderFloor(shadowMappingShader);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, wallTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glDisable(GL_CULL_FACE);
		renderWallRoom(shadowMappingShader);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, wallTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glDisable(GL_CULL_FACE);
		renderBackground1(shadowMappingShader);


		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, wallTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glDisable(GL_CULL_FACE);
		renderBackground2(shadowMappingShader);


		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, dino1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glDisable(GL_CULL_FACE);
		renderDino1(shadowMappingShader);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, treeTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glDisable(GL_CULL_FACE);
		renderTree(shadowMappingShader);


		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, hawkTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glDisable(GL_CULL_FACE);
		renderHawk(shadowMappingShader);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, statue1Texture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glDisable(GL_CULL_FACE);
		renderStatue1(shadowMappingShader);


		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, statue2Texture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glDisable(GL_CULL_FACE);
		renderStatue2(shadowMappingShader);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, GrassTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glDisable(GL_CULL_FACE);
		renderGrass(shadowMappingShader);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, storkTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glDisable(GL_CULL_FACE);
		renderStork(shadowMappingShader);

		//end



				// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	delete pCamera;

	glfwTerminate();
	return 0;
}

// renders the 3D scene
// --------------------
void renderFloor(const Shader& shader)
{
	// floor
	glm::mat4 model;
	shader.SetMat4("model", model);
	renderFloor();

	model = glm::translate(model, glm::vec3(-50.0f, 0.0f, 0.0f));
	shader.SetMat4("model", model);
	renderFloor();

}


void renderWallRoom(const Shader& shader)
{
	//first

	glm::mat4 model;
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(2.f));
	shader.SetMat4("model", model);
	renderWall1();

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(2.f));
	shader.SetMat4("model", model);
	renderWall2();

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(2.f));
	shader.SetMat4("model", model);
	renderWall3();

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(2.f));
	shader.SetMat4("model", model);
	renderWall4();

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(2.f));
	shader.SetMat4("model", model);
	renderWall5();

	//second

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-97.0f, -1.0f, 1.0f));
	model = glm::scale(model, glm::vec3(2.f));
	shader.SetMat4("model", model);
	renderWall6();

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-50.0f, -1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(2.f));
	shader.SetMat4("model", model);
	renderWall7();

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-50.0f, -1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(2.f));
	shader.SetMat4("model", model);
	renderWall8();

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-50.0f, -1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(2.f));
	shader.SetMat4("model", model);
	renderWall9();


}



void renderBackground1(const Shader& shader)
{
	//background2
	glm::mat4 model;
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-49.3f, 9.9f, -22.30f));
	model = glm::scale(model, glm::vec3(6.0f));
	shader.SetMat4("model", model);
	renderBackground();
}

void renderBackground2(const Shader& shader)
{
	//background2

	glm::mat4 model;

	//1

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-25.8f, -1.0f, -24.30f));
	model = glm::scale(model, glm::vec3(3.99f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	shader.SetMat4("model", model);
	renderBackground();

	//2

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-25.8f, -1.0f, 24.30f));
	model = glm::scale(model, glm::vec3(3.99f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	shader.SetMat4("model", model);
	renderBackground();

	//3

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-24.0f, -1.0f, -24.30f));
	model = glm::scale(model, glm::vec3(3.99f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	shader.SetMat4("model", model);
	renderBackground();

	//4

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-24.0f, -1.0f, 24.30f));
	model = glm::scale(model, glm::vec3(3.99f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	shader.SetMat4("model", model);
	renderBackground();
}

void renderDino1(const Shader& shader)
{
	//dino
	glm::mat4 model;

	static float Offset = 0.0f;
	const float Increment = 0.005f;
	Offset += Increment;

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-8.5f, -0.5f, 0.0f));
	model = glm::scale(model, glm::vec3(0.4f));
	model = glm::rotate(model, Offset, glm::vec3(0, 1, 0));
	shader.SetMat4("model", model);
	renderDino1();


}


void renderTree(const Shader& shader)
{
	//tree 1

	glm::mat4 model;
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-39.0f, -0.6f, 13.4f));
	model = glm::scale(model, glm::vec3(0.1f));
	shader.SetMat4("model", model);
	renderTree();

	// tree 2

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-58.8f, -0.6f, 13.4f));
	model = glm::scale(model, glm::vec3(0.1f));
	model = glm::rotate(model, glm::radians(20.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	shader.SetMat4("model", model);
	renderTree();

}


void renderHawk(const Shader& shader)
{

	//hawk

	glm::mat4 model;
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-56.2f, 9.8f, 9.f));
	model = glm::scale(model, glm::vec3(0.10f));
	model = glm::rotate(model, glm::radians(90.f), glm::vec3(-1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(90.f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::rotate(model, glm::radians(180.f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::rotate(model, glm::radians(160.f), glm::vec3(0.0f, 0.0f, -1.0f));
	shader.SetMat4("model", model);
	renderHawk();
}


void renderStatue1(const Shader& shader)
{

	//statue1

	glm::mat4 model;
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(12.7f, -0.4f, 13.9f));
	model = glm::scale(model, glm::vec3(0.015f));
	model = glm::rotate(model, glm::radians(90.f), glm::vec3(-1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(90.f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::rotate(model, glm::radians(100.f), glm::vec3(0.0f, 0.0f, 1.0f));
	shader.SetMat4("model", model);
	renderStatue1();
}

void renderStatue2(const Shader& shader)
{

	//statue2

	glm::mat4 model;
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(12.7f, -0.5f, -11.9f));
	model = glm::scale(model, glm::vec3(0.042f));
	model = glm::rotate(model, glm::radians(90.f), glm::vec3(-1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(90.f), glm::vec3(0.0f, 0.0f, -1.0f));
	model = glm::rotate(model, glm::radians(50.f), glm::vec3(0.0f, 0.0f, 1.0f));
	shader.SetMat4("model", model);
	renderStatue2();
}



void renderGrass(const Shader& shader)
{

	//Grass

	glm::mat4 model;
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-49.42f, -0.9f, 20.6f));
	model = glm::scale(model, glm::vec3(6.f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(-1.0f, 0.0f, 0.0f));

	shader.SetMat4("model", model);
	renderGrass();
}


void renderStork(const Shader& shader)
{

	//stork

	glm::mat4 model;
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-34.0f, -0.3f, 12.9f));
	model = glm::scale(model, glm::vec3(0.12f));
	model = glm::rotate(model, glm::radians(90.f), glm::vec3(-1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(90.f), glm::vec3(0.0f, 0.0f, -1.0f));
	model = glm::rotate(model, glm::radians(50.f), glm::vec3(0.0f, 0.0f, 1.0f));
	shader.SetMat4("model", model);
	renderStork();
}


unsigned int planeVAO = 0;
void renderFloor()
{
	unsigned int planeVBO;

	if (planeVAO == 0) {
		// set up vertex data (and buffer(s)) and configure vertex attributes
		float planeVertices[] = {
			// positions            // normals         // texcoords
			25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
			-25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
			-25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,

			25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
			-25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,  0.0f, 25.0f,
			25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 25.0f
		};
		// plane VAO
		glGenVertexArrays(1, &planeVAO);
		glGenBuffers(1, &planeVBO);
		glBindVertexArray(planeVAO);
		glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindVertexArray(0);
	}

	glBindVertexArray(planeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}



float vertices[82000];
unsigned int indices[72000];

GLuint floorVAO, floorVBO, floorEBO;

GLuint floorVAO1, floorVBO1, floorEBO1;
void renderWall1()
{
	// initialize (if necessary)
	if (floorVAO1 == 0)
	{

		std::vector<float> verticess;
		std::vector<float> indicess;


		Loader.LoadFile("..\\OBJ\\room.obj");
		for (int i = 0; i < Loader.LoadedMeshes.size(); i++)
		{
			objl::Mesh curMesh = Loader.LoadedMeshes[1];
			int size = curMesh.Vertices.size();

			for (int j = 0; j < curMesh.Vertices.size(); j++)
			{

				verticess.push_back((float)curMesh.Vertices[j].Position.X);
				verticess.push_back((float)curMesh.Vertices[j].Position.Y);
				verticess.push_back((float)curMesh.Vertices[j].Position.Z);
				verticess.push_back((float)curMesh.Vertices[j].Normal.X);
				verticess.push_back((float)curMesh.Vertices[j].Normal.Y);
				verticess.push_back((float)curMesh.Vertices[j].Normal.Z);
				verticess.push_back((float)curMesh.Vertices[j].TextureCoordinate.X);
				verticess.push_back((float)curMesh.Vertices[j].TextureCoordinate.Y);
			}
			for (int j = 0; j < verticess.size(); j++)
			{
				vertices[j] = verticess.at(j);
			}

			for (int j = 0; j < curMesh.Indices.size(); j++)
			{

				indicess.push_back((float)curMesh.Indices[j]);

			}
			for (int j = 0; j < curMesh.Indices.size(); j++)
			{
				indices[j] = indicess.at(j);
			}
		}
		glGenVertexArrays(1, &floorVAO1);
		glGenBuffers(1, &floorVBO1);
		glGenBuffers(1, &floorEBO1);
		// fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, floorVBO1);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floorEBO1);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_DYNAMIC_DRAW);
		// link vertex attributes
		glBindVertexArray(floorVAO1);
		glEnableVertexAttribArray(0);


		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	// render Cube
	glBindVertexArray(floorVAO1);
	glBindBuffer(GL_ARRAY_BUFFER, floorVBO1);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floorEBO1);
	int indexArraySize;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &indexArraySize);
	glDrawElements(GL_TRIANGLES, indexArraySize / sizeof(unsigned int), GL_UNSIGNED_INT, 0);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

}

void renderWall6()
{
	// initialize (if necessary)
	if (floorVAO1 == 0)
	{

		std::vector<float> verticess;
		std::vector<float> indicess;



		Loader.LoadFile("..\\OBJ\\room.obj");
		for (int i = 0; i < Loader.LoadedMeshes.size(); i++)
		{
			objl::Mesh curMesh = Loader.LoadedMeshes[1];
			int size = curMesh.Vertices.size();

			for (int j = 0; j < curMesh.Vertices.size(); j++)
			{

				verticess.push_back((float)curMesh.Vertices[j].Position.X);
				verticess.push_back((float)curMesh.Vertices[j].Position.Y);
				verticess.push_back((float)curMesh.Vertices[j].Position.Z);
				verticess.push_back((float)curMesh.Vertices[j].Normal.X);
				verticess.push_back((float)curMesh.Vertices[j].Normal.Y);
				verticess.push_back((float)curMesh.Vertices[j].Normal.Z);
				verticess.push_back((float)curMesh.Vertices[j].TextureCoordinate.X);
				verticess.push_back((float)curMesh.Vertices[j].TextureCoordinate.Y);
			}
			for (int j = 0; j < verticess.size(); j++)
			{
				vertices[j] = verticess.at(j);
			}

			for (int j = 0; j < curMesh.Indices.size(); j++)
			{

				indicess.push_back((float)curMesh.Indices[j]);

			}
			for (int j = 0; j < curMesh.Indices.size(); j++)
			{
				indices[j] = indicess.at(j);
			}
		}
		glGenVertexArrays(1, &floorVAO1);
		glGenBuffers(1, &floorVBO1);
		glGenBuffers(1, &floorEBO1);
		// fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, floorVBO1);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floorEBO1);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_DYNAMIC_DRAW);
		// link vertex attributes
		glBindVertexArray(floorVAO1);
		glEnableVertexAttribArray(0);


		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	// render Cube
	glBindVertexArray(floorVAO1);
	glBindBuffer(GL_ARRAY_BUFFER, floorVBO1);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floorEBO1);
	int indexArraySize;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &indexArraySize);
	glDrawElements(GL_TRIANGLES, indexArraySize / sizeof(unsigned int), GL_UNSIGNED_INT, 0);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

}

GLuint floorVAO2, floorVBO2, floorEBO2;
void renderWall2()
{
	// initialize (if necessary)
	if (floorVAO2 == 0)
	{

		std::vector<float> verticess;
		std::vector<float> indicess;



		Loader.LoadFile("..\\OBJ\\room.obj");
		for (int i = 0; i < Loader.LoadedMeshes.size(); i++)
		{
			objl::Mesh curMesh = Loader.LoadedMeshes[2];
			int size = curMesh.Vertices.size();

			for (int j = 0; j < curMesh.Vertices.size(); j++)
			{

				verticess.push_back((float)curMesh.Vertices[j].Position.X);
				verticess.push_back((float)curMesh.Vertices[j].Position.Y);
				verticess.push_back((float)curMesh.Vertices[j].Position.Z);
				verticess.push_back((float)curMesh.Vertices[j].Normal.X);
				verticess.push_back((float)curMesh.Vertices[j].Normal.Y);
				verticess.push_back((float)curMesh.Vertices[j].Normal.Z);
				verticess.push_back((float)curMesh.Vertices[j].TextureCoordinate.X);
				verticess.push_back((float)curMesh.Vertices[j].TextureCoordinate.Y);
			}
			for (int j = 0; j < verticess.size(); j++)
			{
				vertices[j] = verticess.at(j);
			}

			for (int j = 0; j < curMesh.Indices.size(); j++)
			{

				indicess.push_back((float)curMesh.Indices[j]);

			}
			for (int j = 0; j < curMesh.Indices.size(); j++)
			{
				indices[j] = indicess.at(j);
			}
		}
		glGenVertexArrays(1, &floorVAO2);
		glGenBuffers(1, &floorVBO2);
		glGenBuffers(1, &floorEBO2);
		// fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, floorVBO2);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floorEBO2);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_DYNAMIC_DRAW);
		// link vertex attributes
		glBindVertexArray(floorVAO2);
		glEnableVertexAttribArray(0);


		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	// render Cube
	glBindVertexArray(floorVAO2);
	glBindBuffer(GL_ARRAY_BUFFER, floorVBO2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floorEBO2);
	int indexArraySize;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &indexArraySize);
	glDrawElements(GL_TRIANGLES, indexArraySize / sizeof(unsigned int), GL_UNSIGNED_INT, 0);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

}

void renderWall7()
{
	// initialize (if necessary)
	if (floorVAO2 == 0)
	{

		std::vector<float> verticess;
		std::vector<float> indicess;


		Loader.LoadFile("..\\OBJ\\room.obj");
		for (int i = 0; i < Loader.LoadedMeshes.size(); i++)
		{
			objl::Mesh curMesh = Loader.LoadedMeshes[2];
			int size = curMesh.Vertices.size();

			for (int j = 0; j < curMesh.Vertices.size(); j++)
			{

				verticess.push_back((float)curMesh.Vertices[j].Position.X);
				verticess.push_back((float)curMesh.Vertices[j].Position.Y);
				verticess.push_back((float)curMesh.Vertices[j].Position.Z);
				verticess.push_back((float)curMesh.Vertices[j].Normal.X);
				verticess.push_back((float)curMesh.Vertices[j].Normal.Y);
				verticess.push_back((float)curMesh.Vertices[j].Normal.Z);
				verticess.push_back((float)curMesh.Vertices[j].TextureCoordinate.X);
				verticess.push_back((float)curMesh.Vertices[j].TextureCoordinate.Y);
			}
			for (int j = 0; j < verticess.size(); j++)
			{
				vertices[j] = verticess.at(j);
			}

			for (int j = 0; j < curMesh.Indices.size(); j++)
			{

				indicess.push_back((float)curMesh.Indices[j]);

			}
			for (int j = 0; j < curMesh.Indices.size(); j++)
			{
				indices[j] = indicess.at(j);
			}
		}
		glGenVertexArrays(1, &floorVAO2);
		glGenBuffers(1, &floorVBO2);
		glGenBuffers(1, &floorEBO2);
		// fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, floorVBO2);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floorEBO2);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_DYNAMIC_DRAW);
		// link vertex attributes
		glBindVertexArray(floorVAO2);
		glEnableVertexAttribArray(0);


		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	// render Cube
	glBindVertexArray(floorVAO2);
	glBindBuffer(GL_ARRAY_BUFFER, floorVBO2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floorEBO2);
	int indexArraySize;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &indexArraySize);
	glDrawElements(GL_TRIANGLES, indexArraySize / sizeof(unsigned int), GL_UNSIGNED_INT, 0);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

}

GLuint floorVAO3, floorVBO3, floorEBO3;
void renderWall3()
{
	// initialize (if necessary)
	if (floorVAO3 == 0)
	{

		std::vector<float> verticess;
		std::vector<float> indicess;



		Loader.LoadFile("..\\OBJ\\room.obj");
		objl::Mesh curMesh = Loader.LoadedMeshes[3];
		int size = curMesh.Vertices.size();

		for (int j = 0; j < curMesh.Vertices.size(); j++)
		{

			verticess.push_back((float)curMesh.Vertices[j].Position.X);
			verticess.push_back((float)curMesh.Vertices[j].Position.Y);
			verticess.push_back((float)curMesh.Vertices[j].Position.Z);
			verticess.push_back((float)curMesh.Vertices[j].Normal.X);
			verticess.push_back((float)curMesh.Vertices[j].Normal.Y);
			verticess.push_back((float)curMesh.Vertices[j].Normal.Z);
			verticess.push_back((float)curMesh.Vertices[j].TextureCoordinate.X);
			verticess.push_back((float)curMesh.Vertices[j].TextureCoordinate.Y);
		}
		for (int j = 0; j < verticess.size(); j++)
		{
			vertices[j] = verticess.at(j);
		}

		for (int j = 0; j < curMesh.Indices.size(); j++)
		{

			indicess.push_back((float)curMesh.Indices[j]);

		}
		for (int j = 0; j < curMesh.Indices.size(); j++)
		{
			indices[j] = indicess.at(j);
		}

		glGenVertexArrays(1, &floorVAO3);
		glGenBuffers(1, &floorVBO3);
		glGenBuffers(1, &floorEBO3);
		// fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, floorVBO3);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floorEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_DYNAMIC_DRAW);
		// link vertex attributes
		glBindVertexArray(floorVAO3);
		glEnableVertexAttribArray(0);


		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	// render Cube
	glBindVertexArray(floorVAO3);
	glBindBuffer(GL_ARRAY_BUFFER, floorVBO3);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floorEBO3);
	int indexArraySize;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &indexArraySize);
	glDrawElements(GL_TRIANGLES, indexArraySize / sizeof(unsigned int), GL_UNSIGNED_INT, 0);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

}

void renderWall8()
{
	// initialize (if necessary)
	if (floorVAO3 == 0)
	{

		std::vector<float> verticess;
		std::vector<float> indicess;


		Loader.LoadFile("..\\OBJ\\room.obj");
		objl::Mesh curMesh = Loader.LoadedMeshes[3];
		int size = curMesh.Vertices.size();

		for (int j = 0; j < curMesh.Vertices.size(); j++)
		{

			verticess.push_back((float)curMesh.Vertices[j].Position.X);
			verticess.push_back((float)curMesh.Vertices[j].Position.Y);
			verticess.push_back((float)curMesh.Vertices[j].Position.Z);
			verticess.push_back((float)curMesh.Vertices[j].Normal.X);
			verticess.push_back((float)curMesh.Vertices[j].Normal.Y);
			verticess.push_back((float)curMesh.Vertices[j].Normal.Z);
			verticess.push_back((float)curMesh.Vertices[j].TextureCoordinate.X);
			verticess.push_back((float)curMesh.Vertices[j].TextureCoordinate.Y);
		}
		for (int j = 0; j < verticess.size(); j++)
		{
			vertices[j] = verticess.at(j);
		}

		for (int j = 0; j < curMesh.Indices.size(); j++)
		{

			indicess.push_back((float)curMesh.Indices[j]);

		}
		for (int j = 0; j < curMesh.Indices.size(); j++)
		{
			indices[j] = indicess.at(j);
		}

		glGenVertexArrays(1, &floorVAO3);
		glGenBuffers(1, &floorVBO3);
		glGenBuffers(1, &floorEBO3);
		// fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, floorVBO3);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floorEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_DYNAMIC_DRAW);
		// link vertex attributes
		glBindVertexArray(floorVAO3);
		glEnableVertexAttribArray(0);


		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	// render Cube
	glBindVertexArray(floorVAO3);
	glBindBuffer(GL_ARRAY_BUFFER, floorVBO3);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floorEBO3);
	int indexArraySize;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &indexArraySize);
	glDrawElements(GL_TRIANGLES, indexArraySize / sizeof(unsigned int), GL_UNSIGNED_INT, 0);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

}

GLuint floorVAO4, floorVBO4, floorEBO4;
void renderWall4()
{
	// initialize (if necessary)
	if (floorVAO4 == 0)
	{

		std::vector<float> verticess;
		std::vector<float> indicess;


		Loader.LoadFile("..\\OBJ\\room.obj");
		objl::Mesh curMesh = Loader.LoadedMeshes[4];
		int size = curMesh.Vertices.size();

		for (int j = 0; j < curMesh.Vertices.size(); j++)
		{

			verticess.push_back((float)curMesh.Vertices[j].Position.X);
			verticess.push_back((float)curMesh.Vertices[j].Position.Y);
			verticess.push_back((float)curMesh.Vertices[j].Position.Z);
			verticess.push_back((float)curMesh.Vertices[j].Normal.X);
			verticess.push_back((float)curMesh.Vertices[j].Normal.Y);
			verticess.push_back((float)curMesh.Vertices[j].Normal.Z);
			verticess.push_back((float)curMesh.Vertices[j].TextureCoordinate.X);
			verticess.push_back((float)curMesh.Vertices[j].TextureCoordinate.Y);
		}
		for (int j = 0; j < verticess.size(); j++)
		{
			vertices[j] = verticess.at(j);
		}

		for (int j = 0; j < curMesh.Indices.size(); j++)
		{

			indicess.push_back((float)curMesh.Indices[j]);

		}
		for (int j = 0; j < curMesh.Indices.size(); j++)
		{
			indices[j] = indicess.at(j);
		}

		glGenVertexArrays(1, &floorVAO4);
		glGenBuffers(1, &floorVBO4);
		glGenBuffers(1, &floorEBO4);
		// fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, floorVBO4);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floorEBO4);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_DYNAMIC_DRAW);
		// link vertex attributes
		glBindVertexArray(floorVAO4);
		glEnableVertexAttribArray(0);


		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	// render Cube
	glBindVertexArray(floorVAO4);
	glBindBuffer(GL_ARRAY_BUFFER, floorVBO4);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floorEBO4);
	int indexArraySize;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &indexArraySize);
	glDrawElements(GL_TRIANGLES, indexArraySize / sizeof(unsigned int), GL_UNSIGNED_INT, 0);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

}

void renderWall9()
{
	// initialize (if necessary)
	if (floorVAO4 == 0)
	{

		std::vector<float> verticess;
		std::vector<float> indicess;


		Loader.LoadFile("..\\OBJ\\room.obj");
		objl::Mesh curMesh = Loader.LoadedMeshes[4];
		int size = curMesh.Vertices.size();

		for (int j = 0; j < curMesh.Vertices.size(); j++)
		{

			verticess.push_back((float)curMesh.Vertices[j].Position.X);
			verticess.push_back((float)curMesh.Vertices[j].Position.Y);
			verticess.push_back((float)curMesh.Vertices[j].Position.Z);
			verticess.push_back((float)curMesh.Vertices[j].Normal.X);
			verticess.push_back((float)curMesh.Vertices[j].Normal.Y);
			verticess.push_back((float)curMesh.Vertices[j].Normal.Z);
			verticess.push_back((float)curMesh.Vertices[j].TextureCoordinate.X);
			verticess.push_back((float)curMesh.Vertices[j].TextureCoordinate.Y);
		}
		for (int j = 0; j < verticess.size(); j++)
		{
			vertices[j] = verticess.at(j);
		}

		for (int j = 0; j < curMesh.Indices.size(); j++)
		{

			indicess.push_back((float)curMesh.Indices[j]);

		}
		for (int j = 0; j < curMesh.Indices.size(); j++)
		{
			indices[j] = indicess.at(j);
		}

		glGenVertexArrays(1, &floorVAO4);
		glGenBuffers(1, &floorVBO4);
		glGenBuffers(1, &floorEBO4);
		// fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, floorVBO4);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floorEBO4);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_DYNAMIC_DRAW);
		// link vertex attributes
		glBindVertexArray(floorVAO4);
		glEnableVertexAttribArray(0);


		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	// render Cube
	glBindVertexArray(floorVAO4);
	glBindBuffer(GL_ARRAY_BUFFER, floorVBO4);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floorEBO4);
	int indexArraySize;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &indexArraySize);
	glDrawElements(GL_TRIANGLES, indexArraySize / sizeof(unsigned int), GL_UNSIGNED_INT, 0);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

}

GLuint floorVAO5, floorVBO5, floorEBO5;
void renderWall5()
{
	// initialize (if necessary)
	if (floorVAO5 == 0)
	{

		std::vector<float> verticess;
		std::vector<float> indicess;
		Loader.LoadFile("..\\OBJ\\room.obj");
		objl::Mesh curMesh = Loader.LoadedMeshes[5];
		int size = curMesh.Vertices.size();

		for (int j = 0; j < curMesh.Vertices.size(); j++)
		{

			verticess.push_back((float)curMesh.Vertices[j].Position.X);
			verticess.push_back((float)curMesh.Vertices[j].Position.Y);
			verticess.push_back((float)curMesh.Vertices[j].Position.Z);
			verticess.push_back((float)curMesh.Vertices[j].Normal.X);
			verticess.push_back((float)curMesh.Vertices[j].Normal.Y);
			verticess.push_back((float)curMesh.Vertices[j].Normal.Z);
			verticess.push_back((float)curMesh.Vertices[j].TextureCoordinate.X);
			verticess.push_back((float)curMesh.Vertices[j].TextureCoordinate.Y);
		}
		for (int j = 0; j < verticess.size(); j++)
		{
			vertices[j] = verticess.at(j);
		}

		for (int j = 0; j < curMesh.Indices.size(); j++)
		{

			indicess.push_back((float)curMesh.Indices[j]);

		}
		for (int j = 0; j < curMesh.Indices.size(); j++)
		{
			indices[j] = indicess.at(j);
		}

		glGenVertexArrays(1, &floorVAO5);
		glGenBuffers(1, &floorVBO5);
		glGenBuffers(1, &floorEBO5);
		// fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, floorVBO5);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floorEBO5);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_DYNAMIC_DRAW);
		// link vertex attributes
		glBindVertexArray(floorVAO5);
		glEnableVertexAttribArray(0);


		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	// render Cube
	glBindVertexArray(floorVAO5);
	glBindBuffer(GL_ARRAY_BUFFER, floorVBO5);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floorEBO5);
	int indexArraySize;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &indexArraySize);
	glDrawElements(GL_TRIANGLES, indexArraySize / sizeof(unsigned int), GL_UNSIGNED_INT, 0);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

}

void renderWall10()
{
	// initialize (if necessary)
	if (floorVAO5 == 0)
	{

		std::vector<float> verticess;
		std::vector<float> indicess;
		Loader.LoadFile("..\\OBJ\\room.obj");
		objl::Mesh curMesh = Loader.LoadedMeshes[5];
		int size = curMesh.Vertices.size();

		for (int j = 0; j < curMesh.Vertices.size(); j++)
		{

			verticess.push_back((float)curMesh.Vertices[j].Position.X);
			verticess.push_back((float)curMesh.Vertices[j].Position.Y);
			verticess.push_back((float)curMesh.Vertices[j].Position.Z);
			verticess.push_back((float)curMesh.Vertices[j].Normal.X);
			verticess.push_back((float)curMesh.Vertices[j].Normal.Y);
			verticess.push_back((float)curMesh.Vertices[j].Normal.Z);
			verticess.push_back((float)curMesh.Vertices[j].TextureCoordinate.X);
			verticess.push_back((float)curMesh.Vertices[j].TextureCoordinate.Y);
		}
		for (int j = 0; j < verticess.size(); j++)
		{
			vertices[j] = verticess.at(j);
		}

		for (int j = 0; j < curMesh.Indices.size(); j++)
		{

			indicess.push_back((float)curMesh.Indices[j]);

		}
		for (int j = 0; j < curMesh.Indices.size(); j++)
		{
			indices[j] = indicess.at(j);
		}

		glGenVertexArrays(1, &floorVAO5);
		glGenBuffers(1, &floorVBO5);
		glGenBuffers(1, &floorEBO5);
		// fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, floorVBO5);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floorEBO5);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_DYNAMIC_DRAW);
		// link vertex attributes
		glBindVertexArray(floorVAO5);
		glEnableVertexAttribArray(0);


		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	// render Cube
	glBindVertexArray(floorVAO5);
	glBindBuffer(GL_ARRAY_BUFFER, floorVBO5);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floorEBO5);
	int indexArraySize;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &indexArraySize);
	glDrawElements(GL_TRIANGLES, indexArraySize / sizeof(unsigned int), GL_UNSIGNED_INT, 0);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);


}



float verticesbk[82000];
unsigned int indicesbk[72000];
GLuint backgroundVAO, backgroundVBO, backgroundEBO;

void renderBackground()
{
	// initialize (if necessary)
	if (backgroundVAO == 0)
	{

		std::vector<float> verticess;
		std::vector<float> indicess;



		Loader.LoadFile("..\\OBJ\\background.obj");
		objl::Mesh curMesh = Loader.LoadedMeshes[0];
		int size = curMesh.Vertices.size();

		for (int j = 0; j < curMesh.Vertices.size(); j++)
		{

			verticess.push_back((float)curMesh.Vertices[j].Position.X);
			verticess.push_back((float)curMesh.Vertices[j].Position.Y);
			verticess.push_back((float)curMesh.Vertices[j].Position.Z);
			verticess.push_back((float)curMesh.Vertices[j].Normal.X);
			verticess.push_back((float)curMesh.Vertices[j].Normal.Y);
			verticess.push_back((float)curMesh.Vertices[j].Normal.Z);
			verticess.push_back((float)curMesh.Vertices[j].TextureCoordinate.X);
			verticess.push_back((float)curMesh.Vertices[j].TextureCoordinate.Y);
		}
		for (int j = 0; j < verticess.size(); j++)
		{
			verticesbk[j] = verticess.at(j);
		}

		for (int j = 0; j < curMesh.Indices.size(); j++)
		{

			indicess.push_back((float)curMesh.Indices[j]);

		}
		for (int j = 0; j < curMesh.Indices.size(); j++)
		{
			indicesbk[j] = indicess.at(j);
		}

		glGenVertexArrays(1, &backgroundVAO);
		glGenBuffers(1, &backgroundVBO);
		glGenBuffers(1, &backgroundEBO);
		// fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, backgroundVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(verticesbk), verticesbk, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, backgroundEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesbk), &indicesbk, GL_DYNAMIC_DRAW);
		// link vertex attributes
		glBindVertexArray(backgroundVAO);
		glEnableVertexAttribArray(0);


		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	// render Cube
	glBindVertexArray(backgroundVAO);
	glBindBuffer(GL_ARRAY_BUFFER, backgroundVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, backgroundEBO);
	int indexArraySize;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &indexArraySize);
	glDrawElements(GL_TRIANGLES, indexArraySize / sizeof(unsigned int), GL_UNSIGNED_INT, 0);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

unsigned int indicesDT3[720000];
objl::Vertex verDT3[900000];

GLuint dino1VAO, dino1VBO, dino1EBO;
void renderDino1()
{
	// initialize (if necessary)
	if (dino1VAO == 0)
	{

		std::vector<float> verticess;
		std::vector<float> indicess;



		Loader.LoadFile("..\\OBJ\\dinosaur.obj");
		objl::Mesh curMesh = Loader.LoadedMeshes[0];
		int size = curMesh.Vertices.size();
		objl::Vertex v;
		for (int j = 0; j < curMesh.Vertices.size(); j++)
		{
			v.Position.X = (float)curMesh.Vertices[j].Position.X;
			v.Position.Y = (float)curMesh.Vertices[j].Position.Y;
			v.Position.Z = (float)curMesh.Vertices[j].Position.Z;
			v.Normal.X = (float)curMesh.Vertices[j].Normal.X;
			v.Normal.Y = (float)curMesh.Vertices[j].Normal.Y;
			v.Normal.Z = (float)curMesh.Vertices[j].Normal.Z;
			v.TextureCoordinate.X = (float)curMesh.Vertices[j].TextureCoordinate.X;
			v.TextureCoordinate.Y = (float)curMesh.Vertices[j].TextureCoordinate.Y;


			verDT3[j] = v;
		}
		for (int j = 0; j < verticess.size(); j++)
		{
			vertices[j] = verticess.at(j);
		}

		for (int j = 0; j < curMesh.Indices.size(); j++)
		{

			indicess.push_back((float)curMesh.Indices[j]);

		}
		for (int j = 0; j < curMesh.Indices.size(); j++)
		{
			indicesDT3[j] = indicess.at(j);
		}

		glGenVertexArrays(1, &dino1VAO);
		glGenBuffers(1, &dino1VBO);
		glGenBuffers(1, &dino1EBO);
		// fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, dino1VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(verDT3), verDT3, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dino1EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesDT3), &indicesDT3, GL_DYNAMIC_DRAW);
		// link vertex attributes
		glBindVertexArray(dino1VAO);
		glEnableVertexAttribArray(0);


		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	// render Cube
	glBindVertexArray(dino1VAO);
	glBindBuffer(GL_ARRAY_BUFFER, dino1VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dino1EBO);
	int indexArraySize;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &indexArraySize);
	glDrawElements(GL_TRIANGLES, indexArraySize / sizeof(unsigned int), GL_UNSIGNED_INT, 0);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}



unsigned int indicesT[72000];
objl::Vertex verT[82000];

GLuint treeVAO, treeVBO, treeEBO;

void renderTree()
{
	// initialize (if necessary)
	if (treeVAO == 0)
	{

		std::vector<float> verticess;
		std::vector<float> indicess;



		Loader.LoadFile("..\\OBJ\\acacia.obj");
		objl::Mesh curMesh = Loader.LoadedMeshes[0];
		int size = curMesh.Vertices.size();
		objl::Vertex v;
		for (int j = 0; j < curMesh.Vertices.size(); j++)
		{
			v.Position.X = (float)curMesh.Vertices[j].Position.X;
			v.Position.Y = (float)curMesh.Vertices[j].Position.Y;
			v.Position.Z = (float)curMesh.Vertices[j].Position.Z;
			v.Normal.X = (float)curMesh.Vertices[j].Normal.X;
			v.Normal.Y = (float)curMesh.Vertices[j].Normal.Y;
			v.Normal.Z = (float)curMesh.Vertices[j].Normal.Z;
			v.TextureCoordinate.X = (float)curMesh.Vertices[j].TextureCoordinate.X;
			v.TextureCoordinate.Y = (float)curMesh.Vertices[j].TextureCoordinate.Y;


			verT[j] = v;
		}
		for (int j = 0; j < verticess.size(); j++)
		{
			vertices[j] = verticess.at(j);
		}

		for (int j = 0; j < curMesh.Indices.size(); j++)
		{

			indicess.push_back((float)curMesh.Indices[j]);

		}
		for (int j = 0; j < curMesh.Indices.size(); j++)
		{
			indicesT[j] = indicess.at(j);
		}

		glGenVertexArrays(1, &treeVAO);
		glGenBuffers(1, &treeVBO);
		glGenBuffers(1, &treeEBO);
		// fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, treeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(verT), verT, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, treeEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesT), &indicesT, GL_DYNAMIC_DRAW);
		// link vertex attributes
		glBindVertexArray(treeVAO);
		glEnableVertexAttribArray(0);


		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	// render Cube
	glBindVertexArray(treeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, treeVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, treeEBO);
	int indexArraySize;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &indexArraySize);
	glDrawElements(GL_TRIANGLES, indexArraySize / sizeof(unsigned int), GL_UNSIGNED_INT, 0);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}




unsigned int indiceshawk[720000];
objl::Vertex verhawk[820000];
GLuint  hawkVAO, hawkVBO, hawkEBO;

void renderHawk()
{
	// initialize (if necessary)
	if (hawkVAO == 0)
	{

		std::vector<float> verticess;
		std::vector<float> indicess;



		Loader.LoadFile("..\\OBJ\\10025_Hawk_v1_iterations-2.obj");
		objl::Mesh curMesh = Loader.LoadedMeshes[0];
		int size = curMesh.Vertices.size();
		objl::Vertex v;
		for (int j = 0; j < curMesh.Vertices.size(); j++)
		{
			v.Position.X = (float)curMesh.Vertices[j].Position.X;
			v.Position.Y = (float)curMesh.Vertices[j].Position.Y;
			v.Position.Z = (float)curMesh.Vertices[j].Position.Z;
			v.Normal.X = (float)curMesh.Vertices[j].Normal.X;
			v.Normal.Y = (float)curMesh.Vertices[j].Normal.Y;
			v.Normal.Z = (float)curMesh.Vertices[j].Normal.Z;
			v.TextureCoordinate.X = (float)curMesh.Vertices[j].TextureCoordinate.X;
			v.TextureCoordinate.Y = (float)curMesh.Vertices[j].TextureCoordinate.Y;


			verhawk[j] = v;
		}
		for (int j = 0; j < verticess.size(); j++)
		{
			vertices[j] = verticess.at(j);
		}

		for (int j = 0; j < curMesh.Indices.size(); j++)
		{

			indicess.push_back((float)curMesh.Indices[j]);

		}
		for (int j = 0; j < curMesh.Indices.size(); j++)
		{
			indiceshawk[j] = indicess.at(j);
		}

		glGenVertexArrays(1, &hawkVAO);
		glGenBuffers(1, &hawkVBO);
		glGenBuffers(1, &hawkEBO);
		// fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, hawkVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(verhawk), verhawk, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, hawkEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indiceshawk), &indiceshawk, GL_DYNAMIC_DRAW);
		// link vertex attributes
		glBindVertexArray(hawkVAO);
		glEnableVertexAttribArray(0);


		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	// render Cube
	glBindVertexArray(hawkVAO);
	glBindBuffer(GL_ARRAY_BUFFER, hawkVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, hawkEBO);
	int indexArraySize;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &indexArraySize);
	glDrawElements(GL_TRIANGLES, indexArraySize / sizeof(unsigned int), GL_UNSIGNED_INT, 0);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}



unsigned int indicesstatue1[720000];
objl::Vertex verstatue1[820000];
GLuint statue1VAO, statue1VBO, statue1EBO;

void renderStatue1()
{
	// initialize (if necessary)
	if (statue1VAO == 0)
	{

		std::vector<float> verticess;
		std::vector<float> indicess;



		Loader.LoadFile("..\\OBJ\\12330_Statue_v1_L2.obj");
		objl::Mesh curMesh = Loader.LoadedMeshes[0];
		int size = curMesh.Vertices.size();
		objl::Vertex v;
		for (int j = 0; j < curMesh.Vertices.size(); j++)
		{
			v.Position.X = (float)curMesh.Vertices[j].Position.X;
			v.Position.Y = (float)curMesh.Vertices[j].Position.Y;
			v.Position.Z = (float)curMesh.Vertices[j].Position.Z;
			v.Normal.X = (float)curMesh.Vertices[j].Normal.X;
			v.Normal.Y = (float)curMesh.Vertices[j].Normal.Y;
			v.Normal.Z = (float)curMesh.Vertices[j].Normal.Z;
			v.TextureCoordinate.X = (float)curMesh.Vertices[j].TextureCoordinate.X;
			v.TextureCoordinate.Y = (float)curMesh.Vertices[j].TextureCoordinate.Y;


			verstatue1[j] = v;
		}
		for (int j = 0; j < verticess.size(); j++)
		{
			vertices[j] = verticess.at(j);
		}

		for (int j = 0; j < curMesh.Indices.size(); j++)
		{

			indicess.push_back((float)curMesh.Indices[j]);

		}
		for (int j = 0; j < curMesh.Indices.size(); j++)
		{
			indicesstatue1[j] = indicess.at(j);
		}

		glGenVertexArrays(1, &statue1VAO);
		glGenBuffers(1, &statue1VBO);
		glGenBuffers(1, &statue1EBO);
		// fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, statue1VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(verstatue1), verstatue1, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, statue1EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesstatue1), &indicesstatue1, GL_DYNAMIC_DRAW);
		// link vertex attributes
		glBindVertexArray(statue1VAO);
		glEnableVertexAttribArray(0);


		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	// render Cube
	glBindVertexArray(statue1VAO);
	glBindBuffer(GL_ARRAY_BUFFER, statue1VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, statue1EBO);
	int indexArraySize;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &indexArraySize);
	glDrawElements(GL_TRIANGLES, indexArraySize / sizeof(unsigned int), GL_UNSIGNED_INT, 0);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}




unsigned int indicesstatue2[720000];
objl::Vertex verstatue2[820000];
GLuint statue2VAO, statue2VBO, statue2EBO;

void renderStatue2()
{
	// initialize (if necessary)
	if (statue2VAO == 0)
	{

		std::vector<float> verticess;
		std::vector<float> indicess;



		Loader.LoadFile("..\\OBJ\\12328_Statue_v1_L2.obj");
		objl::Mesh curMesh = Loader.LoadedMeshes[0];
		int size = curMesh.Vertices.size();
		objl::Vertex v;
		for (int j = 0; j < curMesh.Vertices.size(); j++)
		{
			v.Position.X = (float)curMesh.Vertices[j].Position.X;
			v.Position.Y = (float)curMesh.Vertices[j].Position.Y;
			v.Position.Z = (float)curMesh.Vertices[j].Position.Z;
			v.Normal.X = (float)curMesh.Vertices[j].Normal.X;
			v.Normal.Y = (float)curMesh.Vertices[j].Normal.Y;
			v.Normal.Z = (float)curMesh.Vertices[j].Normal.Z;
			v.TextureCoordinate.X = (float)curMesh.Vertices[j].TextureCoordinate.X;
			v.TextureCoordinate.Y = (float)curMesh.Vertices[j].TextureCoordinate.Y;


			verstatue2[j] = v;
		}
		for (int j = 0; j < verticess.size(); j++)
		{
			vertices[j] = verticess.at(j);
		}

		for (int j = 0; j < curMesh.Indices.size(); j++)
		{

			indicess.push_back((float)curMesh.Indices[j]);

		}
		for (int j = 0; j < curMesh.Indices.size(); j++)
		{
			indicesstatue2[j] = indicess.at(j);
		}

		glGenVertexArrays(1, &statue2VAO);
		glGenBuffers(1, &statue2VBO);
		glGenBuffers(1, &statue2EBO);
		// fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, statue2VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(verstatue2), verstatue2, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, statue2EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesstatue2), &indicesstatue2, GL_DYNAMIC_DRAW);
		// link vertex attributes
		glBindVertexArray(statue2VAO);
		glEnableVertexAttribArray(0);


		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	// render Cube
	glBindVertexArray(statue2VAO);
	glBindBuffer(GL_ARRAY_BUFFER, statue2VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, statue2EBO);
	int indexArraySize;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &indexArraySize);
	glDrawElements(GL_TRIANGLES, indexArraySize / sizeof(unsigned int), GL_UNSIGNED_INT, 0);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}



unsigned int indicesGr[72000];
objl::Vertex verGr[82000];
GLuint GrassVAO, GrassVBO, GrassEBO;

void renderGrass()
{
	// initialize (if necessary)
	if (GrassVAO == 0)
	{

		std::vector<float> verticess;
		std::vector<float> indicess;



		Loader.LoadFile("..\\OBJ\\background.obj");
		objl::Mesh curMesh = Loader.LoadedMeshes[0];
		int size = curMesh.Vertices.size();
		objl::Vertex v;
		for (int j = 0; j < curMesh.Vertices.size(); j++)
		{
			v.Position.X = (float)curMesh.Vertices[j].Position.X;
			v.Position.Y = (float)curMesh.Vertices[j].Position.Y;
			v.Position.Z = (float)curMesh.Vertices[j].Position.Z;
			v.Normal.X = (float)curMesh.Vertices[j].Normal.X;
			v.Normal.Y = (float)curMesh.Vertices[j].Normal.Y;
			v.Normal.Z = (float)curMesh.Vertices[j].Normal.Z;
			v.TextureCoordinate.X = (float)curMesh.Vertices[j].TextureCoordinate.X;
			v.TextureCoordinate.Y = (float)curMesh.Vertices[j].TextureCoordinate.Y;


			verGr[j] = v;
		}
		for (int j = 0; j < verticess.size(); j++)
		{
			vertices[j] = verticess.at(j);
		}

		for (int j = 0; j < curMesh.Indices.size(); j++)
		{

			indicess.push_back((float)curMesh.Indices[j]);

		}
		for (int j = 0; j < curMesh.Indices.size(); j++)
		{
			indicesGr[j] = indicess.at(j);
		}

		glGenVertexArrays(1, &GrassVAO);
		glGenBuffers(1, &GrassVBO);
		glGenBuffers(1, &GrassEBO);
		// fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, GrassVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(verGr), verGr, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GrassEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesGr), &indicesGr, GL_DYNAMIC_DRAW);
		// link vertex attributes
		glBindVertexArray(GrassVAO);
		glEnableVertexAttribArray(0);


		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	// render Cube
	glBindVertexArray(GrassVAO);
	glBindBuffer(GL_ARRAY_BUFFER, GrassVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GrassEBO);
	int indexArraySize;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &indexArraySize);
	glDrawElements(GL_TRIANGLES, indexArraySize / sizeof(unsigned int), GL_UNSIGNED_INT, 0);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}


unsigned int indicesStork[720000];
objl::Vertex verStork[820000];
GLuint storkVAO, storkVBO, storkEBO;
void renderStork()
{
	// initialize (if necessary)
	if (storkVAO == 0)
	{

		std::vector<float> verticess;
		std::vector<float> indicess;



		Loader.LoadFile("..\\OBJ\\stork.obj");
		objl::Mesh curMesh = Loader.LoadedMeshes[0];
		int size = curMesh.Vertices.size();
		objl::Vertex v;
		for (int j = 0; j < curMesh.Vertices.size(); j++)
		{
			v.Position.X = (float)curMesh.Vertices[j].Position.X;
			v.Position.Y = (float)curMesh.Vertices[j].Position.Y;
			v.Position.Z = (float)curMesh.Vertices[j].Position.Z;
			v.Normal.X = (float)curMesh.Vertices[j].Normal.X;
			v.Normal.Y = (float)curMesh.Vertices[j].Normal.Y;
			v.Normal.Z = (float)curMesh.Vertices[j].Normal.Z;
			v.TextureCoordinate.X = (float)curMesh.Vertices[j].TextureCoordinate.X;
			v.TextureCoordinate.Y = (float)curMesh.Vertices[j].TextureCoordinate.Y;


			verStork[j] = v;
		}
		for (int j = 0; j < verticess.size(); j++)
		{
			vertices[j] = verticess.at(j);
		}

		for (int j = 0; j < curMesh.Indices.size(); j++)
		{

			indicess.push_back((float)curMesh.Indices[j]);

		}
		for (int j = 0; j < curMesh.Indices.size(); j++)
		{
			indicesStork[j] = indicess.at(j);
		}

		glGenVertexArrays(1, &storkVAO);
		glGenBuffers(1, &storkVBO);
		glGenBuffers(1, &storkEBO);
		// fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, storkVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(verStork), verStork, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, storkEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesStork), &indicesStork, GL_DYNAMIC_DRAW);
		// link vertex attributes
		glBindVertexArray(storkVAO);
		glEnableVertexAttribArray(0);


		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	// render Cube
	glBindVertexArray(storkVAO);
	glBindBuffer(GL_ARRAY_BUFFER, storkVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, storkEBO);
	int indexArraySize;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &indexArraySize);
	glDrawElements(GL_TRIANGLES, indexArraySize / sizeof(unsigned int), GL_UNSIGNED_INT, 0);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}



// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		pCamera->ProcessKeyboard(FORWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		pCamera->ProcessKeyboard(BACKWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		pCamera->ProcessKeyboard(LEFT, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		pCamera->ProcessKeyboard(RIGHT, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
		pCamera->ProcessKeyboard(UP, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
		pCamera->ProcessKeyboard(DOWN, (float)deltaTime);

	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
		int width, height;
		glfwGetWindowSize(window, &width, &height);
		pCamera->Reset(width, height);

	}
}










// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	pCamera->Reshape(width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	pCamera->MouseControl((float)xpos, (float)ypos);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yOffset)
{
	pCamera->ProcessMouseScroll((float)yOffset);
}