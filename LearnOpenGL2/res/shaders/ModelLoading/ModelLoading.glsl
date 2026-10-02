#shader vertex
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out vec2 TexCoords;
out vec3 FragPos;
out mat3 TBN;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	FragPos = vec3(model * vec4(aPos, 1.0));
	TexCoords = aTexCoord;
	mat3 normalMat = mat3(transpose(inverse(model)));
	vec3 N = normalize(normalMat * aNormal);
	vec3 T = normalize(normalMat * aTangent);
	T = normalize(T - dot(T, N) * N);

	vec3 B = cross(N, T);

	TBN = mat3(T, B, N);
	gl_Position = projection * view * model * vec4(aPos, 1.0);
}

#shader fragment
#version 330 core
#define MAX_TEXTURES 16

out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos;
in mat3 TBN;

struct Light
{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float constant;
	float linear;
	float quadratic;
};

const int TEX_DIFFUSE	= 0;
const int TEX_SPECULAR	= 1;
const int TEX_NORMAL	= 2;
const int TEX_EMISSION	= 3;
const int TEX_HEIGHT	= 4;
const int TEX_AMBIENT	= 5;
const int TEX_AO		= 6;
const int TEX_ROUGHNESS = 7;

uniform sampler2D textures[MAX_TEXTURES];
uniform int textureTypes[MAX_TEXTURES];
uniform int textureCount;

uniform Light light;
uniform vec3 viewPos;
uniform float shininess;

void main()
{
	vec3 ambientColor = vec3(1.0);
	vec3 diffuseColor = vec3(1.0);
	vec3 specularColor = vec3(0.0);
	vec3 emissionColor = vec3(0.0);
	float ao = 1.0;
	vec3 sampledNormal = vec3(0.0);
	bool hasNormalMap = false;

	for(int i=0; i < textureCount; i++)
	{
		if(textureTypes[i] == TEX_DIFFUSE) // Diffuse
		{
			diffuseColor *= texture(textures[i], TexCoords).rgb;
		}
		else if(textureTypes[i] == TEX_SPECULAR) // Specular
		{
			specularColor += texture(textures[i], TexCoords).rgb;
		}
		else if(textureTypes[i] == TEX_NORMAL) // Normal
		{
			sampledNormal = texture(textures[i], TexCoords).rgb * 2.0 - 1.0;
			hasNormalMap = true;
		}
		else if(textureTypes[i] == TEX_EMISSION) // Emission
		{
			emissionColor += texture(textures[i], TexCoords).rgb;
		}
		else if(textureTypes[i] == TEX_AO)	// AO
		{
			ao *= texture(textures[i], TexCoords).rgb.r;
		}
	}
	vec3 ambient = light.ambient * diffuseColor * ao;

	vec3 N = hasNormalMap ? normalize(TBN * sampledNormal) : normalize(TBN[TEX_NORMAL]);
	vec3 lightDir = normalize(light.position - FragPos);
	float diff = max(dot(N, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * diffuseColor;

	vec3 viewDir = normalize(viewPos - FragPos);	
	vec3 reflectDir = reflect(-lightDir, N);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
	vec3 specular = light.specular * spec * specularColor;

	float distance = length(light.position - FragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	vec3 result = ambient + diffuse + specular + emissionColor;
	FragColor = vec4(result, 1.0);
}

