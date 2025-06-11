#version 420

//uniform mat4 MVP;
uniform mat4 mProj;
uniform mat4 mView;
uniform mat4 mModel;

uniform bool bUseOverrideColor;
uniform vec3 colorOverride;

in vec3 vCol;
in vec3 vPos;

out vec3 color;

void main()
{
	mat4 MVP = mProj * mView * mModel;

    gl_Position = MVP * vec4(vPos, 1.0);
	
	if (bUseOverrideColor)
	{
		color = colorOverride;
	}
	else
	{
	    color = vCol;
	}
};
