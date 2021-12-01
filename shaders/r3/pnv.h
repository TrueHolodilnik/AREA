//////////////////////////////////////////////////////////////////////////////////////////
//NV Shader by LVutner (basing on yaz NV)
//Last edit by Nuor (24.10.2019)
//////////////////////////////////////////////////////////////////////////////////////////

//defines
//#define NV_BRIGHTNESS 5.0

//effects
//#define NV_FLICKERING
//#define NV_NOISE
//#define NV_VIGNETTE
//#define NV_SCANLINES

//effect settings
#define FLICKERING_INTENSITY 0.0035
#define FLICKERING_FREQ 60.0
#define NOISE_INTENSITY 0.05
#define SCANLINES_INTENSITY 0.02 //0.175
#define VIGNETTE_RADIUS 1.0

float4 calc_night_vision_effect(float2 tc0, float4 color:COLOR, float3 NV_COLOR, float intensity, bool lines)
{
	float 	NV_BRIGHTNESS = intensity;
	bool	NV_SCANLINES = lines;
	bool	NV_FLICKERING = lines;
	bool	NV_NOISE = lines;
	bool	NV_VIGNETTE = lines;

    float lum = dot(color.rgb, float3( 0.3f, 0.38f, 0.22f)*NV_BRIGHTNESS );  //instead of float3 use LUMINANCE_floatTOR in stalker
    color.rgb = NV_COLOR*lum;
    
    //cheap noise function
    float noise  = frac(sin(dot(tc0, float2(12.0, 78.0) + (timers.x*1.17f) )) * 43758.0); 
    
    //////////////////////////////////////////////////////////////////////////////////////////
    // scanlines
    #ifdef NV_SCANLINES
    color += SCANLINES_INTENSITY*sin(tc0.y*screen_res.y*2.0);
    #endif
    //////////////////////////////////////////////////////////////////////////////////////////
    // noise
    #ifdef NV_NOISE
    color  += noise * NOISE_INTENSITY; 
    #endif
    //////////////////////////////////////////////////////////////////////////////////////////
    // screen flickering
    #ifdef NV_FLICKERING
    color += FLICKERING_INTENSITY * sin(timers.x*FLICKERING_FREQ);   
    #endif
    //////////////////////////////////////////////////////////////////////////////////////////
    // vignette
    #ifdef NV_VIGNETTE
	color *= VIGNETTE_RADIUS-(distance(tc0.xy,float2( 0.5f, 0.5f)));
   	#endif
	
   	return color; 
}
