#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;

layout (std140) uniform MVP
{
	mat4 projection;
	mat4 view;
};

uniform mat4 model;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);
}

#shader geometry
#version 330 core
layout (points) in;
layout (points, max_vertices = 1) out;

void main()
{
	gl_Position = gl_in[0].gl_Position;
	EmitVertex();
	EndPrimitive();
}

#shader fragment
#version 330 core
out vec4 FragColor;

void main()
{
	FragColor = vec4(0.0, 1.0, 0.0, 1.0);
}