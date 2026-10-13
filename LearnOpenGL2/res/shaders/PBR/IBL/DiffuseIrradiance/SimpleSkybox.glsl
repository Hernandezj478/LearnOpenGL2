#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 projection;
uniform mat4 view;

out vec3 LocalPos;

void main()
{
	LocalPos = aPos;

	mat4 rotView = mat4(mat3(view));
	vec4 clipPos = projection * rotView * vec4(LocalPos, 1.0);
	gl_Position = clipPos.xyww;
}

#shader fragment
#version 330 core
out vec4 FragColor;
in vec3 LocalPos;

uniform samplerCube envMap;

void main()
{
	vec3 envColor = vec3(texture(envMap, LocalPos));
	envColor = envColor / (envColor + vec3(1.0));
	envColor = pow(envColor, vec3(1.0 / 2.2));

	FragColor = vec4(envColor, 1.0);
}