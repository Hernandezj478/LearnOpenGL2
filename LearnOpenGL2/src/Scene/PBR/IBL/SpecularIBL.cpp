#include "SpecularIBL.h"

#include "../ColorPalette.h"
#include "../Renderer.h"
#include "../Texture.h"
#include "../FrameBuffer.h"
#include "../RenderBuffer.h"
#include "../Cubemap.h"

#include "../Sphere.h"
#include "../Cube.h"
#include "../Plane.h"

SpecularIBL::SpecularIBL(int width, int height)
{
	camera.SetScreenSize(width, height);
	camera.SetCursorPos();
	camera.SetAspectRatio();
}

void SpecularIBL::Run(GLFWwindow* window)
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	Cube cube;
	cube.CreateSkybox();
	Sphere sphere;
	Plane2D screenQuad;

	Renderer renderer;

	Shader pbrShader("res/shaders/PBR_S.shader");
	Shader equirectangularToCubemapShader("res/shaders/EquirectangularToCubemap_S.shader");
	Shader irradianceShader("res/shaders/IrradianceConvolution_S.shader");
	Shader prefilterShader("res/shaders/Prefilter_S.shader");
	Shader brdfShader("res/shaders/BRDF_S.shader");
	Shader backgroundShader("res/shaders/Background_S.shader");

	pbrShader.Bind();
	pbrShader.SetUniform1i("irradianceMap", 0);
	pbrShader.SetUniform1i("prefilterMap", 1);
	pbrShader.SetUniform1i("brdfLUT", 2);
	pbrShader.SetUniform1i("albedoMap", 3);
	pbrShader.SetUniform1i("normalMap", 4);
	pbrShader.SetUniform1i("metallicMap", 5);
	pbrShader.SetUniform1i("roughnessMap", 6);
	pbrShader.SetUniform1i("aoMap", 7);

	backgroundShader.Bind();
	backgroundShader.SetUniform1i("environmentMap", 0);

	Texture Iron_Albedo("res/textures/PBR/Rusted_Iron/albedo.png", true);
	Texture Iron_NormaMap("res/textures/PBR/Rusted_Iron/normal.png", true);
	Texture Iron_MetallicMap("res/textures/PBR/Rusted_Iron/metallic.png", true);
	Texture Iron_RoughnessMap("res/textures/PBR/Rusted_Iron/roughness.png", true);
	Texture Iron_AOMap("res/textures/PBR/Rusted_Iron/ao.png", true);

	Texture Gold_Albedo("res/textures/PBR/Gold/albedo.png", true);
	Texture Gold_NormaMap("res/textures/PBR/Gold/normal.png", true);
	Texture Gold_MetallicMap("res/textures/PBR/Gold/metallic.png", true);
	Texture Gold_RoughnessMap("res/textures/PBR/Gold/roughness.png", true);
	Texture Gold_AOMap("res/textures/PBR/Gold/ao.png", true);

	Texture Grass_Albedo("res/textures/PBR/Grass/albedo.png", true);
	Texture Grass_NormaMap("res/textures/PBR/Grass/normal.png", true);
	Texture Grass_MetallicMap("res/textures/PBR/Grass/metallic.png", true);
	Texture Grass_RoughnessMap("res/textures/PBR/Grass/roughness.png", true);
	Texture Grass_AOMap("res/textures/PBR/Grass/ao.png", true);

	Texture Plastic_Albedo("res/textures/PBR/Plastic/albedo.png", true);
	Texture Plastic_NormaMap("res/textures/PBR/Plastic/normal.png", true);
	Texture Plastic_MetallicMap("res/textures/PBR/Plastic/metallic.png", true);
	Texture Plastic_RoughnessMap("res/textures/PBR/Plastic/roughness.png", true);
	Texture Plastic_AOMap("res/textures/PBR/Plastic/ao.png", true);

	Texture Wall_Albedo("res/textures/PBR/Wall/albedo.png", true);
	Texture Wall_NormaMap("res/textures/PBR/Wall/normal.png", true);
	Texture Wall_MetallicMap("res/textures/PBR/Wall/metallic.png", true);
	Texture Wall_RoughnessMap("res/textures/PBR/Wall/roughness.png", true);
	Texture Wall_AOMap("res/textures/PBR/Wall/ao.png", true);

	std::vector<glm::vec3> lightPositions =
	{
		glm::vec3(-10.0f,  10.0f, 10.0f),
		glm::vec3( 10.0f,  10.0f, 10.0f),
		glm::vec3(-10.0f, -10.0f, 10.0f),
		glm::vec3( 10.0f, -10.0f, 10.0f)
	};

	std::vector<glm::vec3> lightColors =
	{
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f)
	};

	// PBR: setup framebuffer
	FrameBuffer captureFBO;
	RenderBuffer captureRBO;
	captureFBO.Bind();
	captureRBO.Bind();
	captureRBO.CreateStorage(GL_DEPTH_COMPONENT24, 512, 512);
	captureFBO.AttachRenderBuffer(captureRBO, GL_DEPTH_ATTACHMENT);
	captureFBO.FrameBufferComplete();
	// PBR: load the HDR evironment map
	Texture hdrTexture;
	hdrTexture.CreateHDRTexture("res/textures/HDR/newport_loft.hdr", GL_RGB16F, ECLAMP, GL_LINEAR);

	// PBR: setup cubemap to render to and attach to framebuffer
	Cubemap envCubemap;
	envCubemap.CreateHDRCubemap(512, 512, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

	// PBR: setup projection and view matrices for capturing data onto the 6 cubemap face directions
	glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	std::vector<glm::mat4> captureViews =
	{
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};

	// PBR: convert HDR equirectangular environment map to cubemap equivalent
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
	captureFBO.FrameBufferComplete();
	captureFBO.Unbind();

	// let OpenGL generate mipmaps from first mip face (combatting visible dots artifact)
	envCubemap.GenerateMipmap();

	// PBR: create an irradiance cubemap, and re-scale capture FBO to irradiance scale
	Cubemap irradianceMap;
	irradianceMap.CreateHDRCubemap(32, 32, GL_LINEAR, GL_LINEAR);
	captureFBO.Bind();
	captureRBO.Bind();
	captureRBO.CreateStorage(GL_DEPTH_COMPONENT24, 32, 32);
	
	// PBR: solve diffuse integral by convolution to create an irradiance cubemap
	irradianceShader.Bind();
	irradianceShader.SetUniform1i("environmentMap", 0);
	irradianceShader.SetUniformMat4f("projection", captureProjection);
	envCubemap.Bind();

	glViewport(0, 0, 32, 32);
	captureFBO.Bind();
	for (int i = 0; i < 6; ++i)
	{
		irradianceShader.SetUniformMat4f("view", captureViews[i]);
		captureFBO.AttachCubemap(irradianceMap, i);
		renderer.ClearBufferBits(COLOR_DEPTH);

		cube.Draw(irradianceShader, renderer);
	}
	captureFBO.FrameBufferComplete();
	captureFBO.Unbind();
	// PBR: create a pre-filter cubemap, and re-scale capture FBO to pre-filter scale
	Cubemap prefilterMap;
	prefilterMap.CreateHDRCubemap(128, 128, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
	// generate mipmaps for the cubemap so OpenGL automatically allocates the required memory
	prefilterMap.GenerateMipmap();

	// PBR: run a quasi monte-carlo simulation on the environment lighting to create a prefilter cubemap
	prefilterShader.Bind();
	prefilterShader.SetUniform1i("environmentMap", 0);
	prefilterShader.SetUniformMat4f("projection", captureProjection);
	envCubemap.Bind();

	captureFBO.Bind();
	unsigned int maxMipLevels = 5;
	for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
	{
		unsigned int mipWidth = static_cast<unsigned int>(128 * std::pow(0.5, mip));
		unsigned int mipHeight = static_cast<unsigned int>(128 * std::pow(0.5, mip));
		captureRBO.Bind();
		captureRBO.CreateStorage(GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
		glViewport(0, 0, mipWidth, mipHeight);

		float roughness = (float)mip / (float)(maxMipLevels - 1);
		prefilterShader.SetUniform1f("roughness", roughness);
		for (unsigned int i = 0; i < 6; ++i)
		{
			prefilterShader.SetUniformMat4f("view", captureViews[i]);
			captureFBO.AttachCubemap(prefilterMap, i, 0, mip);

			renderer.ClearBufferBits(COLOR_DEPTH);
			cube.Draw(prefilterShader, renderer);
		}
	}
	captureFBO.FrameBufferComplete();
	captureFBO.Unbind();

	// PBR: generate 2D LUT from the BRDF equations
	Texture brdfLUTTexture;
	brdfLUTTexture.CreateLUTBuffer(512, 512, GL_RG16F, GL_RG, GL_FLOAT);

	// then re-configure capture framebuffer object and render screen-space quad with BRDF shader
	captureFBO.Bind();
	captureRBO.Bind();
	captureRBO.CreateStorage(GL_DEPTH_COMPONENT24, 512, 512);
	captureFBO.AttachColorBuffer(brdfLUTTexture);

	glViewport(0, 0, 512, 512);
	brdfShader.Bind();
	renderer.ClearBufferBits(COLOR_DEPTH);
	screenQuad.Draw(brdfShader, renderer);
	captureFBO.FrameBufferComplete();
	captureFBO.Unbind();

	// Initialize static shader uniforms before rendering
	glm::mat4 projection = glm::perspective(glm::radians(camera.GetFOV()), camera.GetAspectRatio(), 0.1f, 100.0f);
	pbrShader.Bind();
	pbrShader.SetUniformMat4f("projection", projection);
	backgroundShader.Bind();
	backgroundShader.SetUniformMat4f("projection", projection);

	// Then before rendering, configure the viewport to the original framebuffer's screen dimensions
	int scrnWidth, scrnHeight;
	glfwGetFramebufferSize(window, &scrnWidth, &scrnHeight);
	glViewport(0, 0, scrnWidth, scrnHeight);

	// Render loop
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		ProcessMovement(window);

		// render
		renderer.Clear(DARK_GREY, COLOR_DEPTH);

		// render scene, supplying the convoluted irradiance map to the final shader
		pbrShader.Bind();
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = camera.GetViewMatrix();
		pbrShader.SetUniformMat4f("view", view);
		pbrShader.SetUniformVec3("camPos", camera.GetPosition());

		// bind pre-computed IBL data
		irradianceMap.Bind(0);
		prefilterMap.Bind(1);
		brdfLUTTexture.Bind(2);

		// Rusted iron
		Iron_Albedo.Bind(3);
		Iron_NormaMap.Bind(4);
		Iron_MetallicMap.Bind(5);
		Iron_RoughnessMap.Bind(6);
		Iron_AOMap.Bind(7);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-5.0f, 0.0f, 2.0f));
		pbrShader.SetUniformMat4f("model", model);
		pbrShader.SetUniformMat3f("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
		sphere.Draw(pbrShader, renderer);

		// gold
		Gold_Albedo.Bind(3);
		Gold_NormaMap.Bind(4);
		Gold_MetallicMap.Bind(5);
		Gold_RoughnessMap.Bind(6);
		Gold_AOMap.Bind(7);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-3.0f, 0.0f, 2.0f));
		pbrShader.SetUniformMat4f("model", model);
		pbrShader.SetUniformMat3f("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
		sphere.Draw(pbrShader, renderer);

		// grass
		Grass_Albedo.Bind(3);
		Grass_NormaMap.Bind(4);
		Grass_MetallicMap.Bind(5);
		Grass_RoughnessMap.Bind(6);
		Grass_AOMap.Bind(7);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 2.0f));
		pbrShader.SetUniformMat4f("model", model);
		pbrShader.SetUniformMat3f("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
		sphere.Draw(pbrShader, renderer);

		// plastic
		Plastic_Albedo.Bind(3);
		Plastic_NormaMap.Bind(4);
		Plastic_MetallicMap.Bind(5);
		Plastic_RoughnessMap.Bind(6);
		Plastic_AOMap.Bind(7);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(1.0f, 0.0f, 2.0f));
		pbrShader.SetUniformMat4f("model", model);
		pbrShader.SetUniformMat3f("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
		sphere.Draw(pbrShader, renderer);

		// wall
		Wall_Albedo.Bind(3);
		Wall_NormaMap.Bind(4);
		Wall_MetallicMap.Bind(5);
		Wall_RoughnessMap.Bind(6);
		Wall_AOMap.Bind(7);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(3.0f, 0.0f, 2.0f));
		pbrShader.SetUniformMat4f("model", model);
		pbrShader.SetUniformMat3f("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
		sphere.Draw(pbrShader, renderer);

		Wall_Albedo.Unbind();

		// render light source (simply re-render sphere at light positions)
		for (unsigned int i = 0; i < lightPositions.size(); ++i)
		{
			glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0f), 0.0f, 0.0f);
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
		// render skybox (render as last to prevent overdraw
		backgroundShader.Bind();

		backgroundShader.SetUniformMat4f("view", view);
		envCubemap.Bind();
		cube.Draw(backgroundShader, renderer);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
}
