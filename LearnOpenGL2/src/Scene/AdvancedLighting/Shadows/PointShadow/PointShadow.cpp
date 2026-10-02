#include "PointShadow.h"
#include "../ColorPalette.h"

#include "../FrameBuffer.h"
#include "../Texture.h"
#include "../Cube.h"
#include "../Plane.h"
#include "../Cubemap.h"
#include "../Shader.h"

#define SHADOW_WIDTH  1024
#define SHADOW_HEIGHT 1024


PointShadow::PointShadow(int width, int height)
{
	camera.SetScreenSize(width, height);
	camera.SetCursorPos();	// Must only be called after setting screen size
	camera.SetAspectRatio();
}

void PointShadow::Run(GLFWwindow* window)
{
	glEnable(GL_DEPTH_TEST);

	std::vector<glm::vec3> shadowTransformTarget =
	{
		glm::vec3( 1.0,  0.0,  0.0),
		glm::vec3(-1.0,  0.0,  0.0),
		glm::vec3( 0.0,  1.0,  0.0),
		glm::vec3( 0.0, -1.0,  0.0),
		glm::vec3( 0.0,  0.0,  1.0),
		glm::vec3( 0.0,  0.0, -1.0),
	};

	std::vector<glm::vec3> shadowUpVectors =
	{
		glm::vec3(0.0, -1.0,  0.0),
		glm::vec3(0.0, -1.0,  0.0),
		glm::vec3(0.0,  0.0,  1.0),
		glm::vec3(0.0,  0.0, -1.0),
		glm::vec3(0.0, -1.0,  0.0),
		glm::vec3(0.0, -1.0,  0.0),
	};
	
	Shader shader("res/shaders/PointShadowMapping.shader");
	Shader depthShader("res/shaders/PointShadowMappingDepth.shader");

	Texture woodTexture("res/textures/wood.png");

	Cubemap depthCubeMap;
	depthCubeMap.CreateDepthCubeMap(SHADOW_WIDTH, SHADOW_HEIGHT);

	FrameBuffer depthMapFBO;
	depthMapFBO.Bind();
	depthMapFBO.AttachDepthCubeMap(depthCubeMap);
	depthMapFBO.NoColorData();
	depthMapFBO.FrameBufferComplete();
	depthMapFBO.Unbind();

	shader.Bind();
	shader.SetUniform1i("diffuseTexture", 0);
	shader.SetUniform1i("depthMap", 1);
	shader.Unbind();

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		Renderer renderer;
		ProcessMovement(window);
		lightPos.x = static_cast<float>(cos(glfwGetTime() * 0.5) * 3.0);
		lightPos.z = static_cast<float>(sin(glfwGetTime() * 0.5) * 3.0);
		renderer.Clear(DARK_GREY, COLOR_DEPTH);
		// create cubemap transform matrices
		float aspect = (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT;
		float nearPlane = 0.1f;
		float far_plane = 20.0f;
		glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, nearPlane, far_plane);
		std::vector<glm::mat4> shadowTransforms;

		for (int i = 0; i < shadowTransformTarget.size(); ++i)
		{
			glm::mat4 transform = 
				shadowProj * glm::lookAt(lightPos, lightPos + shadowTransformTarget[i], shadowUpVectors[i]);
			shadowTransforms.push_back(transform);
		}
	
		// Render Here
		// render scene to depth map
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		depthMapFBO.Bind();
		renderer.ClearBufferBits(DEPTH);
		depthShader.Bind();
		for (unsigned int i = 0; i < 6; ++i)
		{
			depthShader.SetUniformMat4f("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
		}
		depthShader.SetUniform1f("far_plane", far_plane);
		depthShader.SetUniformVec3("lightPos", lightPos);
		RenderScene(depthShader, renderer);
		depthMapFBO.Unbind();
		depthShader.Unbind();

		// render scene as normal
		glViewport(0, 0, camera.GetScreenWidth(), camera.GetScreenHeight());
		renderer.ClearBufferBits(COLOR_DEPTH);
		glm::mat4 projection = glm::perspective(glm::radians(camera.GetFOV()), camera.GetAspectRatio(), 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		shader.Bind();
		shader.SetUniformMat4f("projection", projection);
		shader.SetUniformMat4f("view", view);
		// set lighting uniforms
		shader.SetUniformVec3("lightPos", lightPos);
		shader.SetUniformVec3("viewPos", camera.GetPosition());
		shader.SetUniform1f("far_plane", far_plane);
		woodTexture.Bind(0);
		depthCubeMap.Bind(1);
		RenderScene(shader, renderer);
		shader.Unbind();
		// Check and call events and swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
}

void PointShadow::RenderScene(Shader& shader, const Renderer& renderer)
{

	std::vector<glm::vec3> cubePositions =
	{
		glm::vec3(4.0f, -3.5f, 0.0),
		glm::vec3(2.0f, 3.0f, 1.0),
		glm::vec3(-3.0f, -1.0f, 0.0),
		glm::vec3(-1.5f, 1.0f, 1.5),
		glm::vec3(-1.5f, 2.0f, -3.0),
	};

	std::vector<glm::vec3> cubeScales =
	{
		glm::vec3(0.5f),
		glm::vec3(0.75f),
		glm::vec3(0.5f),
		glm::vec3(0.5f),
		glm::vec3(0.75f),
	};

	Cube cube(true, true, true);
	Cube lightBox(true, false, true);
	// skybox
	glm::mat4 model = glm::mat4(1.0);
	model = glm::scale(model, glm::vec3(5.0f));
	shader.Bind();
	shader.SetUniformMat4f("model", model);
	glDisable(GL_CULL_FACE);
	shader.SetUniform1i("reverse_normals", 1);
	cube.Draw(shader, renderer);
	shader.SetUniform1i("reverse_normals", 0);
	glEnable(GL_CULL_FACE);
	// lightbox
	model = glm::mat4(1.0);
	model = glm::translate(model, lightPos);
	model = glm::scale(model, glm::vec3(0.05));
	shader.SetUniformMat4f("model", model);
	shader.SetUniform1i("isLightbox", true);
	lightBox.Draw(shader, renderer);
	shader.SetUniform1i("isLightbox", false);
	//cubes
	for (int i = 0; i < 5; ++i)
	{
		model = glm::mat4(1.0f);
		model = glm::translate(model, cubePositions[i]);
		if (i == 4)
		{
			model = glm::rotate(model, glm::radians(6.0f), glm::vec3(-1.5f, 2.0f, -3.0f));
		}
		model = glm::scale(model, cubeScales[i]);
		shader.SetUniformMat4f("model", model);
		cube.Draw(shader, renderer);
	}
}
