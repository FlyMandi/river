#version 450

layout(location = 0) in vec3 fragColour;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColour;

void main() {
    outColour = vec4(fragTexCoord, 0.0, 1.0);
}
