#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 5) in mat4 instanceMatrix;

uniform mat4 view;
uniform mat4 projection;

out vec3 fColor;

vec3 colors[4] = vec3[](
	vec3(1.0, 0.0, 0.0),
	vec3(0.0, 1.0, 0.0),
	vec3(0.0, 0.0, 1.0),
	vec3(0.0, 1.0, 1.0)
);

void main()
{
	fColor = colors[gl_VertexID % 4];
	vec3 pos = aPos * (gl_InstanceID / 100.0);
	gl_Position = projection * view * instanceMatrix * vec4(pos, 1.0);
}

#shader fragment
#version 330 core
out vec4 FragColor;
in vec3 fColor;

void main()
{
	FragColor = vec4(fColor, 1.0);
}
