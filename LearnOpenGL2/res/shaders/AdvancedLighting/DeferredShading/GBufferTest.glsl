#shader vertex
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;
uniform mat4 model;

void main()
{
	TexCoord = aTexCoord;
	gl_Position = model * vec4(aPos, 0.0, 1.0);
	//gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
}

#shader fragment
#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

uniform int Selection;

void main()
{
	vec3 color = vec3(0.0);
	switch(Selection)
	{
		case 0:
			// Position
			color = vec3(texture(gPosition, TexCoord));
			break;
		case 1:
			// Normals
			color = vec3(texture(gNormal, TexCoord));
			break;
		case 2:
			color = texture(gAlbedoSpec, TexCoord).rgb;
			break;
		case 3:
			float spec = texture(gAlbedoSpec, TexCoord).a;
			color = vec3(spec);
			break;
	}
	FragColor = vec4(color, 1.0);
}