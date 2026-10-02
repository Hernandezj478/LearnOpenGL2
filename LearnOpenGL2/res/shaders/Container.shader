#shader vertex
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;


out vec2 TexCoords;
out vec3 Normal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	TexCoords = aTexCoord;
	Normal = aNormal;
}


#shader fragment
#version 330 core

in vec2 TexCoords;
in vec3 Normal;

out vec4 FragColor;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

void main()
{
	vec3 result = texture(texture_diffuse1, TexCoords).rgb;
	result += texture(texture_specular1, TexCoords).rgb;
	
	FragColor = vec4(result, 1.0);
}