

#version 330
layout (location = 1) in vec3 vColor;
layout (location = 0) in vec3 vPosition;
uniform mat4 model; 
out vec4 vertexColor;
void main()
{	
	gl_Position =vec4(0.4 * vPosition.x, 0.4 * vPosition.y , 0.4 * vPosition.z ,1.0);
	vertexColor = vec4(vColor,1.0); }