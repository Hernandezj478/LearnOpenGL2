#include "ParallaxMapping.h"

#include "../ColorPalette.h"
#include "../Renderer.h"
#include "../Plane.h"
#include "../Texture.h"

#define SHADOW_WIDTH  1024
#define SHADOW_HEIGHT 1024

ParallaxMapping::ParallaxMapping(int width, int height)
{
	camera.SetScreenSize(width, height);
	camera.SetCursorPos();	// Must only be called after setting screen size
	camera.SetAspectRatio();
}

void ParallaxMapping::Run(GLFWwindow* window)
{
	glEnable(GL_DEPTH_TEST);
	PlaneTBN plane;
	PlaneTBN toyPlane;
	std::vector<glm::vec3> planePos =
	{
		glm::vec3(-1.5f, 0.0f, 0.0f),
		glm::vec3(1.5f, 0.0f, 0.0f)
	};

	Shader shader("res/shaders/ParallaxMapping.shader");

	Texture wallTexture("res/textures/bricks2.jpg", true);
	Texture wallNormal("res/textures/bricks2_normal.jpg", true, true);
	Texture wallHeight("res/textures/bricks2_disp.jpg", true);

	Texture toyTexture("res/textures/wood.png", true);
	Texture toyNormal("res/textures/toy_box_normal.png", true, true);
	Texture toyHeight("res/textures/toy_box_disp.png", true);


	shader.Bind();
	shader.SetUniform1i("diffuseMap", 0);
	shader.SetUniform1i("normalMap", 1);
	shader.SetUniform1i("depthMap", 2);
	shader.SetUniform1f("height_scale", 0.1);

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


		shader.Bind();
		shader.SetUniform1i("normalToggle", bNormal);
		shader.SetUniform1i("parallaxToggle", bParallax);
		shader.SetUniformMat4f("projection", projection);
		shader.SetUniformMat4f("view", view);
		// set lighting uniforms
		shader.SetUniformVec3("lightPos", lightPos);
		shader.SetUniformVec3("viewPos", camera.GetPosition());
		wallTexture.Bind(0);
		wallNormal.Bind(1);
		wallHeight.Bind(2);
		model = glm::mat4(1.0f);
		model = glm::translate(model, planePos[0]);
		//model = glm::rotate(model, glm::radians((float)glfwGetTime() * -10.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0))); // rotate the quad to show parallax mapping from multiple directions
		shader.SetUniformMat4f("model", model);
		plane.Draw(shader, renderer);
		
		model = glm::mat4(1.0f);
		model = glm::translate(model, planePos[1]);
		shader.SetUniformMat4f("model", model);
		toyTexture.Bind(0);
		toyNormal.Bind(1);
		toyHeight.Bind(2);
		toyPlane.Draw(shader, renderer);
		
		
		shader.Unbind();



		// Check and call events and swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
}

void ParallaxMapping::ProcessInput(GLFWwindow* window, int key, int action)
{
	Scene::ProcessInput(window, key, action);

	switch (key)
	{
		case GLFW_KEY_F1:
		{
			switch (action)
			{
				case GLFW_PRESS:
				{
					// Toggle normal maping on/off
					bNormal = !bNormal;
					break;
				}
				default:
					// No action
					break;
			}
			break;
		}
		case GLFW_KEY_F2:
		{
			switch (action)
			{
				case GLFW_PRESS:
				{
					// Toggle Parallax Mapping on/off
					bParallax = !bParallax;
					break;
				}
				default:
					// No action
					break;
			}
			break;
		}
		default:
			// No Action
			break;
	}
}
