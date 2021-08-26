#  
  X                 GLSL.std.450                      main             %   0   ;   >   K   L   P   R   V   W    
    resources/shaders/geometry.vert     �     #version 450

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

layout(set = 0, binding = 1) uniform LightSpace {
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
}  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main         gl_PerVertex             gl_Position         gl_PointSize            gl_ClipDistance         gl_CullDistance               ViewProjection           projection          view         viewProjection       PushModel            model           inverseModel            hasTexture       pushModel     %   pos   0   Normal    ;   normal    >   FragPos   K   fragCol   L   col   P   fragTex   R   tex   T   LightSpace    T       lightSpace    V   lightSpace    W   outShadowCoord  J entry-point main    J client vulkan100    J target-env spirv1.5 J target-env vulkan1.2    J entry-point main    H                H              H              H              G        H            H         #       H               H           H        #   @   H              G        G     "       G     !       H            H         #       H               H           H        #   @   H              H        #   �   G        G  %          G  0         G  ;         G  >         G  K          G  L         G  P         G  R         H  T          H  T       #       H  T             G  T      G  V   "       G  V   !      G  W              !                              	           +  	   
              
                                ;                       +                                              ;                       +                      	         	      ;        	         	        #            $      #   ;  $   %      +     '     �?   -            /      #   ;  /   0        3   #      ;  $   ;      ;  /   >      ;  /   K      ;  $   L        N            O      N   ;  O   P         Q      N   ;  Q   R        T         U      T   ;  U   V      ;  -   W      6               �                 A              =           A              =           �              A               =     !       �     "      !   =  #   &   %   Q     (   &       Q     )   &      Q     *   &      P     +   (   )   *   '   �     ,   "   +   A  -   .         >  .   ,        !       A     1         =     2   1   Q     4   2       O  #   5   4   4             Q     6   2      O  #   7   6   6             Q     8   2      O  #   9   8   8             P  3   :   5   7   9   =  #   <   ;   �  #   =   :   <   >  0   =        #       A     ?         =     @   ?   =  #   A   %   Q     B   A       Q     C   A      Q     D   A      P     E   B   C   D   '   �     F   @   E   Q     G   F       Q     H   F      Q     I   F      P  #   J   G   H   I   >  >   J        $       =  #   M   L   >  K   M        %       =  N   S   R   >  P   S   �  8  