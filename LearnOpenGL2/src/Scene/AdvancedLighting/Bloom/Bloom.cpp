#include "Bloom.h"


#include "../ColorPalette.h"
#include "../Renderer.h"
#include "../Texture.h"
#include "../FrameBuffer.h"
#include "../Plane.h"
#include "../Shader.h"
#include "../Cube.h"

// ImGUI includes
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"



Bloom::Bloom(int screenWidth, int screenHeight)
{
	camera.SetScreenSize(screenWidth, screenHeight);
	camera.SetCursorPos();
	camera.SetAspectRatio();
}

void Bloom::Run(GLFWwindow* window)
{
	// Initialize ImGUI
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui::StyleColorsDark();
	ImGui_ImplOpenGL3_Init("#version 150");


	unsigned int ColorbufferCount = 2;

	glEnable(GL_DEPTH_TEST);
	Plane ground;
	Plane2D screenPlane;
	Cube cube(true, true, true);

	Shader shader("res/shaders/BetterBloom.shader");
	Shader shaderLight("res/shaders/Lightbox.shader");
	Shader shaderBlur("res/shaders/BlurBloom.shader");
	Shader shaderBloomFinal("res/shaders/BloomFinal.shader");

	Texture woodTexture("res/textures/wood.png", true, false, REPEAT, true);
	Texture containerTexture("res/textures/container2.png", true, false, REPEAT, true);

	FrameBuffer hdrFBO;
	Texture colorBuffers(ColorbufferCount);
	hdrFBO.Bind();
	colorBuffers.SetMultiAttachment(true);
	colorBuffers.CreateColorbufferHDR(camera.GetScreenWidth(), camera.GetScreenHeight(), GL_RGBA16F);
	hdrFBO.AttachColorBuffer(colorBuffers);

	RenderBuffer rboDepth;
	rboDepth.Bind();
	rboDepth.CreateStorage(GL_DEPTH_COMPONENT, camera.GetScreenWidth(), camera.GetScreenHeight());
	hdrFBO.AttachRenderBuffer(rboDepth, GL_DEPTH_ATTACHMENT);

	unsigned int* attachments = new unsigned int[2];
	attachments[0] = GL_COLOR_ATTACHMENT0;
	attachments[1] = GL_COLOR_ATTACHMENT1;
	hdrFBO.ConfigureAttachments(attachments, 2);
	hdrFBO.FrameBufferComplete();
	hdrFBO.Unbind();

	// ping-pong framebuffer for blurring
	FrameBuffer pingpongFBO(2);
	Texture pingpongColorbuffer(2);
	pingpongColorbuffer.SetMultiAttachment(false);
	pingpongColorbuffer.CreateColorbufferHDR(camera.GetScreenWidth(), camera.GetScreenHeight(), GL_RGBA16F);
	pingpongFBO.AttachColorBuffer(pingpongColorbuffer);
	pingpongFBO.FrameBufferComplete();
	pingpongFBO.Unbind();

	// Lighting info
	//						Position  | Color
	std::vector<std::pair<glm::vec3, glm::vec3>> lightData =
	{
		std::pair<glm::vec3, glm::vec3>(glm::vec3( 0.0f, 0.5f,  1.5f), glm::vec3( 5.0f, 5.0f,  5.0f)),
		std::pair<glm::vec3, glm::vec3>(glm::vec3(-4.0f, 0.5f, -3.0f), glm::vec3(10.0f, 0.0f,  0.0f)),
		std::pair<glm::vec3, glm::vec3>(glm::vec3( 3.0f, 0.5f,  1.0f), glm::vec3( 0.0f, 0.0f, 15.0f)),
		std::pair<glm::vec3, glm::vec3>(glm::vec3(-0.8f, 2.4f, -1.0f), glm::vec3( 0.0f, 5.0f,  0.0f)),
	};

	// Cube positions
	//					Position | Scale
	std::vector<std::pair<glm::vec3, glm::vec3>> cubeData =
	{
		std::pair<glm::vec3, glm::vec3>( glm::vec3(0.0f,  1.5f,  0.0f), glm::vec3(0.5f)),
		std::pair<glm::vec3, glm::vec3>(glm::vec3( 2.0f,  0.0f,  1.0f), glm::vec3(0.5f)),
		std::pair<glm::vec3, glm::vec3>(glm::vec3(-1.0f, -1.0f,  2.0f), glm::vec3(1.0f)),
		std::pair<glm::vec3, glm::vec3>(glm::vec3( 0.0f,  2.7f,  4.0f), glm::vec3(1.25f)),
		std::pair<glm::vec3, glm::vec3>(glm::vec3(-2.0f,  1.0f, -3.0f), glm::vec3(1.0f)),
		std::pair<glm::vec3, glm::vec3>(glm::vec3(-3.0f,  0.0f,  0.0f), glm::vec3(0.5f))
	};

	shader.Bind();
	shader.SetUniform1i("diffuseTexture", 0);
	shaderBlur.Bind();
	shaderBlur.SetUniform1i("image", 0);
	shaderBloomFinal.Bind();
	shaderBloomFinal.SetUniform1i("scene", 0);
	shaderBloomFinal.SetUniform1i("bloomBlur", 1);

	while (!glfwWindowShouldClose(window))
	{
		// Setup ImGUI frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();


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

		// 1. Render scene into float point framebuffer
		hdrFBO.Bind();
		renderer.ClearBufferBits(COLOR_DEPTH);
		shader.Bind();
		shader.SetUniformMat4f("projection", projection);
		shader.SetUniformMat4f("view", view);
		woodTexture.Bind(0);
		for (int i = 0; i < lightData.size(); i++)
		{
			shader.SetUniformVec3("lights[" + std::to_string(i) + "].Position", lightData[i].first);
			shader.SetUniformVec3("lights[" + std::to_string(i) + "].Color", lightData[i].second);
		}

		shader.SetUniformVec3("viewPos", camera.GetPosition());
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(12.5f));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
		shader.SetUniformMat4f("model", model);
		ground.Draw(shader, renderer);
		for (int i = 0; i < cubeData.size(); ++i)
		{
			model = glm::mat4(1.0);
			model = glm::translate(model, cubeData[i].first);
			model = glm::scale(model, cubeData[i].second);
			shader.SetUniformMat4f("model", model);
			containerTexture.Bind();
			cube.Draw(shader, renderer);
		}

		shaderLight.Bind();
		shaderLight.SetUniformMat4f("projection", projection);
		shaderLight.SetUniformMat4f("view", view);
		for (int i = 0; i < lightData.size(); ++i)
		{
			model = glm::mat4(1.0);
			model = glm::translate(model, lightData[i].first);
			model = glm::scale(model, glm::vec3(0.25f));
			shaderLight.SetUniformMat4f("model", model);
			shaderLight.SetUniformVec3("lightColor", lightData[i].second);
			cube.Draw(shaderLight, renderer);

		}
		hdrFBO.Unbind();

		// 2. blur bright fragments
		bool horizontal = true, firstIteration = true;
		unsigned int amount = 10;
		shaderBlur.Bind();
		for (unsigned int i = 0; i < amount; ++i)
		{
			unsigned int targetFBO = horizontal ? 0 : 1;
			pingpongFBO.Bind(targetFBO);
			shaderBlur.SetUniform1i("horizontal", horizontal);
			if (firstIteration)
			{
				colorBuffers.Bind(0, 1);
			}
			else
			{
				unsigned int sampleIndex = horizontal ? 1 : 0;
				pingpongColorbuffer.Bind(0, sampleIndex);
			}
			screenPlane.Draw(shaderBlur, renderer);
			horizontal = !horizontal;
			if (firstIteration)
			{
				firstIteration = false;
			}
		}
		pingpongFBO.Unbind();

		// 3. Render floating point color buffer to 2D quad and tonemap HDR colors to default framebuffer's color range
		renderer.ClearBufferBits(COLOR_DEPTH);
		shaderBloomFinal.Bind();
		colorBuffers.Bind(0, 0);
		pingpongColorbuffer.Bind(1, !horizontal);
		shaderBloomFinal.SetUniform1i("bloom", bloom);
		shaderBloomFinal.SetUniform1f("exposure", exposure);
		screenPlane.Draw(shaderBloomFinal, renderer);

		// ImGui window
		{
			std::string title = "Bloom: ";
			title += (bloom ? "on" : "off");
			title += "| Exposure: ";
			title += std::to_string(exposure);
			ImGui::Begin("Info Panel");
			ImGui::Text(title.c_str());
			ImGui::End();
		}

		//ImGui 
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


		// Check and call events and swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	delete[] attachments;
	glfwTerminate();
}

