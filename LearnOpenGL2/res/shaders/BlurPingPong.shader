#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	TexCoords = aTexCoord;

	gl_Position = projection * view * model * vec4(aPos, 1.0);
}

#shader fragment
#version 330 core

in vec2 TexCoords;

uniform sampler2D image;
uniform bool herizontal;

void main()
{
	vec2 tex_offset = 1.0 / textureSize(image, 0);
	vec3 result = vec3(0.0);

	flat weight[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

	for(int i = -4; i <= 4; i++)
	{
		vec2 offset = vec2(horizonal ? i : 0, horizonal ? 0 : i) * tex_offset;
		result += texture(image, TexCoords + offset).rgb * weight[abs(i)];
	}
	FragColor = vec4(result, 1.0);
}