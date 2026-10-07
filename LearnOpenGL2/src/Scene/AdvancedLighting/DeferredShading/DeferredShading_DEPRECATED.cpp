#include "DeferredShading.h"

#include "../ColorPalette.h"
#include "../Renderer.h"
#include "../Texture.h"
#include "../FrameBuffer.h"
#include "../Plane.h"
#include "../Shader.h"
#include "../Cube.h"
#include "../Model.h"

DeferredShading::DeferredShading(int width, int height)
{
	camera.SetScreenSize(width, height);
	camera.SetCursorPos();
	camera.SetAspectRatio();
}

void DeferredShading::Run(GLFWwindow* window)
{
	glEnable(GL_DEPTH_TEST);
	Shader shaderGeometryPass("res/shaders/GBuffer.shader");
	Shader shaderLightingPass("res/shaders/DeferredShading.shader");
	Shader shaderLightbox("res/shaders/DeferredLightbox.shader");

	Model backpack("res/meshes/Backpack/Backpack.obj");

	Cube cube(true, true, true);
	Plane floor;
	Plane2D screenQuad;

	std::vector<glm::vec3> objectPositions =
	{
		glm::vec3(-3.0, -0.5, -3.0),
		glm::vec3( 0.0, -0.5, -3.0),
		glm::vec3( 3.0, -0.5, -3.0),
		glm::vec3(-3.0, -0.5,  0.0),
		glm::vec3( 0.0, -0.5,  0.0),
		glm::vec3( 3.0, -0.5,  0.0),
		glm::vec3(-3.0, -0.5,  3.0),
		glm::vec3( 0.0, -0.5,  3.0),
		glm::vec3( 3.0, -0.5,  3.0)
	};

	FrameBuffer gBuffer;
	Texture gPosition, gNormal, gAlbedoSpec;
	gPosition.CreateColorbufferHDR(camera.GetScreenWidth(), camera.GetScreenHeight(), GL_RGBA16F);
	gNormal.CreateColorbufferHDR(camera.GetScreenWidth(), camera.GetScreenHeight(), GL_RGBA16F);
	gAlbedoSpec.CreateColorbufferHDR(camera.GetScreenWidth(), camera.GetScreenHeight(), GL_RGBA16F);

	gBuffer.Bind();
	gBuffer.AttachColorBuffer(gPosition, 0);
	gBuffer.AttachColorBuffer(gNormal, 1);
	gBuffer.AttachColorBuffer(gAlbedoSpec, 2);

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

	// Lighting info
	const unsigned int NR_LIGHTS = 32;
	std::vector<glm::vec3> lightPositions;
	std::vector<glm::vec3> lightColors;
	srand(13);
	for (unsigned int i = 0; i < NR_LIGHTS; i++)
	{
		float xPos = static_cast<float>(((rand() % 100) / 100.0f) * 6.0f - 3.0f);
		float yPos = static_cast<float>(((rand() % 100) / 100.0f) * 6.0f - 4.0f);
		float zPos = static_cast<float>(((rand() % 100) / 100.0f) * 6.0f - 3.0f);
		lightPositions.push_back(glm::vec3(xPos, yPos, zPos));
		// also calculate random color
		float rColor = static_cast<float>(((rand() % 100) / 200.0f) + 0.5); // between 0.5 and 1
		float gColor = static_cast<float>(((rand() % 100) / 200.0f) + 0.5); // between 0.5 and 1
		float bColor = static_cast<float>(((rand() % 100) / 200.0f) + 0.5); // between 0.5 and 1
		lightColors.push_back(glm::vec3(rColor, gColor, bColor));
	}

	shaderLightingPass.Bind();
	shaderLightingPass.SetUniform1i("gPosition", 0);
	shaderLightingPass.SetUniform1i("gNormal", 1);
	shaderLightingPass.SetUniform1i("gAlbedoSpec", 2);

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
		glm::mat4 model = glm::mat4(1.0);

		// 1. Geometry pass: render scene's geometry/color data into gBuffer
		gBuffer.Bind();
		renderer.ClearBufferBits(COLOR_DEPTH);
		shaderGeometryPass.Bind();
		shaderGeometryPass.SetUniformMat4f("projection", projection);
		shaderGeometryPass.SetUniformMat4f("view", view);
		for (unsigned int i = 0; i < objectPositions.size(); i++)
		{
			model = glm::mat4(1.0);
			model = glm::translate(model, objectPositions[i]);
			model = glm::scale(model, glm::vec3(0.25f));
			shaderGeometryPass.SetUniformMat4f("model", model);
			backpack.Draw(shaderGeometryPass, renderer);
		}
		gBuffer.Unbind();

		// 2. Lighting pass: calculate lighting by iterating over a screen filled quad pixel-by-pixel using the gbuffer's content
		renderer.ClearBufferBits(COLOR_DEPTH);
		shaderLightingPass.Bind();
		gPosition.Bind(0);
		gNormal.Bind(1);
		gAlbedoSpec.Bind(2);
		// send relevant uniforms
		for (unsigned int i = 0; i < NR_LIGHTS; i++)
		{
			shaderLightingPass.SetUniformVec3("lights[" + std::to_string(i) + "].Position", lightPositions[i]);
			shaderLightingPass.SetUniformVec3("lights[" + std::to_string(i) + "].Color", lightColors[i]);
			// update attenuation parameters and calculate radius
			const float constant = 1.0f;
			const float linear = 0.7f;
			const float quadratic = 1.8f;
			shaderLightingPass.SetUniform1f("lights[" + std::to_string(i) + "].Linear", linear);
			shaderLightingPass.SetUniform1f("lights[" + std::to_string(i) + "].Quadratic", quadratic);
			// then calculate radius of light volume/sphere
			const float maxBrightness = std::fmaxf(std::fmaxf(lightColors[i].r, lightColors[i].g), lightColors[i].b);
			float radius = (-linear + std::sqrt(linear * linear - 4 * quadratic * (constant - (256.0f / 5.0f) * maxBrightness))) / (2.0f * quadratic);
			shaderLightingPass.SetUniform1f("lights[" + std::to_string(i) + "].Radius", radius);
		}
		shaderLightingPass.SetUniformVec3("viewPos", camera.GetPosition());
		// finally render quad
		screenQuad.Draw(shaderLightingPass, renderer);

		// 2.5. copy content of geometry's depth buffer to default framebuffer's depth buffer
		gBuffer.BindRead();
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		// Blit to default frambuffer. note this may or may not work as internal formats of both the FBO and default frambuffer have to match
		// the internal formats are implementation defined.
		
		glBlitFramebuffer(0, 0, camera.GetScreenWidth(), camera.GetScreenHeight(), 0, 0, camera.GetScreenWidth(), camera.GetScreenHeight(), GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// 3. render lights on top of scene
		shaderLightbox.Bind();
		shaderLightbox.SetUniformMat4f("projection", projection);
		shaderLightbox.SetUniformMat4f("view", view);
		for (int i = 0; i < lightPositions.size(); i++)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, lightPositions[i]);
			model = glm::scale(model, glm::vec3(0.125f));
			shaderLightbox.SetUniformMat4f("model", model);
			shaderLightbox.SetUniformVec3("lightColor", lightColors[i]);
			cube.Draw(shaderLightbox, renderer);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
}

void DeferredShading::AdjustScreenSize(int width, int height)
{
	Scene::AdjustScreenSize(width, height);
	// We need to readjust our viewport, colorbuffer, and depthbuffer to resize the screen quad properly
	glViewport(0, 0, width, height);
	/*glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);*/
}
