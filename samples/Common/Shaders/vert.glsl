#version 330 core

layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNorm;
layout(location = 2) in vec2 vertUv;


out vec3 normal;
out vec2 uv;

uniform mat4 mvp;
uniform mat4 model;

void main(){

    gl_Position = mvp * vec4(vertPos,1);
    mat3 normalMatrix = mat3(transpose(inverse(model)));
    normal = normalize(normalMatrix * vertNorm);
    uv = vertUv;
}

