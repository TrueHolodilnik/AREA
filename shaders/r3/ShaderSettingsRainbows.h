///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Dynamic Shaders 1.2 Beta - ShaderSettingsRainbows - Default Preset - By Meltac
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Feature Enable/Disable: Comment a feature (Add the //) to disable, or uncomment (Remove the //) to enable.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef RB_Mount
#define RB_Mount

#define RB								// Enables/disables rainbows.
#define RB_INTENSITY float(0.2)			// Absolute maximal intensitiy of the rainbows.
#define RB_SECONDARY float(0.1)			// Relative intensitiy of the secondary rainbow.
#define RB_SATURATION float(0.75)		// Saturation of the rainbows.
#define RB_DECAY float(5.0)				// Intensity decay / falloff.
#define RB_COLRATIO float(0.60)			// Color ratio.
#define RB_COLSHIFT float(0.0)			// Color shift.
#define RB_COLPOWER float(0.52)			// Color power.	
#define RB_ENDXMIN float(-6.0)			// AMOUNT OF FADE OUT TOWARDS THE GROUND. Best between -6 and 5.
#define RB_ENDXMAX float(5.0)			// AMOUNT OF FADE OUT TOWARDS THE GROUND. Best between -6 and 5.
#define RB_COFX float(1.5)				// AMOUNT OF COLOR TOWARDS THE GROUND. Best between 1.5 and 2.
#define RB_ENDY float(2.0)				// AMOUNT OF FADE OUT TOWARDS THE SKY
#define RB_COFY float(1.0)				// AMOUNT OF COLOR TOWARDS THE SKY
#define RB_VIEWANGLE float4(0.5,1.0,-1.0,0.999)	// Minimum and maximum view angle for rainbows to show up. Required to avoid false positives. Do not change normally.
#define RB_GREENREDDIFF float(-0.05)	// Minimum green-red color difference for rainbows to show up. To be adjusted according to used weather mod. Default weather: rainbow at around 7AM, 11AM, 2PM, 8PM.
#define RB_GREENBLUEDIFF float(0.05)	// Minimum green-blue color difference for rainbows to show up. To be adjusted according to used weather mod.	
#define RB_POSITION float2(0.9,-0.6)	// Rainbow x/y position. Must be fix. Do not change normally. X-coord also be 0.8.	
#define RB_ORIGIN float2(-2.0,-0.6)		// Coordinate system origin for computing rainbow. Do not change unless changing RB_POSITION.
#define RB_COF float(9.0)				// Maximum intensity of rainbows.
#define RB_SUNFACTOR float(3.0)			// Effect amount of sunlight intensity on rainbow shape/length.
#define RB_WIDESCREEN					// Enables wide-screen bow adjustment. Disable if not using a wide-screen monitor.	
#define RB_DEBUGPOS					// Debug only.

#endif