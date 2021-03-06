#ifndef        COMMON_H
#define        COMMON_H

// #define USE_SUPER_SPECULAR

#define H_MAIN		7.0f
#define H_TERR		0.5f*H_MAIN
#define H_GRASS		0.5f*H_MAIN
#define H_MODELS	0.2f*H_MAIN
#define H_BUSHES	0.2f*H_MAIN

#define L_RANGE		1.0f
#define L_BRIGHT	1.0f

#define TEX_CONT_COEF float(0.9)
#define TEX_CONT_ADD float(0.0)
#include "shared\common.h"
#include "_shaders_config.h"
//////////////////////////////////////////////////////////////////////////////////////////
// *** options

// #define DBG_TEST_NMAP
// #define DBG_TEST_NMAP_SPEC
// #define DBG_TEST_SPEC
// #define DBG_TEST_LIGHT
// #define DBG_TEST_LIGHT_SPEC

// #define USE_GAMMA_22
// #define USE_SJITTER
// #define USE_SUNFILTER
// #define USE_FETCH4
// #define USE_MBLUR                	//- HW-options defined
// #define USE_HWSMAP                	//- HW-options defined

// #define USE_HWSMAP_PCF				//- nVidia GF3+, R600+

// #define USE_BRANCHING        		//- HW-options defined
// #define USE_VTF                		//- HW-options defined, VertexTextureFetch
// #define FP16_FILTER                	//- HW-options defined
// #define FP16_BLEND                	//- HW-options defined
//
// #define USE_PARALLAX                	//- shader defined
// #define USE_TDETAIL                	//- shader defined
// #define USE_LM_HEMI                	//- shader defined
// #define USE_DISTORT                	//- shader defined
 #define USE_SUNMASK                		//- shader defined
// #define DBG_TMAPPING
//////////////////////////////////////////////////////////////////////////////////////////
#ifndef SMAP_size
#define SMAP_size        4096
#endif
#define PARALLAX_H 0.03
#define parallax float2(PARALLAX_H, -PARALLAX_H/2)

#ifdef        USE_R2_STATIC_SUN
#  define xmaterial half(1.0h/4.h)
#else
#  define xmaterial half(L_material.w)
#endif
#define SCR_WIDTH screen_res.x
#define SCR_HEIGHT screen_res.y
#define PIXEL_SIZE float2( 1/SCR_WIDTH, 1/SCR_HEIGHT ) 
//////////////////////////////////////////////////////////////////////////////////////////
uniform half4                hemi_cube_pos_faces;
uniform half4                hemi_cube_neg_faces;
uniform half4                L_material;                            // 0,0,0,mid
uniform half4                Ldynamic_color;                      // dynamic light color (rgb1)        - spot/point
uniform half4                Ldynamic_pos;                       // dynamic light pos+1/range(w) - spot/point
uniform half4                Ldynamic_dir;                        // dynamic light direction         - sun
uniform	float4				 screen_res;
uniform half4                J_direct        [6];
uniform half4                J_spot                [6];

half          calc_fogging               (half4 w_pos)      { return dot(w_pos,fog_plane);         }
half2         calc_detail                (half3 w_pos)      {
        float                 dtl        = distance                (w_pos,eye_position)*dt_params.w;
                              dtl        = min              (dtl*dtl, 1);
        half                  dt_mul     = 1  - dtl;        // dt*  [1 ..  0 ]
        half                  dt_add     = .5 * dtl;        // dt+  [0 .. 0.5]
        return                half2      (dt_mul,dt_add);
}
float3         calc_reflection     (float3 pos_w, float3 norm_w)
{
    return reflect(normalize(pos_w-eye_position), norm_w);
}

float3        calc_sun_r1                (float3 norm_w)    { return L_sun_color*saturate(dot((norm_w),-L_sun_dir_w));                 }
float3        calc_model_hemi_r1         (float3 norm_w)    { return max(0,norm_w.y)*L_hemi_color;                                         }
float3        calc_model_lq_lighting     (float3 norm_w)    { return L_material.x*calc_model_hemi_r1(norm_w) + L_ambient + L_material.y*calc_sun_r1(norm_w);         }

