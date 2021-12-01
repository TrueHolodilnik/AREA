/* --- Settings --- */

// -- Sharpening --
//#define sharp_clamp    0.035  //[0.000 to 1.000] Limits maximum amount of sharpening a pixel recieves - Default is 0.035

// -- Advanced sharpening settings --
//#define pattern 2        //[1|2|3|4] Choose a sample pattern. 1 = Fast, 2 = Normal, 3 = Wider, 4 = Pyramid shaped.
//#define offset_bias 1.0  //[0.0 to 6.0] Offset bias adjusts the radius of the sampling pattern.

//#define show_sharpen 1
#define px (1/screen_res.x)
#define py (1/screen_res.y)
   /*-----------------------------------------------------------.
  /                      Developer settings                     /
  '-----------------------------------------------------------*/
#define CoefLuma float3(0.2126, 0.7152, 0.0722)      // BT.709 & sRBG luma coefficient (Monitors and HD Television)
//#define CoefLuma float3(0.299, 0.587, 0.114)       // BT.601 luma coefficient (SD Television)
//#define CoefLuma float3(1.0/3.0, 1.0/3.0, 1.0/3.0) // Equal weight coefficient

   /*-----------------------------------------------------------.
  /                          Main code                          /
  '-----------------------------------------------------------*/

