#ifndef BLUR
#define BLUR

struct 	v2p
{
  float4 tc0: 		TEXCOORD0;	// Center
  float4 tc1: 		TEXCOORD1;	// LT 		 
  float4 tc2: 		TEXCOORD2;	// RB
  float4 tc3: 		TEXCOORD3;	// RT 
  float4 tc4: 		TEXCOORD4;	// LB
  float4 tc5:		TEXCOORD5;	// Left	/ Right	
  float4 tc6:		TEXCOORD6;	// Top  / Bottom 
};

#ifdef USE_MSAA
	float4 gauss(Texture2DMS <float4> ss, float2 center, float factor)
#else
	float4 gauss(Texture2D <float4> ss, float2 center, float factor)
#endif
{   
   float4 col = 0;
   #ifndef PERFORMANCE_GAUSBLUR
   float dx = factor * .5f/screen_res.x;
   float dy = factor * .5f/screen_res.y; 
	#ifndef USE_MSAA   
		col.rgb =   
		(		
			1.f * ss.SampleLevel( smp_nofilter, float3(center,0), 0).rgb +
			1.f * ss.SampleLevel( smp_nofilter, float3(center +  float2(dx, 0),0), 0).rgb +
			1.f * ss.SampleLevel( smp_nofilter, float3(center +  float2(dy, 0),0), 0).rgb +
			1.f * ss.SampleLevel( smp_nofilter, float3(center +  float2(-dx, 0),0), 0).rgb +
			1.f * ss.SampleLevel( smp_nofilter, float3(center +  float2(-dx, -dy),0), 0).rgb +
			1.f * ss.SampleLevel( smp_nofilter, float3(center +  float2(dx, -dy),0), 0).rgb
		)/6.f;	
	#else
		col.rgb =   
		(		
			1.f * ss.Load( int3(center * pos_decompression_params2.xy,0), 0).rgb +
			1.f * ss.Load( int3(center * pos_decompression_params2.xy +  float2(dx, 0),0), 0).rgb +
			1.f * ss.Load( int3(center * pos_decompression_params2.xy +  float2(dy, 0),0), 0).rgb +
			1.f * ss.Load( int3(center * pos_decompression_params2.xy +  float2(-dx, 0),0), 0).rgb +
			1.f * ss.Load( int3(center * pos_decompression_params2.xy +  float2(-dx, -dy),0), 0).rgb +
			1.f * ss.Load( int3(center * pos_decompression_params2.xy +  float2(dx, -dy),0), 0).rgb
		)/6.f;
	#endif
	#else
	   const float delta = factor * (.5f/1024.f);
	   
		col.rgb = ss.SampleLevel( smp_nofilter, float3(center+delta,0), 0).rgb +
			ss.SampleLevel( smp_nofilter, float3(center-delta,0), 0).rgb * (1.f/2.f);
	#endif
	return col;
}

//float4 gaussblur(float2 center, float factor) { return gauss(s_image, center, factor); }

//float4 gaussbloom(float2 center, float factor) { return gauss(s_bloom, center, factor); }

// Classic blur algorithm from Sky4ace
// I: input texcoords
// sum: input color of pixel to process
// factor: blur amount
// samples: blur quality / samples count
// RETURNS: blurred color component (must be added to existing img)
float3 classicblur(float3 sum, float2 cent, float factor, int samples)
{
	float2 bias=float2(.5f/screen_res.x,.5f/screen_res.y);
	float2 scale = bias * factor;
	float2 o [48];
	o[0]=float2(-0.326212f,-0.405810f)*scale;
	o[1]=float2(-0.840144f,-0.073580f)*scale;
	o[2]=float2(-0.695914f,0.457137f)*scale;
	o[3]=float2(-0.203345f,0.620716f)*scale;
	o[4]=float2(0.962340f,-0.194983f)*scale;
	o[5]=float2(0.473434f,-0.480026f)*scale;
	o[6]=float2(0.519456f,0.767022f)*scale;
	o[7]=float2(0.185461f,-0.893124f)*scale;
	o[8]=float2(0.507431f,0.064425f)*scale;
	o[9]=float2(0.896420f,0.412458f)*scale;
	o[10]=float2(-0.321940f,-0.932615f)*scale;
	o[11]=float2(-0.791559f,-0.597710f)*scale;	
		
	float contrib=1.h;
	int j = 0;
	
	for (int i=0;i<samples;i++)
	{	
		float2 tap=cent+o[j];
		float3 tap_color = s_image.SampleLevel( smp_nofilter, float3(tap,0), 0).rgb;
		sum+=tap_color;
		contrib++;
		j++; if (j > 11) { j=0; }
	}
	
	return sum;
}