// ???????????????????? ?????????????????? ???????????????? --#SM+#--
uniform float4x4	m_script_params; 
uniform	half4		m_hud_params;	// zoom_rotate_factor, secondVP_zoom_factor, NULL, NULL
uniform	half4		m_blender_mode;	// x\y = [0 - default, 1 - night vision, 2 - thermal vision]; x - ???????????????? ??????????????, y - ???????????? ????????????????, z = ?, w = [0 - ???????? ???????????? ???????????????? ??????????????, 1 - ???????? ???????????? ?????????????????? ???????????????? (??????????, ??????????)]

// ??????????????????, ???????????????????? ?????? ???????????? ?????????????? --#SM+#--
uniform float4x4	m_obj_camo_data; 
uniform half4		m_obj_custom_data;
uniform half4		m_obj_generic_data;

// ??????????????-???? ?????????????? ???????????? --#SM+#--
inline bool isSecondVPActive()
{
	return (m_blender_mode.z == 1.f);
}

// ???????????????????? 1.f, ???????? ???????????? ???????? ???????????? ?????????????? ???????????????? --#SM+#--
inline bool isSecondVP()
{
	return m_blender_mode.z > 0.5f;
}

// ???????????????????? ?????????? ?????????????????? ?????? ???????????????? ???????????????? --#SM+#--
float blender_mode()
{
	float ret = m_blender_mode.x;
	
	if (isSecondVP() == true)
		ret = m_blender_mode.y;
		
	return ret;
}

// ?? ???????????? ???????????? ???????????????????? ?????????????????? ???????????????? (??????????, ??????????) --#SM+#--
inline bool isDetailRender()
{
	return (m_blender_mode.w == 1.f);
}

// ?????????????? ??????????-?????????? --#SM+#--
inline bool isThermalMode()
{
	return (blender_mode() > 2.f);
}

//////////////////////////////////////////////////////////////////////////////////////////
struct         v_static                {
        float4      P                	: POSITION;                        // (float,float,float,1)
        float4      Nh                	: NORMAL;                // (nx,ny,nz,hemi occlusion)
        float4      T                 	: TANGENT;                // tangent
        float4      B                 	: BINORMAL;                // binormal
        float2      tc                	: TEXCOORD0;        // (u,v)
        float2      lmh                	: TEXCOORD1;        // (lmu,lmv)
        float4      color               : COLOR0;                        // (r,g,b,dir-occlusion)
};

struct         v_tree               	{
        float4      P                	: POSITION;                // (float,float,float,1)
        float4      Nh                	: NORMAL;                // (nx,ny,nz)
        float3      T                 	: TANGENT;                // tangent
        float3      B                 	: BINORMAL;                // binormal
        float4      tc                	: TEXCOORD0;        // (u,v,frac,???)
};

struct         v_model                 	{
        float4      P                   : POSITION;                // (float,float,float,1)
        float3      N                	: NORMAL;                // (nx,ny,nz)
        float3      T                	: TANGENT;                // (nx,ny,nz)
        float3      B                	: BINORMAL;                // (nx,ny,nz)
        float2      tc                	: TEXCOORD0;        // (u,v)
};

struct        v_detail                    {
        float4      pos                : POSITION;                // (float,float,float,1)
        int4        misc        : TEXCOORD0;        // (u(Q),v(Q),frac,matrix-id)
};

#ifdef  USE_HWSMAP
struct         v_shadow_direct_aref
{
        float4      hpos:        POSITION;       // Clip-space position         (for rasterization)
        float2      tc0:        TEXCOORD1;       // Diffuse map for aref
};
struct         v_shadow_direct
{
        float4      hpos:        POSITION;       // Clip-space position         (for rasterization)
};
#else
struct         v_shadow_direct_aref
{
        float4      hpos:        POSITION;       // Clip-space position         (for rasterization)
        float       depth:         TEXCOORD0;     // Depth
        float2      tc0:        TEXCOORD1;       // Diffuse map for aref
};
struct         v_shadow_direct
{
        float4      hpos:        POSITION;       // Clip-space position         (for rasterization)
        float       depth:         TEXCOORD0;     // Depth
};


