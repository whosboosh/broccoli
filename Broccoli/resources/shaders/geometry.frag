#version 450

layout(location = 0) in vec3 fragCol;
layout(location = 1) in vec2 fragTex;
layout(location = 2) in vec3 Normal;
layout(location = 3) in vec3 FragPos;
layout(location = 4) in vec4 inShadowCoord;

layout(location = 0) out vec4 outColour; 	// Final output colour (must also have location
 
layout(set = 1, binding = 0) uniform sampler2D textureSampler;
//layout(set = 2, binding = 0) uniform sampler2D shadowMap;

// Uniform buffer for light

/*
layout(set = 0, binding = 0) uniform DirectionalLight {
	float directionX;
	float directionY;
	float directionZ;
	float colourR;
	float colourG;
	float colourB;
	float ambientIntensity;
	float diffuseIntensity;
} directionalLight;
*/

//layout(set = 1, binding = 1) uniform CameraPosition {
	//vec3 cameraPos;	
//} cameraPosition;


/*
layout(push_constant) uniform PushModel {
	mat4 model;
	mat4 inverseModel;
	bool hasTexture;
} pushModel;
*/

void main() 
{
	outColour = texture(textureSampler, fragTex) * 0.8;
	//outColour = vec4(fragCol, 1.0);
}