#define radiusScale float(0.0625)
#define pixelSizes float4(1,1,1,1)
#define dofParamsBlur float2(2,2)

#define tapCount int(25)

static const float2 texsize = float2(screen_res.x/1024.f,screen_res.y/768.f);

static const float2 poisson[141] =
  {
      
        1.242,    -1.395,  
        1.276,    -1.212,  
        1.311,    -1.028,  
        1.345,    -0.845,  
        1.380,    -0.661,  
        1.414,    -0.478,  
        1.449,    -0.295,  
        1.483,    -0.111,  
        1.518,     0.072,  
        1.553,     0.255,  
        1.587,     0.439,  
        1.622,     0.622,  
        1.656,     0.806,  
        1.611,     0.951,  
        1.448,     1.041,  
        1.284,     1.130,  
        1.120,     1.220,  
        0.956,     1.309,  
        0.793,     1.399,  
        0.629,     1.488,  
        0.465,     1.578,  
        0.302,     1.668,  
        0.138,     1.757,  
       -0.026,     1.847,  
       -0.190,     1.936,  
       -0.353,     2.026,  
       -0.495,     1.941,  
       -0.630,     1.813,  
       -0.766,     1.685,  
       -0.902,     1.557,  
       -1.038,     1.429,  
       -1.173,     1.301,  
       -1.309,     1.173,  
       -1.445,     1.045,  
       -1.581,     0.917,  
       -1.716,     0.789,  
       -1.852,     0.660,  
       -1.988,     0.532,  
       -2.080,     0.396,  
       -2.000,     0.228,  
       -1.921,     0.059,  
       -1.841,    -0.110,  
       -1.761,    -0.278,  
       -1.681,    -0.447,  
       -1.601,    -0.616,  
       -1.521,    -0.784,  
       -1.441,    -0.953,  
       -1.362,    -1.122,  
       -1.282,    -1.290,  
       -1.202,    -1.459,  
       -1.122,    -1.628,  
       -0.979,    -1.681,  
       -0.794,    -1.657,  
       -0.609,    -1.633,  
       -0.424,    -1.609,  
       -0.239,    -1.585,  
       -0.054,    -1.562,  
        0.131,    -1.538,  
        0.316,    -1.514,  
        0.501,    -1.490,  
        0.687,    -1.466,  
        0.872,    -1.443,  
        1.057,    -1.419,  
        1.242,    -1.395,  
        1.294,     0.776,  
        1.287,     0.493,  
        1.211,     0.154,  
        1.192,    -0.173,  
        1.147,    -0.537,  
        0.637,     1.168,  
        0.637,     0.821,  
        0.637,     0.475,  
        0.637,     0.128,  
        0.637,    -0.218,  
        0.567,    -0.532,  
        0.636,    -0.889,  
        0.954,    -1.135,  
        0.937,     1.006,  
        0.937,     0.660,  
        0.937,     0.313,  
        0.937,    -0.033,  
        0.930,    -0.353,  
        0.814,    -0.639,  
        1.055,    -0.834,  
        0.024,     1.514,  
        0.024,     1.168,  
        0.024,     0.821,  
        0.024,     0.475,  
        0.024,     0.128,  
        0.024,    -0.218,  
        0.024,    -0.565,  
        0.024,    -0.911,  
        0.024,    -1.211,  
        0.324,     1.353,  
        0.324,     1.006,  
        0.324,     0.660,  
        0.324,     0.313,  
        0.324,    -0.033,  
        0.324,    -0.380,  
        0.324,    -0.726,  
        0.337,    -1.109,  
        0.649,    -1.208,  
       -0.589,     1.514,  
       -0.589,     1.168,  
       -0.594,     0.864,  
       -0.537,     0.538,  
       -0.589,     0.128,  
       -0.589,    -0.218,  
       -0.589,    -0.565,  
       -0.589,    -0.911,  
       -0.589,    -1.258,  
       -0.289,     1.699,  
       -0.289,     1.353,  
       -0.289,     1.006,  
       -0.289,     0.660,  
       -0.289,     0.313,  
       -0.289,    -0.033,  
       -0.289,    -0.380,  
       -0.289,    -0.726,  
       -0.289,    -1.045,  
       -0.263,    -1.359,  
       -0.994,     0.964,  
       -1.176,     0.494,  
       -1.202,     0.128,  
       -1.202,    -0.218,  
       -1.202,    -0.565,  
       -1.202,    -0.911,  
       -0.837,     1.256,  
       -0.862,     0.646,  
       -0.902,     0.313,  
       -0.902,    -0.033,  
       -0.902,    -0.380,  
       -0.902,    -0.726,  
       -0.902,    -1.072,  
       -0.882,    -1.359,  
       -1.589,     0.557,  
       -1.736,     0.301,  
       -1.275,     0.798,  
       -1.444,     0.294,  
       -1.515,    -0.033,  
       -1.515,    -0.380,   
  };
  
