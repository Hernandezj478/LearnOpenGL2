#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 5) in mat4 aInstanceMatrix;

uniform bool bUseInstanceMatrix;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 TexCoord;

void main()
{
	mat4 matrix;
	if(bUseInstanceMatrix)
	{
		matrix = aInstanceMatrix;
	}
	else
	{
		matrix = model;
	}
	gl_Position = projection * view * matrix * vec4(aPos, 1.0);
	TexCoord = aTexCoord;
}

#shader fragment
#version 330 core
#define MAX_TEXTURE_COUNT 16
out vec4 FragColor;

in vec2 TexCoord;

uniform int textureCount;
uniform sampler2D textures[MAX_TEXTURE_COUNT];
uniform int textureTypes[MAX_TEXTURE_COUNT];

const int TEX_DIFFUSE	= 0;
const int TEX_SPECULAR	= 1;
const int TEX_NORMAL	= 2;
const int TEX_EMISSION	= 3;
const int TEX_HEIGHT	= 4;
const int TEX_AMBIENT	= 5;
const int TEX_AO		= 6;
const int TEX_ROUGHNESS = 7;

void main()
{
	vec3 ambientColor = vec3(1.0);
	vec3 diffuseColor = vec3(1.0);
	vec3 specularColor = vec3(0.0);
	vec3 emissionColor = vec3(0.0);
	vec3 sampledNormal = vec3(0.0);
	bool bHasNormal = false;
	float ao = 1.0;

	for(int i = 0; i < textureCount; i++)
	{
		switch(textureTypes[i])
		{
			case TEX_DIFFUSE:
				diffuseColor *= vec3(texture(textures[i], TexCoord));
				break;
			case TEX_SPECULAR:
				specularColor += vec3(texture(textures[i], TexCoord));
				break;
			case TEX_NORMAL:
				sampledNormal += vec3(texture(textures[i], TexCoord));
				break;
			case TEX_EMISSION:
				emissionColor += vec3(texture(textures[i], TexCoord));
				break;
			case TEX_AO:
				ao *= texture(textures[i], TexCoord).r;
		}
	}
	vec3 ambient = diffuseColor * ao;
	FragColor = vec4(ambient, 1.0);
}