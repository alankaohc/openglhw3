#version 450 core

//layout (location = 0) in vec3 aPos;
out vec4 color;

struct ssbo_vertices {
	float x;
	float y;
	float z;
	float w;
};

layout (std430, binding=1) buffer layoutName {
	ssbo_vertices data[];
};

layout (std430, binding=2) buffer layoutName2 {
	ssbo_vertices data2[];
};
	
void main()
{
    gl_Position = vec4(data2[gl_VertexID].x, data2[gl_VertexID].y, data2[gl_VertexID].z, 1.0);

	if (data2[2].x == 0.5)
		color = vec4(0.0, 1.0, 0.0, 1.0);
	else 
		color = vec4(1.0, 0.0, 0.0, 1.0);
}