float3 crysisblur(Texture2D input, float2 uv, float factor, bool noise)
{
	float4 OUT = 0;
	float4 cOut=0;
	float discRadius;
	float discRadiusLow;
	float centerDepth;
	float centerDepthLow;
	
	float2 vNoise = 0;
	
	if (noise) vNoise = frac(sin(dot(uv, float2(12.9898,78.233))) * 43758.5453);

	// fetch center tap from blured low res image
	centerDepth = 1.005; // tex2D(Texture1, uv + vNoise * pixelSizes.zw).w;    

	discRadius=(centerDepth*(float)dofParamsBlur.y-(float)dofParamsBlur.x);
	discRadiusLow=discRadius*(float)radiusScale*2*factor;
	
	float2 texsize_low = pixelSizes.zw * discRadiusLow;
	


	#ifdef D3D10
	[unroll]
	#endif
	for(int t=0; t<tapCount; t++) // tapCount
	{ 	
		if (pp_control_params.w) {
			float2 taptc = uv + (poisson[t] + saturate(t)*vNoise)*texsize_low;
			float4 tapHigh = input.SampleLevel(smp_rtlinear, float4(taptc, 0, 0),0);
			tapHigh.g = input.SampleLevel(smp_rtlinear, float4(taptc + uv*chrom_hdr_luma_params.x, 0, 0),0).g;	
			tapHigh.b = input.SampleLevel(smp_rtlinear, float4(taptc - uv*chrom_hdr_luma_params.x, 0, 0),0).b;			
			cOut.rgb += tapHigh.rgb;
			cOut.w += 1;
		}
		else {
			float4 tapHigh = input.SampleLevel(smp_rtlinear, float4(uv+ (poisson[t] + saturate(t)*vNoise)*texsize_low, 0, 0),0);	
			cOut.rgb += tapHigh.rgb;
			cOut.w += 1;
		}
	}
	//#ifdef D3D10
	//[unroll]
	//#endif
	//for(int t=70; t<tapCount; t++) // tapCount
	//{ 	
	//	float4 tapHigh =input.SampleLevel(smp_rtlinear, float4(uv+ (poisson[t] + saturate(t)*vNoise)*texsize_low, 0, 0),0);
	//	cOut.rgb += tapHigh.rgb;
	//	cOut.w += 1;
	//}
	
	OUT = cOut/cOut.w;  
	return OUT.rgb;	
}

#if defined(USE_MSAA)
	float3 crysisblur(Texture2DMS <float4> input, float2 uv, float factor, bool noise)
#else
	float3 crysisblur(Texture2D <float3> input, float2 uv, float factor, bool noise)
#endif
{
	float4 OUT = 0;
	float4 cOut=0;
	float discRadius;
	float discRadiusLow;
	float centerDepth;
	float centerDepthLow;

	centerDepth = 1.005;

	discRadius=(centerDepth*(float)dofParamsBlur.y-(float)dofParamsBlur.x);
	discRadiusLow=discRadius*(float)radiusScale*2*factor;
	
	float2 texsize_low = pixelSizes.zw * discRadiusLow;
	
	#ifdef D3D10
	[unroll]
	#endif
	for(int t=0; t<12; t++) // tapCount
	{ 	
		#ifndef USE_MSAA
			float3 tapHigh = input.Sample( smp_rtlinear, uv + poisson[t]*texsize_low);
		#else
		#	ifdef GBUFFER_OPTIMIZATION
			float4 tapHigh = input.Load( int3( uv + poisson[t]*texsize_low, 0 ), 0);
		#	else
			float4 tapHigh = input.Load( int3( uv + poisson[t]*texsize_low, 0 ), 0 );
		#	endif
		#endif
		cOut.rgb += tapHigh.rgb;
		cOut.w += 1;
	}
	
	OUT = cOut/cOut.w;  
	return OUT.rgb;	
}
#endif