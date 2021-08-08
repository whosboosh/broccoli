#  
  b                 GLSL.std.450                      main             $   /   :   =   J   K   O   Q   S   V        C:/Users/natha/source/repos/Broccoli/Broccoli/resources/shaders/geometry.vert    :   �     #version 450

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 col;
layout(location = 2) in vec2 tex;
layout(location = 3) in vec3 normal;

layout(set = 0, binding = 0) uniform ViewProjection {
	mat4 projection;
	mat4 view;
} viewProjection;

layout(set = 0, binding = 1) uniform PushModel {
	mat4 model;
	mat4 inverseModel;
	bool hasTexture;
} pushModel;

layout(set = 0, binding = 2) uniform LightSpace {
	mat4 lightSpace;
} lightSpace;

layout(location = 0) out vec3 fragCol;
layout(location = 1) out vec2 fragTex;
layout(location = 2) out vec3 Normal;
layout(location = 3) out vec3 FragPos;
layout(location = 4) out vec4 outShadowCoord;

void main() {
	gl_Position = viewProjection.projection * viewProjection.view * pushModel.model * vec4(pos, 1.0);
	
	//gl_Position = viewProjection.projection * viewProjection.view * vec4(pos, 1.0);
	
	//outUV = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);
	//gl_Position = vec4(outUV * 2.0f - 1.0f, 0.0f, 1.0f);
	
	Normal = mat3(pushModel.inverseModel) * normal;  
	
	FragPos = vec3(pushModel.model * vec4(pos, 1.0)); 	
	fragCol = col;
	fragTex = tex;

	outShadowCoord = lightSpace.lightSpace * pushModel.model * vec4(pos, 1.0);	
}   
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main         gl_PerVertex             gl_Position         gl_PointSize            gl_ClipDistance         gl_CullDistance               ViewProjection           projection          view         viewProjection       PushModel            model           inverseModel            hasTexture       pushModel     $   pos   /   Normal    :   normal    =   FragPos   J   fragCol   K   col   O   fragTex   Q   tex   S   outShadowCoord    T   LightSpace    T       lightSpace    V   lightSpace  J entry-point main    J client vulkan100    J target-env spirv1.5 J target-env vulkan1.2    J entry-point main    H                H              H              H              G        H            H         #       H               H           H        #   @   H              G        G     "       G     !       H            H         #       H               H           H        #   @   H              H        #   �   G        G     "       G     !      G  $          G  /         G  :         G  =         G  J          G  K         G  O         G  Q         G  S         H  T          H  T       #       H  T             G  T      G  V   "       G  V   !           !                              	           +  	   
              
                                ;                       +                                              ;                       +                      	               ;             "            #      "   ;  #   $      +     &     �?   ,            .      "   ;  .   /        2   "      ;  #   :      ;  .   =      ;  .   J      ;  #   K        M            N      M   ;  N   O         P      M   ;  P   Q      ;  ,   S        T         U      T   ;  U   V      6               �                 A              =           A              =           �              A              =            �     !          =  "   %   $   Q     '   %       Q     (   %      Q     )   %      P     *   '   (   )   &   �     +   !   *   A  ,   -         >  -   +        %       A     0         =     1   0   Q     3   1       O  "   4   3   3             Q     5   1      O  "   6   5   5             Q     7   1      O  "   8   7   7             P  2   9   4   6   8   =  "   ;   :   �  "   <   9   ;   >  /   <        '       A     >         =     ?   >   =  "   @   $   Q     A   @       Q     B   @      Q     C   @      P     D   A   B   C   &   �     E   ?   D   Q     F   E       Q     G   E      Q     H   E      P  "   I   F   G   H   >  =   I        (       =  "   L   K   >  J   L        )       =  M   R   Q   >  O   R        +       A     W   V      =     X   W   A     Y         =     Z   Y   �     [   X   Z   =  "   \   $   Q     ]   \       Q     ^   \      Q     _   \      P     `   ]   ^   _   &   �     a   [   `   >  S   a   �  8  