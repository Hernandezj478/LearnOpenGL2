#include "BlinnPhong.h"

#include "../ColorPalette.h"

#include "../Plane.h"
#include "../Texture.h"

BlinnPhong::BlinnPhong(int width, int height)
{
	camera.SetScreenSize(width, height);
	camera.SetCursorPos();	// Must only be called after setting screen size
	camera.SetAspectRatio();
}

void BlinnPhong::Run(GLFWwindow* window)
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Plane floor(1.0f);
	Texture floorTexture("res/textures/wood.png");
	Shader shader("res/shaders/BlinnPhong.shader");
	shader.Bind();
	shader.SetUniform1i("texture1", 0);

	glm::vec3 lightPos(0.0f, 0.25f, 0.0f);

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		Renderer renderer;
		ProcessMovement(window);
		renderer.Clear(DARK_GREY, COLOR_DEPTH);

		// Render Here
		glm::mat4 projection = glm::perspective(glm::radians(camera.GetFOV()), camera.GetAspectRatio(), .1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		shader.Bind();
		shader.SetUniformMat4f("projection", projection);
		shader.SetUniformMat4f("view", view);

		shader.SetUniformVec3("lightPos", lightPos);
		shader.SetUniformVec3("viewPos", camera.GetPosition());
		shader.SetUniform1i("blinn", isBlinn);
		
		floorTexture.Bind();
		floor.Draw(shader, renderer);

		// Check and call events and swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
}

void BlinnPhong::ProcessInput(GLFWwindow* window, int key, int action)
{
	Scene::ProcessInput(window, key, action);
	switch (key)
	{
		case GLFW_KEY_B:
			switch (action)
			{
				case GLFW_PRESS:
					isBlinn = !isBlinn;
					std::cout << (isBlinn ? "BlinnPhong" : "Phong") << std::endl;
					return;
				default:
					//No action
					return;
			}
	}
}
