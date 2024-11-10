#version 450 core

layout (location = 6) in vec3 aPos;
layout (location = 8) in vec3 aTexCoord;

out vec3 TexCoord;

void main() {
	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
	TexCoord = aTexCoord;
}
