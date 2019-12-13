#version 330 core

layout (location=0) in vec3 aPos;
layout (location=1) in vec3 aColor;

out vec3 myColor;

uniform float time;

void main() {
    myColor = aColor;
    gl_Position = vec4(aPos.x+0.1*time*cos(time),aPos.y+0.1*time*sin(time),aPos.z, 1.0);
}