#version 330 core

out vec4 color;

uniform vec3 buttonColor;

void main()
{
	color = vec4(buttonColor, 1.0f);
}