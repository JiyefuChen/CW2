#version 330 core

in vec4 vertexColor;
in vec2 TexCoord;

uniform sampler2D ourTexture;
uniform sampler2D ourFace;
uniform sampler2D pic;


out vec4 FragColor;
void main()
{
	FragColor = texture(pic, TexCoord);
	//FragColor = vec4(1,1,1,1);
}