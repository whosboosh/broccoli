#  
  _                 GLSL.std.450                      main             )   ,   7   :   G   H   L   N   P   S        C:/Users/natha/source/repos/Broccoli/Broccoli/resources/shaders/geometry.vert       �     #version 450

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
	//gl_Position = viewProjection.projection * viewProjection.view * pushModel.model * vec4(pos, 1.0);
	
	gl_Position = viewProjection.projection * viewProjection.view * vec4(pos, 1.0);
	
	Normal = mat3(pushModel.inverseModel) * normal;  
	
	FragPos = vec3(pushModel.model * vec4(pos, 1.0)); 	
	fragCol = col;
	fragTex = tex;

	outShadowCoord = lightSpace.lightSpace * pushModel.model * vec4(pos, 1.0);	
}    
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main         gl_PerVertex             gl_Position         gl_PointSize            gl_ClipDistance         gl_CullDistance               ViewProjection           projection          view         viewProjection       pos   )   Normal    *   PushModel     *       model     *      inverseModel      *      hasTexture    ,   pushModel     7   normal    :   FragPos   G   fragCol   H   col   L   fragTex   N   tex   P   outShadowCoord    Q   LightSpace    Q       lightSpace    S   lightSpace  J entry-point main    J client vulkan100    J target-env spirv1.5 J target-env vulkan1.2    J entry-point main    H                H              H              H              G        H            H         #       H               H           H        #   @   H              G        G     "       G     !       G            G  )         H  *          H  *       #       H  *             H  *         H  *      #   @   H  *            H  *      #   �   G  *      G  ,   "       G  ,   !      G  7         G  :         G  G          G  H         G  L         G  N         G  P         H  Q          H  Q       #       H  Q             G  Q      G  S   "       G  S   !           !                              	           +  	   
              
                                ;                       +                                              ;                       +                                  ;           +           �?   &            (         ;  (   )        *         	      +      *   ;  +   ,        /         ;     7      ;  (   :      ;  (   G      ;     H        J            K      J   ;  K   L         M      J   ;  M   N      ;  &   P        Q         R      Q   ;  R   S      6               �                  A              =           A              =           �              =           Q     !          Q     "         Q     #         P     $   !   "   #       �     %      $   A  &   '         >  '   %        "       A     -   ,      =     .   -   Q     0   .       O     1   0   0             Q     2   .      O     3   2   2             Q     4   .      O     5   4   4             P  /   6   1   3   5   =     8   7   �     9   6   8   >  )   9        $       A     ;   ,      =     <   ;   =     =      Q     >   =       Q     ?   =      Q     @   =      P     A   >   ?   @       �     B   <   A   Q     C   B       Q     D   B      Q     E   B      P     F   C   D   E   >  :   F        %       =     I   H   >  G   I        &       =  J   O   N   >  L   O        (       A     T   S      =     U   T   A     V   ,      =     W   V   �     X   U   W   =     Y      Q     Z   Y       Q     [   Y      Q     \   Y      P     ]   Z   [   \       �     ^   X   ]   >  P   ^   �  8  