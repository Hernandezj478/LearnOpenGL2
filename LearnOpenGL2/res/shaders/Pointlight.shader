#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main()
{
	FragPos = vec3(model * vec4(aPos, 1.0));
	Normal = mat3(transpose(inverse(model))) * aNormal;
	TexCoords = aTexCoord;

	gl_Position = projection * view * model * vec4(aPos, 1.0);
}


#shader fragment
#version 330 core

struct Material
{
	sampler2D diffuse;
	sampler2D specular;
	sampler2D emission;
	float shininess;
};

struct Light 
{
	vec3 position;
	//vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main()
{
	vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;

	vec3 norm			= normalize(Normal);
	vec3 lightDir		= normalize(light.position - FragPos);
	float diff			= max(dot(norm, lightDir), 0.0);
	vec3 diffuse		= light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;

	vec3 viewDir		= normalize(viewPos - FragPos);
	vec3 reflectDir		= reflect(-lightDir, norm);
	float spec			= pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular		= light.specular * spec * texture(material.specular, TexCoords).rgb;

	float d				= length(light.position - FragPos);
	float attenuation	= 1.0 / (light.constant + light.linear * d + light.quadratic * (d * d));

	ambient		*= attenuation;
	diffuse		*= attenuation;
	specular	*= attenuation;

	vec3 result = ambient + diffuse + specular;
	FragColor = vec4(result, 1.0);

}