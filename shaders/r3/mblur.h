#ifndef	MBLUR_H
#define MBLUR_H

#ifndef 	USE_MBLUR
	float3 	mblur	(float2 UV, float3 pos, float3 c_original)	{ return c_original; }
#else
#include "common.h"

//	SkyLoader: DX10: Reimplement done
//	DON`T TORCH IT!!

uniform float4x4	m_current;
uniform float4x4	m_previous;
uniform float2 	m_blur;		// scale_x / 12, scale_y / 12

#define MBLUR_SAMPLES	float(12.0f)
#define MBLUR_CLAMP		float(0.001f)
#define SKY_DEPTH	float(10000.f)
#define SKY_EPS		float(0.001)

float3 	mblur	(float2 UV, float3 pos, float3 c_original)	
{
	// The sky depth value is 0 since we're rendering it without z-test
	if (pos.z < 0.00001f)
		pos.z = SKY_DEPTH;

	float4	pos4		= float4(pos, 1.0f);

	float4	p_current	= mul(m_current, pos4);
	float4 	p_previous 	= mul	(m_previous,	pos4);
	float2 	p_velocity 	= m_blur * ( (p_current.xy/p_current.w)-(p_previous.xy/p_previous.w) );
	p_velocity	= clamp	(p_velocity,-MBLUR_CLAMP,+MBLUR_CLAMP);
	float3 	blurred 	= 	c_original	;

	for (float i = 1; i < MBLUR_SAMPLES; i++)
		blurred 	+= 	s_image.Sample( smp_rtlinear, p_velocity * i*0.4  + UV).rgb;

	return 	blurred/MBLUR_SAMPLES;
}

float3 	mblur_r(float2 UV, float3 pos, float3 c_original)	
{
	pos.z   +=		(saturate(0.1 - pos.z)*1000000000.h);

	float4 	pos4		= float4	(pos,0.01h);

	float4 	p_current	= mul	(m_previous,	pos4);
	float4 	p_previous 	= mul	(m_current,	pos4);
	float2 	p_velocity 	= m_blur * ( (p_current.xy/p_current.w)-(p_previous.xy/p_previous.w) );
	p_velocity	= clamp	(p_velocity,-MBLUR_CLAMP,+MBLUR_CLAMP);
	float3 	blurred 	= 	c_original	;

	for (float i = 1; i < MBLUR_SAMPLES; i++)
		blurred 	+= 	s_image.Sample( smp_rtlinear, p_velocity * i*0.4  + UV).rgb;

	return 	blurred/MBLUR_SAMPLES;
}

#endif

#endif