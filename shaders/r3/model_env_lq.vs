#include "common.h"
#include "skin.h"

struct 	vf
{
	float2 tc0	: TEXCOORD0;		// base
	float3 tc1	: TEXCOORD1;		// environment
	float3 pos_w: TEXCOORD2;
	float3 norm : TEXCOORD3;
	float3 M1	: TEXCOORD4;	
	float3 M2	: TEXCOORD5;
	float3 M3	: TEXCOORD6;	
	float3 c0	: COLOR0;
	float  fog	: FOG;
	float4 hpos	: SV_Position;
};

struct wivs_in
{
	float4	P		: POSITION;		
	float4	N		: NORMAL;		
	float4	T		: TANGENT;
	float4	B		: BINORMAL;
	float4	color	: COLOR0;		
	int2	uv		: TEXCOORD0;	
};

vf 	_main (wivs_in v)
{
	vf 		o;

	float4 	pos 	= v.P;
	float3  pos_ww 	= mul			(m_W, pos);
	float3 	norm_w 	= normalize 		(mul(m_W,v.N));

	//o.tbase                = unpack_tc_base        (v.uv,v.T.w,v.B.w);
	o.pos_w		= pos.xyz;
	o.hpos 		= mul			(m_WVP, pos);		// xform, input in world coords
	o.tc0		= v.tc.xy;					// copy tc
	o.tc1		= calc_reflection	(pos_ww, norm_w);
	o.norm		= norm_w; 
	o.c0 		= calc_model_lq_lighting(norm_w);
	
	float3          N         = unpack_bx2(v.N);        // just scale (assume normal in the -.5f, .5f)
    float3          T         = unpack_bx2(v.T);        //
    float3          B         = unpack_bx2(v.B);        //
    float3x3 xform        = mul        ((float3x3)m_W, float3x3(
                                                T.x,B.x,N.x,
                                                T.y,B.y,N.y,
                                                T.z,B.z,N.z
												));
	o.M1                 = xform        [0];
    o.M2                 = xform        [1];
    o.M3                 = xform        [2];
	
	o.fog 		= saturate(calc_fogging(float4(pos_ww,1)));	// fog, input in world coords

	return o;
}

/////////////////////////////////////////////////////////////////////////
#ifdef 	SKIN_NONE
vf	main(v_model v) 		{ return _main(v); 		}
#endif

#ifdef 	SKIN_0
vf	main(v_model_skinned_0 v) 	{ return _main(skinning_0(v)); }
#endif

#ifdef	SKIN_1
vf	main(v_model_skinned_1 v) 	{ return _main(skinning_1(v)); }
#endif

#ifdef	SKIN_2
vf	main(v_model_skinned_2 v) 	{ return _main(skinning_2(v)); }
#endif

#ifdef	SKIN_3
vf	main(v_model_skinned_3 v) 	{ return _main(skinning_3(v)); }
#endif

#ifdef	SKIN_4
vf	main(v_model_skinned_4 v) 	{ return _main(skinning_4(v)); }
#endif