#version 330 core 

uniform sampler2D u_Tex;

in vec2 oUV;

out vec4 FragColor;

void main() {
    FragColor = texture(u_Tex, oUV);
}
