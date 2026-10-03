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
	vec3 TLightPos;
	vec3 TViewPos;
	vec3 TFragPos;
} vs_out;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
	vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
	vs_out.TexCoord = aTexCoord;
	vec3 T = normalize(vec3(model * vec4(aTangent, 0.0)));
	vec3 N = normalize(mat3(transpose(inverse(model))) * aNormal);
	T = normalize(T - dot(T, N) * N);
	vec3 B = cross(N, T);
	mat3 TBN = transpose(mat3(T, B, N));

	vs_out.TLightPos = TBN * lightPos;
	vs_out.TViewPos = TBN * viewPos;
	vs_out.TFragPos = TBN * vec3(model * vec4(aPos, 1.0));

	gl_Position = projection * view * model * vec4(aPos, 1.0);
}

#shader fragment
#version 330 core
out vec4 FragColor;

in VS_OUT
{
	vec3 FragPos;
	vec2 TexCoord;
	vec3 TLightPos;
	vec3 TViewPos;
	vec3 TFragPos;
} fs_in;

uniform sampler2D wallAlbedo;
uniform sampler2D wallNormal;

void main()
{
	vec3 color = vec3(texture(wallAlbedo, fs_in.TexCoord));
	vec3 ambient = vec3(0.3) * color;

	vec3 normal = vec3(texture(wallNormal, fs_in.TexCoord));
	normal = normalize(normal * 2.0 - 1.0);

	vec3 lightDir = normalize(fs_in.TLightPos - fs_in.TFragPos);
	float diff = max(dot(lightDir, normal), 0.0);
	vec3 diffuse = diff * color;

	vec3 viewDir = normalize(fs_in.TViewPos - fs_in.TFragPos);
	vec3 halfwayDir = normalize(lightDir + normal);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
	vec3 specular = spec * vec3(0.3);

	float distance = length(fs_in.TLightPos - fs_in.TFragPos);
	float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * (distance * distance));

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	vec3 result = ambient + diffuse + specular;

	FragColor = vec4(result, 1.0);
}