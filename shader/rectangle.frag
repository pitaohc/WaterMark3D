#version 330 core                            
out vec4 FragColor;
in vec3 ourColor;
in vec2 outTexCord;
uniform sampler2D ourTexture;
uniform sampler2D ourTexture2;
void main()                                  
{                   
	FragColor = mix(texture(ourTexture, outTexCord),texture(ourTexture2, outTexCord),1.0);
	//0����ȫΪ��һ�����룬1����ȫΪ�ڶ�������
} 