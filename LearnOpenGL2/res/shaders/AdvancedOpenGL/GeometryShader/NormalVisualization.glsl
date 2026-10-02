#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec3 aNormal;

layout (std140) uniform MVP
{
	mat4 projection;
	mat4 view;
};

out VS_OUT
{
	vec3 normal;
} vs_out;

uniform mat4 model;

out mat4 proj;

void main()
{
	gl_Position = view * model * vec4(aPos, 1.0);
	mat3 normalMatirx = mat3(transpose(inverse(view * model)));
	vs_out.normal = normalize(vec3(vec4(normalMatirx * aNormal, 0.0)));
	proj = projection;
}

#shader geometry
#version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

layout (std140) uniform MVP
{
	mat4 projection;
	mat4 view;
};

in VS_OUT
{
	vec3 normal;
} gs_in[];


const float MAGNITUDE = 0.2;

void GenerateLine(int index)
{
	gl_Position = projection * gl_in[index].gl_Position;
	EmitVertex();
	gl_Position = projection * (gl_in[index].gl_Position + vec4(gs_in[index].normal, 0.0) * MAGNITUDE);
	EmitVertex();
	EndPrimitive();
}

void main()
{
	GenerateLine(0);
	GenerateLine(1);
	GenerateLine(2);
}

#shader fragment
#version 330 core
out vec4 FragColor;

void main()
{
	FragColor = vec4(1.0, 1.0, 0.0, 1.0);
}