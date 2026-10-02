#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out VS_OUT 
{
	vec3 FragPos;
    vec3 Normal;
    vec3 Tangent;
    vec3 Bitangent;
} vs_out;

out vec3 FragPos;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
	vec4 worldPos = model * vec4(aPos, 1.0);
	vs_out.FragPos = worldPos.xyz;

	mat3 normalMatrix = transpose(inverse(mat3(model)));
	vec3 T = normalize(normalMatrix * aTangent);
	vec3 N = normalize(normalMatrix * aNormal);
	T = normalize(T - dot(T, N) * N);
	vec3 B = cross(N, T);
	// float handedness = (dot(cross(N, T), aBitangent) < 0.0) ? -1.0 : 1.0;
	// B *= handedness;

	vs_out.Normal = N;
	vs_out.Tangent = T;
	vs_out.Bitangent = B;

	gl_Position = projection * view * worldPos;
}

#shader geometry
#version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 18) out;

in VS_OUT 
{
	vec3 FragPos;
    vec3 Normal;
    vec3 Tangent;
    vec3 Bitangent;
} gs_in[];

out vec3 fragColor;

uniform mat4 view;
uniform mat4 projection;
const float MAGNITUDE = 0.2;

void GenerateLine(vec3 pos, vec3 dir, vec3 color)
{
	gl_Position = projection * view * vec4(pos, 1.0);
	fragColor = color;
	EmitVertex();

	gl_Position = projection * view * vec4(pos + dir * MAGNITUDE, 1.0);
	fragColor = color;
	EmitVertex();

	EndPrimitive();

}

void main()
{
	for(int i = 0; i < 3; ++i)
	{
		vec3 pos = gs_in[i].FragPos;

		//Tangent
		GenerateLine(pos, gs_in[i].Tangent, vec3(1.0, 0.0, 0.0));
		// Bitangent
		GenerateLine(pos, gs_in[i].Bitangent, vec3(0.0, 1.0, 0.0));
		// Normal
		GenerateLine(pos, gs_in[i].Normal, vec3(0.0, 0.0, 1.0));
	}
}

#shader fragment
#version 330 core
out vec4 FragColor;

in vec3 fragColor;

void main()
{
	FragColor = vec4(fragColor, 1.0);
}