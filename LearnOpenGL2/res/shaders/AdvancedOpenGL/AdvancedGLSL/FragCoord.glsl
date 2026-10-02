#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;

layout (std140) uniform Matrices
{
	mat4 projection;
	mat4 view;
};

uniform mat4 model;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	gl_PointSize = gl_Position.z * 4;
}

#shader fragment
#version 330 core

out vec4 FragColor;
uniform int windowWidth;
void main()
{
	if(gl_FragCoord.x < windowWidth)
	{
		FragColor = vec4(1.0, 0.0, 0.0, 1.0);
	}
	else
	{
		FragColor = vec4(0.0, 1.0, 0.0, 1.0);
	}
}