#vertex shader
#version 330 core
layout (location = 0)in vec3 aPos;

uniform mat4 u_ViewMatrix;
uniform mat4 u_ProjectMatrix;
uniform mat4 u_ModelMatrix;
void main()
{
	gl_Position=u_ModelMatrix*vec4(aPos.x,aPos.y,aPos.z,1.0);
}


#fragment shader
#version 330 core

uniform vec3 u_Color;

out vec4 FragColor;

void main()
{
	
	FragColor=vec4(u_Color,1.0f);
}