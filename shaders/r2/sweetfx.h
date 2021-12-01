#ifndef PPS_SWEETFX_H
#define PPS_SWEETFX_H
	
//#define USE_TECHICOLOR
//#define TechniAmount 0.7         						// [0.00, 1.00]
//#define TechniPower  4.0  // 2.0       						// [0.00, 8.00]
//#define redNegativeAmount   1.00  						// [0.00, 1.00]
//#define greenNegativeAmount 1.00 						// [0.00, 1.00]
//#define blueNegativeAmount  1.00 						// [0.00, 1.00]

#define cyanfilter float3(0.0, 1.35, 0.65) // float3(0.1, 1.20, 0.75)
#define magentafilter float3(1.0, 0.0, 1.05) 
#define yellowfilter float3(1.7, 1.6, 0.05) // float3(1.7, 1.65, 0.05)

#define redorangefilter float2(1.05, 0.620) 			// RG_
#define greenfilter float2(0.30, 1.0)       			// RG_
#define magentafilter2 magentafilter.rb     			// R_B

//#define USE_VIGNETTE	
#define VignetteType       1  //[1|2|3] 1 = Original, 2 = New, 3 = TV style
#define VignetteRatio   1.00  //[0.15 to 6.00]  Sets a width to height ratio. 1.00 (1/1) is perfectly round, while 1.60 (16/10) is 60 % wider than it's high.
#define VignetteRadius  2.00  //[-1.00 to 3.00] lower values = stronger radial effect from center
#define VignetteAmount -1.00  //[-2.00 to 1.00] Strength of black. -2.00 = Max Black, 1.00 = Max White.
#define VignetteSlope      2  //[2 to 16] How far away from the center the change should start to really grow strong (odd numbers cause a larger fps drop than even numbers)
#define VignetteCenter float2(0.500, 0.500)  //[0.000 to 1.000, 0.000 to 1.000] Center of effect for VignetteType 1. 2 and 3 do not obey this setting.

////////////////////////////////////////////////////////////////////////////////////

//#define USE_CHROMATIC_ABERRATION
#define Chromatic_shift float2(1.0,0.5) //[-100.0:100.0] //-Distance (X,Y) in pixels to shift the color components. For a slightly blurred look try fractional values (.5) between two pixels.
//Fisheye aberration
#define fvChroma float3(1.000, 0.995, 1.005)	// displacement scales of red, green and blue respectively
#define fEyeChromaBaseRadius float(1.8)			// below this radius the effect is less visible
#define fEyeChromaFalloffRadius float(2.1)		// over this radius the effects is maximal

////////////////////////////////////////////////////////////////////////////////////
/// FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////

float4 TechnicolorPass( float4 colorInput )
{
	float3 tcol = colorInput.rgb;
	
	float2 rednegative_mul   = tcol.rg * (1.0 / (technicolor_params1.z * technicolor_params1.y));
	float2 greennegative_mul = tcol.rg * (1.0 / (technicolor_params1.w * technicolor_params1.y));
	float2 bluenegative_mul  = tcol.rb * (1.0 / (technicolor_params2.x * technicolor_params1.y));
	
	float rednegative   = dot( redorangefilter, rednegative_mul );
	float greennegative = dot( greenfilter, greennegative_mul );
	float bluenegative  = dot( magentafilter2, bluenegative_mul );
	
	float3 redoutput   = rednegative.rrr + cyanfilter;
	float3 greenoutput = greennegative.rrr + magentafilter;
	float3 blueoutput  = bluenegative.rrr + yellowfilter;
	
	float3 result = redoutput * greenoutput * blueoutput;
	colorInput.rgb = lerp(tcol, result, technicolor_params1.x);
	return colorInput;
}

float3 VignettePass( float3 img, float2 center )
{
	img *= 1.4 - ( 1.4 * saturate( distance( center.xy, half2( 0.5f, 0.5f ) ) ) );
	return img;
}

