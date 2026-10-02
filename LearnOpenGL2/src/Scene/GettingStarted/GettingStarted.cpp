#include "GettingStarted.h"
#include "stb_image.h"

#include "Core/Window.h"

#include "Graphics/Shader.h"
#include "Graphics/Vertex.h"
#include "Graphics/Renderer.h"
#include "Graphics/Texture.h"
#include "Graphics/Buffers/VertexBufferLayout.h"
#include "Graphics/Buffers/VertexArray.h"
#include "Graphics/Buffers/VertexBuffer.h"
#include "Graphics/Buffers/IndexBuffer.h"
#include "Graphics/Shapes/Cube.h"
#include "Graphics/Shapes/Plane.h"
#include "Graphics/Shapes/Grid.h"

#include "Scene/SceneContext.h"

#include <time.h>
#include <vector>

Startup::Startup(const SceneContext& context) : 
	Scene3D(context)
{
	srand((unsigned int)time(0));
	
	m_Shader = std::make_unique<Shader>("res/shaders/GettingStarted/Basic.glsl");
	m_GridShader = std::make_unique<Shader>("res/shaders/GridLine/Line.glsl");
	m_Texture1 = std::make_unique<Texture>("res/textures/container.jpg");
	m_Texture2 = std::make_unique<Texture>("res/textures/awesomeface.png");
	m_Texture3 = std::make_unique<Texture>("res/textures/grass.jpg");
	m_Cube = std::make_unique<Cube>();
	m_Plane = std::make_unique<Plane>();
	m_Grid = std::make_unique<Grid>(1000);

	m_Texture1->LoadPixels();
	m_Texture2->LoadPixels();
	m_Texture3->LoadPixels();

	m_Texture1->Upload();
	m_Texture2->Upload();
	m_Texture3->Upload();


	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Cube
#pragma region Cube
	float cubeCenterRadius = 0.5f;
	float offset = 0.0f;
	for (int i = 0; i < 10; i++)
	{
		offset += cubeCenterRadius;
		cubePositions[i] = glm::vec3(offset, cubeCenterRadius, i < 5 ? -0.5f : -1.5f);
		offset += cubeCenterRadius;
		//cubePositions[i] = glm::vec3(rand() % 20 - 10, cubeCenterRadius, rand() % 20 - 10);
		if (i == 4)
		{
			offset = 0.0f;
		}
	}
#pragma endregion
	// Activate Shader
	m_Shader->Bind();
	m_Shader->SetUniform1i("texture1", 0);
	m_Shader->SetUniform1i("texture2", 1);
	m_Shader->SetUniform1i("texture3", 2);


	// random axis rotation for each cube (numCubes / 3) + 1
	
	const int numCubes = sizeof(cubePositions) / (3 * sizeof(float));
	const int cubesToRotate = ((sizeof(cubePositions) / (3 * sizeof(float))) / frequency) + (frequency % 2 == 1 ? 1 : 0);
	cubeRotations = new glm::vec3[cubesToRotate];
	for (int i = 0; i < cubesToRotate; i++)
	{
		cubeRotations[i].x = (rand() % 101) * 0.01f;
		cubeRotations[i].y = (rand() % 101) * 0.01f;
		cubeRotations[i].z = (rand() % 101) * 0.01f;
	}
}

Startup::~Startup() = default;

void Startup::Render()
{
		float rotationSpeed = 1.0f;

		// Render Here
		m_Context.Renderer.Clear(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), ALL);

		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = m_Camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(m_Camera.GetFOV()), m_Camera.GetAspectRatio(), .1f, 1000.0f);

		// Draw cubes
		m_Shader->Bind();
		m_Shader->SetUniformMat4f("view", view);
		m_Shader->SetUniformMat4f("projection", projection);
		
		m_GridShader->Bind();
		m_GridShader->SetUniformMat4f("view", view);
		m_GridShader->SetUniformMat4f("projection", projection);
		m_GridShader->Unbind();

		m_Shader->Bind();
		m_Shader->SetUniform1i("isCube", true);
		m_Texture1->Bind(0);
		m_Texture2->Bind(1);

		int cubeIndex = 0;
		for (int i = 0; i < sizeof(cubePositions) / sizeof(glm::vec3); i++)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			if (i % frequency == 0)
			{
				model = glm::rotate(model, (float)glfwGetTime() * rotationSpeed, glm::vec3(0.0f, 1.0f, 0.0f/*cubeRotations[cubeIndex].x, cubeRotations[cubeIndex].y, cubeRotations[cubeIndex].z*/));
				cubeIndex++;
			}
			else {
				float angle = 20.0f * i;
				model = glm::rotate(model, 0.0f/*glm::radians(angle)*/, glm::vec3(1.0f, 0.3f, 0.5f));
			}
			m_Shader->SetUniformMat4f("model", model);
			m_Cube->Draw(*m_Shader, m_Context.Renderer);
		}

		// Draw plane
		m_Shader->Bind();
		m_Shader->SetUniform1i("isCube", false);
		m_Texture3->Bind(2);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
		m_Shader->SetUniformMat4f("model", model);
		
		m_Plane->Draw(*m_Shader, m_Context.Renderer);

		// Draw Grid
		m_GridShader->Bind();
		view = m_Camera.GetViewMatrix();
		projection = glm::perspective(glm::radians(m_Camera.GetFOV()), m_Camera.GetAspectRatio(), .1f, 100.0f);
		model = glm::mat4(1.0f);

		m_GridShader->SetUniformMat4f("view", view);
		m_GridShader->SetUniformMat4f("projection", projection);
		m_GridShader->SetUniformMat4f("model", model);
		m_GridShader->SetUniformVec3("cameraPos", m_Camera.GetPosition());
		m_GridShader->SetUniform1f("maxDistance", 50.0f);
		m_GridShader->SetUniform1f("minDistance", 5.0f);
		m_Grid->Draw(*m_GridShader, m_Context.Renderer);

}