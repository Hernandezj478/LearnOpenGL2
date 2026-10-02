#include "NormalMapping.h"

#include "../ColorPalette.h"
#include "../Renderer.h"
#include "../Plane.h"
#include "../Texture.h"
#include "../FrameBuffer.h"
#include "../Cube.h"

#define SHADOW_WIDTH  1024
#define SHADOW_HEIGHT 1024

NormalMapping::NormalMapping(int width, int height)
{
	camera.SetScreenSize(width, height);
	camera.SetCursorPos();	// Must only be called after setting screen size
	camera.SetAspectRatio();
}

void NormalMapping::Run(GLFWwindow* window)
{
	glEnable(GL_DEPTH_TEST);
	Texture wallTexture("res/textures/brickwall.jpg", true);
	Texture wallNormal("res/textures/brickwall_normal.jpg", true);

	//Plane plane;
	PlaneTBN plane;
	Cube cube(false, false, true);

	Shader shader("res/shaders/NormalMapping.shader");
	Shader lightbox("res/shaders/LightSource.shader");
	Shader normalVis("res/shaders/TBNVisualization.shader");

	shader.Bind();
	shader.SetUniform1i("diffuseMap", 0);
	shader.SetUniform1i("normalMap", 1);
	
	lightbox.Bind();
	lightbox.SetUniform1f("brightness", 0.8f);
	lightbox.SetUniformVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));

	glm::vec3 lightPos(0.5f, 1.0f, 0.3f);
	while (!glfwWindowShouldClose(window))
	{

		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		Renderer renderer;
		ProcessMovement(window);
		renderer.Clear(DARK_GREY, COLOR_DEPTH);

		// Render Here
		glm::mat4 projection = glm::perspective(glm::radians(camera.GetFOV()), camera.GetAspectRatio(), 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 model = glm::mat4(1.0);

		//lightPos = glm::vec3(cos(glfwGetTime()) * 1.0, sin(glfwGetTime()) * 1.0, 1.5);

		shader.Bind();
		shader.SetUniformMat4f("projection", projection);
		shader.SetUniformMat4f("view", view);
		// set lighting uniforms
		shader.SetUniformVec3("lightPos", lightPos);
		shader.SetUniformVec3("viewPos", camera.GetPosition());
		wallTexture.Bind(0);
		wallNormal.Bind(1);
		model = glm::mat4(1.0f);
		//model = glm::rotate(model, glm::radians(-180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians((float)glfwGetTime() * -10.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
		shader.SetUniformMat4f("model", model);
		plane.Draw(shader, renderer);
		shader.Unbind();

		normalVis.Bind();
		normalVis.SetUniformMat4f("projection", projection);
		normalVis.SetUniformMat4f("view", view);
		normalVis.SetUniformMat4f("model", model);
		plane.Draw(normalVis, renderer);

		lightbox.Bind();
		lightbox.SetUniformMat4f("projection", projection);
		lightbox.SetUniformMat4f("view", view);
		model = glm::mat4(1.0);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.1));
		lightbox.SetUniformMat4f("model", model);
		cube.Draw(lightbox, renderer);
		lightbox.Unbind();


		// Check and call events and swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
}
