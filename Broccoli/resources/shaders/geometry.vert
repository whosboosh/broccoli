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

/*
layout(set = 0, binding = 1) uniform Model {
	mat4 model;
	mat4 inverseModel;
	bool hasTexture;
} model;


layout(set = 0, binding = 2) uniform LightSpace {
	mat4 lightSpace;
} lightSpace;
*/

layout(location = 0) out vec3 fragCol;
layout(location = 1) out vec2 fragTex;
layout(location = 2) out vec3 Normal;
layout(location = 3) out vec3 FragPos;
layout(location = 4) out vec4 outShadowCoord;


void main() {
	gl_Position = viewProjection.projection * viewProjection.view * pushModel.model * vec4(pos, 1.0);
	
	//gl_Position = proj * mat4(1.0f) * pushModel.model * vec4(pos, 1.0);
	
	//outUV = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);
	//gl_Position = vec4(outUV * 2.0f - 1.0f, 0.0f, 1.0f);
	
	//Normal = mat3(pushModel.inverseModel) * normal;  
	
	//FragPos = vec3(pushModel.model * vec4(pos, 1.0)); 	
	fragCol = col;
	fragTex = tex;

	//outShadowCoord = lightSpace.lightSpace * pushModel.model * vec4(pos, 1.0);	
}