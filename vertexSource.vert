#version 430 core

//layout (location = 0) in vec3 aPos;


layout(std430, binding = 0) buffer VertexData {
    vec4 positions[];
};


void main()
{
    gl_Position = vec4(positions[gl_VertexID].xyz, 1.0);
}