float4 LumaSharpenPass(float4 inputcolor, float2 tex, float sharp_strength)
{
  float3 ori = s_image.SampleLevel(smp_nofilter, float3(tex,0),0).xyz;       // ori = original pixel

  // -- Combining the strength and luma multipliers --
	float3 sharp_strength_luma = (CoefLuma * sharp_strength*3); //I'll be combining even more multipliers with it later on
	
	float3 blur_ori;
	
	float l_offset;
	
	if (technicolor_params2.z == 0) l_offset = 5;
	else l_offset = technicolor_params2.z;
	
  // -- Pattern 1 -- A (fast) 7 tap gaussian using only 2+1 texture fetches.
	if (chrom_hdr_luma_params.w == 1) {

		// -- Gaussian filter --
		//   [ 1/9, 2/9,    ]     [ 1 , 2 ,   ]
		//   [ 2/9, 8/9, 2/9]  =  [ 2 , 8 , 2 ]
		//   [    , 2/9, 1/9]     [   , 2 , 1 ]
	
		blur_ori = s_image.SampleLevel(smp_nofilter, float3(tex + (float2(px,py) / 3.0) * l_offset,0),0).rgb;  // North West
		blur_ori += s_image.SampleLevel(smp_nofilter, float3(tex + (float2(-px,-py) / 3.0) * l_offset,0),0).rgb; // South East
	
		//blur_ori += tex2D(s_image, tex + float2(px,py) / 3.0 * l_offset); // North East
		//blur_ori += tex2D(s_image, tex + float2(-px,-py) / 3.0 * l_offset); // South West
	
		blur_ori /= 2;  //Divide by the number of texture fetches
	
		sharp_strength_luma *= 1.5; // Adjust strength to aproximate the strength of pattern 2
	
	}

  // -- Pattern 2 -- A 9 tap gaussian using 4+1 texture fetches.
	else if (chrom_hdr_luma_params.w == 2) {
		
		// -- Gaussian filter --
		//   [ .25, .50, .25]     [ 1 , 2 , 1 ]
		//   [ .50,   1, .50]  =  [ 2 , 4 , 2 ]
		//   [ .25, .50, .25]     [ 1 , 2 , 1 ]
		
		
		blur_ori = s_image.SampleLevel(smp_nofilter, float3(tex + float2(px,-py) * 0.5 * l_offset,0),0).rgb; // South East
		blur_ori += s_image.SampleLevel(smp_nofilter, float3(tex + float2(-px,-py) * 0.5 * l_offset,0),0).rgb;  // South West
		blur_ori += s_image.SampleLevel(smp_nofilter, float3(tex + float2(px,py) * 0.5 * l_offset,0),0).rgb; // North East
		blur_ori += s_image.SampleLevel(smp_nofilter, float3(tex + float2(-px,py) * 0.5 * l_offset,0),0).rgb; // North West
		
		blur_ori *= 0.25;  // ( /= 4) Divide by the number of texture fetches

	}

  // -- Pattern 3 -- An experimental 17 tap gaussian using 4+1 texture fetches.
	else if (chrom_hdr_luma_params.w == 3) {

		// -- Gaussian filter --
		//   [   , 4 , 6 ,   ,   ]
		//   [   ,16 ,24 ,16 , 4 ]
		//   [ 6 ,24 ,   ,24 , 6 ]
		//   [ 4 ,16 ,24 ,16 ,   ]
		//   [   ,   , 6 , 4 ,   ]
	
		blur_ori = s_image.SampleLevel(smp_nofilter, float3(tex + float2(0.4*px,-1.2*py)* l_offset,0),0).rgb;  // South South East
		blur_ori += s_image.SampleLevel(smp_nofilter, float3(tex + float2(-1.2*px,-0.4*py) * l_offset,0),0).rgb; // West South West
		blur_ori += s_image.SampleLevel(smp_nofilter, float3(tex + float2(1.2*px,0.4*py) * l_offset,0),0).rgb; // East North East
		blur_ori += s_image.SampleLevel(smp_nofilter, float3(tex + float2(-0.4*px,1.2*py) * l_offset,0),0).rgb; // North North West
	
		blur_ori *= 0.25;  // ( /= 4) Divide by the number of texture fetches
	
		sharp_strength_luma *= 0.51;
	}

  // -- Pattern 4 -- A 9 tap high pass (pyramid filter) using 4+1 texture fetches.
	else if (chrom_hdr_luma_params.w == 4) {

		// -- Gaussian filter --
		//   [ .50, .50, .50]     [ 1 , 1 , 1 ]
		//   [ .50,    , .50]  =  [ 1 ,   , 1 ]
		//   [ .50, .50, .50]     [ 1 , 1 , 1 ]
	
		blur_ori = s_image.SampleLevel(smp_nofilter, float3(tex + float2(0.5 * px,-py * l_offset),0),0).rgb;  // South South East
		blur_ori += s_image.SampleLevel(smp_nofilter, float3(tex + float2(l_offset * -px,0.5 * -py),0),0).rgb; // West South West
		blur_ori += s_image.SampleLevel(smp_nofilter, float3(tex + float2(l_offset * px,0.5 * py),0),0).rgb; // East North East
		blur_ori += s_image.SampleLevel(smp_nofilter, float3(tex + float2(0.5 * -px,py * l_offset),0),0).rgb; // North North West
	
		//blur_ori += (2 * ori); // Probably not needed. Only serves to lessen the effect.
	
		blur_ori /= 4.0;  //Divide by the number of texture fetches
	
		sharp_strength_luma *= 0.666; // Adjust strength to aproximate the strength of pattern 2
	}

  // -- Pattern 8 -- A (slower) 9 tap gaussian using 9 texture fetches.
	else if (chrom_hdr_luma_params.w == 5) {

		// -- Gaussian filter --
		//   [ 1 , 2 , 1 ]
		//   [ 2 , 4 , 2 ]
		//   [ 1 , 2 , 1 ]
	
		blur_ori = s_image.SampleLevel(smp_nofilter, float3(tex + float2(-px,py) * l_offset,0),0).rgb; // North West
		blur_ori += s_image.SampleLevel(smp_nofilter, float3(tex + float2(px,-py) * l_offset,0),0).rgb;     // South East
		blur_ori += s_image.SampleLevel(smp_nofilter, float3(tex + float2(-px,-py)  * l_offset,0),0).rgb;  // South West
		blur_ori += s_image.SampleLevel(smp_nofilter, float3(tex + float2(px,py) * l_offset,0),0).rgb;    // North East
	
		float3 blur_ori2 = s_image.SampleLevel(smp_nofilter, float3(tex + float2(0,py) * l_offset,0),0).rgb; // North
		blur_ori2 += s_image.SampleLevel(smp_nofilter, float3(tex + float2(0,-py) * l_offset,0),0).rgb;    // South
		blur_ori2 += s_image.SampleLevel(smp_nofilter, float3(tex + float2(-px,0) * l_offset,0),0).rgb;   // West
		blur_ori2 += s_image.SampleLevel(smp_nofilter, float3(tex + float2(px,0) * l_offset,0),0).rgb;   // East
		blur_ori2 *= 2.0;
	
		blur_ori += blur_ori2;
		blur_ori += (ori * 4); // Probably not needed. Only serves to lessen the effect.
	
		// dot()s with gaussian strengths here?
	
		blur_ori /= 16.0;  //Divide by the number of texture fetches
	
		//sharp_strength_luma *= 0.75; // Adjust strength to aproximate the strength of pattern 2
	}

  // -- Pattern 9 -- A (slower) 9 tap high pass using 9 texture fetches.
	else if (chrom_hdr_luma_params.w == 6) {

		// -- Gaussian filter --
		//   [ 1 , 1 , 1 ]
		//   [ 1 , 1 , 1 ]
		//   [ 1 , 1 , 1 ]
	
		blur_ori = s_image.SampleLevel(smp_nofilter, float3(tex + float2(-px,py) * l_offset,0),0).rgb; // North West
		blur_ori += s_image.SampleLevel(smp_nofilter, float3(tex + float2(px,-py) * l_offset,0),0).rgb;     // South East
		blur_ori += s_image.SampleLevel(smp_nofilter, float3(tex + float2(-px,-py)  * l_offset,0),0).rgb;  // South West
		blur_ori += s_image.SampleLevel(smp_nofilter, float3(tex + float2(px,py) * l_offset,0),0).rgb;    // North East
	
		blur_ori += ori.rgb; // Probably not needed. Only serves to lessen the effect.
	
		blur_ori += s_image.SampleLevel(smp_nofilter, float3(tex + float2(0,py) * l_offset,0),0).rgb;    // North
		blur_ori += s_image.SampleLevel(smp_nofilter, float3(tex + float2(0,-py) * l_offset,0),0).rgb;  // South
		blur_ori += s_image.SampleLevel(smp_nofilter, float3(tex + float2(-px,0) * l_offset,0),0).rgb; // West
		blur_ori += s_image.SampleLevel(smp_nofilter, float3(tex + float2(px,0) * l_offset,0),0).rgb; // East
	
		blur_ori /= 9;  //Divide by the number of texture fetches
	
		//sharp_strength_luma *= (8.0/9.0); // Adjust strength to aproximate the strength of pattern 2
	}

   /*-----------------------------------------------------------.
  /                            Sharpen                          /
  '-----------------------------------------------------------*/

  // -- Calculate the sharpening --
  float3 sharp = ori - blur_ori;  //Subtracting the blurred image from the original image
	
  float sharp_luma;
  //if (chrom_hdr_luma_params.w == 0) { //SweetFX 1.4 code
  //  // -- Adjust strength of the sharpening --
  //  sharp_luma = dot(sharp, sharp_strength_luma); //Calculate the luma and adjust the strength
  //
  //  // -- Clamping the maximum amount of sharpening to prevent halo artifacts --
  //  sharp_luma = clamp(sharp_luma, -chrom_hdr_luma_params.z, chrom_hdr_luma_params.z);  //TODO Try a curve function instead of a clamp
  //}
  //else {//SweetFX 1.5.1 code
    // -- Adjust strength of the sharpening and clamp it--
    float4 sharp_strength_luma_clamp = float4(sharp_strength_luma * (0.5 / chrom_hdr_luma_params.z),0.5); //Roll part of the clamp into the dot

    //sharp_luma = saturate((0.5 / chrom_hdr_luma_params.z) * sharp_luma + 0.5); //scale up and clamp
    sharp_luma = saturate(dot(float4(sharp,1.0), sharp_strength_luma_clamp)); //Calculate the luma, adjust the strength, scale up and clamp
    sharp_luma = (chrom_hdr_luma_params.z * 2.0) * sharp_luma - chrom_hdr_luma_params.z; //scale down
  //}

  // -- Combining the values to get the final sharpened pixel	--
  //float4 done = ori + sharp_luma;    // Add the sharpening to the original.
  inputcolor.rgb = inputcolor.rgb + sharp_luma;    // Add the sharpening to the input color.

   /*-----------------------------------------------------------.
  /                     Returning the output                    /
  '-----------------------------------------------------------*/
  //if (chrom_hdr_luma_params.w == 2) {
  //  //inputcolor.rgb = abs(sharp * 4.0);
  //  inputcolor.rgb = saturate(0.5 + (sharp_luma * 4)).rrr;
  //}

  return saturate(inputcolor);

}