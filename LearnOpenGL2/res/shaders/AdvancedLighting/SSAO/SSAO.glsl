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
out float FragColor;

in vec2 TexCoord;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D texNoise;

uniform mat4 projection;

uniform vec3 samples[64];
uniform vec2 noiseScale;

uniform int kernelSize;
uniform float radius;
uniform float bias;

void main()
{
	vec3 FragPos = vec3(texture(gPosition, TexCoord));
	vec3 Normal = normalize(vec3(texture(gNormal, TexCoord)));
	vec3 RandVec = normalize(vec3(texture(texNoise, TexCoord * noiseScale)));
	vec3 Tangent = normalize(RandVec - Normal * dot(RandVec, Normal));
	vec3 Bitangent = cross(Normal, Tangent);
	mat3 TBN = mat3(Tangent, Bitangent, Normal);
	float occlusion = 0.0;
	for(int i = 0; i < kernelSize; i++)
	{
		vec3 samplePos = TBN * samples[i];
		samplePos = FragPos + samplePos * radius;

		vec4 offset = vec4(samplePos, 1.0);
		offset = projection * offset;
		offset.xyz /= offset.w;
		offset.xyz = offset.xyz * 0.5 + 0.5;

		float sampleDepth = texture(gPosition, offset.xy).z;

		float rangeCheck = smoothstep(0.0, 1.0, radius / abs(FragPos.z - sampleDepth));
		occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;
	}
	occlusion = 1.0 - (occlusion / kernelSize);
	FragColor = occlusion;
}