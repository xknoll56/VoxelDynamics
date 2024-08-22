#version 330 core

in vec3 normal;
out vec4 color;

in vec2 uv;

uniform int textInd;
uniform sampler2DArray textureArray;
uniform float colorMix;
uniform vec3 solidColor;

void main()
{
	vec3 lightDir = vec3(-0.4f,1.0f,-0.8f);
	lightDir = normalize(lightDir);

	float diffuse = dot(normal, lightDir);
	diffuse = max(diffuse, 0.4f);
	vec4 baseColor = mix(texture(textureArray, vec3(uv, textInd)), vec4(solidColor, 1.0f), colorMix);
	color = vec4(baseColor.x*diffuse, baseColor.y*diffuse, baseColor.z*diffuse, baseColor.w);
}