#include "Renderer.h"

#include "Graphics/Buffers/VertexArray.h"
#include "Graphics/Buffers/IndexBuffer.h"
#include "Graphics/Shader.h"
#include "Graphics/Renderer.h"

#include "Common/Logger.h"

void Renderer::Clear(glm::vec4 cColor, ClearBuffer clearState) const
{
	glClearColor(cColor.x, cColor.y, cColor.z, cColor.w);
	GLCall(glClear(clearState));
}

void Renderer::ClearBufferBits(ClearBuffer clearState)
{
	GLCall(glClear(clearState));
}

void Renderer::Draw(const VertexArray& vertexArray, const IndexBuffer& indexBuffer, const Shader& shader, const unsigned int instanceAmount, GLenum mode) const
{
	shader.Bind();
	vertexArray.Bind();
	indexBuffer.Bind();

	GLCall(glDrawElementsInstanced(mode, indexBuffer.GetCount(), GL_UNSIGNED_INT, 0, instanceAmount));
}

void Renderer::DrawLine(const VertexArray& vertexArray, const Shader& shader, const int& vertexCount) const
{
	shader.Bind();
	vertexArray.Bind();

	GLCall(glDrawArrays(GL_LINES, 0, vertexCount));
}

void Renderer::DrawPoint(const VertexArray& vertexArray, const Shader& shader, const int& vertexCount) const
{
	shader.Bind();
	vertexArray.Bind();

	GLCall(glDrawArrays(GL_POINTS, 0, vertexCount));
}



