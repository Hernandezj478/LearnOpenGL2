#shader vertex
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

void main()
{
	TexCoord = aTexCoord;
	gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
}

#shader fragment
#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

struct Light
{
	vec3 Position;
	vec3 Color;
};

const int NR_LIGHTS = 32;
uniform Light lights[NR_LIGHTS];
uniform vec3 viewPos;

void main()
{
	vec3 Albedo = vec3(texture(gAlbedoSpec, TexCoord));
	
	vec3 ambient = 0.3 * Albedo;
	vec3 Normal = vec3(texture(gNormal, TexCoord));
	vec3 FragPos = vec3(texture(gPosition, TexCoord));
	float Specular = texture(gAlbedoSpec, TexCoord).a;
	vec3 lighting = vec3(0.0);

	for(int i = 0; i < NR_LIGHTS; i++)
	{
		vec3 lightDir = normalize(lights[i].Position - FragPos);
		float diff = max(dot(lightDir, Normal), 0.0);
		vec3 diffuse = diff * lights[i].Color * Albedo;

		vec3 viewDir = normalize(viewPos - FragPos);
		vec3 halfway = normalize(lightDir + viewDir);
		float spec = pow(max(dot(Normal, halfway), 0.0), 32.0);
		vec3 specular = spec * lights[i].Color * Albedo * Specular;

		float distance = length(lights[i].Position - FragPos);
		float attenuation = 1 / (distance * distance);
		diffuse *= attenuation;
		specular *= attenuation;

		lighting += (diffuse + specular);
	}
	vec3 result = ambient + lighting;
	FragColor = vec4(result, 1.0);
}
