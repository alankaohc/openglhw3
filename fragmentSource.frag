#version 450 core


in vec2 TexCoord;

uniform sampler2D ourTexture;
uniform sampler2D ourFace;

out vec4 FragColor;

void main() {
   //FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
   FragColor = mix(texture(ourTexture, TexCoord), texture(ourFace, TexCoord), 0.2);;
}