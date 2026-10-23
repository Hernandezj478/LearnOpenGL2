#shader vertex
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec4 aUVData;
layout (location = 3) in mat4 instanceMatrix;

out vec2 TexCoord;

uniform mat4 projection;

void main()
{
	TexCoord.x = mix(aUVData.x, aUVData.z, aTexCoord.x);
	TexCoord.y = mix(aUVData.y, aUVData.w, aTexCoord.y);
	gl_Position = projection * instanceMatrix * vec4(aPos, 0.0, 1.0);
}

#shader fragment
#version 330 core
out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D atlas;
uniform vec3 textColor;

void main()
{
	float alpha = texture(atlas, TexCoord).r;
	FragColor = vec4(textColor, alpha);
}