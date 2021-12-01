#ifndef	common_defines_h_included
#define	common_defines_h_included

//////////////////////////////////////////////////////////////////////////////////////////
// Defines                                		//

#define H_MAIN		7.0f
#define H_TERR		0.5f*H_MAIN
#define H_GRASS		1.0f*H_MAIN
#define H_MODELS	0.3f*H_MAIN
#define H_BUSHES	2.0f*H_MAIN

#define L_RANGE		1.0f
#define L_BRIGHT	1.0f

#define SKY_WITH_DEPTH				// sky renders with depth to avoid some problems with reflections
#define SKY_DEPTH float(10000.f)
#define SKY_EPS float(0.001)
#define FARPLANE float(180.0)

#define def_gloss       float(2.f /255.f)
#define def_aref        float(200.f/255.f)
#define def_dbumph      float(0.333f)
#define def_virtualh    float(0.05f)              // 5cm
#define def_distort     float(0.05f)             // we get -0.5 .. 0.5 range, this is -512 .. 512 for 1024, so scale it
#define def_hdr         float(9.h)         		// hight luminance range float(3.h)
#define def_hdr_clip	float(0.75h)        		//
#define def_lum_hrange	float(0.7h)	// hight luminance range

#define	LUMINANCE_VECTOR	float3(0.3f, 0.48f, 0.22f)

#define MAXCOF		7.h
#define EPSDEPTH	0.001h

#define SCR_WIDTH screen_res.x
#define SCR_HEIGHT screen_res.y
#define PIXEL_SIZE float2( 1/SCR_WIDTH, 1/SCR_HEIGHT ) 

//////////////////////////////////////////////////////////////////////////////////////////
#ifndef SMAP_size
#define SMAP_size        4096
#endif
#define PARALLAX_H 0.02
#define parallax float2(PARALLAX_H, -PARALLAX_H/2)
//////////////////////////////////////////////////////////////////////////////////////////

#define USE_GRASS_WAVE								// enable grass wave
#define GRASS_WAVE_FREQ 	float(0.7)				// frequency of waves
#define GRASS_WAVE_POWER 	float(3.0)				// brightness of waves
#define MAT_FLORA 0.47451

#define ROUGHNESS_LOW 0.65	// 0.45	// 0.75
#define ROUGHNESS_HIGH 1.00
#define ROUGHNESS_POW 0.65	// 0.45	// 0.65

#define SPECULAR_BASE 0.056	// 0.056
#define SPECULAR_RANGE 1	// 1
#define SPECULAR_POW 10	// 10

#endif	//	common_defines_h_included