#  
  ;             2        GLSL.std.450                     main    2   �   �   �   �     $  ,  -  0  6               C:/Users/natha/source/repos/Broccoli/Broccoli/resources/shaders/geometry.frag    7   �     #version 450

layout(location = 0) in vec3 fragCol;
layout(location = 1) in vec2 fragTex;
layout(location = 2) in vec3 Normal;
layout(location = 3) in vec3 FragPos;
layout(location = 4) in vec4 inShadowCoord;
 
layout(set = 1, binding = 0) uniform sampler2D textureSampler;
layout(set = 2, binding = 0) uniform sampler2D shadowMap;

layout(location = 0) out vec4 outColour; 	// Final output colour (must also have location

// Dynamic buffers
layout(set = 0, binding = 1) uniform UboModel {
	mat4 model;
	mat4 inverseModel;
	bool hasTexture;
} uboModel;

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

float CalcDirectionalShadowFactor()
{ 
	vec3 projCoords = inShadowCoord.xyz / inShadowCoord.w;
	projCoords = (projCoords * 0.5) + 0.5; // Scale between 0 and 1 from -1 and 1
	
	float current = projCoords.z;
	
	vec3 normal = normalize(Normal);
	vec3 lightDir = normalize(vec3(directionalLight.directionX, directionalLight.directionY, directionalLight.directionZ) - FragPos);
	
	float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.0005);
	
	float shadow = 0.0;
	
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	for (int i = -1; i <= 1; i++)
	{
		for (int k = -1; k <= 1; k++)
		{
			float pcfDepth = texture(shadowMap, projCoords.xy + vec2(i,k) * texelSize).r;
			shadow += current - bias > pcfDepth ? 1.0 : 0.0;;
		}
	}
	shadow /= 9.0;
	
	if(projCoords.z > 1.0)
	{
		shadow = 0.0;
	}
	
	return shadow;
}

float textureProj(vec4 shadowCoord, vec2 off)
{
	float shadow = 1.0;
	if ( shadowCoord.z > -1.0 && shadowCoord.z < 1.0 ) 
	{
		float dist = texture( shadowMap, shadowCoord.st + off ).r;
		if ( shadowCoord.w > 0.0 && dist < shadowCoord.z ) 
		{
			shadow = 0.1;
		}
	}
	return shadow;
}

float filterPCF(vec4 sc)
{
	ivec2 texDim = textureSize(shadowMap, 0);
	float scale = 1.5;
	float dx = scale * 1.0 / float(texDim.x);
	float dy = scale * 1.0 / float(texDim.y);

	float shadowFactor = 0.0;
	int count = 0;
	int range = 1;
	
	for (int x = -range; x <= range; x++)
	{
		for (int y = -range; y <= range; y++)
		{
			shadowFactor += textureProj(sc, vec2(dx*x, dy*y));
			count++;
		}
	
	}
	return shadowFactor / count;
}

vec4 CalcLightByDirection(float shadowFactor)
{
	vec3 colour = vec3(directionalLight.colourR, directionalLight.colourG, directionalLight.colourB);
	vec3 direction = vec3(directionalLight.directionX, directionalLight.directionY, directionalLight.directionZ); 

	vec4 ambientColour = vec4(colour, 1.0f) * directionalLight.ambientIntensity;
	
	vec3 normal = normalize(Normal);
	vec3 lightDir = normalize(direction - FragPos);
	
	float diffuseFactor = max(dot(normal, lightDir), 0.0f);
	vec4 diffuseColour = vec4(colour * directionalLight.diffuseIntensity * diffuseFactor, 1.0f);
	
	vec4 specularColour = vec4(0, 0, 0, 0);
	if(diffuseFactor > 0.0f)
	{
		vec3 fragToEye = normalize(cameraPosition.cameraPos - FragPos);
		vec3 reflectedVertex = normalize(reflect(-lightDir, normal));
		
		float specularFactor = dot(fragToEye, reflectedVertex);
		if(specularFactor > 0.0f)
		{
			specularFactor = pow(specularFactor, 128);
			specularColour = vec4(colour * 0.5 * specularFactor, 1.0f);
		}
	}
	
	return (ambientColour + (diffuseColour * (shadowFactor)) + specularColour);
}

vec4 CalcDirectionalLight()
{
	float shadowFactor = filterPCF(inShadowCoord / inShadowCoord.w);
	//float shadowFactor = textureProj(inShadowCoord / inShadowCoord.w, vec2(0.0));
	//float shadowFactor = CalcDirectionalShadowFactor();
	return CalcLightByDirection(shadowFactor);
}

