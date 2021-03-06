#define WATER_VERTEX
#include "common.h"
#include "water_pv.h"
#include "shared\waterconfig.h"
#include "shared\watermove.h"

struct        v_vert
{
	float4	P		: POSITION;		// (float,float,float,1)
	float4	N		: NORMAL;		// (nx,ny,nz,hemi occlusion)
	float4	T		: TANGENT;
	float4	B		: BINORMAL;
	float4	color	: COLOR0;		// (r,g,b,dir-occlusion)
//	float2	uv		: TEXCOORD0;	// (u0,v0)
	int2	uv		: TEXCOORD0;	// (u0,v0)
};

wivs_out main (v_vert v)
{
	v.N		=	unpack_D3DCOLOR(v.N);
	v.T		=	unpack_D3DCOLOR(v.T);
	v.B		=	unpack_D3DCOLOR(v.B);
	v.color	=	unpack_D3DCOLOR(v.color);

        wivs_out                 o;
		o.T = v.T;

        float4         P         = v.P        ;                // world
    float3 NN = unpack_normal(v.N);
                //P         = watermove        (P)        ;

		o.position_w = float4	(P.xyz, 1.0	);
        o.v2point        = P-eye_position        ;
        o.tbase                = unpack_tc_base        (v.uv,v.T.w,v.B.w);                // copy tc
		// o.tnorm0.xy        = watermove_tc                 (o.tbase*W_DISTORT_BASE_TILE_0, P.xz, W_DISTORT_AMP_0);
        //o.tnorm0.zw        = watermove_tc                 (o.tbase*W_DISTORT_BASE_TILE_1, P.xz, W_DISTORT_AMP_1);
        o.tnorm0.xy        = P.xz;
        o.tnorm0.zw        = P.xz;


        // Calculate the 3x3 transform from tangent space to eye-space
        // TangentToEyeSpace = object2eye * tangent2object
        //                     = object2eye * transpose(object2tangent) (since the inverse of a rotation is its transpose)
        float3          N         = unpack_bx2(v.N);        // just scale (assume normal in the -.5f, .5f)
        float3          T         = unpack_bx2(v.T);        //
        float3          B         = unpack_bx2(v.B);        //
        float3x3 xform        = mul        ((float3x3)m_W, float3x3(
                                                T.x,B.x,N.x,
                                                T.y,B.y,N.y,
                                                T.z,B.z,N.z
                                ));
        // The pixel shader operates on the bump-map in [0..1] range
        // Remap this range in the matrix, anyway we are pixel-shader limited :)
        // ...... [ 2  0  0  0]
        // ...... [ 0  2  0  0]
        // ...... [ 0  0  2  0]
        // ...... [-1 -1 -1  1]
        // issue: strange, but it's slower :(
        // issue: interpolators? dp4? VS limited? black magic?

        // Feed this transform to pixel shader
        o.M1                 = xform        [0];
        o.M2                 = xform        [1];
        o.M3                 = xform        [2];

        float3         L_rgb         = v.color.xyz;                                                // precalculated RGB lighting
        float3         L_hemi         = v_hemi(N)*v.N.w;                                        // hemisphere
        float3         L_sun         = v_sun(N)*v.color.w;                                        // sun
        float3         L_final        = L_rgb + L_hemi + L_sun + L_ambient;
                // L_final        = v.N.w        + L_ambient;

        o.hpos                 = mul                        (m_VP, P);                        // xform, input in world coords
		o.fog       = saturate( calc_fogging  (v.P));
		//o.fog		*= o.fog;

		o.c0		= float4		(L_final,1);

	//o.tctexgen = mul( m_texgen, P);

	o.tctexgen.xyz = mul(m_V,  P).xyz;

        return o;
}