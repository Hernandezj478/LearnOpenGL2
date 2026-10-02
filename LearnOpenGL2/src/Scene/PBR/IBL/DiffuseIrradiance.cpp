#include "DiffuseIrradiance.h"

#include "../ColorPalette.h"
#include "../Renderer.h"
#include "../Texture.h"
#include "../FrameBuffer.h"
#include "../RenderBuffer.h"
#include "../Cubemap.h"

#include "../Sphere.h"
#include "../Cube.h"

DiffuseIrradiance::DiffuseIrradiance(int width, int height)
{
	camera.SetScreenSize(width, height);
	camera.SetCursorPos();
	camera.SetAspectRatio();
}

void DiffuseIrradiance::Run(GLFWwindow* window)
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	Sphere sphere;
	Cube cube;
	cube.CreateSkybox();
	Shader pbrShader("res/shaders/PBR_DI.shader");
	Shader equirectangularToCubemapShader("res/shaders/EquirectangularToCubemap_DI.shader");
	Shader irradianceShader("res/shaders/Irradiance_DI.shader");
	Shader backgroundShader("res/shaders/Background_DI.shader");

	pbrShader.Bind();
	pbrShader.SetUniform1i("irradianceMap", 0);
	pbrShader.SetUniformVec3("albedo", glm::vec3(0.5f, 0.0f, 0.0f));
	pbrShader.SetUniform1f("ao", 1.0f);

	backgroundShader.Bind();
	backgroundShader.SetUniform1i("environmentMap", 0);

	Renderer renderer;

	// Lights
	std::vector<glm::vec3> lightPositions =
	{
		glm::vec3(-10.0f,  10.0f, 10.0f),
		glm::vec3( 10.0f,  10.0f, 10.0f),
		glm::vec3(-10.0f, -10.0f, 10.0f),
		glm::vec3( 10.0f, -10.0f, 10.0f)
	};

	std::vector<glm::vec3> lightColors =
	{
		glm::vec3(300.0f),
		glm::vec3(300.0f),
		glm::vec3(300.0f),
		glm::vec3(300.0f)
	};
	int nrRows = 7;
	int nrColumns = 7;
	float spacing = 2.5;

	// pbr: setup framebuffer
	FrameBuffer captureFBO;
	RenderBuffer captureRBO;
	captureRBO.Bind();
	captureRBO.CreateStorage(GL_DEPTH_COMPONENT24, 512, 512);
	captureFBO.Bind();
	captureFBO.AttachRenderBuffer(captureRBO, GL_DEPTH_ATTACHMENT);

	//pbr: load the HDR environment map
	Texture hdrTexture;
	hdrTexture.CreateHDRTexture("res/textures/PBR/newport_loft.hdr", GL_RGB16F, ECLAMP, GL_LINEAR);
	
	// pbr: setup cubemap to render to and attach to framebuffer
	Cubemap envCubemap;
	envCubemap.CreateHDRCubemap(512, 512);

	// pbr: set up projection and view matrices for capturing data onto the 6 cubemap face directions
	glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	std::vector<glm::mat4> captureViews =
	{
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	};

	// pbr: convert HDR equirectangular environment map to cubemap equivalent
	equirectangularToCubemapShader.Bind();
	equirectangularToCubemapShader.SetUniform1i("equirectangularMap", 0);
	equirectangularToCubemapShader.SetUniformMat4f("projection", captureProjection);
	hdrTexture.Bind();

	glViewport(0, 0, 512, 512);
	captureFBO.Bind();
	for (unsigned int i = 0; i < 6; ++i)
	{
		equirectangularToCubemapShader.SetUniformMat4f("view", captureViews[i]);
		captureFBO.AttachCubemap(envCubemap, i);
		renderer.ClearBufferBits(COLOR_DEPTH);
		cube.Draw(equirectangularToCubemapShader, renderer);
	}
	captureFBO.Unbind();

	// pbr: create an irradiance cubemap and re-scale capture FBO to irradiance scale
	Cubemap irradianceMap;
	irradianceMap.CreateHDRCubemap(32, 32);
	captureFBO.Bind();
	captureRBO.Bind();
	captureRBO.CreateStorage(GL_DEPTH_COMPONENT24, 32, 32);

	// pbr: solve diffuse integral by convolution to create an irradiance cubemap
	irradianceShader.Bind();
	irradianceShader.SetUniform1i("environmentMap", 0);
	irradianceShader.SetUniformMat4f("projection", captureProjection);
	envCubemap.Bind();

	glViewport(0, 0, 32, 32);
	captureFBO.Bind();
	for (unsigned int i = 0; i < 6; ++i)
	{
		irradianceShader.SetUniformMat4f("view", captureViews[i]);
		captureFBO.AttachCubemap(irradianceMap, i);
		renderer.ClearBufferBits(COLOR_DEPTH);
		cube.Draw(irradianceShader, renderer);
	}
	captureFBO.Unbind();

	// initialize static shader uniforms before rendering
	glm::mat4 projection = glm::perspective(glm::radians(camera.GetFOV()), camera.GetAspectRatio(), 0.1f, 100.0f);
	pbrShader.Bind();
	pbrShader.SetUniformMat4f("projection", projection);
	backgroundShader.Bind();
	backgroundShader.SetUniformMat4f("projection", projection);

	// then before rendering, configure the viewport to the original framebuffer's screen dimensions
	int scrnWidth, scrnHeight;
	glfwGetFramebufferSize(window, &scrnWidth, &scrnHeight);
	glViewport(0, 0, scrnWidth, scrnHeight);

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		ProcessMovement(window);

		renderer.Clear(DARK_GREY, COLOR_DEPTH);

		//render scene, supplying the convoluted irradiance map to the final shader
		pbrShader.Bind();
		glm::mat4 view = camera.GetViewMatrix();
		pbrShader.SetUniformMat4f("view", view);
		pbrShader.SetUniformVec3("camPos", camera.GetPosition());

		// bind pre-computed IBL data
		irradianceMap.Bind();

		// render rows*column number of spheres with varying metallic/roughness values scaled by rows and columns respectively
		glm::mat4 model = glm::mat4(1.0f);
		for (int row = 0; row < nrRows; ++row)
		{
			pbrShader.SetUniform1f("metallic", (float)row / (float)nrRows);
			for (int col = 0; col < nrColumns; ++col)
			{
				// clamp the roughness to 0.025 - 1.0 as perfectly smooth surfaces (roughness of 0.0) tend to look a bit off
				pbrShader.SetUniform1f("roughness", glm::clamp((float)col / (float)nrColumns, 0.05f, 1.0f));

				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(
					(float)(col - (nrColumns / 2)) * spacing,
					(float)(row - (nrRows / 2)) * spacing,
					-2.0f
				));
				pbrShader.SetUniformMat4f("model", model);
				pbrShader.SetUniformMat3f("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
				sphere.Draw(pbrShader, renderer);
			}
		}

		// render light source (simply re-render sphere at light positions)
		// this looks a bit off as we use the same shader, but itll make their positions obvious and
		// keep the codeprint small
		for (unsigned int i = 0; i < lightPositions.size(); ++i)
		{
			glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0f) * 5.0f, 0.0f, 0.0f);
			newPos = lightPositions[i];
			pbrShader.SetUniformVec3("lightPositions[" + std::to_string(i) + "]", newPos);
			pbrShader.SetUniformVec3("lightColors[" + std::to_string(i) + "]", lightColors[i]);

			model = glm::mat4(1.0f);
			model = glm::translate(model, newPos);
			model = glm::scale(model, glm::vec3(0.5f));
			pbrShader.SetUniformMat4f("model", model);
			pbrShader.SetUniformMat3f("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
			sphere.Draw(pbrShader, renderer);
		}

		// render skybox (render as last to prevent overdraw)
		backgroundShader.Bind();
		backgroundShader.SetUniformMat4f("view", view);
		envCubemap.Bind();
		cube.Draw(backgroundShader, renderer);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
}
