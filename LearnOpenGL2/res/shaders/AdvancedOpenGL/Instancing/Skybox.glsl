#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

void main()
{
	TexCoords = aTexCoord;
	vec4 pos = projection * view * model * vec4(aPos, 1.0);
	gl_Position = pos.xyww;
}

#shader fragment
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D skybox;

void main()
{
	FragColor = texture(skybox, TexCoords);
}