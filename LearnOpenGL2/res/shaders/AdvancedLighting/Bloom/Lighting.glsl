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

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
	vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
	vs_out.TexCoord = aTexCoord;
	vs_out.Normal = transpose(inverse(mat3(model))) * aNormal;

	gl_Position = projection * view * model * vec4(aPos, 1.0);
}

#shader fragment
#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in VS_OUT
{
	vec3 FragPos;
	vec2 TexCoord;
	vec3 Normal;
} fs_in;

struct Light
{
	vec3 Position;
	vec3 Color;
};

uniform sampler2D diffuseTexture;
uniform vec3 viewPos;
uniform Light lights[16];
uniform int numLights;

void main()
{
	vec3 color = vec3(texture(diffuseTexture, fs_in.TexCoord));
	vec3 ambient = 0.3 * color;
	vec3 normal = normalize(fs_in.Normal);
	vec3 lighting = vec3(0.0);

	for(int i = 0; i < numLights; i++)
	{
		vec3 lightDir = normalize(lights[i].Position - fs_in.FragPos);
		float diff = max(dot(lightDir, normal), 0.0);
		vec3 diffuse = diff * lights[i].Color * color;

		vec3 viewDir = normalize(viewPos - fs_in.FragPos);
		vec3 halfway = normalize(lightDir + viewDir);
		float spec = pow(max(dot(halfway, normal), 0.0), 32.0);
		vec3 specular = spec * lights[i].Color * color;

		float distance = length(lights[i].Position - fs_in.FragPos);
		float attenuation = 1.0 / (distance * distance);

		diffuse *= attenuation;
		specular *= attenuation;

		lighting += (diffuse + specular);
	}
	vec3 result = ambient + lighting;
	float brightness = dot(result, vec3(0.2126, 0.7152, 0.0722));
	if(brightness > 1.0)
	{
		BrightColor = vec4(result, 1.0);
	}
	else
	{
		BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
	}
	FragColor = vec4(result, 1.0);
}
