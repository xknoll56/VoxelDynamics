#version 330 core

out vec4 color;


uniform vec3 solidColor;

void main()
{
	color = vec4(solidColor, 1.0f);
}