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

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D ssao;

struct Light
{
	vec3 Position;
	vec3 Color;

	float Linear;
	float Quadratic;
};

uniform Light light;
uniform bool bUseSSAO;
void main()
{
	vec3 FragPos = vec3(texture(gPosition, TexCoord));
	vec3 Normal = vec3(texture(gNormal, TexCoord));
	vec3 Diffuse = vec3(texture(gAlbedoSpec, TexCoord));
	float Specular = texture(gAlbedoSpec, TexCoord).a;
	float AmbientOcclusion = texture(ssao, TexCoord).r;

	vec3 ambient = vec3(0.3 * Diffuse * (bUseSSAO ? AmbientOcclusion : 1.0));
	vec3 lighting = ambient;
	vec3 viewDir = normalize(-FragPos);

	vec3 lightDir = normalize(light.Position - FragPos);

	float diff = max(dot(lightDir, Normal), 0.0);
	vec3 diffuse = diff * Diffuse * light.Color;

	vec3 halfway = normalize(lightDir + viewDir);
	float spec = pow(max(dot(Normal, halfway), 0.0), 8.0);
	vec3 specular = Specular * spec * light.Color;
	
	float distance = length(light.Position - FragPos);
	float attenuation = 1.0 / (1.0 + light.Linear * distance + light.Quadratic * distance * distance);
	diffuse *= attenuation;
	specular *= attenuation;
	lighting += diffuse + specular;

	FragColor = vec4(lighting, 1.0);

}