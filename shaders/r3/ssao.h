#ifndef SSAO_H
#define SSAO_H

	uniform int dwframe;
	
	uniform float4x4 m_VP_prev;
	
	Texture2D	jitter0;
	
	bool check_screen_space_0_1(in float2 tc)
	{
		return (tc.x <= 1) && (tc.y <= 1) && (tc.x >= 0) && (tc.y >= 0);
	}
	
	float load_depth_wsky(float2 tc)
	{
		float depth = load_depth(tc);
		return (depth >= 0.01) ? (depth) : (DwmSkyConstDepth);
	}

	// Uniformly distributed points on the surface of a hemisphere that is oriented by the normal
	// https://stackoverflow.com/questions/9600801/evenly-distributing-n-points-on-a-sphere/44164075#44164075
	float3 hemisphere_on_normal(uint points_num, uint point_curr, float3 normal, float2 noise_2d, float radius) 
	{
		float indices = (point_curr + 0.5) * (1 + noise_2d.x);

		float theta = 10.16640738463119 * indices; // (pi * (1 + sqrt(5))) * indices
		float2 sc_theta;
		sincos(theta, sc_theta.x, sc_theta.y);

		float phi = acos(1 - 2 * indices / points_num);
		float2 sc_phi;
		sincos(phi, sc_phi.x, sc_phi.y);

		float3 sphere = float3(sc_theta.y * sc_phi.x, sc_theta.x * sc_phi.x, sc_phi.y) * noise_2d.y;

		float3 hemisphere = sphere * sign(dot(sphere, normal));

		return hemisphere;
	}

	struct INTERSECTION
	{
		uint found;
		float3 position;
		float2 tc;
		float error;
	};

	INTERSECTION find_intersection(uint steps, float3 position, float3 direction, float radius, float increase, float bias, float thickness)
	{
		INTERSECTION intersection = { 0, position, float2(0, 0), 0 };

		direction = direction * radius / steps;

		for(uint j = 0; j < steps; j++)
		{
			intersection.position += direction;

			direction *= increase;

			intersection.tc = (intersection.position.xy / intersection.position.zz + pos_decompression_params.xy) / pos_decompression_params.zw / pos_decompression_params2.xy;

			if(!check_screen_space_0_1(intersection.tc))
				break;

			float hit = load_depth_wsky(intersection.tc);

			intersection.error = intersection.position.z - hit;

			if(is_in_range(intersection.error, bias, thickness))
			{
				intersection.found = 1;
				break;
			}
		}

		return intersection;
	}

	float ssao_path_trace(G_BUFFER::GBD gbd)
	{
		if(gbd.P.z <= 0.01)
			return 1;

		static const int 	rays 		= 5;		// Количество лучей
		static const int 	steps 		= 4;		// Количество шагов при поиске пересечения
		static const float 	radius 		= 3.5;		// Максимальный радиус SSAO
		static const float 	increase 	= 1.15;		// Прирост шага после неудачного поиска пересечения
		static const float	bias 		= 0.015;	// Смещение от точки пересечения
		static const float 	thickness 	= 1;		// Толщина затенения
		static const float 	power 		= 1.5;		// Сила затенения

		float2 hash = noise::hash23(float3(gbd.P.xy * 64, dwframe));

		float3	rm_position	= gbd.P + gbd.N * bias;
		float 	rm_radius	= radius / steps;

		float ssao = 0;

		for(int i = 0; i < rays; i++) 
		{
			float3 hemisphere = hemisphere_on_normal(rays, i, gbd.N, hash, radius);

			INTERSECTION intersection = find_intersection(steps, rm_position, hemisphere, rm_radius, increase, bias, thickness);

			ssao += intersection.found;
		}

		ssao = pow(1 - ssao / rays, power);

		return ssao;
	}

	uniform float ssao_jitter_param;

	float ssao(GBD gbd, uint2 pos2d)
	{
		if(gbd.P.z <= 0.01)
			return 1;

		static const int	rays 		= 5;		// Количество лучей
		static const float	radius 		= 0.8;		// Максимальный радиус SSAO
		static const float	bias 		= 0.015;	// Смещение от точки пересечения
		static const float	power 		= 4;		// Сила затенения
		static const float	randomize 	= 0.02;		// Случайный шум
		
		float ssao_jitter_param = 2.0f;
		ssao_jitter_param *= tan(deg2rad(67.5f / 2.0f));
		ssao_jitter_param /= tan(deg2rad(80 / 2.0f));

		float3 Pw = mul((float3x4)m_v2w, float4(gbd.P, 1));

		float2 tcj = (Pw.xz + Pw.y) * 2; //ssao_jitter_param

		float2 hash_hq = jitter0.SampleLevel(smp_jitter, tcj, 0);
		float2 hash_lq = noise::hash22(pos2d % 4);

		float2 hash = lerp(hash_lq, hash_hq, randomize);

		float3 rm_position	= gbd.P + gbd.N * bias;

		float ssao = 0;

		for(int i = 0; i < rays; i++) 
		{
			float3 hemisphere = hemisphere_on_normal(rays, i, gbd.N, hash, radius);

			INTERSECTION intersection = find_intersection(1, rm_position, hemisphere, radius, 1, bias, 1);

			if(intersection.found)
				ssao += 1 - intersection.error * intersection.error;
		}

		ssao = pow(1 - ssao / rays, power);

		return ssao;
	}

	uniform Texture2D<float> s_occ;

	float ssao_filter(float2 tc)
	{
		return (s_occ.Sample(smp_rtlinear, tc + screen_res.zw * float2( 1.5, -0.5))
			  + s_occ.Sample(smp_rtlinear, tc + screen_res.zw * float2( 1.5,  1.5))
			  + s_occ.Sample(smp_rtlinear, tc + screen_res.zw * float2(-0.5, -0.5))
			  + s_occ.Sample(smp_rtlinear, tc + screen_res.zw * float2(-0.5,  1.5))) * 0.25;
	}
#endif
