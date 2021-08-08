#  
  &                 GLSL.std.450                     main    
                         %                C:/Users/natha/source/repos/Broccoli/Broccoli/resources/shaders/geometry.frag    �    �     #version 450

layout(location = 0) in vec3 fragCol;
layout(location = 1) in vec2 fragTex;
layout(location = 2) in vec3 Normal;
layout(location = 3) in vec3 FragPos;
layout(location = 4) in vec4 inShadowCoord;
 
//layout(set = 1, binding = 0) uniform sampler2D textureSampler;
//layout(set = 2, binding = 0) uniform sampler2D shadowMap;

layout(location = 0) out vec4 outColour; 	// Final output colour (must also have location


// Uniform buffer for light
layout(set = 0, binding = 2) uniform DirectionalLight {
	float directionX;
	float directionY;
	float directionZ;
	float colourR;
	float colourG;
	float colourB;
	float ambientIntensity;
	float diffuseIntensity;
} directionalLight;

layout(set = 0, binding = 3) uniform CameraPosition {
	vec3 cameraPos;	
} cameraPosition;

layout(push_constant) uniform PushModel {
	mat4 model;
	mat4 inverseModel;
	bool hasTexture;
} pushModel;

void main() 
{
	outColour = vec4(fragCol, 1.0);
}  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main      
   outColour        fragCol      fragTex      Normal       FragPos      inShadowCoord        DirectionalLight             directionX          directionY          directionZ          colourR         colourG         colourB         ambientIntensity            diffuseIntensity         directionalLight         CameraPosition           cameraPos         cameraPosition    #   PushModel     #       model     #      inverseModel      #      hasTexture    %   pushModel   J entry-point main    J client vulkan100    J target-env spirv1.5 J target-env vulkan1.2    J entry-point main    G  
          G            G           G           G           G           H         #       H        #      H        #      H        #      H        #      H        #      H        #      H        #      G        G     "       G     !      H         #       G        G      "       G      !      H  #          H  #       #       H  #             H  #         H  #      #   @   H  #            H  #      #   �   G  #           !                               	         ;  	   
                             ;           +          �?                       ;           ;           ;                       ;            
                                        ;                               ;              !           "             #   !   !   "      $   	   #   ;  $   %   	   6               �          '       =           Q               Q              Q              P                    >  
      �  8  