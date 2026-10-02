#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoords;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
	TexCoords = aTexCoord;
	gl_Position = projection * view * model * vec4(aPos, 1.0);
}


#shader fragment
#version 330 core

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D texture1;
uniform bool bDepthTest;
uniform float near;
uniform float far;
uniform float UV;

float LinearizeDepth(float depth)
{
	float z = depth * 2.0 - 1.0;
	return (2.0 * near * far) / (far + near - z * (far - near));
}

void main()
{
	if(bDepthTest)
	{
		float depth = LinearizeDepth(gl_FragCoord.z) / far;
		FragColor = vec4(vec3(depth), 1.0);
	}
	else
	{
		FragColor = texture(texture1, TexCoords * UV);
	}
}