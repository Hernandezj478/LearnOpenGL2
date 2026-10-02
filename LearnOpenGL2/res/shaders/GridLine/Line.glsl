#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 fragWorldPos;

void main()
{
	vec4 worldPos = model * vec4(aPos, 1.0);
	fragWorldPos = worldPos.xyz;

	gl_Position = projection * view * worldPos;
}

#shader fragment
#version 330 core

in vec3 fragWorldPos;

out vec4 FragColor;

uniform vec3 cameraPos;
uniform float maxDistance;
uniform float minDistance;

void main()
{
	float distance = length(fragWorldPos - cameraPos);
	float max = maxDistance;
	float min = minDistance;

	float fade = clamp(1.0 - ((distance - min) / (max - min)), 0.0, 1.0);
	vec3 gridColor = vec3(0.3);

	float epsilon = 0.001;
	if(abs(fragWorldPos.z) < epsilon)
	{
		gridColor = vec3(1.0, 0.0, 0.0);
	}
	else if(abs(fragWorldPos.x) < epsilon)
	{
		gridColor = vec3(0.0, 1.0, 0.0);
	}
	if(abs(fragWorldPos.x) < fade || abs(fragWorldPos.y) < fade)
	{
		FragColor = vec4(gridColor, 1.0);
	}
}