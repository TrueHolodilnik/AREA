#ifndef	MBLUR_H
#define MBLUR_H

#include "common.h"

uniform half4x4	m_current;
uniform half4x4	m_previous;
uniform half2 	m_blur;		// scale_x / 12, scale_y / 12

#define MBLUR_SAMPLES 	half(24)
#define MBLUR_CLAMP	half(0.001)

half3 	mblur_r	(float2 UV, half3 pos, half3 c_original)	{
	pos.z   +=		(saturate(0.1 - pos.z)*1000000000.h);
	half4 	pos4		= half4	(pos,0.01h);

	half4 	p_current	= mul	(m_current,	pos4);
	half4 	p_previous 	= mul	(m_previous,	pos4);
	half2 	p_velocity 	= m_blur * ( (p_current.xy/p_current.w)-(p_previous.xy/p_previous.w) );
	p_velocity	= clamp	(p_velocity,-MBLUR_CLAMP,+MBLUR_CLAMP);
	half3 	blurred 	= 	c_original	;

	for (half i = 1; i < MBLUR_SAMPLES; i++)
		blurred 	+= 	tex2D(s_image, p_velocity * i*0.4  + UV).rgb;

	return 	blurred/MBLUR_SAMPLES;
}

half3 	mblur	(float2 UV, half3 pos, half3 c_original)	{
	pos.z   +=		(saturate(0.1 - pos.z)*1000000000.h);
	half4 	pos4		= half4	(pos,0.01h);

	half4 	p_current	= mul	(m_previous,	pos4);
	half4 	p_previous 	= mul	(m_current,	pos4);
	half2 	p_velocity 	= m_blur * ( (p_current.xy/p_current.w)-(p_previous.xy/p_previous.w) );
	p_velocity	= clamp	(p_velocity,-MBLUR_CLAMP,+MBLUR_CLAMP);
	half3 	blurred 	= 	c_original	;
        
	for (half i = 1; i < MBLUR_SAMPLES; i++)
		blurred 	+= 	tex2D(s_image, p_velocity * i*0.4  + UV).rgb;

	return 	blurred/MBLUR_SAMPLES;
}


half3 	mblur_bloom	(float2 UV, half3 pos, half3 c_original)	{
	
	int samples = 5;
	half velocity = 12.5;
	
	pos.z   +=		(saturate(0.1 - pos.z)*1000000000.h);
	half4 	pos4		= half4	(pos,0.01h);

	half4 	p_current	= mul	(m_previous,	pos4);
	half4 	p_previous 	= mul	(m_current,	pos4);
	
	
	half2 	p_velocity 	= m_blur * ( (p_current.xy/p_current.w)-(p_previous.xy/p_previous.w) );
	half2 	t_velocity 	= p_velocity * 3 * velocity;
	
	
	p_velocity	= clamp	(p_velocity,-MBLUR_CLAMP,+MBLUR_CLAMP);
	half3 	blurred 	= 	c_original	;
        
	for (half i = 1; i < samples; i++)
		blurred 	+= 	tex2D(s_bloom, t_velocity * i + UV).rgb;

	return 	blurred/samples;
}

#endif

