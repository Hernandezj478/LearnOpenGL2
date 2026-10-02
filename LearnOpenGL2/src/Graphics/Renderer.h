#pragma once
#include <glm/glm.hpp>
#include <glad/glad.h>

class VertexArray;
class IndexBuffer;
class Shader;

#define CLEAR_ALL_BUFFERS	(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT)
#define CLEAR_COLOR_DEPTH	(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
#define CLEAR_COLOR_STENCIL (GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT)
#define CLEAR_DEPTH_STENCIL (GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT)
#define CLEAR_COLOR			(GL_COLOR_BUFFER_BIT)
#define CLEAR_DEPTH			(GL_DEPTH_BUFFER_BIT)
#define CLEAR_STENCIL		(GL_STENCIL_BUFFER_BIT)

enum ClearBuffer
{
	ALL				= CLEAR_ALL_BUFFERS,
	COLOR_DEPTH		= CLEAR_COLOR_DEPTH,
	COLOR_STENCIL	= CLEAR_COLOR_STENCIL,
	DEPTH_STENCIL	= CLEAR_DEPTH_STENCIL,
	COLOR			= CLEAR_COLOR,
	DEPTH			= CLEAR_DEPTH,
	STENCIL			= CLEAR_STENCIL
};

class Renderer {
public:
	void Clear(glm::vec4 cColor, ClearBuffer clearState = COLOR) const;
	void ClearBufferBits(ClearBuffer clearState);
	/*
	*! @brief Draw object to the screen
	* @param in vertexArray - VAO of object
	* @param in indexBuffer - VBO of object
	* @param in shader - shader to be used to render object to screen
	* @param OptionalParam instanceAmount - number of instances to draw
	* @param OptionalParam mode - GL draw call type e.g. GL_TRIAGLES 
	*/
	void Draw(const VertexArray& vertexArray, const IndexBuffer& indexBuffer, const Shader& shader, 
				const unsigned int instanceAmount = 1, GLenum mode = GL_TRIANGLES) const;
	void DrawLine(const VertexArray& vertexArray, const Shader& shader, const int& vertexCount) const;
	void DrawPoint(const VertexArray& vertexArray, const Shader& shader, const int& vertexCount) const;
};