#ifndef OGSE_REFLECTIONS_H
#define OGSE_REFLECTIONS_H
#define REFL_RANGE 100

#include "ogse_functions.h"
#include "ogse_gbuffer.h"
#include "gauss.h"

static const float2 resolution = screen_res.xy;
static const float2 inv_resolution = screen_res.zw;

uniform samplerCUBE s_env0;
uniform samplerCUBE s_env1;

float4 get_reflection (float3 screen_pixel_pos, float3 next_screen_pixel_pos, float3 reflect)
{
	float4 final_color = {1.0,1.0,1.0,1.0};
	float2 factors = {1.f,1.f};
	
	float3 main_vec = next_screen_pixel_pos - screen_pixel_pos;
	float3 grad_vec = main_vec / (max(abs(main_vec.x), abs(main_vec.y)) * 256);
	
	// handle case when reflect vector faces the camera
	factors.x = dot(eye_direction, reflect);
	
	if ((factors.x < -0.5) || (screen_pixel_pos.z > REFL_RANGE)) return final_color;
	else
	{	
		float3 curr_pixel = screen_pixel_pos;
		curr_pixel.xy += float2(0.5,0.5)*screen_res.zw;
		float max_it = 64;
		float i = 0;
		bool br = false;
		
		while ((i < max_it) && (br == false))
		{
			curr_pixel.xyz += grad_vec.xyz;
			float depth = get_depth_fast(curr_pixel.xy);
			depth = lerp(depth, 0.f, is_sky(depth));
			float delta = step(depth, curr_pixel.z)*step(screen_pixel_pos.z, depth);
			if (delta > 0.5)
			{
				final_color.xyz = Gauss(s_image, curr_pixel.xy, 3, true);//tex2Dlod(s_image, float4(curr_pixel.xy,0,0)).xyz;
				float2 tmp = curr_pixel.xy;
				tmp.y = lerp(tmp.y, 0.5, step(0.5, tmp.y));
				float screendedgefact = saturate(distance(tmp , float2(0.5, 0.5)) * 2.0);
				final_color.w = pow(screendedgefact,6);
				//final_color.xyz = float3(0,0,0);
				br = true;
			}
			i += 1.0;
		}
		return lerp(final_color,float4(1.0,1.0,1.0,1.0),screen_pixel_pos.z/REFL_RANGE);
	}
}

float3 calc_envmap(float3 vreflect)
{
	vreflect.y = vreflect.y*2-1;
	float3	env0	= texCUBElod (s_env0, float4(vreflect.xyz, 0)).xyz;
	float3	env1	= texCUBElod (s_env1, float4(vreflect.xyz, 0)).xyz;
	return lerp (env0,env1,L_ambient.w);
}
float4 calc_reflections(float4 pos, float3 vreflect)
{
	float4 refl = {1.0,1.0,1.0,1.0};
	
	float3 v_pixel_pos = mul((float3x4)m_V, pos);
	float4 p_pixel_pos = mul(m_VP, pos);
	float4 s_pixel_pos = proj_to_screen(p_pixel_pos);
	s_pixel_pos.xy /= s_pixel_pos.w;
	s_pixel_pos.z = v_pixel_pos.z;
		
	float3 reflect_vec = normalize(vreflect);
	float3 W_m_point = pos.xyz + reflect_vec;

	float3 V_m_point = mul((float3x4)m_V, float4(W_m_point, 1.0));
	float4 P_m_point = mul(m_VP, float4(W_m_point, 1.0));
	float4 S_m_point = proj_to_screen(P_m_point);
	S_m_point.xy /= S_m_point.w;
	S_m_point.z = V_m_point.z;
		
	refl = get_reflection(s_pixel_pos.xyz, S_m_point.xyz, reflect_vec);
	
	return refl;
}
float4 calc_reflections_late_out(float4 pos, float3 vreflect, float sw)
{
	float4 refl = {1.0,1.0,1.0,1.0};

	float3 v_pixel_pos = mul((float3x4)m_V, pos);
	float4 p_pixel_pos = mul(m_VP, pos);
	float4 s_pixel_pos = proj_to_screen(p_pixel_pos);
	s_pixel_pos.xy /= s_pixel_pos.w;
	s_pixel_pos.z = v_pixel_pos.z;
		
	float3 reflect_vec = normalize(vreflect);
	float3 W_m_point = pos.xyz + reflect_vec;

	float3 V_m_point = mul((float3x4)m_V, float4(W_m_point, 1.0));
	float4 P_m_point = mul(m_VP, float4(W_m_point, 1.0));
	float4 S_m_point = proj_to_screen(P_m_point);
	S_m_point.xy /= S_m_point.w;
	S_m_point.z = V_m_point.z;

	if (sw > 0.01)
		refl = get_reflection(s_pixel_pos.xyz, S_m_point.xyz, reflect_vec);

	return refl;
}

float ripples(float2 tc)
{
	float resolution = 15.;
	float2 uv = tc * resolution;
    float2 p0 = floor(uv);

    float2 circles;
    for (int j = -2; j <= 2; ++j)
    {
        for (int i = -2; i <= 2; ++i)
        {
			float2 pi = p0 + float2(i, j);
            float2 hsh = hash22(pi);
            float2 p = pi + hash22(hsh);

            float t = frac(0.35f*timers.x + hash12(hsh));
            float2 v = p - uv;
            float d = length(v) - (float(2.f) + 1.)*t;

            float h = 1e-3;

			//1st pass
            float d1 = d - h;
            float p1 = sin(31.*d1) * smoothstep(-0.6, -0.3, d1) * smoothstep(0., -0.3, d1);
			//sec pass
            float d2 = d + h;
            float p2 = sin(31.*d2) * smoothstep(-0.6, -0.3, d2) * smoothstep(0., -0.3, d2);
            circles += 0.5 * normalize(v) * ((p2 - p1) / (2. * h) * (1. - t) * (1. - t));
        }
    }
    circles /= float((2*3+1)*(2*2+1));

    float intensity = lerp(0.025f, 0.15, smoothstep(0.1, 0.6, frac(0.05 + 0.05)*2.-1.));
    float3 n = float3(circles, sqrt(1. - dot(circles, circles)));
	
	float final = 10.*pow(clamp(dot(n, normalize(float3(1., 0.7, 0.5))), 0., 1.), 6.);
	return final;
}

#endif