#if !defined(WATER_PV)
#define WATER_PV
#define RIPPLE_MAX_RADIUS 4
#define RIPPLE_FREQUENCY 0.5

	#define	NEED_SOFT_WATER

	#if !defined(WATER_DISTORT)
		struct wivs_out
		{
			float2	tbase		: TEXCOORD0;
			float4	tnorm0		: TEXCOORD1;
			float4	position_w	: TEXCOORD2;
			float3	M1			: TEXCOORD3;
			float3	M2			: TEXCOORD4;
			float3	M3			: TEXCOORD5;
			float3	v2point		: TEXCOORD6;
			float4	tctexgen	: TEXCOORD7;
			float	for_pr		: TEXCOORD8;
			float4	c0			: COLOR0;
			float4	c1			: COLOR1;
			float	fog			: FOG;
			float4	hpos		: SV_Position;
			float4	T	: TANGENT;
		};

		#if defined(WATER_VERTEX)
			struct wivs_in
			{
				float4	P		: POSITION;		
				float4	N		: NORMAL;		
				float4	T		: TANGENT;
				float4	B		: BINORMAL;
				float4	color	: COLOR0;		
				int2	uv		: TEXCOORD0;	
			};
		#endif

		uniform float4x4	m_texgen;

		#if defined(WATER_PIXEL)
			Texture2D	s_base_static;
			Texture2D	s_leaves;
			Texture2D	s_foam;
			Texture2D	s_mud;
			Texture2D	s_nmap;
			Texture2D	s_nmap_leaves;
			//Texture2D	jitter0;
			TextureCube	s_env0;
			TextureCube	s_env1;
			TextureCube	s_empty;
			uniform 	float4 various;
		#endif
	#else

		Texture2D	s_nmap;
		Texture2D	s_nmap_blurred;
			
		struct v2p
		{
			float2	tbase		: TEXCOORD0;
			float4	tnorm0		: TEXCOORD1;
			float4	position_w	: TEXCOORD2;
			float3	M1			: TEXCOORD3;
			float3	M2			: TEXCOORD4;
			float3	M3			: TEXCOORD5;
			float3	v2point		: TEXCOORD6;
			float4	tctexgen	: TEXCOORD7;
			float	for_pr		: TEXCOORD8;
			float4	c0			: COLOR0;
			float4	c1			: COLOR1;
			float	fog			: FOG;
			float4	hpos		: SV_Position;
		};

		#define POWER        .5h

		Texture2D	s_distort;
		
	#endif

	#if !defined(WATER_PV2)
	#define WATER_PV2

		Texture2D	s_nmap_empty;
		Texture2D	s_dmap_empty;
		Texture2D  	s_dudv_water_map;
		Texture2D	s_nmap_noise;
		Texture2D	s_pmask;
		
		float4 RipplesPS(Texture2D you_tex, sampler you_smp, float2 tc, float intensity)
		{
			float resolution = 30;

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
			
			float4 	ripples 	 = you_tex.Sample(you_smp, uv/resolution - n.xy);
					ripples.xyz /= intensity;
					ripples.xyz	+= intensity*(10.*pow(clamp(dot(n, normalize(float3(1., 0.7, 0.5))), 0., 1.), 6.));

			return ripples;
		}
	#endif
#endif