///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Dynamic Shaders 1.2 Beta - ShaderSettingsVisor - Default Preset - By Meltac
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Feature Enable/Disable: Comment a feature (Add the //) to disable, or uncomment (Remove the //) to enable.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef VIS_Mount
#define VIS_Mount

//////////////////////////////////////////////////////////////
// Visor Overlay and Refraction / Reflection
//////////////////////////////////////////////////////////////

#define VIS									// Enables/disables the visor reflection effect
#define VIS_XFACTOR float(2.0)
#define VIS_YFACTOR float(1.3)
#define VIS_CRUNCHINTENSITY float (0.35)	// Intensity of the peripheral/radial crunch effect in the visor
#define VIS_MIRRORINTENSITY float(0.25)		// Intensity of the mirroring effect in the visor
#define VIS_RADIALCOEFF float(1.0)			// Increment power of radial effect towards periphery (1.0 = linearly incrementing)
#define VIS_BREAK float(1.4)				// Was 0.9
#define VIS_MIRRORMINDIST float (2.50)		// Objects nearer than this values won't be mirror. Use 2.5 to prevent the player's weapon to be mirrored too much.
//#define VIS_BLUR float (2.5)				// Blur amount of the mirroring effect. Caution: Slows down startup / loading times heavily!
//#define VIS_BLOOM							// Use bloom for the mirroring effect. r2_ls_bloom_fast must be on, use_bloom must be true in visor_control.script. If enabled, increase VIS_MIRRORINTENSITY.
#define VIS_SCREENRATIO float(0.625)		// Screen Ratio; use 0.75 for 4:3 (normalscreen) and 0.625 for 16:10 (widescreen) resolution
#define VIS_SATURATION float(0.6)			// Level of color saturation before applying visor mirror color (if other than (1,1,1)). 0 is fully desaturated, 1 is original color
#define VIS_SAT_SUN_COEF float(0.5)			// How much influence sun lighting has on the visor mirror color saturation.
#define VIS_CONTRAST float(1.0)				// Contrast ratio of mirrored color (1.0 = normal, 0.0 = none).
//#define VIS_COLOR float4(0.9,0.8,0.7,1.0)	// Color filter applied to visor ("sunglass effect") (RGBA)
#define VIS_COLOR float4(1,1,1,1)			// Color filter applied to visor ("sunglass effect") (RGBA)
#define VIS_COLORCOEFF float(2.0)			// Power of color filter of mirrored object (1.0 = linear, 2.0 = square)

// visor border settings
  #define VIS_CENTER float2(0.0,0.0)		// Y-axix center of border black out.
  #define VIS_BLACKASPECT float2(1.5,0.1)		// Aspect ratio of border black out.
  #define VIS_BLACKRADIUSMIN float2(0,0)	// Screen radius at which border black out begins.
  #define VIS_BLACKRADIUSMAX float2(0,0)	// Screen radius at which border black out is complete.

//#define VIS_BLACKSMOOTH						// Defines whether to use smoothing on border black out. Costs a few FPS.


//////////////////////////////////////////////////////////////
// Visor Water Drops
//////////////////////////////////////////////////////////////

// Basic Settings
#define VSD_MIX 1.0f							// Amount/mix between original and modified image
#define VSD_ASPECT float(1.5)					// screen aspect ratio (change if not using wide-screen), makes drops round or not

// Advanced Settings
#define VSD_RADIUS float(0.055)					// primary drops pinch / magnification reference radius
#define VSD_INNERRADIUS float(0.005)			// secondary drops pinch / magnification  reference radius
#define VSD_STRENGTH float(100.0)				// intensity of the pinch / magnification effect inside the drops
#define VSD_DEFORM float(0.0080)	// 00350	// drops deformation amount 
#define VSD_SMOOTH 0.0f							// smoothness of drops rendering (not sure...) - looks awful, leave it at 0.0 for the moment
#define VSD_LIGHT 0.20f //0.30f							// Amount of lighting impact. Values 0-1. 0=no lighting, 1=full lighting.
#define VSD_FIXLIGHT float3(.5, .5, 1.)			// fixed light source position to illuminate drops

// Turbulence Settings (make drops render irregularily / more "natural")
#define VSD_TURBCOF 0.4 //0.33 //0.39					// Amount of drop turbulence
#define VSD_TURBSHIFT float4(0.35,1.00,0,1)		// Position shift for drop turbulence
#define VSD_TURBTIME sin(0.08/3) // 0.1			// Time shift for drop turbulence
#define VSD_TURBSIZE 15.f						// Size of drop turbulence 

// Wave Warp Settings (required to spread drops over the screen unorderedly, do not change normally)
#define VSD_WSIZE 128.f		// 32-256			// Wave size (amount of drops spreading)
#define VSD_WTIME .1f			// 0-1			// Wave time (just some value)
#define VSD_WVAL1 0.128f		// 0-1			// Wave value 1 (just some value)
#define VSD_WVAL2 0.64f		// 0-1				// Wave value 2 (just some value)
#define VSD_WVAL3 0.192f		// 0-1			// Wave value 3 (just some value)
#define VSD_WVAL4 7.f			// 0-10			// Wave value 4 (just some value)

#endif