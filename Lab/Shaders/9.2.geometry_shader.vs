#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoords;

out VS_OUT {
    vec2 texCoords;
} vs_out;

void main()
{
    vs_out.texCoords = aTexCoords;
    gl_Position = vec4(aPos, 1.0); 
}