#endif

//////////////////////////////////////////////////////////////////////////////////////////
struct         p_bumped        {
        float4      hpos        : POSITION;
#if defined(USE_R2_STATIC_SUN) && !defined(USE_LM_HEMI)
        float4            tcdh        : TEXCOORD0;        // Texture coordinates,         w=sun_occlusion
#else
        float2            tcdh        : TEXCOORD0;        // Texture coordinates
#endif
        float4      position    : TEXCOORD1;        // position + hemi
        float3       M1         : TEXCOORD2;        // nmap 2 eye - 1
        float3       M2         : TEXCOORD3;        // nmap 2 eye - 2
        float3       M3         : TEXCOORD4;        // nmap 2 eye - 3
#ifdef USE_TDETAIL
        float2      tcdbump          : TEXCOORD5;        // d-bump
    #ifdef USE_LM_HEMI
        float2      lmh             : TEXCOORD6;        // lm-hemi
    #endif
#else
    #ifdef USE_LM_HEMI
        float2      lmh             : TEXCOORD5;        // lm-hemi
    #endif
#endif
};
//////////////////////////////////////////////////////////////////////////////////////////
struct         p_flat                  {
        float4                 hpos        : POSITION;
#if defined(USE_R2_STATIC_SUN) //&& !defined(USE_LM_HEMI)
    float4                    tcdh        : TEXCOORD0;        // Texture coordinates,         w=sun_occlusion
#else
    float2                    tcdh        : TEXCOORD0;        // Texture coordinates
#endif
        float4                position        : TEXCOORD1;        // position + hemi
        half3                N                : TEXCOORD2;        // Eye-space normal        (for lighting)
  #ifdef USE_TDETAIL
        float2                tcdbump                : TEXCOORD3;        // d-bump
    #ifdef USE_LM_HEMI
        float2         lmh      : TEXCOORD4;        // lm-hemi
    #endif
  #else
    #ifdef USE_LM_HEMI
        float2         lmh      : TEXCOORD3;        // lm-hemi
    #endif
  #endif
};

//////////////////////////////////////////////////////////////////////////////////////////
struct                  f_deffer        		{
        half4           position        		: COLOR0;        // px,py,pz, m-id
        half4           Ne                		: COLOR1;        // nx,ny,nz, hemi
        half4       	C                		: COLOR2;        // r, g, b,  gloss
};
//////////////////////////////////////////////////////////////////////////////////////////
struct  				p_screen                {
        float4          hpos               		: POSITION;
        float2          tc0                		: TEXCOORD0;        // Texture coordinates         (for sampling maps)
};
//////////////////////////////////////////////////////////////////////////////////////////
// Geometry phase / deferring               	//
uniform sampler2D       s_base;             	//
uniform sampler2D       s_bump;             	//
uniform sampler2D       s_bumpX;                //
uniform sampler2D       s_detail;               //
uniform sampler2D       s_detailBump;           //
uniform sampler2D       s_detailBumpX;          //	Error for bump detail
uniform sampler2D       s_bumpD;                //
uniform sampler2D       s_hemi;             	//

uniform sampler2D       s_mask;                	//

uniform sampler2D       s_dt_r;                	//
uniform sampler2D       s_dt_g;                	//
uniform sampler2D       s_dt_b;                	//
uniform sampler2D       s_dt_a;                	//

uniform sampler2D       s_dn_r;                	//
uniform sampler2D       s_dn_g;                	//
uniform sampler2D       s_dn_b;                	//
uniform sampler2D       s_dn_a;                	//

