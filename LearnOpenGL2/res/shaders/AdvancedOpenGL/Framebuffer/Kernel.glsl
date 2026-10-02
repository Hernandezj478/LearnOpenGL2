#shader vertex
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoords;

void main()
{
	gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
	TexCoords = aTexCoord;
}

#shader fragment
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform float textureOffset;
uniform float kernel[9];

void main()
{
	float offset = textureOffset;
	vec2 offsets[9] = vec2[](
		vec2(-offset,  offset),
		vec2( 0.0f,    offset),
		vec2( offset,  offset),
		vec2(-offset,  0.0f),
		vec2( 0.0f,    0.0f),
		vec2( offset,  0.0f),
		vec2(-offset, -offset),
		vec2( 0.0f,   -offset),
		vec2( offset, -offset)
	);

	vec3 sampleTex[9];
	for(int i = 0; i < 9; i++)
	{
		sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
	}
	vec3 color = vec3(0.0);
	for(int i = 0; i < 9; i++)
	{
		color += sampleTex[i] * kernel[i];
	}
	FragColor = vec4(color, 1.0);
}