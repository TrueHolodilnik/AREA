#include "common.h"

uniform float4 screen_res;

struct input
{
	float4 P		: POSITIONT;	// 
	float2 Tex0		: TEXCOORD0; 	// UV
};

struct output
{
	float2 Tex0 	: TEXCOORD0;	// UV
	float4 HPos 	: SV_Position;	// Clip-space position 	(for rasterization)
};

output main(input I)
{
	output O;

	{
		O.HPos		= float4((I.P.xy + 0.5) * screen_res.zw, I.P.zw);
		O.HPos.xy	= (O.HPos.xy * 2 - 1) * float2(1, -1);
	}

	O.Tex0 = I.Tex0;

 	return O;
}