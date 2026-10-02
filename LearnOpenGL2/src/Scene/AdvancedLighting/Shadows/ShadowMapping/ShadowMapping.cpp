#include "ShadowMapping.h"
#include "../ColorPalette.h"

#include "../Renderer.h"
#include "../FrameBuffer.h"
#include "../Texture.h"
#include "../Cube.h"
#include "../Plane.h"

ShadowMapping::ShadowMapping(int width, int height)
{
	camera.SetScreenSize(width, height);
	camera.SetCursorPos();	// Must only be called after setting screen size
	camera.SetAspectRatio();
}

void ShadowMapping::Run(GLFWwindow* window)
{
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);

	Cube cube(true, true);
	Plane2D quad;
	Plane floor(10.0f);
	Texture woodTexture("res/textures/wood.png");

	FrameBuffer depthMapFBO;
	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	Texture depthBuffer;
	depthMapFBO.Bind();
	depthBuffer.CreateDepthbufferTexture(SHADOW_WIDTH, SHADOW_HEIGHT, BCLAMP, GL_LINEAR);
	depthBuffer.CreateBorder(WHITE);

	depthMapFBO.Bind();
	depthMapFBO.AttachDepthBuffer(depthBuffer);
	depthMapFBO.NoColorData();
	depthMapFBO.Unbind();

	Shader shader("res/shaders/ShadowMapping.shader");
	Shader depthShader("res/shaders/ShadowMappingDepth.shader");
	Shader debugDepth("res/shaders/DebugQuad.shader");

	shader.Bind();
	shader.SetUniform1i("diffuseTexture", 0);
	shader.SetUniform1i("shadowMap", 1);

	debugDepth.Bind();
	debugDepth.SetUniform1i("depthMap", 0);

	glm::vec3 cubePositions[] =
	{
		glm::vec3{ 0.0f, 1.5f, 0.0},
		glm::vec3{ 2.0f, 0.0f, 1.0},
		glm::vec3{-1.0f, 0.0f, 2.0}
	};

	glm::vec3 cubeScale[] =
	{
		glm::vec3(0.5f),
		glm::vec3(0.5f),
		glm::vec3(0.25f)
	};

	glm::vec3 lightPos(-2.0f, 4.0f, -1.0f);

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		Renderer renderer;
		ProcessMovement(window);

		// Render Here
		// 1. Render to depth
		//glCullFace(GL_FRONT);
		float near = 1.0f, far = 7.5f;
		glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near, far);
		glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 lightSpaceMatrix = lightProjection * lightView;
		depthShader.Bind();
		depthShader.SetUniformMat4f("lightSpaceMatrix", lightSpaceMatrix);

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		depthMapFBO.Bind();
		woodTexture.Bind();
		renderer.ClearBufferBits(DEPTH);
		//Floor
		glm::mat4 model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -0.25f, 0.0f));
		model = glm::scale(model, glm::vec3(25.0f));
		depthShader.SetUniformMat4f("model", model);
		floor.Draw(depthShader, renderer);
		//Cubes
		model = glm::mat4(1.0f);
		model = glm::translate(model, cubePositions[0]);
		model = glm::scale(model, cubeScale[0]);
		depthShader.SetUniformMat4f("model", model);
		cube.Draw(depthShader, renderer);
		model = glm::mat4(1.0f);
		model = glm::translate(model, cubePositions[1]);
		model = glm::scale(model, cubeScale[1]);
		depthShader.SetUniformMat4f("model", model);
		cube.Draw(depthShader, renderer);
		model = glm::mat4(1.0f);
		model = glm::translate(model, cubePositions[2]);
		model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f)));
		model = glm::scale(model, cubeScale[2]);
		depthShader.SetUniformMat4f("model", model);
		cube.Draw(depthShader, renderer);

		depthMapFBO.Unbind();
		//glCullFace(GL_BACK);
		// reset viewport
		glViewport(0, 0, camera.GetScreenWidth(), camera.GetScreenHeight());
		renderer.ClearBufferBits(COLOR_DEPTH);
		//2. Render as normal
		shader.Bind();
		glm::mat4 projection = glm::perspective(glm::radians(camera.GetFOV()), camera.GetAspectRatio(), 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		shader.SetUniformMat4f("projection", projection);
		shader.SetUniformMat4f("view", view);
		// set light uniforms
		shader.SetUniformVec3("viewPos", camera.GetPosition());
		shader.SetUniformVec3("lightPos", lightPos);
		shader.SetUniformMat4f("lightSpaceMatrix", lightSpaceMatrix);
		woodTexture.Bind(0);
		depthBuffer.Bind(1);
		//Floor
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -0.25f, 0.0f));
		model = glm::scale(model, glm::vec3(25.0f));
		shader.SetUniformMat4f("model", model);
		floor.Draw(shader, renderer);
		//Cubes
		model = glm::mat4(1.0f);
		model = glm::translate(model, cubePositions[0]);
		model = glm::scale(model, cubeScale[0]);
		shader.SetUniformMat4f("model", model);
		cube.Draw(shader, renderer);
		model = glm::mat4(1.0f);
		model = glm::translate(model, cubePositions[1]);
		model = glm::scale(model, cubeScale[1]);
		shader.SetUniformMat4f("model", model);
		cube.Draw(shader, renderer);
		model = glm::mat4(1.0f);
		model = glm::translate(model, cubePositions[2]);
		model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f)));
		model = glm::scale(model, cubeScale[2]);
		shader.SetUniformMat4f("model", model);
		cube.Draw(shader, renderer);

		// Render depth map to quad for debugging
		debugDepth.Bind();
		depthBuffer.Bind();
		debugDepth.SetUniform1f("near", near);
		debugDepth.SetUniform1f("far", far);
		//quad.Draw(debugDepth, renderer);

		// Check and call events and swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
}
