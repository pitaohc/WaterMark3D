#version 330 core                            
out vec4 FragColor;
in vec3 ourColor;
in vec2 outTexCord;
uniform sampler2D ourTexture;
uniform sampler2D ourTexture2;
void main()                                  
{                   
	FragColor = mix(texture(ourTexture, outTexCord),texture(ourTexture2, outTexCord),1.0);
	//0代表全为第一个输入，1代表全为第二个输入
} 