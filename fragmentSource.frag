#version 450 core


// the input from rasterizer
in vec3 TexCoord;
out vec4 fragColor;
uniform sampler2DArray albedoTextureArray;



void main(){
	// once the sampler is sampler2DArray, the uv must be vec3
	vec4 texel = texture(albedoTextureArray, TexCoord) ;
	// discard the transparent texel

	// output color
	fragColor = texel;
}