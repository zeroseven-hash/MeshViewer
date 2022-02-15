#vertex shader
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos, 1.0);
}



#fragment shader
#version 330 core
layout(location=0)out vec4 FragColor;
//layout(location=1)out vec4 Color;
in vec2 TexCoords;
struct Material
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

uniform Material material;


uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D ssao;
//for integer sampler
uniform isampler2D gIndex;

uniform vec3 lightColor;
uniform vec3 lightPos;

uniform bool IsSSAO;

uniform float lightAmbient;
uniform float lightdiffuse;
uniform float lightspec;


void main()
{            
	vec3 FragPos=texture(gPosition,TexCoords).rgb;
	vec3 o_Normal=texture(gNormal,TexCoords).rgb;
	vec3 o_Color=texture(gAlbedo,TexCoords).rgb;
	int index = texture(gIndex,TexCoords).r;

	if(index==-1)
		FragColor=vec4(FragPos,1.0f);
	else
	{
		vec3 ambient;
		if(IsSSAO)
		{
			float AmbientOcclusion=texture(ssao,TexCoords).r;
			ambient=lightAmbient*material.ambient.x*o_Color*AmbientOcclusion;
		}
		else
			ambient=lightAmbient*material.ambient.x*o_Color;

		vec3 lightDir=normalize(lightPos-FragPos);
		float diff=max(dot(o_Normal,lightDir),0.0);
		vec3 diffuse=lightdiffuse*diff*material.diffuse*lightColor*o_Color;

		
		vec3 viewDir=normalize(-FragPos);
		vec3 reflectDir=reflect(-lightDir,o_Normal);
		
		float spec=pow(max(dot(viewDir,reflectDir),0.0),material.shininess);

		vec3 specular=lightdiffuse*(material.specular*spec)*lightColor;

		vec3 result = ambient+diffuse+specular;

		
		FragColor=vec4(result,1.0f);
	}

	//FragColor=vec4(vec3(AmbientOcclusion),1.0f);
	//Color=vec4(vec3(viewDir),1.0f);
	//FragColor=vec4(o_Color*AmbientOcclusion,1.0f);
	
}