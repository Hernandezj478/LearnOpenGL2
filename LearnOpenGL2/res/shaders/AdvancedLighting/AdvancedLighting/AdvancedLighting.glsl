#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

out VS_OUT
{
	vec3 FragPos;
	vec2 TexCoord;
	vec3 Normal;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	vs_out.FragPos = aPos;
	vs_out.TexCoord = aTexCoord;
	vs_out.Normal = aNormal;

	gl_Position = projection * view * model * vec4(aPos, 1.0);
}

#shader fragment
#version 330 core
out vec4 FragColor;

in VS_OUT
{
	vec3 FragPos;
	vec2 TexCoord;
	vec3 Normal;
} fs_in;

const int LIGHT_POINT = 0;
const int LIGHT_SPOT  = 1;

struct Light
{
	vec3 position;
	vec3 direction;
	
	float cutoff;
	float outerCutoff;

	vec3 ambient;
	vec3 specular;
	
	float constant;
	float linear;
	float quadratic;
};

uniform Light light;
uniform bool bBlinnPhong;
uniform int lightType;
uniform vec3 viewPos;
uniform sampler2D floorTexture;

vec3 CalcPointLight()
{
	vec3 color = vec3(texture(floorTexture, fs_in.TexCoord));
	vec3 ambient = light.ambient * color;

	vec3 normal = normalize(fs_in.Normal);
	vec3 lightDir = normalize(light.position - fs_in.FragPos);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = color * diff;

	vec3 viewDir = normalize(viewPos - fs_in.FragPos);
	vec3 theta = vec3(0.0);
	if(bBlinnPhong)
	{
		theta = normalize(lightDir + viewDir);
	}
	else
	{
		theta = reflect(-lightDir, normal);
		
	}
	float spec = pow(max(dot(normal, theta), 0.0), 32.0);
	vec3 specular = light.specular * spec;

	float distance = length(light.position - fs_in.FragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return ambient + diffuse + specular;
}

vec3 CalcSpotLight()
{
	vec3 color = vec3(texture(floorTexture, fs_in.TexCoord));
	vec3 ambient = light.ambient * color;

	vec3 normal = normalize(fs_in.Normal);
	vec3 lightDir = normalize(light.position - fs_in.FragPos);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = color * diff;

	vec3 viewDir = normalize(viewPos - fs_in.FragPos);
	vec3 s = vec3(0.0);
	if(bBlinnPhong)
	{
		s = normalize(lightDir + viewDir);
	}
	else
	{
		s = reflect(-lightDir, normal);
		
	}
	float spec = pow(max(dot(normal, s), 0.0), 32.0);
	vec3 specular = light.specular * spec;

	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutoff - light.outerCutoff;
	float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0);

	diffuse *= intensity;
	specular *= intensity;

	float distance = length(light.position - fs_in.FragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return ambient + diffuse + specular;
}

void main()
{
	vec3 result = vec3(0.0);
	switch(lightType)
	{
		case 0:
			result = CalcPointLight();
			break;
		case 1:
			result = CalcSpotLight();
			break;
	}
	FragColor = vec4(result, 1.0);
}