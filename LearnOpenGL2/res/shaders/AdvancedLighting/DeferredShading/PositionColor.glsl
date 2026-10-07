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
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;


in VS_OUT
{
	vec3 FragPos;
	vec2 TexCoord;
	vec3 Normal;
} fs_in;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

void main()
{
	gPosition = fs_in.FragPos;
	gNormal = normalize(fs_in.Normal);
	gAlbedoSpec.rgb = texture(texture_diffuse1, fs_in.TexCoord).rgb;
	gAlbedoSpec.a = texture(texture_specular1, fs_in.TexCoord).r;
}