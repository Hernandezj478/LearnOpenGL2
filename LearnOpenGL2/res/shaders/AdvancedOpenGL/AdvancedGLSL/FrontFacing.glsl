#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

layout (std140) uniform Matrices
{
	mat4 projection;
	mat4 view;
};

uniform mat4 model;

out vec2 TexCoords;


void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	gl_PointSize = gl_Position.z * 4;
	TexCoords = aTexCoord;
}

#shader fragment
#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D frontTexture;
uniform sampler2D backTexture;

void main()
{
	if(gl_FrontFacing)
	{
		FragColor = texture(frontTexture, TexCoords);
	}
	else
	{
		FragColor = texture(backTexture, TexCoords);
	}
}