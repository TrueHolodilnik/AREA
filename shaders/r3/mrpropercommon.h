#ifndef EXTCOMMON_H_INCLUDED
#define EXTCOMMON_H_INCLUDED

//////////////////////////////////////////////////////////////////////////////////////////
//
#define		MrProperDepthConstant		float(100)//COP
struct		AntiAliasingStruct
{
	float4		texCoord0		:		TEXCOORD0;
	float4		texCoord1		:		TEXCOORD1;
	float4		texCoord2		:		TEXCOORD2;
	float4		texCoord3		:		TEXCOORD3;
	float4		texCoord4		:		TEXCOORD4;
	float4		texCoord5 : TEXCOORD5;
	float4		texCoord6 : TEXCOORD6;
};

struct		OUTStruct
{
	float4		Position		:		SV_Target0;
	float4		Normal		:		SV_Target1;
};
// Rain drops
uniform	Texture2D sScene;
uniform	Texture2D sPosition;
uniform	Texture2D sMask;          //sunshafts occlusion mask
uniform	Texture2D sMaskBlur;      //smoothed mask
uniform	Texture2D sSunShafts;     //

uniform float4 SSParams;
uniform float4 SSParamsDISPLAY;

Texture2D s_rain_drops0;
//#define RAIN_DROPS_DEBUG

struct OptimizeAA
{
	float2 texCoord0 : TEXCOORD0;
};


float		NormalizeDepth(float Depth)
{
	return saturate(Depth/MrProperDepthConstant);
}


#endif//EXTCOMMON_H_INCLUDED