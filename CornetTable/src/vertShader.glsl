

#version 330
uniform vec3  vColor; 
layout (location = 0) in vec3 vPosition;
uniform vec3  alterColor; 
uniform mat4 model; 
out vec4 vertexColor;
void main()
{	
	gl_Position =vec4(0.2 * vPosition.x, 0.2 * vPosition.y , 0.2 * vPosition.z ,1.0);
	vertexColor = vec4(vColor,1.0); 
}