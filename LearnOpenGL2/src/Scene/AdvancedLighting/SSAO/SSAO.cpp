#include "SSAO.h"
#include "../ColorPalette.h"
#include "../Renderer.h"
#include "../Texture.h"
#include "../FrameBuffer.h"
#include "../Plane.h"
#include "../Shader.h"
#include "../Cube.h"
#include "../Model.h"

#include <random>

SSAO::SSAO(int width, int height)
{
	camera.SetScreenSize(width, height);
	camera.SetCursorPos();
	camera.SetAspectRatio();
}

void SSAO::Run(GLFWwindow* window)
{
	glEnable(GL_DEPTH_TEST);

	Shader shaderGeometryPass("res/shaders/SSAO_Geometry.shader");
	Shader shaderLightingPass("res/shaders/SSAO_Lighting.shader");
	Shader shaderSSAO("res/shaders/SSAO.shader");
	Shader shaderSSAOBlur("res/shaders/SSAO_Blur.shader");

	Cube cube(true, true, true);
	Model backpack("res/meshes/Backpack/Backpack.obj");
	Plane2D screenQuad;

	FrameBuffer gBuffer;
	Texture gPosition, gNormal, gAlbedo;
	gPosition.CreateColorbufferHDR(camera.GetScreenWidth(), camera.GetScreenHeight(), GL_RGBA16F);
	gNormal.CreateColorbufferHDR(camera.GetScreenWidth(), camera.GetScreenHeight(), GL_RGBA16F);
	gAlbedo.CreateColorbufferHDR(camera.GetScreenWidth(), camera.GetScreenHeight(), GL_RGBA16F);
	gBuffer.Bind();
	gBuffer.AttachColorBuffer(gPosition, 0);
	gBuffer.AttachColorBuffer(gNormal, 1);
	gBuffer.AttachColorBuffer(gAlbedo, 2);

	unsigned int* attachments = new unsigned int[3];
	attachments[0] = GL_COLOR_ATTACHMENT0;
	attachments[1] = GL_COLOR_ATTACHMENT1;
	attachments[2] = GL_COLOR_ATTACHMENT2;
	gBuffer.ConfigureAttachments(attachments, 3);

	RenderBuffer rboDepth;
	rboDepth.Bind();
	rboDepth.CreateStorage(GL_DEPTH_COMPONENT, camera.GetScreenWidth(), camera.GetScreenHeight());
	gBuffer.AttachRenderBuffer(rboDepth, GL_DEPTH_ATTACHMENT);
	gBuffer.FrameBufferComplete();
	gBuffer.Unbind();

	FrameBuffer ssaoFBO, ssaoBlurFBO;
	Texture ssaoColorBuffer, ssaoColorBufferBlur;
	ssaoColorBuffer.CreateColorbuffer(camera.GetScreenWidth(), camera.GetScreenHeight(), GL_RED);
	ssaoColorBufferBlur.CreateColorbuffer(camera.GetScreenWidth(), camera.GetScreenHeight(), GL_RED);
	ssaoFBO.Bind();
	ssaoFBO.AttachColorBuffer(ssaoColorBuffer);
	ssaoFBO.FrameBufferComplete();

	ssaoBlurFBO.Bind();
	ssaoBlurFBO.AttachColorBuffer(ssaoColorBufferBlur);
	ssaoBlurFBO.FrameBufferComplete();

	// Generate sample kernel
	std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0);	// generates random floats between 0.0 and 1.0
	std::default_random_engine generator;
	std::vector<glm::vec3> ssaoKernel;
	for (unsigned int i = 0; i < 64; i++)
	{
		glm::vec3 sample(randomFloats(generator) * 2.0f - 1.0f, randomFloats(generator) * 2.0f - 1.0f, randomFloats(generator));
		sample = glm::normalize(sample);
		sample *= randomFloats(generator);
		float scale = float(i) / 64.0f;

		// scale samples s.t. theyre more aligned to center of kernel
		scale = ourLerp(0.1f, 1.0f, scale * scale);
		sample *= scale;
		ssaoKernel.push_back(sample);
	}
	// Generate noise texture
	std::vector<glm::vec3> ssaoNoise;
	for (unsigned int i = 0; i < 16; i++)
	{
		glm::vec3 noise(randomFloats(generator) * 2.0f - 1.0f, randomFloats(generator) * 2.0f - 1.0f, 0.0f);
		ssaoNoise.push_back(noise);
	}
	Texture noiseTexture;
	noiseTexture.CreateColorbufferHDR(4, 4, GL_RGBA16F, REPEAT, GL_NEAREST);

	// lighting info
	glm::vec3 lightPos = glm::vec3(2.0f, 4.0f, -2.0f);
	glm::vec3 lightColor = glm::vec3(0.2f, 0.2f, 0.7f);

	// Shader config
	shaderLightingPass.Bind();
	shaderLightingPass.SetUniform1i("gPosition", 0);
	shaderLightingPass.SetUniform1i("gNormal", 1);
	shaderLightingPass.SetUniform1i("gAlbedo", 2);
	shaderLightingPass.SetUniform1i("ssao", 3);
	shaderSSAO.Bind();
	shaderSSAO.SetUniform1i("gPosition", 0);
	shaderSSAO.SetUniform1i("gNormal", 1);
	shaderSSAO.SetUniform1i("texNoise", 2);
	shaderSSAOBlur.Bind();
	shaderSSAOBlur.SetUniform1i("ssaoInput", 0);

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		Renderer renderer;
		ProcessMovement(window);
		renderer.Clear(BLACK, COLOR_DEPTH);

		// Render Here
		glm::mat4 projection = glm::perspective(glm::radians(camera.GetFOV()), camera.GetAspectRatio(), 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 model = glm::mat4(1.0f);

		// 1. Geometry pass: render scene's geometry/color data
		gBuffer.Bind();
		renderer.ClearBufferBits(COLOR_DEPTH);
		shaderGeometryPass.Bind();
		shaderGeometryPass.SetUniformMat4f("projection", projection);
		shaderGeometryPass.SetUniformMat4f("view", view);
		// room cube
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 7.0f, 0.0f));
		model = glm::scale(model, glm::vec3(7.5f));
		shaderGeometryPass.SetUniformMat4f("model", model);
		shaderGeometryPass.SetUniform1i("invertedNormals", 1);
		cube.Draw(shaderGeometryPass, renderer);
		shaderGeometryPass.SetUniform1i("invertedNormals", 0);
		// backpack model on the floor
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		shaderGeometryPass.SetUniformMat4f("model", model);
		backpack.Draw(shaderGeometryPass, renderer);
		gBuffer.Unbind();

		// 2. Generate SSAO texture
		ssaoFBO.Bind();
		renderer.ClearBufferBits(COLOR);
		shaderSSAO.Bind();
		for (unsigned int i = 0; i < 64; i++)
		{
			shaderSSAO.SetUniformVec3("samples[" + std::to_string(i) + "]", ssaoKernel[i]);
		}
		shaderSSAO.SetUniformMat4f("projection", projection);
		gPosition.Bind(0);
		gNormal.Bind(1);
		noiseTexture.Bind(2);
		screenQuad.Draw(shaderSSAO, renderer);
		ssaoFBO.Unbind();

		// 3. Blur SSAO texture to remove noise
		ssaoBlurFBO.Bind();
		renderer.ClearBufferBits(COLOR);
		shaderSSAOBlur.Bind();
		ssaoColorBuffer.Bind();
		screenQuad.Draw(shaderSSAOBlur, renderer);
		ssaoBlurFBO.Unbind();

		// 4. Lighting pass: traditional deferred Blinn-Phong lighting with added screen-space ambient occlusion
		renderer.ClearBufferBits(COLOR_DEPTH);
		shaderLightingPass.Bind();
		// send light relevant uniforms
		glm::vec3 lightPosView = glm::vec3(camera.GetViewMatrix() * glm::vec4(lightPos, 1.0));
		shaderLightingPass.SetUniformVec3("light.Position", lightPosView);
		shaderLightingPass.SetUniformVec3("light.Color", lightColor);
		// update attenuation parameters
		const float linear = 0.09f;
		const float quadratic = 0.032f;
		shaderLightingPass.SetUniform1f("light.Linear", linear);
		shaderLightingPass.SetUniform1f("light.Quadratic", quadratic);
		gPosition.Bind(0);
		gNormal.Bind(1);
		gAlbedo.Bind(2);
		ssaoColorBuffer.Bind(3);
		screenQuad.Draw(shaderLightingPass, renderer);


		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
}

float SSAO::ourLerp(float a, float b, float f)
{
	return a + f * (b - a);
}
