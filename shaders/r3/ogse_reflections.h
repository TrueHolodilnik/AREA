#ifndef OGSE_REFLECTIONS_H
#define OGSE_REFLECTIONS_H

// улучшил огсе отражения
// смотрел работы dynamic shaders meltac, ogse, reshade quint
// можете использовать в модах, но не удаляйте меня тут: Миша Юдин

//uniform float4 screen_res;

float4 get_reflection (float3 screen_pixel_pos, float3 screen_reflect_vec, float3 reflect)
{
	float4 final_color = {1.0,1.0,1.0,1.0};
	float2 factors = {1.f,1.f};
	
	// handle case when reflect vector faces the camera
	factors.x = dot(eye_direction, reflect);

	if (factors.x >= -0.5)
	{
		float max_it = 140;
		float refine_it = 15;

		float tap_size = 65 / max_it; // 65 это длина отражения
		float tap_range = 1.5; // это регулирует обрезание полос
		
		screen_reflect_vec *= tap_size;
		
		float3 curr_pixel = screen_pixel_pos;

		float i = 0;
		
		bool need_refine = false;
		
		[unroll(140)]
		while (i < max_it)
		{
			curr_pixel.xyz += screen_reflect_vec;

			float2 pixel_tc_tap = curr_pixel.xy / curr_pixel.zz;

			#if defined(USE_MSAA)
				float depth = s_position.Load(int3(pixel_tc_tap * screen_res.xy, 0), 0).z;
			#else
				float depth = s_position.Sample(smp_nofilter, pixel_tc_tap).z;
			#endif
	
			//float depth = tex2Dlod(s_position, float4(curr_pixel.xy, 0, 0)).z;
			//depth = lerp(depth, 0.f, is_sky(depth));

			//float delta = step(depth, curr_pixel.z)*step(screen_pixel_pos.z, depth);
			
			// Я думаю, так лучше будет
			float depth_difference = curr_pixel.z - depth;
			bool delta = step(screen_pixel_pos.z, depth) * step(0, depth_difference) * step(0.01, depth);

			// если пиксель за экраном, он идет нахер
			if(pixel_tc_tap.x > 1 || pixel_tc_tap.y > 1 || pixel_tc_tap.x < 0 || pixel_tc_tap.y < 0)
			{
				delta = false;
				i = max_it;
			}
	
			if (delta)
			{
				// ray_in_range делает обрезание полос
				bool ray_in_range = false;

				if(!need_refine)
				{
					ray_in_range = step(depth_difference, tap_range * tap_size);
				
					// тут я делаю подготовку к рефайну
					if(ray_in_range)
					{
						need_refine = true;
						curr_pixel.xyz -= screen_reflect_vec;
						screen_reflect_vec /= refine_it;
						final_color.w = 1;
						i = max_it - refine_it - 1;
					}
				}
				else
				{
					// тут попал в конец рефайна уже, проверку на пиксель не надо
					#if defined(USE_MSAA)
					//alpha *= 1.5;
						final_color.rgb = s_pptemp.Load(pixel_tc_tap * screen_res.xy, 0).rgb;
					#else
						final_color.rgb = s_pptemp.Sample(smp_nofilter, pixel_tc_tap).rgb;
					#endif
					//final_color.xyz = s_image.Sample(smp_rtlinear, pixel_tc_tap).xyz;
					float2 temp = pixel_tc_tap;
					// make sure that there is no fade down the screen
					temp.y = lerp(temp.y, 0.5, step(0.5, temp.y));
					float screendedgefact = saturate(distance(temp , float2(0.5, 0.5)) * 2.0);
					final_color.w = pow(screendedgefact,6);// * screendedgefact;
					i = max_it;
				}
			}
			else
				i += 1.0;
		}
	}

	return saturate(final_color);
}

float4 calc_reflections(float3 pos, float3 vreflect)
{
	// я думаю так лучше будет
	
	// xy это texcoord, z это глубина

	float4 screen_pixel_pos = mul(m_VP, float4(pos, 1));
	screen_pixel_pos.xy = float2(screen_pixel_pos.xy + screen_pixel_pos.ww * float2(1, -1)) * float2(0.5, -0.5) / screen_pixel_pos.ww * screen_pixel_pos.zz;

	float3 reflect_vec = normalize(vreflect);

	// это не кривой вектор отрожения, ху это texcoord, z без изменений в виев спейс, потому что будем мешать его с другими виев спейсами

	float4 screen_reflect_vec = mul(m_VP, float4(vreflect, 0));
	screen_reflect_vec.xy = float2(screen_reflect_vec.xy + screen_reflect_vec.ww * float2(1, -1)) * float2(0.5, -0.5) / screen_reflect_vec.ww * screen_reflect_vec.zz;

	return get_reflection(screen_pixel_pos, screen_reflect_vec, reflect_vec);
}
#endif