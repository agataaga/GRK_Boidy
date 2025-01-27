#version 330 core
out vec4 FragColor;

uniform vec3 camPos;
uniform vec3 groupColor;  

void main() {
    FragColor = vec4(groupColor, 1.0); 
}