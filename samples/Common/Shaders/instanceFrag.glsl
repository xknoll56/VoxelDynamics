#version 330 core

in vec3 normal;
out vec4 color;

in vec2 uv;
flat in int texInd;

uniform sampler2DArray textureArray;
uniform vec3 solidColor;

void main()
{
	vec3 lightDir = vec3(-0.4f,1.0f,-0.8f);
	lightDir = normalize(lightDir);

	float diffuse = dot(normal, lightDir);
	diffuse = max(diffuse, 0.4f);
	vec4 baseColor; 
	if(texInd == -1)
		baseColor = vec4(solidColor,1.0f);
	else
		baseColor = texture(textureArray, vec3(uv, texInd));
	color = vec4(baseColor.x*diffuse, baseColor.y*diffuse, baseColor.z*diffuse, baseColor.w);
}