#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

layout (std140) uniform MVP
{
	uniform mat4 projection;
	uniform mat4 view;
};

out VS_OUT
{
	vec2 texCoords;
	vec3 normal;
} vs_out;


uniform mat4 model;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	vs_out.texCoords = aTexCoord;
	vs_out.normal = mat3(transpose(inverse(model))) * aNormal;
}

#shader geometry
#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 5) out;

in VS_OUT
{
	vec2 texCoords;
	vec3 normal;
} gs_in[];

out vec2 TexCoords;

uniform float time;

vec4 explode(vec4 position, vec3 normal)
{
	float magnitude = 2.0;
	vec3 direction = normal * ((sin(time) + 1.0) / 2.0) * magnitude;
	return position + vec4(direction, 0.0);
}

vec3 GetNormal()
{
	vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
	vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
	return normalize(cross(a, b));
}

void main()
{
	vec3 normal = GetNormal();
	gl_Position = explode(gl_in[0].gl_Position, normal);
	TexCoords = gs_in[0].texCoords;
	EmitVertex();
	gl_Position = explode(gl_in[1].gl_Position, normal);
	TexCoords = gs_in[2].texCoords;
	EmitVertex();
	gl_Position = explode(gl_in[2].gl_Position, normal);
	TexCoords = gs_in[2].texCoords;
	EmitVertex();
	EndPrimitive();
}

#shader fragment
#version 330 core
#define MAX_TEXTURES 16

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D textures[MAX_TEXTURES];
uniform int textureTypes[MAX_TEXTURES];

void main()
{
	FragColor = texture(textures[0], TexCoords);
}