#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out VS_OUT
{
	vec2 TexCoords;
} vs_out;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	vs_out.TexCoords = aTexCoords;
}

#shader fragment
#version 330 core
out vec4 FragColor;

in VS_OUT
{
	vec2 TexCoords;
} fs_in;

uniform sampler2D texture1;

void main()
{
	FragColor = texture(texture1, fs_in.TexCoords);
}