//////////////////////////////////////////////////////////////////////////////////////////
// Lighting/shadowing phase                     //
uniform sampler2D       s_depth;                //
uniform sampler2D       s_position;             //
uniform sampler2D       s_normal;               //
uniform sampler         s_lmap;             	// 2D/cube projector lightmap
uniform sampler3D       s_material;             //
uniform sampler1D       s_attenuate;        	//
//////////////////////////////////////////////////////////////////////////////////////////
// Combine phase                                //
uniform sampler2D       s_diffuse;              // rgb.a = diffuse.gloss
uniform sampler2D       s_accumulator;      	// rgb.a = diffuse.specular
uniform sampler2D       s_generic;              //
uniform sampler2D       s_bloom;                //
uniform sampler         s_image;                // used in various post-processing
uniform sampler2D       s_tonemap;              // actually MidleGray / exp(Lw + eps)
uniform sampler2D 		s_dirt;
uniform sampler2D 		s_raindrops;
uniform sampler2D 		s_rainstreak;
uniform sampler2D 		s_rainscroll;

uniform sampler2D s_SSLR; // DEBUG
uniform sampler2D s_wetness; // DEBUG

uniform half4		postprocess;
uniform half4		lensprocess;

uniform half4		temp;

//Control
uniform 	float4 		rain_drops_params0; //rain density, drops power debug, env far plane, fog distance
											//								   mask, drops - for rain drops phase
uniform 	float4 		control_params; //mask, drops, wetness, sslr
uniform 	float4 		wenv_params; //fog density, wind velocity, shafts intensity, wind dir
uniform 	float4 		pp_control_params; //postprocess, chromatic abberation, bloom, dof
uniform 	float4 		sky_water_control_params; //rainbow, ripples, mudfoam, 0
												//	sslr samples,			   sslr - water phase
uniform 	float4 		technicolor_params1; //amount, power, r, g
uniform 	float4 		technicolor_params2; //b, luma power, 0, 0
uniform 	float4 		chrom_hdr_luma_params; //chromatic abberation factor, hdr factor, luma factor, luma pattern
uniform 	float4 		dpx_params1; //r, g, b, gamma
uniform 	float4 		dpx_params2; //saturation, power, 0, 0
uniform 	float4 		curves_sslr_params; //curves formula, curves contrast, sslr samples, chromatic abberation power

//////////////////////////////////////////////////////////////////////////////////////////
// Defines                                		//
#define def_gloss       half(2.f /255.f)
#define def_aref        half(200.f/255.f)
#define def_dbumph      half(0.333f)
#define def_virtualh    half(0.05f)              // 5cm
#define def_distort     half(0.05f)             // we get -0.5 .. 0.5 range, this is -512 .. 512 for 1024, so scale it
#define def_hdr         half(9.h)         		// hight luminance range half(3.h)
#define def_hdr_clip	half(0.75h)        		//
#define def_lum_hrange	float(0.7h)	// hight luminance range

//SSLR
uniform float4x4 m_inv_v:
register
(ps,c3); 

struct SQ
{
	float4 hpos:POSITION;
	float2 tc0:TEXCOORD0;
};

float3 getWpos(float2 tc)
{
	float3 pos = tex2D(s_position,tc).xyz;
	float3 Wpos = mul(m_inv_v,float4(pos,1.f)).xyz;
	return Wpos;
}

float3 getWnorm(float2 tc)
{
	float posZ = tex2D(s_position,tc).z;
	float3 norm = tex2D(s_normal,tc).xyz;
	float3 Wnorm = mul(m_inv_v,float4(norm,0.f)).xyz;
	Wnorm.y *= clamp(posZ,1.f,10.f);
	Wnorm = normalize(Wnorm);
	return Wnorm;
}

