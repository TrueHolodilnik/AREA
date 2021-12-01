#ifndef BLEND_WETNESS
#define BLEND_WETNESS
	#if USE_WET_SURFACES==1
		#ifndef USE_SUNMASK
			float3x4 m_sunmask;
		#endif

		Texture2D s_wetmap;

		class wetmap_data
		{
			float road;
			float grnd;
		};

		wetmap_data get_wetmap(float2 tc)
		{
			wetmap_data wmp;

			wmp.road = s_wetmap.Sample(smp_nofilter, tc).x;
			wmp.grnd = s_wetmap.Sample(smp_nofilter, tc).y;

			return wmp;
		}

		float hash12(float2 p)
		{
			float3 p3  = frac(float3(p.xyx) * HASHSCALE1);
			p3 += dot(p3, p3.yzx + 19.19);
			return frac((p3.x + p3.y) * p3.z);
		}

		float2 hash22(float2 p)
		{
			float3 p3 = frac(float3(p.xyx) * HASHSCALE3);
			p3 += dot(p3, p3.yzx+19.19);
			return frac((p3.xx+p3.yz)*p3.zy);
		}

		Texture2D s_test_tex;	

		float4 get_ripples_droplets(float2 tc)
		{
			float resolution = 6;

			float2 uv = tc * resolution;
			float2 p0 = floor(uv);

			float2 circles;

			for (int j = -RIPPLE_MAX_RADIUS; j <= RIPPLE_MAX_RADIUS; ++j)
			{
				for (int i = -RIPPLE_MAX_RADIUS; i <= RIPPLE_MAX_RADIUS; ++i)
				{
					float2 pi = p0 + float2(i, j);
					float2 hsh = hash22(pi);
					float2 p = pi + hash22(hsh);

					float t = frac(RIPPLE_FREQUENCY*timers.x + hash12(hsh));
					float2 v = p - uv;
					float d = length(v) - (float(RIPPLE_MAX_RADIUS) + 1)*t;

					float h = 1e-3;

					float d1 = d - h;
					float p1 = sin(31*d1) * smoothstep(-0.6, -0.3, d1) * smoothstep(0., -0.3, d1);

					float d2 = d + h;
					float p2 = sin(31*d2) * smoothstep(-0.6, -0.3, d2) * smoothstep(0., -0.3, d2);
					circles += 0.5 * normalize(v) * ((p2 - p1) / (2 * h) * (1 - t) * (1 - t));
				}
			}

			circles /= float((2*4+1)*(2*2+1));

			float3 	n = float3(circles, sqrt(1 - dot(circles, circles)));
			
			float4 	ripples 	 = s_test_tex.Sample(smp_base, uv/resolution - n.xy);
					ripples.xyz /= 3;
					ripples.xyz	+= 3 *(10.*pow(clamp(dot(n, normalize(float3(1., 0.7, 0.5))), 0., 1.), 6.));

			return ripples;
		}

		Texture3D 	s_water;

		float3 get_water_droplets(float2 tc)
		{
			//	Unpack NVidia normal map
			float4 water = s_water.SampleBias(smp_base, float3(tc, timers.x * 3.0), -3.) - 0.5;

			//	Swizzle
			water.xyz = water.wyz;

			//	Renormalize (*2) and scale (*3)
			water.xyz *= 6;

			water.y = 0;

			return water.xyz;
		}

		Texture2D 	s_waterFall;

		float3 get_fall_water(float2 tc)
		{
			//	Unpack normal map
			float4 water = s_waterFall.Sample(smp_base, tc);
			water.xyz = (water.xzy-0.5)*2;

			water.xyz *= 0.3;

			water.y = 0;

			return water.xyz;
		}

		float3 GetWaterNMap(float4 water)
		{
			water.xyz = (water.xzy-0.5)*2;

			water.xyz *= 0.3;

			water.y = 0;

			return water.xyz;
		}
	#endif
#endif