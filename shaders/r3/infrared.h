#ifndef INFRARED
#define INFRARED

//////////////////////////////////////////////////////////////
//Thermal/Infrared Vision - By Meltac
//////////////////////////////////////////////////////////////
	
	#define THM	
	#define THM_MinVertG float(1.20)
	#define THM_MaxVertG float(2.00)
	#define THM_MaxAlpha float(0.1)
	#define THM_Infra float(0.635)
	#define THM_Infra2 float(0.665)
	#define THM_REDCOF float(0.01)
	#define THM_REDFAC float(0.30)
	#define THM_LOWCOF float(5.0)
	#define THM_POWER float(0.25)
	#define THM_STRIPESRES float(70.0)
	#define THM_STRIPESVEL float(12.0)
	
int mcls(half mcol)
{
	mcol = (mcol + 0.25) / 4.f;
	if      (mcol < 0.14)	// Yellow - Concrete walls, stone/brick buildings, NPCs, mutants, suits
		return 1;
	else if (mcol < 0.16)	// Red - Terrain, grass, water, metal cases, wood crates, signs, plastics, medkits, food, TVs, bulbs/lamps, some doors
		return 2;
	else if (mcol < 0.22)	// Purple - Wood, bushes, trees, glass bottles
		return 3;
	else if (mcol < 0.29)	// Teal - Bottles, vehicles, wheels, metall constructions
		return 4;
	else if (mcol < 0.33)	// White - Vehicles, steel boxes, gas cans, fuel cans, barrels
		return 5;
	else if (mcol < 0.36)	// RGB 87 - Metallic structures/objects
		return 6;
	else
		return -1;
}
	
float4 l__2_ks(float f)
{
	f += .35;
	f =- (f - .5) + .5;
	f /= 1.1;
	
	float i = (int)(f * 7 % 7) + 6 * (f < 0);
	float z = f * 7 % 1 + (f < 0);
	float T = 1 - z;
	float s;
	float w;
	float y;
	
	if (i == 0) {s = 1; w = z; y = 0;}
	else if (i == 1) {s = T; w = 1; y = 0;}
	else if (i == 2) {s = 0; w = 1; y = z;}
	else if (i == 3) {s = 0; w = T; y = 1;}
	else if (i == 4) {s = z; w = 0; y = 1;}
	else if (i == 5) {s = z; w = 0; y = 1;}
	else if (i == 6) {s = 1; w = 0; y = T;}
	
	float4 t = float4(s, w, y, 1);
	if (i == 5) t = t.x * t + (1 - t.x) * float4(2, 2, 2, 1);
	if (i == 4) t = (1 - t.x) * t + t.x * float4(2, 2, 2, 1);
	return t;
}

float3 l_2_ks(Texture2D f, float2 i, float T)
//float3 l_2_ks(float2 i, float T)
{
	float s = T * .5f / 1024.f;
	float w = T * .5f / 768.f;
	return (
			f.Sample(smp_rtlinear, float4(i, 					0, 0)).xyz +
			f.Sample(smp_rtlinear, float4(i + float2(s, 0), 	0, 0)).xyz +
			f.Sample(smp_rtlinear, float4(i + float2(-s, 0), 	0, 0)).xyz +
			f.Sample(smp_rtlinear, float4(i + float2(0, w), 	0, 0)).xyz +
			f.Sample(smp_rtlinear, float4(i + float2(0, -w), 	0, 0)).xyz +
			f.Sample(smp_rtlinear, float4(i + float2(s, w), 	0, 0)).xyz +
			f.Sample(smp_rtlinear, float4(i + float2(s, -w), 	0, 0)).xyz +
			f.Sample(smp_rtlinear, float4(i + float2(-s, w), 	0, 0)).xyz +
			f.Sample(smp_rtlinear, float4(i + float2(-s, -w), 	0, 0)).xyz
			) / 9;
	//float4 pos2d = (0,0,0,0);
	//gbuffer_data gbd1 = gbuffer_load_data(float4(i, 				0, 0),pos2d);
	//gbuffer_data gbd2 = gbuffer_load_data(float4(i + float2(s, 0), 	0, 0),pos2d);
	//gbuffer_data gbd3 = gbuffer_load_data(float4(i + float2(-s, 0), 0, 0),pos2d);
	//gbuffer_data gbd4 = gbuffer_load_data(float4(i + float2(0, w), 	0, 0),pos2d);
	//gbuffer_data gbd5 = gbuffer_load_data(float4(i + float2(0, -w), 0, 0),pos2d);
	//gbuffer_data gbd6 = gbuffer_load_data(float4(i + float2(s, w), 	0, 0),pos2d);
	//gbuffer_data gbd7 = gbuffer_load_data(float4(i + float2(s, -w), 0, 0),pos2d);
	//gbuffer_data gbd8 = gbuffer_load_data(float4(i + float2(-s, w), 0, 0),pos2d);
	//gbuffer_data gbd9 = gbuffer_load_data(float4(i + float2(-s, -w),0, 0),pos2d);
	//
	//return(float4( gbd1.N, gbd1.hemi ) +
	//	   float4( gbd2.N, gbd2.hemi ) +
	//	   float4( gbd3.N, gbd3.hemi ) +
	//	   float4( gbd4.N, gbd4.hemi ) +
	//	   float4( gbd5.N, gbd5.hemi ) +
	//	   float4( gbd6.N, gbd6.hemi ) +
	//	   float4( gbd7.N, gbd7.hemi ) +
	//	   float4( gbd8.N, gbd8.hemi ) +
	//	   float4( gbd9.N, gbd9.hemi )) / 9;
}

