#version 430 core

//layout (location = 0) in vec3 aPos;



struct vertex4 {
    float x;
    float y;
    float z;
    float w;
};

layout(std430, binding = 0) buffer VertexData {
    vertex4 positions[];
};


void main()
{
    gl_Position = vec4(positions[gl_VertexID].x, positions[gl_VertexID].y, positions[gl_VertexID].z, 1.0);
}