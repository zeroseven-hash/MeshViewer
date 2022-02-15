#vertex shader
#version 450 core
layout(location=0) in vec3 aPos;
layout(location=1) in vec3 aColor;
layout(location=2) in vec3 aNormal;
layout(location=3) in int aEntityId;


out flat int EntityId;

uniform mat4 u_ProjectMatrix;
uniform mat4 u_ModelMatrix;
uniform mat4 u_ViewMatrix;
uniform float u_Scale;


void main()
{
	
	EntityId=aEntityId;
	vec3 normal=normalize(aNormal);
	gl_Position=u_ProjectMatrix*u_ViewMatrix*u_ModelMatrix*vec4(aPos,1.0f)+u_Scale*0.008*normalize(u_ProjectMatrix*u_ViewMatrix*u_ModelMatrix*vec4(normal,0.0f));
}


#fragment shader
#version 450 core
layout(location=2) out vec4 gAlbedo;
layout(location=3) out int gIndex;

in flat int EntityId;
void main()
{
	gAlbedo=vec4(0.541f,0.788f,0.972f,1.0f);
	gIndex=EntityId;
}
