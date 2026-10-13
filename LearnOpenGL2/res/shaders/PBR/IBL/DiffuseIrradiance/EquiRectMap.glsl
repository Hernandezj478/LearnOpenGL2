#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 LocalPos;

uniform mat4 projection;
uniform mat4 view;

void main()
{
	LocalPos = aPos;
	gl_Position = projection * view * vec4(LocalPos, 1.0);
}

#shader fragment
#version 330 core
out vec4 FragColor;
in vec3 LocalPos;

uniform sampler2D equirectMap;

const vec2 invATan = vec2(0.1591, 0.3183);
vec2 SampleSphericalMap(vec3 v)
{
	vec2 UV = vec2(atan(v.z, v.x), asin(v.y));
	UV *= invATan;
	UV += 0.5;
	return UV;
}

void main()
{
	vec2 UV = SampleSphericalMap(normalize(LocalPos));
	vec3 color = vec3(texture(equirectMap, UV));

	FragColor = vec4(color, 1.0);
}