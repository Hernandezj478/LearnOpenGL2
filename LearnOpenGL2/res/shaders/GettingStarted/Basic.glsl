#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	TexCoord = aTexCoord;
}

#shader fragment
#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform bool isCube;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;


void main()
{
	if(isCube)
	{
		FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.1);
	}
	else
	{
		FragColor = texture(texture3, TexCoord);
	}
}