void Bloom::ProcessInput(GLFWwindow* window, int key, int action)
{
	Scene::ProcessInput(window, key, action);
	switch (key)
	{
		case GLFW_KEY_SPACE:
		{
			switch (action)
			{
				case GLFW_PRESS:
				{
					if (!bloomKeyPressed) 
					{
						bloom = !bloom;
						bloomKeyPressed = true;
					}
					break;
				}
				case GLFW_RELEASE:
				{
					bloomKeyPressed = false;
					break;
				}
				default:
					// No action
					break;
			}
			break;
		}
		case GLFW_KEY_1:
		{
			switch (action)
			{
				case GLFW_REPEAT:
				{
					if (exposure > 0.0)
					{
						exposure -= 0.01f;
					}
					else
					{
						exposure = 0.0f;
					}
					break;
				}
				default:
					// No action
					break;
			}
			break;
		}
		case GLFW_KEY_2:
		{
			switch (action)
			{
				case GLFW_REPEAT:
				{
					exposure += 0.01f;
					break;
				}
				default:
					// No action
					break;
			}
			break;
		}
		case GLFW_KEY_3:
		{
			switch (action)
			{
				case GLFW_PRESS:
				{
					exposure = 1.0f;
					break;
				}
				default:
					// No action
					break;
			}
			break;
		}
		default:
		{
			// Non-mapped keybind
			break;
		}
	}
}