float3 ChromaticAberrationBase( float3 colorInput, float2 tex, float val)
{
	float3 color = float3(0.0,0.0,0.0);
  
	float2 shift = Chromatic_shift;
  
	color.r = colorInput.r;
	color.r = tex2D(s_image, tex + (PIXEL_SIZE * shift)).r;
	color.g = colorInput.g;
	color.b = tex2D(s_image, tex - (PIXEL_SIZE * shift)).g;
	color.b = colorInput.b;
	//color.b = tex2D(s_image, tex - (PIXEL_SIZE * shift)).b;
    
	colorInput.rgb = lerp(colorInput.rgb, color, val);
  
  
	return saturate(colorInput);
}
float3 ChromaticAberration( float3 colorInput, float2 texcoord, float val)
{
	
	float RedOffset = -0.5;
	float BlueOffset = 0.5;
	float GreenOffset = 0.0;
	
	int OffsetSamples = 5;
	
	float2 Coords = texcoord - float2(0.5,0.5);
			
	float3 CA;
	
	CA.r = (tex2D(s_image,(Coords*(1.0 + (RedOffset*0.01))) + 0.5).r);
	CA.b = (tex2D(s_image,(Coords*(1.0 + (BlueOffset*0.01))) + 0.5).b);
	CA.g = (tex2D(s_image,(Coords*(1.0 + (GreenOffset*0.01))) + 0.5).g);
		
	
	float Offsets[5] = { 0.0, 0.75, 0.5, 0.25, 0.125 };
	float Offset = (abs(RedOffset) + abs(BlueOffset) + abs(GreenOffset))*0.25;
	float Weights[5] = { 0.333, 0.2666, 0.2, 0.1333, 0.0666 };
	
	CA *= 0.333;
	
	for(int i = 1; i < 5; ++i)
	{
		if(RedOffset == 0.000)
		{
			CA.r += tex2D(s_image,(Coords*(1.0 + (Offsets[i]*Offset*0.01))) + 0.5).r*Weights[i]*0.5; //(Offset + (Offset*Offsets[i]))
			CA.r += tex2D(s_image,(Coords*(1.0 + (Offsets[i]*Offset*0.01))) + 0.5).r*Weights[i]*0.5; //(Offset*0.1*Offsets[i])
		}
		else
		{
			CA.r += tex2D(s_image,(Coords*(1.0 + (Offsets[i]*RedOffset*0.01))) + 0.5).r*Weights[i]; //((Offset + (Offset*Offsets[i]))*0.1)
		}
	
		if(GreenOffset == 0.000)
		{
			CA.g += tex2D(s_image,(Coords*(1.0 + (Offsets[i]*Offset*0.01))) + 0.5).g*Weights[i]*0.5;
			CA.g += tex2D(s_image,(Coords*(1.0 + (Offsets[i]*Offset*0.01))) + 0.5).g*Weights[i]*0.5;
		}
		else
		{
			CA.g += tex2D(s_image,(Coords*(1.0 + (Offsets[i]*GreenOffset*0.01))) + 0.5).g*Weights[i];
		}
		if(BlueOffset == 0.000)
		{
			CA.b += tex2D(s_image,(Coords*(1.0 + (Offsets[i]*Offset*0.01))) + 0.5).b*Weights[i]*0.5;
			CA.b += tex2D(s_image,(Coords*(1.0 + (Offsets[i]*Offset*0.01))) + 0.5).b*Weights[i]*0.5;
		}
		else
		{
			CA.b += tex2D(s_image,(Coords*(1.0 + (Offsets[i]*BlueOffset*0.01))) + 0.5).b*Weights[i];
		}
	}
	
	CA = lerp(colorInput,CA,val);
	return CA.rgb;
}

float3 ACESFilm( float3 x )
{
	float a = 2.51f;
	float b = 0.03f;
	float c = 2.43f;
	float d = 0.59f;
	float e = 0.14f;
	return saturate((x*(a*x+b))/(x*(c*x+d)+e));
}

float3 MoodPass( float3 colorInput )
{
	float moodR = 1.0;
	float moodG = 0.7;
	float moodB = 0.0;
	float fRatio = 0.35;
	
	float3 colInput = colorInput;
	float3 colMood = 1.0f;
	colMood.r = moodR;
	colMood.g = moodG;
	colMood.b = moodB;
	float fLum = ( colInput.r + colInput.g + colInput.b ) / 3;
	colMood = lerp(0, colMood, saturate(fLum * 2.0));
	colMood = lerp(colMood, 1, saturate(fLum - 0.5) * 2.0);
	float3 colOutput = lerp(colInput, colMood, saturate(fLum * fRatio));
	colorInput=max(0, colOutput);
	return colorInput;
}			   

#endif // PPS_SWEETFX_H

