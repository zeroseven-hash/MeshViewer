#vertex shader
#version 450 core
layout(location=0) in vec3 aPos;
layout(location=1) in vec3 aColor;
layout(location=2) in vec3 aNormal;
layout(location=3) in int aEntityId;

out vec3 FragPos;
out vec3 Normal;
out vec3 Color1;
out flat int EntityId;

uniform mat4 u_ProjectMatrix;
uniform mat4 u_ModelMatrix;
uniform mat4 u_ViewMatrix;




void main()
{
	vec4 viewPos=u_ViewMatrix*u_ModelMatrix*vec4(aPos,1.0f);
	FragPos=viewPos.xyz;

	mat3 normalMatrix=transpose(inverse(mat3(u_ViewMatrix*u_ModelMatrix)));

	Normal=normalMatrix*aNormal;

	Color1=aColor;
	EntityId=aEntityId;
	gl_Position=u_ProjectMatrix*viewPos;
}


#fragment shader
#version 450 core
layout(location=0) out vec4 gPosition;
layout(location=1) out vec4 gNormal;
layout(location=2) out vec4 gAlbedo;
layout(location=3) out int gIndex;

in flat int EntityId;
in vec3 FragPos;
in vec3 Normal;
in vec3 Color1;
void main()
{
	gPosition=vec4(FragPos,1.0f);
	gNormal=vec4(normalize(Normal),1.0f);
	gAlbedo=vec4(Color1,1.0f);
	gIndex=EntityId;
}
