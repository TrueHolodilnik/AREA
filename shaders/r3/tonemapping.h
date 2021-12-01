#ifndef	TONEMAPPING_H
#define	TONEMAPPING_H

uniform float tnmp_a;
uniform float tnmp_b;
uniform float tnmp_c;
uniform float tnmp_d;
uniform float tnmp_e;
uniform float tnmp_f;	
uniform float tnmp_w;
uniform float tnmp_exposure;
uniform float tnmp_gamma;
uniform float tnmp_onoff;

float4 Uncharted2ToneMapping(float4 color)
{

    color *= tnmp_exposure;
    color = ((color * (tnmp_a * color + tnmp_c * tnmp_b) + tnmp_d * tnmp_e) / (color * (tnmp_a * color + tnmp_b) + tnmp_d * tnmp_f)) - tnmp_e / tnmp_f;
    float white = ((tnmp_w * (tnmp_a * tnmp_w + tnmp_c * tnmp_b) + tnmp_d * tnmp_e) / (tnmp_w * (tnmp_a * tnmp_w + tnmp_b) + tnmp_d * tnmp_f)) - tnmp_e / tnmp_f;
    color /= white;
    color = pow(color, (1.f / tnmp_gamma));
    return color;
}

float3 ColorFilmicToneMapping(/*in*/ float3 x)
{
	const float3 A = float3(0.55f, 0.50f, 0.45f);	// Shoulder strength
	const float3 B = float3(0.30f, 0.27f, 0.22f);	// Linear strength
	const float3 C = float3(0.10f, 0.10f, 0.10f);	// Linear angle
	const float3 D = float3(0.10f, 0.07f, 0.03f);	// Toe strength
	const float3 E = float3(0.01f, 0.01f, 0.01f);	// Toe Numerator
	const float3 F = float3(0.30f, 0.30f, 0.30f);	// Toe Denominator
	const float3 W = float3(2.80f, 2.90f, 3.10f);	// Linear White Point Value
	const float3 F_linearWhite = ((W*(A*W+C*B)+D*E)/(W*(A*W+B)+D*F))-(E/F);
	float3 F_linearColor = ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-(E/F);
	return pow(saturate(F_linearColor * 1.25 / F_linearWhite),1.25);
}

float3 Uncharted2Tonemap(float3	x)
{
	float a = 2.51f;
	float b = 0.03f;
	float c = 2.43f;
	float d = 0.59f;
	float e = 0.14f;
	return saturate((x*(a*x+b))/(x*(c*x+d)+e));
}
#endif