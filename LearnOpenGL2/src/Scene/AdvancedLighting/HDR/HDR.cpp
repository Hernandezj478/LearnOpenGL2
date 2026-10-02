#include "HDR.h"

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


HDR::HDR(int width, int height)
{
	camera.SetScreenSize(width, height);
	camera.SetCursorPos();	// Must only be called after setting screen size
	camera.SetAspectRatio();
}

void HDR::Run(GLFWwindow* window) 
{
	// Initialize ImGUI
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui::StyleColorsDark();
	ImGui_ImplOpenGL3_Init("#version 150");


	glEnable(GL_DEPTH_TEST);
	
	Plane2D screenQuad;
	Cube cube(true, true, true);
	Shader shader("res/shaders/BasicLighting.shader");
	Shader hdrShader("res/shaders/HDR.shader");

	Texture woodTexture("res/textures/wood.png", true, false, ECLAMP, true);

	// Configure floating point framebuffer
	FrameBuffer hdrFBO;
	// Create floating point color buffer
	Texture colorBuffer;
	colorBuffer.CreateColorbufferHDR(camera.GetScreenWidth(), camera.GetScreenHeight(), GL_RGBA16F);

	// Create Depthbuffer
	RenderBuffer rboDepth;
	rboDepth.Bind();
	rboDepth.CreateStorage(GL_DEPTH_COMPONENT, camera.GetScreenWidth(), camera.GetScreenHeight());

	// Attach buffers
	hdrFBO.Bind();
	hdrFBO.AttachColorBuffer(colorBuffer);
	hdrFBO.AttachRenderBuffer(rboDepth, GL_DEPTH_COMPONENT);
	hdrFBO.FrameBufferComplete();

	//Lighting info
	std::vector<glm::vec3> lightPositions =
	{
		glm::vec3( 0.0f,  0.0f, -49.5f),
		glm::vec3(-1.4f, -1.9f, -9.0f),
		glm::vec3( 0.0f, -1.8f, -4.0f),
		glm::vec3( 0.8f, -1.7f, -6.0f)
	};

	std::vector<glm::vec3> lightColors =
	{
		glm::vec3(200.0f, 200.0f, 200.0f),
		glm::vec3(0.1f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 0.2f),
		glm::vec3(0.0f, 0.1f, 0.0f)
	};

	// Shader config
	shader.Bind();
	shader.SetUniform1i("diffuseTexture", 0);
	hdrShader.Bind();
	hdrShader.SetUniform1i("hdrBuffer", 0);


	while (!glfwWindowShouldClose(window))
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		Renderer renderer;
		ProcessMovement(window);

		hdrFBO.Bind();
		renderer.Clear(DARK_GREY, COLOR_DEPTH);

		// Render Here
		glm::mat4 projection = glm::perspective(glm::radians(camera.GetFOV()), camera.GetAspectRatio(), 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 model = glm::mat4(1.0);
		
		// Render  scene into floating point framebuffer
		renderer.ClearBufferBits(COLOR_DEPTH);
		shader.Bind();
		shader.SetUniformMat4f("projection", projection);
		shader.SetUniformMat4f("view", view);
		woodTexture.Bind();
		for (unsigned int i = 0; i < lightPositions.size(); ++i)
		{
			shader.SetUniformVec3("lights[" + std::to_string(i) + "].Position", lightPositions[i]);
			shader.SetUniformVec3("lights[" + std::to_string(i) + "].Color", lightColors[i]);
		}
		shader.SetUniformVec3("viewPos", camera.GetPosition());
		// Render tunnel
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0, 0.0, -25.0));
		model = glm::scale(model, glm::vec3(2.5f, 2.5f, -28.5f));
		shader.SetUniformMat4f("model", model);
		shader.SetUniform1i("inverse_normals", true);
		cube.Draw(shader, renderer);
		hdrFBO.Unbind();

		// Render floating point color buffer to 2D plane and tonemap HDR colors to default framebuffer's color range
		renderer.ClearBufferBits(COLOR_DEPTH);
		hdrShader.Bind();
		colorBuffer.Bind();
		hdrShader.SetUniform1i("hdr", hdr);
		hdrShader.SetUniform1f("exposure", exposure);
		screenQuad.Draw(hdrShader, renderer);

		// ImGui window
		{
			std::string title = "HDR: ";
			title += (hdr ? "on" : "off");
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
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
}

void HDR::ProcessInput(GLFWwindow* window, int key, int action)
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
					if (!hdrKeyPressed)
					{
						hdr = !hdr;
						hdrKeyPressed = true;
					}
					break;
				}
				case GLFW_RELEASE:
				{
					hdrKeyPressed = false;
					break;
				}
				default:
					// No Action
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
					if (exposure > 0.0f)
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
				default: break;
			}
			break;
		}
		default:
			// No Action
			break;
	}
}

void HDR::AdjustScreenSize(int width, int height)
{
	Scene::AdjustScreenSize(width, height);
	// We need to readjust our viewport, colorbuffer, and depthbuffer to resize the screen quad properly
	glViewport(0, 0, width, height);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
}

