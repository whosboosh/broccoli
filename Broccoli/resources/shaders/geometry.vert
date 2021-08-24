#version 450

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 col;
layout(location = 2) in vec2 tex;
layout(location = 3) in vec3 normal;

layout(push_constant) uniform PushModel {
	mat4 model;
	mat4 inverseModel;
	bool hasTexture;
} pushModel;

layout(set = 0, binding = 0) uniform ViewProjection {
	mat4 projection;
	mat4 view;
} viewProjection;

layout(set = 1, binding = 0) uniform LightSpace {
	mat4 lightSpace;
} lightSpace;

layout(location = 0) out vec3 fragCol;
layout(location = 1) out vec2 fragTex;
layout(location = 2) out vec3 Normal;
layout(location = 3) out vec3 FragPos;
layout(location = 4) out vec4 outShadowCoord;


void main() {
	gl_Position = viewProjection.projection * viewProjection.view * pushModel.model * vec4(pos, 1.0);
	
	Normal = mat3(pushModel.inverseModel) * normal;  
	
	FragPos = vec3(pushModel.model * vec4(pos, 1.0)); 	
	fragCol = col;
	fragTex = tex;

	//outShadowCoord = lightSpace.lightSpace * pushModel.model * vec4(pos, 1.0);	
}