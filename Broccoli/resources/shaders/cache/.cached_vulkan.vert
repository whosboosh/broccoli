#  
  @                 GLSL.std.450                      main             %   0   1   5   7   9   <   =   >   ?        C:/Users/natha/source/repos/Broccoli/Broccoli/resources/shaders/geometry.vert    W   �     #version 450

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

/*
layout(set = 0, binding = 1) uniform Model {
	mat4 model;
	mat4 inverseModel;
	bool hasTexture;
} model;
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
}  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main         gl_PerVertex             gl_Position         gl_PointSize            gl_ClipDistance         gl_CullDistance               ViewProjection           projection          view         viewProjection       PushModel            model           inverseModel            hasTexture       pushModel     %   pos   0   fragCol   1   col   5   fragTex   7   tex   9   normal    :   LightSpace    :       lightSpace    <   lightSpace    =   Normal    >   FragPos   ?   outShadowCoord  J entry-point main    J client vulkan100    J target-env spirv1.5 J target-env vulkan1.2    J entry-point main    H                H              H              H              G        H            H         #       H               H           H        #   @   H              G        G     "       G     !       H            H         #       H               H           H        #   @   H              H        #   �   G        G  %          G  0          G  1         G  5         G  7         G  9         H  :          H  :       #       H  :             G  :      G  <   "      G  <   !       G  =         G  >         G  ?              !                              	           +  	   
              
                                ;                       +                                              ;                       +                      	         	      ;        	         	        #            $      #   ;  $   %      +     '     �?   -            /      #   ;  /   0      ;  $   1        3            4      3   ;  4   5         6      3   ;  6   7      ;  $   9        :         ;      :   ;  ;   <      ;  /   =      ;  /   >      ;  -   ?      6               �          +       A              =           A              =           �              A               =     !       �     "      !   =  #   &   %   Q     (   &       Q     )   &      Q     *   &      P     +   (   )   *   '   �     ,   "   +   A  -   .         >  .   ,        5       =  #   2   1   >  0   2        6       =  3   8   7   >  5   8   �  8  