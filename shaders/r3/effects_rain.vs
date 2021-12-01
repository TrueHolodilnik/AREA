#include "common.h"

//By Holodilnik

struct 	vf
{
	float2 tc0	: TEXCOORD0;		// base
	float2 tcss	: TEXCOORD1;
	float4 color	: COLOR0; 
	float4 fade	 : COLOR1; 
	//float  fog	: FOG;
	float4 hpos	: SV_Position; 
};

Texture2D s_nmap;

vf main ( v_TL I)
{
	vf O;

	O.hpos = mul( m_WVP, I.P);
	O.tc0 = I.Tex0;
	O.color = I.Color.bgra;	//	swizzle vertex colour
	
	//float3  pos_w 	= mul			(m_W, I.P);
	
	float4	p_posi	   = mul(m_VP, float4(I.P.xyz, 1));
	float4	ss_tc 	   = proj_to_screen(p_posi);
			ss_tc.xy  /= ss_tc.w;
			
	O.tcss = ss_tc.xy;
	
	//O.fog 		= saturate(calc_fogging(float4(pos_w,1)));
	
	float4  norm = s_nmap.SampleLevel( smp_base, I.Tex0, 0);
	float3  dir_v   = normalize    (mul(m_WV,I.P));
	float3  norm_v  = normalize    (mul(m_WV,norm));
	float   f    = 0.9*abs      (dot(dir_v,norm_v));
	
	O.fade = f;
	O.fade.a = mul(m_V, I.P).z;
	

 	return O;
}