/*
DPX/Cineon shader by Loadus ( http://www.loadusfx.net/virtualdub/DPX.fx )
Ported to SweetFX by CeeJay.dk

Version 1.0
*/

#define Red 1.0
#define Green 1.0
#define Blue 1.0
//#define ColorGamma 1.1
//#define DPXSaturation 0.90
//#define RedC 0.36
//#define GreenC 0.36
//#define BlueC 0.34
//#define Blend 0.90

//#define Red   1.0  			//[1.0 to 15.0]
//#define Green 1.0  			//[1.0 to 15.0]
//#define Blue  1.0  			//[1.0 to 15.0]
//
//#define ColorGamma    1.9  	//[0.1 to 2.5] Adjusts the colorfulness of the effect in a manner similar to Vibrance. 1.0 is neutral.
//#define DPXSaturation 1.7  	//[0.0 to 8.0] Adjust saturation of the effect. 1.0 is neutral.
//
//#define RedC   0.33  		//[0.60 to 0.20]
//#define GreenC 0.36  		//[0.60 to 0.20]
//#define BlueC  0.35  		//[0.60 to 0.20]
//
//#define Blend 0.85    		//[0.00 to 1.00] How strong the effect should be

float4 DPXPass(float4 InputColor) : COLOR0 {

	float DPXContrast = 0.1;

	float DPXGamma = 1.0;

	float RedCurve = Red;
	float GreenCurve = Green;
	float BlueCurve = Blue;

	float3 B = InputColor.rgb;
	//float3 Bn = B; // I used InputColor.rgb instead.

	B = pow(B, 1.0/DPXGamma);

	B.r = pow(B.r, 1.00);
	B.g = pow(B.g, 1.00);
	B.b = pow(B.b, 1.00);

        B = (B * (1.0 - DPXContrast)) + DPXContrast / 2.0;
 	
 	B.r = (1.0 /(1.0 + exp(- RedCurve * (B.r - dpx_params1.x))) - (1.0 / (1.0 + exp(RedCurve / 2.0))))/(1.0 - 2.0 * (1.0 / (1.0 + exp(RedCurve / 2.0))));				
	B.g = (1.0 /(1.0 + exp(- GreenCurve * (B.g - dpx_params1.y))) - (1.0 / (1.0 + exp(GreenCurve / 2.0))))/(1.0 - 2.0 * (1.0 / (1.0 + exp(GreenCurve / 2.0))));				
	B.b = (1.0 /(1.0 + exp(- BlueCurve * (B.b - dpx_params1.z))) - (1.0 / (1.0 + exp(BlueCurve / 2.0))))/(1.0 - 2.0 * (1.0 / (1.0 + exp(BlueCurve / 2.0))));					

        //TODO use faster code for conversion between RGB/HSV  -  see http://www.chilliant.com/rgb2hsv.html
	   float value = max(max(B.r, B.g), B.b);
	   float3 color = B / value;
	
	   color = pow(color, 1.0/dpx_params1.w);
	
	   float3 c0 = color * value;

	   c0 = mul(XYZ, c0);

	   float luma = dot(c0, float3(0.30, 0.59, 0.11)); //Use BT 709 instead?
	   float3 chroma = c0 - luma;

	   c0 = luma + chroma * dpx_params2.x;
	   c0 = mul(RGB, c0);
	
	InputColor.rgb = lerp(InputColor.rgb, c0, dpx_params2.y); //as long as Blend is always 0 we don't really need to lerp. The compiler *should* be smart enough to optimize this though (check to be sure)

	return InputColor;
}