#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);
}

#shader fragment
#version 330 core
out vec4 FragColor;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

struct Light
{
	vec3 Position;
	vec3 Color;
};

uniform Light light;
uniform vec3 viewPos;
uniform vec2 gScreenSize;

void main()
{
	vec3 lighting = vec3(0.0);
	vec2 TexCoord = gl_FragCoord.xy / gScreenSize;

	vec3 Albedo = vec3(texture(gAlbedoSpec, TexCoord));
	vec3 Normal = vec3(texture(gNormal, TexCoord));
	vec3 FragPos = vec3(texture(gPosition, TexCoord));
	float Specular = texture(gAlbedoSpec, TexCoord).a;

	vec3 lightDir = normalize(light.Position - FragPos);

	float diff = max(dot(lightDir, Normal), 0.0);
	vec3 diffuse = diff * light.Color * Albedo;

	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 halfway = normalize(lightDir + viewDir);

	float spec = pow(max(dot(Normal, halfway), 0.0), 32.0);
	vec3 specular = spec * light.Color * Albedo * Specular;

	float distance = length(light.Position - FragPos);
	float attenuation = 1 / (distance * distance);
	
	diffuse *= attenuation;
	specular *= attenuation;

	lighting = diffuse + specular;
	FragColor = vec4(lighting, 1.0);
}
