#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);
}

#shader fragment
#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

uniform float brightness;
uniform vec3 lightColor;

void main()
{
	FragColor = vec4(lightColor, 1.0);
	float luminance = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	if(luminance > 1.0)
	{
		BrightColor = vec4(FragColor.rgb, 1.0);
	}
	else
	{
		BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
	}
}