float4 get_6x6_image(float2 tc)
{
	half 	depth 	= tex2D(s_position, tc).z;
	half    depth_s = depth * (1 - step(0.001, abs(depth - 10000)));

	float4 sample;

	sample.xyz = tex2D(s_image, tc).xyz;

	float red_normal = tex2D(s_image, tc).x;

	float red_low1, red_high1, red_low2,  red_high2,  red_low3,  red_high3,  red_low4,  red_high4, 
		  red_low5, red_high5, red_low6,  red_high6,  red_low7,  red_high7,  red_low8,  red_high8, 
		  red_low9, red_high9, red_low10, red_high10, red_low11, red_high11, red_low12, red_high12;

		  red_low1 	= tex2D(s_image, float2(tc.x,tc.y-screen_res.w*1)).x;	red_high1 	= tex2D(s_image, float2(tc.x,tc.y+screen_res.w*1)).x;
		  red_low2 	= tex2D(s_image, float2(tc.x,tc.y-screen_res.w*4)).x;	red_high2 	= tex2D(s_image, float2(tc.x,tc.y+screen_res.w*4)).x;

		  red_low3 	= tex2D(s_image, float2(tc.x-screen_res.w*4,tc.y)).x;	red_high3 	= tex2D(s_image, float2(tc.x+screen_res.w*4,tc.y)).x;
		  red_low4 	= tex2D(s_image, float2(tc.x-screen_res.w*8,tc.y)).x;	red_high4 	= tex2D(s_image, float2(tc.x+screen_res.w*8,tc.y)).x;

		  red_low5 	= tex2D(s_image, float2(tc.x,tc.y-screen_res.w*8)).x;	red_high5 	= tex2D(s_image, float2(tc.x,tc.y+screen_res.w*8)).x;
		  red_low6 	= tex2D(s_image, float2(tc.x,tc.y-screen_res.w*12)).x;	red_high6 	= tex2D(s_image, float2(tc.x,tc.y+screen_res.w*12)).x;

		  red_low7 	= tex2D(s_image, float2(tc.x-screen_res.w*12,tc.y)).x;	red_high7 	= tex2D(s_image, float2(tc.x+screen_res.w*12,tc.y)).x;
		  red_low8 	= tex2D(s_image, float2(tc.x-screen_res.w*16,tc.y)).x;	red_high8 	= tex2D(s_image, float2(tc.x+screen_res.w*16,tc.y)).x;

		  red_low9 	= tex2D(s_image, float2(tc.x,tc.y-screen_res.w*16)).x;	red_high9 	= tex2D(s_image, float2(tc.x,tc.y+screen_res.w*16)).x;
		  red_low10 = tex2D(s_image, float2(tc.x,tc.y-screen_res.w*20)).x;	red_high10  = tex2D(s_image, float2(tc.x,tc.y+screen_res.w*20)).x;

		  red_low11 = tex2D(s_image, float2(tc.x-screen_res.w*1,tc.y)).x;	red_high11  = tex2D(s_image, float2(tc.x+screen_res.w*1,tc.y)).x;
		  red_low12 = tex2D(s_image, float2(tc.x-screen_res.w*20,tc.y)).x;	red_high12  = tex2D(s_image, float2(tc.x+screen_res.w*20,tc.y)).x;

	if(depth == depth_s)
	{sample.w = 0.012345;}

	if
	(
	    	(red_normal != red_low1)  && (red_normal != red_high1)
		&& 	(red_normal != red_low2)  && (red_normal != red_high2)

		&&	(red_normal != red_low3)  && (red_normal != red_high3)
		&& 	(red_normal != red_low4)  && (red_normal != red_high4)

		&&	(red_normal != red_low5)  && (red_normal != red_high5)
		&& 	(red_normal != red_low6)  && (red_normal != red_high6)

		&&	(red_normal != red_low7)  && (red_normal != red_high7)
		&& 	(red_normal != red_low8)  && (red_normal != red_high8)

		&&	(red_normal != red_low9)  && (red_normal != red_high9)
		&& 	(red_normal != red_low10) && (red_normal != red_high10)

		&&	(red_normal != red_low11) && (red_normal != red_high11)
		&& 	(red_normal != red_low12) && (red_normal != red_high12)
	)
	{ 
		sample.w 	= 1; 
	}
	else
	{
		sample.w 	= 0; 
	}

		sample.w 	= 1;
	return sample;
}


uniform float4 SSLR_params; // x - L, y - error, zw - 1.f
//SSLE end

