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
	
	if(intensity > 0.95)
		color = vec4(0.7, 0.7, 0.7, 1.0);
	else if(intensity > 0.5)
		color = vec4(0.5, 0.5, 0.5, 1.0);
	else if(intensity > 0.25)
		color = vec4(0.3, 0.3, 0.3, 1.0);
	else
		color = vec4(0.7, 1.0, 1.0, 1.0);
		
	FragColor = texture(ourTexture, TexCoord) * color;
	
}