#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;
out vec3 Normal;


void main()
{
	FragPos = vec3(model * vec4(aPos, 1.0));
	Normal = mat3(transpose(inverse(model))) * aNormal;
	gl_Position = projection * view * model * vec4(aPos, 1.0);
}

#shader geometry
#version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float normalLength;

in vec3 Normal[];
in vec3 FragPos[];

void main()
{
	for(int i = 0; i < 3; i++)
	{
		vec3 p = FragPos[i];
		vec3 n = normalize(Normal[i]);

		gl_Position = projection * view * vec4(p, 1.0);
		EmitVertex();
		gl_Position = projection * view * vec4(p + n * normalLength, 1.0);
		EmitVertex();
		EndPrimitive();

		n = normalize(-Normal[i]);
		gl_Position = projection * view * vec4(p, 1.0);
		EmitVertex();
		gl_Position = projection * view * vec4(p + n * 1.0, 1.0);
		EmitVertex();
		EndPrimitive();
	}
}

#shader fragment
#version 330 core

in vec3 Normal;
out vec4 FragColor;

void main()
{
	//FragColor = vec4(normalize(Normal) * 0.5 + 0.5, 1.0);
	FragColor = vec4(1.0, 1.0, 0.0, 1.0);
}