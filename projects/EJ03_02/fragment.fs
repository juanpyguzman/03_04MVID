#version 330 core

in vec3 myColor;
out vec4 FragColor;

uniform float time;

void main() {
    FragColor = vec4(myColor.r*abs(sin(time)),myColor.g*abs(cos(time)),myColor.b*abs(tan(time+1)), 1.0);
}