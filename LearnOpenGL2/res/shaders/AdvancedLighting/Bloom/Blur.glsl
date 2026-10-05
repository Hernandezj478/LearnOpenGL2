#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

void main()
{
	TexCoord = aTexCoord;
	gl_Position = vec4(aPos, 1.0);
}

#shader fragment
#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D image;

uniform bool horizontal;
uniform float weight[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main()
{
	vec2 tex_offset = 1.0 / textureSize(image, 0);
	vec3 result = vec3(texture(image, TexCoord)) * weight[0];
	if(horizontal)
	{
		for(int i = 1; i < 5; i++)
		{
			result += vec3(texture(image, TexCoord + vec2(tex_offset.x * i, 0.0))) * weight[i];
			result += vec3(texture(image, TexCoord - vec2(tex_offset.x * i, 0.0))) * weight[i];
		}
	}
	else
	{
		for(int i = 1; i < 5; i++)
		{
			result += vec3(texture(image, TexCoord + vec2(0.0, tex_offset.y * i))) * weight[i];
			result += vec3(texture(image, TexCoord - vec2(0.0, tex_offset.y * i))) * weight[i];
		}
	}
	FragColor = vec4(result, 1.0);
}