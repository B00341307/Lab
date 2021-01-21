#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 normal;
in vec2 TexCoord;

uniform vec3 lightDir;
uniform sampler2D ourTexture;


void main()
{
	float intensity;
	vec4 color;
	intensity = dot(lightDir,normalize(normal));
	
	if(intensity > 10)
		color = vec4(0.75, 0, 1, 1.0);
	else if(intensity > 5)
		color = vec4(0, 0.75, 1, 1.0);
	else if(intensity > 0.1)
		color = vec4(0, 0, 1, 1.0);
	else
		color = vec4(0, 0, 0.25, 1.0);
		
	FragColor = texture(ourTexture, TexCoord) * color;
	
}