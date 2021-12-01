#ifndef SSLR_INCLUDED
#define SSLR_INCLUDED
	#include "hmodel.h"

	
	uniform float4 ogse_c_screen;
	uniform float4 screen_res;
	uniform float4 sky_params;
	
	// Там не надо с небом условий делать, оно там убирается
	#if defined(USE_MSAA)
		float gbuf_get_depth(float2 texcoord, uint iSample)
	#else
		float gbuf_get_depth(float2 texcoord)
	#endif
	{
		#if defined(USE_MSAA)
			float depth = s_position.Load(int3(texcoord * screen_res.xy, 0), iSample).z;
		#else
			float depth = s_position.Sample(smp_nofilter, texcoord).z;
		#endif

		return depth;
	}
	
	#define InvFocalLen float2(tan(0.5f*radians(ogse_c_screen.x)) / (float)screen_res.y * (float)screen_res.x, tan(0.5f*radians(ogse_c_screen.x)))
	
	float2 ss_tc_from_wo_posi(float3 wo_posi)
	{
		float4	p_posi	   = mul(m_VP, float4(wo_posi, 1));
		float4	ss_tc 	   = proj_to_screen(p_posi);
				ss_tc.xy  /= ss_tc.w;

		return ss_tc.xy;
	}
	
	bool check_screen_space_0_1(in float2 tc)
	{
		return (tc.x <= 1) && (tc.y <= 1) && (tc.x >= 0) && (tc.y >= 0);
	}
	
	float calc_vignette_0_1(in float2 tc, in float power, bool need_check_ss = false)
	{
	float vgn = 1;
	
	if(need_check_ss) if(!check_screen_space_0_1(tc))
	vgn = 0;
	
	if(tc.y < power)
	vgn *= saturate(tc.y) / power;
	
	if(tc.x < power)
	vgn *= saturate(tc.x) / power;
	
	float2 tci = 1 - tc;
	
	if(tci.y < power)
	vgn *= saturate(tci.y) / power;
	
	if(tci.x < power)
	vgn *= saturate(tci.x) / power;
	
	return vgn;
	}
	
	bool check_screen_space_0_USR(in unsigned int2 tc)
	{
	return (tc.x < (unsigned int)(pos_decompression_params2.x)) && (tc.y < (unsigned int)(pos_decompression_params2.y));
	}


	
	#if defined(USE_MSAA) && defined(SSR_ROAD)
		float3 gbuf_get_position(float2 uv, uint iSample)
	#else
		float3 gbuf_get_position(float2 uv)
	#endif
	{
		float eye_z; 
		#if defined(SSR_ROAD)
			#if defined(USE_MSAA)
				eye_z = s_position.Load(int2(uv * screen_res.xy), iSample).z;
			#else
				eye_z = s_position.Sample(smp_nofilter, uv).z;
			#endif
		#else
			#if defined(USE_MSAA)
				eye_z = s_position.Load(int2(uv * screen_res.xy), 0).z;
			#else
				eye_z = s_position.Sample(smp_nofilter, uv).z;
			#endif
		#endif
		
		uv = (uv * float2(2.0, -2.0) - float2(1.0, -1.0));
		
		float3 pos = float3(uv * InvFocalLen, 1);
		
		return pos * eye_z;
	}
	
	#define SSR_USE_WE_SKY_C
	#define SSR_NEAR_AF_FOUNDER
	
	#if defined(USE_MSAA)
		Texture2DMS <float4> s_pptemp;
	#else
		Texture2D <float3> s_pptemp;
	#endif
	
	#ifdef USE_MSAA
		#ifndef USE_DX11
			Texture2DMS<float4,MSAA_SAMPLES>	s_volumetric;     	
		#else
			Texture2DMS<float4>	s_volumetric;      	
		#endif
	#else
		Texture2D	s_volumetric;     
	#endif
	
	// Random 4x4 noise tab
	static const float4 noise2D4x4random[4] =
	{
		// 1 line
		float4(0.04,0.75,0.18,1.00),
		// 2 line
		float4(0.25,0.70,0.38,0.89),
		// 3 line
		float4(0.64,0.28,0.13,0.50),
		// 4 line
		float4(0.81,0.42,0.94,0.37),
	};

	#if defined(SSR_ROAD)
		#if defined(USE_MSAA)
			#ifdef GBUFFER_OPTIMIZATION
				_out depth_traced_ssr(in float3 scposition, in float3 scvreflect, in float3 wsvreflect, uint iSample, float4 pos2d, gbuffer_data gbd, float2 uv, float frenel)
			#else
				_out depth_traced_ssr(in float3 scposition, in float3 scvreflect, in float3 wsvreflect, uint iSample, gbuffer_data gbd, float2 uv, float frenel)
			#endif
		#else
			#ifdef GBUFFER_OPTIMIZATION
				_out depth_traced_ssr(in float3 scposition, in float3 scvreflect, in float3 wsvreflect,  float4 pos2d, gbuffer_data gbd, float2 uv, float frenel)
			#else
				_out depth_traced_ssr(in float3 scposition, in float3 scvreflect, in float3 wsvreflect, gbuffer_data gbd, float2 uv, float frenel)
			#endif
		#endif
	#else
		#if defined(USE_MSAA)
			#ifdef GBUFFER_OPTIMIZATION
				float3 depth_traced_ssr(in float3 scposition, in float3 scvreflect, in float3 wsvreflect, in float4 pos2d, in float3 sun, in float hemi, uint iSample)
			#else
				float3 depth_traced_ssr(in float3 scposition, in float3 scvreflect, in float3 wsvreflect, in float3 sun, in float hemi, uint iSample)
			#endif
		#else
			#ifdef GBUFFER_OPTIMIZATION
				float3 depth_traced_ssr(in float3 scposition, in float3 scvreflect, in float3 wsvreflect, in float4 pos2d, in float3 sun, in float hemi)
			#else
				float3 depth_traced_ssr(in float3 scposition, in float3 scvreflect, in float3 wsvreflect, in float3 sun, in float hemi)
			#endif
		#endif
	#endif
	{
		
		//#if !defined(SSR_ROAD)
		#if defined(USE_MSAA)
			float3 sky = sky_true_reflection(sky_s0, sky_s1, wsvreflect); // Get true sky cubemap
		#else
			float3 sky = sky_fake_reflection(sky_s0, sky_s1, wsvreflect); // Get true sky cubemap
		#endif
		//#endif
		
		#if defined(SSR_ROAD)
		if (frenel == 0) {
			_out o;
			o.color.rgb = sky * gbd.hemi * gbd.hemi * gbd.hemi * 0.5;
			o.accum = 0; 
			return o;
		}
		#endif
		
		#if defined(SSR_ROAD)
			int SSR_HQ_RAY_STEPS = 2*frenel;
		#else
			int SSR_HQ_RAY_STEPS = 3;
		#endif

		float depth_scale = clamp(scposition.z, SSR_HQ_MARCH_MIN_DIST, SSR_HQ_MARCH_MAX_DIST);
		float scale = SSR_HQ_RAY_BASE_LENGTH * sqrt(depth_scale) / SSR_HQ_RAY_STEPS;
		
		float jitter_scale = 0;
		
		int need_denoise = 0;
		
		uint _y = (scposition.xy * pos_decompression_params2.xy).y % 4;
		uint _x = (scposition.xy * pos_decompression_params2.xy).x % 4;

		float4 _noise = 0;

		_noise = noise2D4x4random[_y];


		switch (_x)
		{
			case 0 : jitter_scale = _noise.x; break;
			case 1 : jitter_scale = _noise.y; break;
			case 2 : jitter_scale = _noise.z; break;
			case 3 : jitter_scale = _noise.w; break;
		}

		
		scale *= lerp(1, jitter_scale, SSR_HQ_JITTER_SCALE);
		
		float3 scvreflect_cycle = scvreflect * scale;
	
		float3 scposition_cycle = scposition;
		
		float diff_threshold = SSR_HQ_DIFF_THRESHOLD * scale;
		
		#if defined(SSR_ROAD)
			need_denoise = 0;
		#endif
		
		float2 tc_new;
		float error_near = 0;
	
		// Depth tracing
		[unroll(SSR_HQ_INC_SAMPLES)]		
		for(int i = 0; i < SSR_HQ_INC_SAMPLES; i++) {
			
			scvreflect_cycle *= SSR_HQ_RAY_INCREASE;
			diff_threshold *= SSR_HQ_RAY_INCREASE;
			
			for(int j = 0; j < SSR_HQ_RAY_STEPS; j++) 
			{
				scposition_cycle += scvreflect_cycle;
				
				float2 tc_step = scposition_cycle.xy / scposition_cycle.zz;
				
				if(check_screen_space_0_1(tc_step)) {
					
					tc_new = tc_step;
					
					#if defined(USE_MSAA)
						float depth_new = gbuf_get_depth(tc_step, iSample);
					#else
						float depth_new = gbuf_get_depth(tc_step);
					#endif
					
					float depth_diff = scposition_cycle.z - depth_new;
					
					// Сравним новую глубину со старой, это уберет небо и артефакты переднего плана
					bool need_reflection = step(scposition.z, depth_new) * step(0.01, depth_new);
					
					if(need_reflection) {
						bool it_reflection = is_in_range(float3(depth_diff, 0, diff_threshold));
					
						if(it_reflection) {
							error_near = 1;
							i = SSR_HQ_INC_SAMPLES;
							break;
						}
					}
				}
				#if defined(SSR_ROAD)
				if(i >= SSR_HQ_INC_SAMPLES - 1)
				need_denoise = 1;
				#endif
			}
		}
		
		scposition_cycle -= scvreflect_cycle;
		scvreflect_cycle /= SSR_HQ_RAY_REFINE;
		
		[unroll(SSR_HQ_RAY_REFINE)]
		for(int i = 0; i < SSR_HQ_RAY_REFINE; i++) {
			
			scposition_cycle += scvreflect_cycle;
			
			float2 tc_step = scposition_cycle.xy / scposition_cycle.zz;
			
			if(check_screen_space_0_1(tc_step)) {
				
				tc_new = tc_step;
				
				#if defined(USE_MSAA)
					float depth_new = gbuf_get_depth(tc_step, iSample);
				#else
					float depth_new = gbuf_get_depth(tc_step);
				#endif
				
				float depth_diff = scposition_cycle.z - depth_new;
				
				// Сравним новую глубину со старой, это уберет небо и артефакты переднего плана
				bool need_reflection = step(scposition.z, depth_new) * step(0.01, depth_new);
				
				if(need_reflection) {
					
					bool it_reflection = is_in_range(float3(depth_diff, 0, diff_threshold));
					
					if(it_reflection) {
						error_near = 1;
						i = SSR_HQ_INC_SAMPLES;
						break;
					}
				}
			}
		}
	
		
		error_near *= calc_vignette_0_1(tc_new, SSR_OUTRANGE_TC_CUTER);
		
		float dx = .3f/screen_res.x;
		float dy = .3f/screen_res.y;
		
		#if defined(SSR_ROAD)
			#if defined(USE_MSAA)
				float3 color = s_frame.Load( int3(tc_new * pos_decompression_params2.xy, 0 ), iSample );
				//float3 accum = s_accumulator.Load( int3(ref.tc * pos_decompression_params2.xy, 0 ), iSample );
				
				//color += s_frame.Load( int3(float2(tc_new.x + 1, tc_new.y) * pos_decompression_params2.xy, 0 ), iSample );
				//color += s_frame.Load( int3(float2(tc_new.x, tc_new.y + 1) * pos_decompression_params2.xy, 0 ), iSample );
				//color += s_frame.Load( int3(float2(tc_new.x - 1, tc_new.y) * pos_decompression_params2.xy, 0 ), iSample );
				//color += s_frame.Load( int3(float2(tc_new.x, tc_new.y - 1) * pos_decompression_params2.xy, 0 ), iSample );
				
				//color /= 2.5;

				//float3 accum = float3(0,0,0);
				
				//float3 plight = plight_infinity(gbd.mtl, screen_to_proj(ref.tc, gbuf_get_depth(ref.tc)), gbd.N.xyz, float4(gbd.C.xyz, gbd.gloss), Ldynamic_dir).rgb;
				//accum += Ldynamic_color * plight;
				
				//[unroll] 
				//for (int iSample = 0; iSample < MSAA_SAMPLES; ++iSample) accum += s_accumulator.Load( int3(tc_new * pos_decompression_params2.xy, 0 ), iSample).rgb;
				
				float4 accum = s_accumulator.Load( int3(tc_new * pos_decompression_params2.xy, 0 ), 0);
				
				//accum /= MSAA_SAMPLES;
			
			#else
				float3 color = s_frame.Sample( smp_nofilter, tc_new).rgb;
				float4 accum = s_accumulator.Sample( smp_nofilter, tc_new);
			#endif
			
			//#ifndef USE_MSAA
			//	float3 reflection = s_accumulator.Sample( smp_nofilter, ref.tc).rgb;	// diffuse.specular
			//#else
			//	float3 reflection = s_accumulator.Load( int3( ref.tc * pos_decompression_params2.xy, 0 ), iSample ).rgb;
			//#endif
		#else
			#if defined(USE_MSAA)
				float3 color = s_pptemp.Load( tc_new * screen_res.xy, iSample).rgb;
				(		
					1.f * s_pptemp.Load( tc_new * screen_res.xy, 0).rgb +
					1.f * s_pptemp.Load( tc_new * screen_res.xy +  float2(dx, 0),0).rgb +
					1.f * s_pptemp.Load( tc_new * screen_res.xy +  float2(dy, 0),0).rgb +
					1.f * s_pptemp.Load( tc_new * screen_res.xy +  float2(-dx, 0),0).rgb +
					1.f * s_pptemp.Load( tc_new * screen_res.xy +  float2(-dx, -dy),0).rgb +
					1.f * s_pptemp.Load( tc_new * screen_res.xy +  float2(dx, -dy),0).rgb
				)/6.f;
			#else
				float3 color = s_pptemp.Sample( smp_nofilter, tc_new).rgb;
				(		
					1.f * s_pptemp.Sample( smp_nofilter, tc_new).rgb +
					1.f * s_pptemp.Sample( smp_nofilter, tc_new +  float2(dx, 0)).rgb +
					1.f * s_pptemp.Sample( smp_nofilter, tc_new +  float2(dy, 0)).rgb +
					1.f * s_pptemp.Sample( smp_nofilter, tc_new +  float2(-dx, 0)).rgb +
					1.f * s_pptemp.Sample( smp_nofilter, tc_new +  float2(-dx, -dy)).rgb +
					1.f * s_pptemp.Sample( smp_nofilter, tc_new +  float2(dx, -dy)).rgb
				)/6.f;	
			#endif
		#endif	
		
		//if (error_near == 0) s = 1;
		//else s = (1 - error_near)/1.25;
		#if defined(SSR_ROAD)
			#if defined(USE_MSAA)
				float4 refl_pos = s_position.Load( tc_new * screen_res.xy, iSample);
			#else
				float4 refl_pos = s_position.Sample( smp_nofilter, tc_new );
			#endif
			float refl_hemi = gbuf_unpack_hemi( refl_pos.w );
		#endif
		

		#if defined(SSR_ROAD)
			color *= error_near;
			float br = (color.r + color.b + color.g)/3;
			if (br == 0) color = sky * gbd.hemi * gbd.hemi * gbd.hemi * 0.5; 
			_out o;
			o.color = float4(color, refl_hemi);
			o.accum = accum * error_near; 
			return o;
		#else
			color *= pow(error_near, 0.25);
			float br = (color.r + color.b + color.g)/3;
			if (br == 0) color = (sky + sun) * hemi * 0.3; 
			return (color * 1.3);
		#endif

	}
#endif