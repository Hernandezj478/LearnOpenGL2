#include "PBR.h"

#include "../ColorPalette.h"
#include "../Renderer.h"
#include "../Texture.h"

#include "../Sphere.h"

PBR::PBR(int width, int height)
{
	camera.SetScreenSize(width, height);
	camera.SetCursorPos();
	camera.SetAspectRatio();
}

void PBR::Run(GLFWwindow* window)
{
	glEnable(GL_DEPTH_TEST);

	Sphere sphere;

	Shader shader("res/shaders/PBR_Texture.shader");
	shader.Bind();
	shader.SetUniform1i("albedoMap", 0);
	shader.SetUniform1i("normalMap", 1);
	shader.SetUniform1i("metallicMap", 2);
	shader.SetUniform1i("roughnessMap", 3);
	shader.SetUniform1i("aoMap", 4);

	//load PBR material texutres
	Texture albedo("res/textures/Rusted_Iron/albedo.png", true);
	Texture normal("res/textures/Rusted_Iron/normal.png", true);
	Texture metallic("res/textures/Rusted_Iron/metallic.png", true);
	Texture roughness("res/textures/Rusted_Iron/roughness.png", true);
	Texture ao("res/textures/Rusted_Iron/ao.png", true);

	// Lights
	std::vector<glm::vec3> lightPositions =
	{
		glm::vec3(-10.0f,  10.0f, 10.0f),
	};
	std::vector<glm::vec3> lightColors =
	{
		glm::vec3(150.0f),
	};
	int nrRows = 7;
	int nrColumns = 7;
	float spacing = 2.5f;


	// initialize static shader uniforms before rendering
	glm::mat4 projection = glm::perspective(glm::radians(camera.GetFOV()), camera.GetAspectRatio(), 0.1f, 100.0f);
	shader.Bind();
	shader.SetUniformMat4f("projection", projection);

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		Renderer renderer;
		ProcessMovement(window);
		renderer.Clear(DARK_GREY, COLOR_DEPTH);

		// Render Here
		glm::mat4 view = camera.GetViewMatrix();
		shader.SetUniformMat4f("view", view);
		shader.SetUniformVec3("camPos", camera.GetPosition());

		glm::mat4 model = glm::mat4(1.0f);
		albedo.Bind(0);
		normal.Bind(1);
		metallic.Bind(2);
		roughness.Bind(3);
		ao.Bind(4);
		for (int row = 0; row < nrRows; ++row)
		{
			shader.SetUniform1f("metallic", (float)row / (float)nrRows);
			for (int col = 0; col < nrColumns; ++col)
			{
				model = glm::mat4(1.0);
				model = glm::translate(model, glm::vec3(
					(col - (nrColumns / 2)) * spacing,
					(row - (nrRows / 2)) * spacing,
					0.0f
				));
				shader.SetUniformMat4f("model", model);
				shader.SetUniformMat3f("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
				sphere.Draw(shader, renderer);
			}
		}
		// render light source (simply re-render sphere at light positions)
		// this looks a bit off as we use the same shader, but it'll make their positions obvious and
		// keep shte codeprint small
		for (unsigned int i = 0; i < lightPositions.size(); ++i)
		{
			glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0f) * 5.0f, 0.0f, 0.0f);
			newPos = lightPositions[i];
			shader.SetUniformVec3("lightPositions[" + std::to_string(i) + "]", newPos);
			shader.SetUniformVec3("lightColors[" + std::to_string(i) + "]", lightColors[i]);

			model = glm::mat4(1.0f);
			model = glm::translate(model, newPos);
			model = glm::scale(model, glm::vec3(0.5f));
			shader.SetUniformMat4f("model", model);
			shader.SetUniformMat3f("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
			sphere.Draw(shader, renderer);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
}
