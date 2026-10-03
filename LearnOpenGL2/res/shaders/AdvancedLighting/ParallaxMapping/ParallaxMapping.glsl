#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out VS_OUT
{
	vec3 FragPos;
	vec2 TexCoord;
	vec3 TanLightPos;
	vec3 TanViewPos;
	vec3 TanFragPos;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
	vs_out.TexCoord = aTexCoord;

	vec3 T = normalize(mat3(model) * aTangent);
	vec3 B = normalize(mat3(model) * aBitangent);
	vec3 N = normalize(transpose(inverse(mat3(model))) * aNormal);
	mat3 TBN = transpose(mat3(T, B, N));

	vs_out.TanLightPos = TBN * lightPos;
	vs_out.TanViewPos = TBN * viewPos;
	vs_out.TanFragPos = TBN * vs_out.FragPos;
}

#shader fragment
#version 330 core
out vec4 FragColor;

in VS_OUT
{
	vec3 FragPos;
	vec2 TexCoord;
	vec3 TanLightPos;
	vec3 TanViewPos;
	vec3 TanFragPos;
} fs_in;

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D heightMap;

float height_scale = 0.1;

vec2 ParallaxMapping(vec2 texCoord, vec3 viewDir);

void main()
{
	vec3 viewDir = normalize(fs_in.TanViewPos - fs_in.TanFragPos);
	vec2 texCoord = ParallaxMapping(fs_in.TexCoord, viewDir);
	if(texCoord.x > 1.0 || texCoord.y > 1.0 || texCoord.x < 0.0 || texCoord.y < 0)
	{
		discard;
	}
	vec3 color = vec3(texture(albedoMap, texCoord));
	vec3 ambient = vec3(0.3) * color;

	vec3 normal = vec3(texture(normalMap, texCoord));
	normal = normalize(normal * 2.0 - 1.0);

	vec3 lightDir = fs_in.TanLightPos - fs_in.TanFragPos;
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * color;

	vec3 halfway = normalize(lightDir + normal);
	float spec = pow(max(dot(normal, halfway), 0.0), 32.0);
	vec3 specular = spec * vec3(0.3);

	float distance = length(fs_in.TanLightPos - fs_in.TanFragPos);
	float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * (distance * distance));

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	vec3 result = ambient + diffuse + specular;
	FragColor = vec4(result, 1.0);
}

vec2 ParallaxMapping(vec2 texCoord, vec3 viewDir)
{
	const float minLayers = 8.0;
	const float maxLayers = 32.0;
	float numLayers = mix(maxLayers, minLayers, max(dot(vec3(0.0, 0.0, 1.0), viewDir), 0.0));
	float layerHeight = 1.0 / numLayers;
	float currentLayerHeight = 0.0;
	vec2 P = viewDir.xy * height_scale;
	vec2 deltaTexCoord = P / numLayers;

	vec2 currentTexCoord = texCoord;
	float currentHeightMapValue = 1.0 - texture(heightMap, currentTexCoord).r;

	while(currentLayerHeight < currentHeightMapValue)
	{
		currentTexCoord -= deltaTexCoord;
		currentHeightMapValue = 1.0 - texture(heightMap, currentTexCoord).r;
		currentLayerHeight += layerHeight;
	}

	vec2 prevTexCoord = currentTexCoord + deltaTexCoord;
	float afterHeight = currentHeightMapValue - currentLayerHeight;
	float beforeHeight = (1.0 - texture(heightMap, prevTexCoord).r) - currentLayerHeight + layerHeight;

	float weight = afterHeight / (afterHeight - beforeHeight);
	vec2 finalTexCoord = prevTexCoord * weight + currentTexCoord * (1.0 - weight);

	return finalTexCoord;
}