float4 l___2_ks(float2 f:TEXCOORD0, float4 i, bool s):COLOR
{
	float4 t = s_image.Sample(smp_rtlinear, float4(f.xy, 0, 0));
	float3 w = normalize(mul(i.xyz, (float3x3)m_WV).xyz).xyz;
	float4 T = float4(i.w, i.w, i.w, 1);
	bool y = abs(t.w - round(t.w * 100.f) / 100.f - 1e-05f) < 1e-05f || !(abs(t.w - round(t.w * 100.f) / 100.f - 1e-05f) < 1.1e-05f) || t.w == 1;
	#if defined(USE_MSAA)
		int z = mcls(s_position.Load(f, 0).w);
	#else
		int z = mcls(s_position.Sample(smp_rtlinear, float4(f, 0, 0)).w);
	#endif
	bool M = z == 1;
	bool G = z == 2;
	bool x = z == 3;
	bool R = z == 4;
	bool m = z == 5;
	bool V = z == 6;
	
	float e = 1;
	if (V) e = 1.05f;
	else if (R) e = 1.2f;
	else if (m) e = 1.3f;
	else if (G) e = 1.4f;
	else if (x) e = 2.f;
	else if (M && !s) e = 1.5f; //color
	else if (M && s) e = 3.f;	//b&w
	e *= .05;
	
	if (M)
	{
		float r = 1 - w.y;
		float l = THM_MinVertG + (THM_MaxVertG - THM_MinVertG) / 2;
	
		if (r < THM_MaxVertG && r > THM_MinVertG)
		{
			#ifdef THM_BLUR
				//i = l_2_ks(f, THM_BLUR);
				i = l_2_ks(s_normal, f, THM_BLUR);
			#endif
			
			float3 I = normalize(mul(i.xyz, (float3x3)m_WV).xyz).xyz;
			r = 1 - I.y;
			if (r < l) e += saturate((r - THM_MinVertG)/(l - THM_MinVertG));
			else if (r >= l) e += saturate((r - l)/(THM_MaxVertG - l));
			e = pow(e, THM_POWER);
		}
		
		if (e < .7) e *= THM_LOWCOF;
	}
	
	if (t.w > 0.74 && t.w < 1.0)
	{
		if (!s)
			return float4(0, 0, .1, 1);
		else 
			return float4(.1, .1, .1, 1);
	}
	else
	{
		float r = lerp(THM_Infra, THM_Infra2, saturate((sin(timers.x - f.y) + 1) / 2.f));
		e *= r;
		float4 I = l__2_ks(e);
		I.xyz *= max(T.x * 2.f, 0.35);
		if (I.x < THM_REDCOF) {I.z *= THM_REDFAC; I.y *= THM_REDFAC;}
		if (s)
		{
			float l = dot(I.xyz, float3(1, .5, .2));
			float b = I.x * .5 * I.y * .25 * I.z;
			I.xyz = saturate(lerp(float3(l, l, l), float3(i.w, i.w, i.w), .2));
		}
		return I;
	}
}

float4 l___2_kz(float2 f:TEXCOORD0, float3 i):COLOR
{
	float t = THM_STRIPESRES;
	float w = timers.x * THM_STRIPESVEL;
	i *= .9 + .1 * sin(10. * w + f.y * t);
	i *= .97 + .03 * sin(110. * w);
	return float4(i, 1.f);
}

float4 infrared(float2 f:TEXCOORD0, bool w):COLOR
{
	//float4 pos2d = (0,0,0,0);
	//gbuffer_data gbd = gbuffer_load_data((f,0,0), pos2d);
	//float4	i	= float4( gbd.N, gbd.hemi ), y = l___2_ks(f, i, w);
	
	#if defined(USE_MSAA)
		float4 i = s_normal.Load(f.xy, 0);
	#else
		float4 i = s_normal.Sample(smp_rtlinear, float4(f.xy, 0, 0));
	#endif
	float4 y = l___2_ks(f, i, w);
	return l___2_kz(f, y);
}

#endif