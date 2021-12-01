#ifndef        COMMON_H
#define        COMMON_H

#include "shared\common.h"

#include "common_defines.h"
#include "common_policies.h"
#include "common_iostructs.h"
#include "common_samplers.h"
#include "common_cbuffers.h"
#include "common_functions.h"
#include "common_gbuffer.h"
uniform float4		temp;

//Control
uniform 	float4 		rain_drops_params0; //rain density, drops power debug, env far plane, fog distance
											//								   mask, drops - for rain drops phase
uniform 	float4 		control_params; //mask, drops, wetness, sslr
uniform 	float4 		wenv_params; //fog density, wind velocity, shafts intensity, wind dir
uniform 	float4 		pp_control_params; //postprocess, chromatic abberation, bloom, dof
uniform 	float4 		sky_water_control_params; //rainbow, ripples, mudfoam, 0
												//	sslr samples,			   sslr - water phase
uniform 	float4		pp_control_params1; //metall refls, fake tonemap, vignette, vignette power 
												
uniform 	float4 		technicolor_params1; //amount, power, r, g
uniform 	float4 		technicolor_params2; //b, luma power, 0, 0
uniform 	float4 		chrom_hdr_luma_params; //chromatic abberation factor, hdr factor, luma factor, luma pattern
uniform 	float4 		dpx_params1; //r, g, b, gamma
uniform 	float4 		dpx_params2; //saturation, power, 0, 0
uniform 	float4 		curves_sslr_params; //curves formula, curves contrast, sslr samples, chromatic abberation power

#define USE_SUNMASK                		//- shader defined


#ifdef        USE_R2_STATIC_SUN
#  define xmaterial float(1.0h/4.h)
#else
#  define xmaterial float(L_material.w)
#endif

// Global shader options --#SM+#--
uniform float4x4	m_script_params; 
uniform	float4		m_hud_params;	// zoom_rotate_factor, secondVP_zoom_factor, NULL, NULL
uniform	float4		m_blender_mode;	// x\y = [0 - default, 1 - night vision, 2 - thermal vision]; x - main viewport, y - second viewport, z = ?, w = [0 - renders a regular object, 1 - rendering of detailed objects (grass, trash)]

// Is dual rendering active? --#SM+#--
inline bool isSecondVPActive()
{
	return (m_blender_mode.z == 1.f);
}

// Returns 1.f if the second viewport is currently rendering --#SM+#--
inline bool isSecondVP()
{
	return m_blender_mode.z > 0.5f;
}

// Returns the blending mode for the current viewport. --#SM+#--
float blender_mode()
{
	float ret = m_blender_mode.x;
	
	if (isSecondVP() == true)
		ret = m_blender_mode.y;
		
	return ret;
}

// At the moment, detailed elements are being rendered (grass, garbage) --#SM+#--
inline bool isDetailRender()
{
	return (m_blender_mode.w == 1.f);
}

// Thermal mode is on --#SM+#--
inline bool isThermalMode()
{
	return (blender_mode() > 5.f);
}

/*
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
// #define USE_SUNMASK                		//- shader defined
// #define DBG_TMAPPING
//////////////////////////////////////////////////////////////////////////////////////////

uniform float4                J_direct        [6];
uniform float4                J_spot                [6];

float2         calc_detail                (float3 w_pos)      {
        float                 dtl        = distance                (w_pos,eye_position)*dt_params.w;
                              dtl        = min              (dtl*dtl, 1);
        float                  dt_mul     = 1  - dtl;        // dt*  [1 ..  0 ]
        float                  dt_add     = .5 * dtl;        // dt+  [0 .. 0.5]
        return                float2      (dt_mul,dt_add);
}


//////////////////////////////////////////////////////////////////////////////////////////

#ifdef  USE_HWSMAP
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

//////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
struct  				p_screen                {
        float4          hpos               		: POSITION;
        float2          tc0                		: TEXCOORD0;        // Texture coordinates         (for sampling maps)
};


//////////////////////////////////////////////////////////////////////////////////////////

float3	v_hemi_wrap     (float3 n, float w)                	{        return L_hemi_color*(w + (1-w)*n.y);                   }
float3	v_sun_wrap      (float3 n, float w)                	{        return L_sun_color*(w+(1-w)*dot(n,-L_sun_dir_w));      }
*/
#define FXPS technique _render{pass _code{PixelShader=compile ps_3_0 main();}}
#define FXVS technique _render{pass _code{VertexShader=compile vs_3_0 main();}}

#endif
