//=================================================================================================
//Defines for and variables for ACES and color grading
//=================================================================================================
#define USE_ACES //Use ACES tonemapping and color space
#define USE_LOG_GRADING //Use log space to prevent clipping
//=================================================================================================

//
//manual settings
//
/*
	Slope = float3(1.000, 1.000, 1.000);
	Offset = float3(0.000, 0.000, 0.000);
	Power = float3(1.000, 1.000, 1.000);
	Saturation = 1.000;
*/
//
//settings for supporting in-game console commands
//
	Slope = float3(1.0, 1.0, 1.0); 	//brightness
	Offset = float3(0.000, 0.000, 0.000); 		//color grading
	Power = float3(1.0, 1.0, 1.0); 		//color grading
	Saturation = 1.000; 	//saturation	