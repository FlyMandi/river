#version 450

//little triangle to test woo
vec2 positions[3] = vec2[](
    vec2(0.0, -0.3),
    vec2(0.1, 0.5),
    vec2(-0.5, 0.7)
);

void main(){
   gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
}
