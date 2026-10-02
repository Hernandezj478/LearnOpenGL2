#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormals;

out VS_OUT
{
	vec3 FragPos;
	vec2 TexCoords;
	vec3 Normal;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    vec4 worldPos = model * vec4(aPos, 1.0);
	vs_out.FragPos = worldPos.xyz;

	vs_out.TexCoords = aTexCoords;
	vs_out.Normal = mat3(transpose(inverse(model))) * aNormals;
	gl_Position = projection * view * worldPos;
}

#shader fragment
#version 330 core
out vec4 FragColor;

in VS_OUT
{
	vec3 FragPos;
	vec2 TexCoords;
	vec3 Normal;
} fs_in;

uniform sampler2D floorTexture;
uniform bool bLinearGamma;
uniform float uvScale;
uniform vec3 lightPositions[4];
uniform vec3 lightColors[4];
uniform vec3 viewPos;

vec3 BlinnPhong(vec3 normal, vec3 fragPos, vec3 lightPos, vec3 lightColor)
{
	// diffuse
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 halfway = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfway), 0.0), 64.0);
    vec3 specular = spec * lightColor;    
    // simple attenuation
    float max_distance = 1.5;
    float distance = length(lightPos - fragPos);
    float denom = bLinearGamma ? distance : (distance * distance);
    float attenuation = 1.0 / denom;
    
    diffuse *= attenuation;
    specular *= attenuation;
    
    return diffuse + specular;
}


void main()
{
	vec3 color = texture(floorTexture, fs_in.TexCoords * uvScale).rgb;
    vec3 lighting = vec3(0.0);
    for(int i = 0; i < 4; ++i)
        lighting += BlinnPhong(normalize(fs_in.Normal), fs_in.FragPos, lightPositions[i], lightColors[i]);
    color *= lighting;
    color = pow(color, vec3(1.0/2.2));
    FragColor = vec4(color, 1.0);
}
