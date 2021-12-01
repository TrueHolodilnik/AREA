#ifndef lensFlaresPass
#define lensFlaresPass

// Copyright (c) 2009-2015 Gilcher Pascal aka Marty McFly

#define fChapFlareDispersal float(0.125)
#define fChapFlareSize float(0.25)
#define fChapFlareCA float3(0.0, 0.01, 0.02)
#define iChapFlareCount int(5)
#define fChapFlareTreshold float(0.4)
#define fChapFlareIntensity float(2.0)

uniform sampler2D 	SamplerDirt;
float3 GetDnB(Texture2D tex, float2 coords) {
	float3 color = max(0, dot(tex.SampleLevel(smp_rtlinear, float4(coords.xy, 0, 4),0).rgb, 0.333) - fChapFlareTreshold) * fChapFlareIntensity;
	
	//#ifdef CHAPMAN_DEPTH_CHECK
	//	if (tex2Dlod(s_position, float4(coords.xy, 0, 3)).z < 0.99999)
	//		color = 0;
	//#endif // CHAPMAN_DEPTH_CHECK	
	
	return color;
}

float3 GetDistortedTex(Texture2D tex, float2 sample_center, float2 sample_vector, float3 distortion) {
	float2 final_vector = sample_center + sample_vector * min(min(distortion.r, distortion.g), distortion.b);
	
	if (final_vector.x > 1.0 || final_vector.y > 1.0 || final_vector.x < -1.0 || final_vector.y < -1.0)
		return float3(0, 0, 0);
	else
		return float3(GetDnB(tex, sample_center + sample_vector * distortion.r).r,GetDnB(tex, sample_center + sample_vector * distortion.g).g,GetDnB(tex, sample_center + sample_vector * distortion.b).b);
}

float3 LensFlarePass(float3 init_img, float2 init_center) {
	
	// Chapman Lens
	float2 sample_vector = (float2(0.5, 0.5) - init_center.xy) * fChapFlareDispersal;
	float2 halo_vector = normalize(sample_vector) * fChapFlareSize;
		
	float3 chaplens = GetDistortedTex(s_image, init_center.xy + halo_vector, halo_vector, fChapFlareCA * 2.5f).rgb;
		
	for (int j = 0; j < iChapFlareCount; j++) {
		float2 foffset = sample_vector * float(j);
		chaplens += GetDistortedTex(s_image, init_center.xy + foffset, foffset, fChapFlareCA).rgb;
	}
	
	chaplens *= 1.0/iChapFlareCount;
	init_img.xyz += chaplens;
	
	return init_img.rgb;
}

#endif // lensFlaresPass