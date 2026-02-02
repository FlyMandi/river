#version 450

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragColour;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColour;

void main() {
    outColour = vec4(fragColour * texture(texSampler, fragTexCoord * 2.0f).rgb * 2.0f, 1.0f);
}