void main() 
{
	vec4 finalColour = CalcDirectionalLight();		
	if (pushModel.hasTexture) {
		outColour = texture(textureSampler, fragTex) * finalColour;
	}
	else {
		//outColour = vec4(0,0,1,1);
		outColour = vec4(fragCol, 1.0) * finalColour;
	}
}  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main      2   shadowMap     �   DirectionalLight      �       directionX    �      directionY    �      directionZ    �      colourR   �      colourG   �      colourB   �      ambientIntensity      �      diffuseIntensity      �   directionalLight      �   Normal    �   FragPos   �   CameraPosition    �       cameraPos     �   cameraPosition      inShadowCoord     "  PushModel     "      model     "     inverseModel      "     hasTexture    $  pushModel     ,  outColour     -  textureSampler    0  fragTex   6  fragCol J entry-point main    J client vulkan100    J target-env spirv1.5 J target-env vulkan1.2    J entry-point main    G  2   "      G  2   !       H  �       #       H  �      #      H  �      #      H  �      #      H  �      #      H  �      #      H  �      #      H  �      #      G  �      G  �   "       G  �   !      G  �         G  �         H  �       #       G  �      G  �   "       G  �   !      G          H  "         H  "      #       H  "            H  "        H  "     #   @   H  "           H  "     #   �   G  "     G  ,         G  -  "      G  -  !       G  0        G  6              !                              
         +          �?                   +     $     �� 	 /                              0   /      1       0   ;  1   2       +      9       +      ;      +     >       +     I   ���=  M            N   M      +  M   R       +     V     �?+  M   j        �          
 �                              �      �   ;  �   �      +  M   �         �         +  M   �      +  M   �      +  M   �      +  M   �         �      �   ;  �   �      ;  �   �      +  M   �      ,     �   >   >   >   >     �   �      �      �   ;  �   �         �      �   +     �      C+     �      ?           ;                      !          "  !  !         #  	   "  ;  #  $  	      %  	          +        ;  +  ,     ;  1   -         /     
   ;  /  0     ;  �   6     +  M   8  ����6               �          �       =     F    A    G    ;   =     H  G  P     I  H  H  H  H  �     J  F  I       \       =  0   [  2   d  /   \  [  g  N   ]  \  R        ^       Q  M   a  ]      o     b  a  �     c  V   b       _       Q  M   g  ]     o     h  g  �     i  V   h       e       �  l  �  l  �  M     R      -  �  �       >      (  �  �  M     8     �  �  �     p    j   �  �  �      �  p  q  �  �  q       g       �  t  �  t  �  M   -    q  �  �  �     (    q  �  �  �  M      8  q  �  �  �     x     j   �  �  �      �  x  y  �  �  y       i       o     |    �     }  c  |  o     �     �     �  i  �  P  
   �  }  �       O       Q     �  J     �     �  �  $   �  �      �  �  �  �  �  �  �     �  �     �  �  �  �  �     �  �  y  �  �  �  �      �  �  �  �  �  �       Q       O  
   �  J  J         �  
   �  �  �  W     �  [  �  Q     �  �           R       Q     �  J     �     �  �  >   �  �      �  �  �  �  �  �  �     �  �  �  �  �  �  �  �     �  �  �  �  �  �     :  �  I      �  �  �  �  �     "     �  :  �       i       �     �  (  "       j       �  M   �  -  j   �  �  �  �       g       �  M   �     j   �  t  �  �  �  �  �  �       e       �  M   �    j   �  l  �  �       n       o     �    �     �    �       s       A  �   �  �   �   =     �  �  A  �   �  �   �   =     �  �  A  �   �  �   �   =     �  �  P  �   �  �  �  �       t       A  �   �  �   R   =     �  �  A  �   �  �   j   =     �  �  A  �   �  �   �   =     �  �  P  �   �  �  �  �       v       P     �  �  �  �     A  �   �  �   �   =     �  �  �     �  �  �       x       =  �   �  �     �   �     E   �       y       =  �   �  �   �  �   �  �  �    �   �     E   �       {       �     �  �  �       �     (   �  >        |       A  �   �  �   �   =     �  �  �  �   �  �  �  �  �   �  �  �  Q     �  �      Q     �  �     Q     �  �     P     �  �  �  �                 �     �  �  >   �        �  �  �    �  �       �       A  �   �  �   R   =  �   �  �  �  �   �  �  �    �   �     E   �       �         �   �  �    �   �     G   �  �    �   �     E   �       �       �     �  �  �       �       �        �  >   �        �         �         �                    �  �        �       �  �     �  �   �  �         Q             Q     	       Q     
       P         	  
     �    �    �       �   �      �    �    �       �   �           �       �       �  �  �       �    �                �       A  %  &  $  �   =      '  &  �     (  '  9   �  *      �  (  )  5  �  )       �       =  0   .  -  =  
   1  0  W     2  .  1  �     4  2    >  ,  4  �  *  �  5       �       =  �   7  6  Q     8  7      Q     9  7     Q     :  7     P     ;  8  9  :     �     =  ;    >  ,  =  �  *  �  *  �  8  