void Bloom::AdjustScreenSize(int width, int height)
{
	Scene::AdjustScreenSize(width, height);
	// We need to readjust our viewport, colorbuffer, and depthbuffer to resize the screen quad properly
	glViewport(0, 0, width, height);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
}

void Bloom::CheckFramebufferStatus(const char* name)
{
	GLint objectType, objectName;

	std::cout << "\n--- Checking framebuffer: " << name << " ---\n";
	for (int i = 0; i < 2; ++i) // check COLOR_ATTACHMENT0 + 0,1
	{
		glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER,
			GL_COLOR_ATTACHMENT0 + i,
			GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE,
			&objectType);

		if (objectType == GL_NONE)
		{
			std::cout << "Attachment " << i << ": NONE\n";
			continue;
		}

		glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER,
			GL_COLOR_ATTACHMENT0 + i,
			GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME,
			&objectName);

		std::string typeStr = (objectType == GL_TEXTURE) ? "Texture" :
			(objectType == GL_RENDERBUFFER) ? "Renderbuffer" : "Unknown";
		std::cout << "Attachment " << i << ": " << typeStr
			<< " ID = " << objectName << std::endl;
	}

	GLint depthType, depthName;
	glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER,
		GL_DEPTH_ATTACHMENT,
		GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE,
		&depthType);

	if (depthType != GL_NONE)
	{
		glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER,
			GL_DEPTH_ATTACHMENT,
			GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME,
			&depthName);
		std::cout << "Depth attachment: ID = " << depthName << std::endl;
	}
}

void Bloom::CheckStatus()
{
	GLint drawBuffers[8];
	GLint maxDrawBuffers;
	glGetIntegerv(GL_MAX_DRAW_BUFFERS, &maxDrawBuffers);
	glGetIntegerv(GL_DRAW_BUFFER0, drawBuffers);

	std::cout << "Max draw buffers: " << maxDrawBuffers << std::endl;
	for (int i = 0; i < 2; ++i)
	{
		GLint buf;
		glGetIntegerv(GL_DRAW_BUFFER0 + i, &buf);
		std::cout << "Draw buffer " << i << " bound to: "
			<< ((buf == GL_NONE) ? "NONE" : std::to_string(buf)) << std::endl;
	}
}
