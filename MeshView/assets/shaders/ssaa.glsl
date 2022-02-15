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

in vec2 TexCoords;

uniform sampler2D SuperSampleTexture;
uniform int SSlevel;

void main()
{            
	vec2 texelSize=1.0/vec2(textureSize(SuperSampleTexture,0));
	int sampleCount=SSlevel/2;
	vec4 result = vec4(0.0,0.0,0.0,0.0);
    for (int x = -sampleCount; x < sampleCount; ++x) 
    {
        for (int y = -sampleCount; y < sampleCount; ++y) 
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(SuperSampleTexture, TexCoords + offset).rgba;
        }
    }
   
    FragColor = result / pow(SSlevel,2);
	//FragColor=texture(SuperSampleTexture,TexCoords);
}