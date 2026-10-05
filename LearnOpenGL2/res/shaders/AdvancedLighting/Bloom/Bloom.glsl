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
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D lighting;
uniform sampler2D blur;
uniform float exposure;
uniform bool bloom;
void main()
{
	const float gamma = 2.2;
	vec3 hdrColor = vec3(texture(lighting, TexCoord));
	vec3 bloomColor = vec3(texture(blur, TexCoord));
	if(bloom)
		hdrColor += bloomColor;
	vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
	result = pow(result, vec3(1.0 / gamma));
	FragColor = vec4(result, 1.0);
}