//////////////////////////////////////////////////////////////////////////////////////////
#define	LUMINANCE_VECTOR                 half3(0.3f, 0.38f, 0.22f)
void        tonemap              (out half4 low, out half4 high, half3 rgb, half scale)
{
        rgb     =      	rgb*scale       ;

		const float fWhiteIntensity = 1.7;

		const float fWhiteIntensitySQR = fWhiteIntensity*fWhiteIntensity;
#ifdef	USE_BRANCHING		// ps_3_0
        //low		=       rgb.xyzz		;

		low		=	( (rgb*(1+rgb/fWhiteIntensitySQR)) / (rgb+1) ).xyzz;

        high	=		low/def_hdr		;        // 8x dynamic range
#else
        low		=       half4           ( ( (rgb*(1+rgb/fWhiteIntensitySQR)) / (rgb+1) ),           0 )	;
        high	=       half4       	(rgb/def_hdr,   0 )	;		// 8x dynamic range
#endif

/*
	rgb		=	rgb*scale;

	low		=	rgb.xyzz;
	high	=	low/def_hdr;	// 8x dynamic range
*/

//		low		= 	half4	(rgb, 0);
//		rgb		/=	def_hdr	;
//		high	= 	half4	(rgb, dot(rgb,0.333f)-def_hdr_clip)		;
}
half4		combine_bloom        (half3  low, half4 high)	{
        return        half4(low + high*high.a, 1.h);
}

float3	v_hemi        	(float3 n)                        	{        return L_hemi_color*(.5f + .5f*n.y);                   }
float3	v_hemi_wrap     (float3 n, float w)                	{        return L_hemi_color*(w + (1-w)*n.y);                   }
float3	v_sun           (float3 n)                        	{        return L_sun_color*dot(n,-L_sun_dir_w);                }
float3	v_sun_wrap      (float3 n, float w)                	{        return L_sun_color*(w+(1-w)*dot(n,-L_sun_dir_w));      }
half3   p_hemi          (float2 tc)                         {
        half4        	t_lmh         = tex2D             	(s_hemi, tc);
        return t_lmh.a;
}

half   get_hemi( half4 lmh)
{
	return lmh.a;
}

half   get_sun( half4 lmh)
{
	return lmh.g;
}

//	contrast function
half Contrast(half Input, half ContrastPower)
{
     //piecewise contrast function
     bool IsAboveHalf = Input > 0.5 ;
     half ToRaise = saturate(2*(IsAboveHalf ? 1-Input : Input));
     half Output = 0.5*pow(ToRaise, ContrastPower);
     Output = IsAboveHalf ? 1-Output : Output;
     return Output;
}

half3 Vibrance( half3 i, half val )
{
	float luminance = dot( float3( i.rgb ), LUMINANCE_VECTOR );
	return half3( lerp( luminance, float3( i.rgb ), val ));
}

half4 sat( half4 i, half val )
{
	float luminance = dot( float3( i.rgb ), LUMINANCE_VECTOR );
	return half4( lerp( luminance, float3( i.rgb ), val ), i.w );
}

half4 proj2screen(half4 Project)
{
	half4	Screen;
			Screen.x = (Project.x + Project.w) * 0.5h;
			Screen.y = (Project.w - Project.y) * 0.5h;
			Screen.z = Project.z;
			Screen.w = Project.w;
			
	return Screen;
}

float2 hash22(float2 p)
{
	float3 p3 = frac(float3(p.xyx) * float3(.1031, .1030, .0973));
    p3 += dot(p3, p3.yzx+19.19);
    return frac((p3.xx+p3.yz)*p3.zy);
}

float hash12(float2 p)
{
	float3 p3  = frac(float3(p.xyx) * .1031);
    p3 += dot(p3, p3.yzx + 19.19);
    return frac((p3.x + p3.y) * p3.z);
}

static float3x3 RGB =
{
	2.67147117265996,-1.26723605786241,-0.410995602172227,
	-1.02510702934664,1.98409116241089,0.0439502493584124,
	0.0610009456429445,-0.223670750812863,1.15902104167061
};

static float3x3 XYZ =
{
	0.500303383543316,0.338097573222739,0.164589779545857,
	0.257968894274758,0.676195259144706,0.0658358459823868,
	0.0234517888692628,0.1126992737203,0.866839673124201
};

#define FXPS technique _render{pass _code{PixelShader=compile ps_3_0 main();}}
#define FXVS technique _render{pass _code{VertexShader=compile vs_3_0 main();}}

#endif
