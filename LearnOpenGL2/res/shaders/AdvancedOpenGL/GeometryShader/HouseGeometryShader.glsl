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
layout (triangle_strip, max_vertices = 5) out;

out vec3 fColor;
vec3 colors[4] = vec3[](
	vec3(1.0, 1.0, 0.0),
	vec3(0.0, 0.0, 1.0),
	vec3(0.0, 1.0, 0.0),
	vec3(1.0, 0.0, 0.0)
);

void build_house(vec4 position)
{
	gl_Position = position + vec4(-0.2, -0.2, 0.0, 0.0);
	EmitVertex();
	gl_Position = position + vec4( 0.2, -0.2, 0.0, 0.0);
	EmitVertex();
	gl_Position = position + vec4(-0.2,  0.2, 0.0, 0.0);
	EmitVertex();
	gl_Position = position + vec4( 0.2,  0.2, 0.0, 0.0);
	EmitVertex();
	gl_Position = position + vec4( 0.0,  0.4, 0.0, 0.0);
	fColor = vec3(1.0, 1.0, 1.0);
	EmitVertex();
	EndPrimitive();
}

void main()
{
	fColor = colors[gl_PrimitiveIDIn % 4];
	build_house(gl_in[0].gl_Position);
}

#shader fragment
#version 330 core
out vec4 FragColor;
in vec3 fColor;
void main()
{
	FragColor = vec4(fColor, 1.0);
}