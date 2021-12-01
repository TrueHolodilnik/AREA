#ifndef	common_functions_h_included
#define	common_functions_h_included
	
	//-------------------------------------------------------------------
	// Доп. функции для упаковки float2 во float с потерями от 0.0001, 
	// Перед использованием привести float2 в saturated!!!
		float gbuf_pack_float2(float input_value_x, float input_value_y)
		{
			int first_value = saturate(input_value_x) * 10000;

			float second_value = saturate(input_value_y);

			return float(first_value) + clamp(second_value, 0.0001, 0.9999);
		}

		float2 gbuf_unpack_float2(float input_value_xy)
		{
			// Unpack float to float2
			float2	unpacked_value;

					unpacked_value.x  =  int(input_value_xy) / 10000;
					unpacked_value.y  =  float(input_value_xy) - int(input_value_xy);

			return  saturate(unpacked_value);
		}
	//-------------------------------------------------------------------

	float gbuf_get_depth(float2 texcoord)
	{
		return s_position.Sample(smp_nofilter, texcoord).z;
	}

	float gbuf_get_depth(float4 texcoord_proj)
	{
		float2 texcoord = texcoord_proj.xy/texcoord_proj.w;

		return s_position.Sample(smp_nofilter, texcoord).z;
	}

	float gbuf_get_true_depth(float2 texcoord)
	{
		float depth = gbuf_get_depth(texcoord);

		return (depth > 0.0025) ? (depth) : (1000);
	}

	float gbuf_transform_depth(float depth)
	{
		return depth * (1 - step(0.001, abs(depth - 10000)));
	}

	float3 gbuf_get_position(float2 texcoord, float4 pos2d)
	{
		float	pz		= s_position.Sample(smp_nofilter, texcoord).z;
		float2 	pxy		= float2(pz * (pos2d * pos_decompression_params.zw - pos_decompression_params.xy));

		return float3(pxy, pz);
	}

	float3 gbuf_get_position(float depth, float4 pos2d)
	{
		float2 	pxy = float2(depth * (pos2d * pos_decompression_params.zw - pos_decompression_params.xy));

		return float3(pxy, depth);
	}


	float3 gbuf_get_position(float2 uv, bool normalize)
	{
		float eye_z;

		if(normalize==true) 
		{eye_z = normalize_depth(gbuf_get_depth(uv));}
		else
		{eye_z = gbuf_get_true_depth(uv);}

		uv = (uv * float2(2.0, -2.0) - float2(1.0, -1.0));

		float3 pos = float3(uv * InvFocalLen * eye_z, eye_z);

		return pos;
	}

	float3 gbuf_get_position(float2 uv)
	{
		float eye_z = gbuf_get_depth(uv);

		uv = (uv * float2(2.0, -2.0) - float2(1.0, -1.0));

		float3 pos = float3(uv * InvFocalLen * eye_z, eye_z);

		return pos;
	}

	float3 gbuf_get_diffuse(float2 uv)
	{
		return s_diffuse.Sample(smp_nofilter, uv).xyz;
	}

	// Holger Gruen AMD - I change normal packing and unpacking to make sure N.z is accessible without ALU cost
	// this help the HDAO compute shader to run more efficiently
	float2 gbuf_pack_normal( float3 norm )
	{
	   float2 res;

	   res.x  = norm.z;
	   res.y  = 0.5f * ( norm.x + 1.0f ) ;
	   res.y *= ( norm.y < 0.0f ? -1.0f : 1.0f );

	   return res;
	}

	float3 gbuf_unpack_normal( float2 norm )
	{
	   float3 res;

	   res.z  = norm.x;
	   res.x  = ( 2.0f * abs( norm.y ) ) - 1.0f;
	   res.y = ( norm.y < 0 ? -1.0 : 1.0 ) * sqrt( abs( 1 - res.x * res.x - res.z * res.z ) );

	   return res;
	}

	float3 gbuf_get_normal( float2 texcoord )
	{
		float2 nxy	= s_position.Sample(smp_nofilter, texcoord).xy;

		return gbuf_unpack_normal(nxy);
	}

	uniform float4x4 m_DWMP;

	float2 ss_tc_from_wo_posi(float3 wo_posi)
	{
		float4	p_posi	   = mul(m_VP, float4(wo_posi, 1));
		float4	ss_tc 	   = proj_to_screen(p_posi);
				ss_tc.xy  /= ss_tc.w;

		return ss_tc.xy;
	}

	float3 wo_posi_from_ss_posi(float3 ss_posi)
	{
		float4 wposi  = float4(0, 0, 0, 1);
			   wposi  = mul(m_DWMP, float4(ss_posi, 1));

		return wposi.xyz;
	}

	float3 wo_posi_from_ss_tc(float2 tc)
	{
		return wo_posi_from_ss_posi(gbuf_get_position(tc, false));
	}

	float gbuf_pack_hemi_mtl(float hemi, float mtl)
	{
	   uint packed_mtl = uint( ( mtl / 1.333333333 ) * 31.0 );

		//	Clamp hemi max value
		uint packed = ( MUST_BE_SET + ( uint( saturate(hemi) * 255.9 ) << 13 ) + ( ( packed_mtl & uint( 31 ) ) << 21 ) );

	   if( ( packed & USABLE_BIT_13 ) == 0 )
		  packed |= USABLE_BIT_14;

	   if( packed_mtl & uint( 16 ) )
		  packed |= USABLE_BIT_15;

	   return asfloat( packed );
	}

	float gbuf_unpack_hemi( float mtl_hemi )
	{
		return float( ( asuint( mtl_hemi ) >> 13 ) & uint(255) ) * (1.0/254.8);
	}

	float gbuf_unpack_mtl( float mtl_hemi )
	{
	   uint packed       = asuint( mtl_hemi );
	   uint packed_hemi  = ( ( packed >> 21 ) & uint(15) ) + ( ( packed & USABLE_BIT_15 ) == 0 ? 0 : 16 );
	   return float( packed_hemi ) * (1.0/31.0) * 1.333333333;
	}

	void pack_gbuffer(out f_deffer O, in float4 norm, in float4 pos, in float4 col, in float arg1, in float arg2, in float arg3, in float arg4)
	{
		O.C			= col;
		O.position	= float4(gbuf_pack_normal(norm.xyz), pos.z, gbuf_pack_hemi_mtl(norm.w, pos.w));

		#if defined(ENGINE_RENDER_DWMBUFFER)
			O.dwmbuffer	= float4(arg1, arg2, arg3, arg4);
		#endif
	}

	#if defined(OLDSTYLE_GBUFFER) && !defined(NEWSTYLE_GBUFFER)
		f_deffer pack_gbuffer(float4 norm, float4 pos, float4 col)
		{
			f_deffer res;

			res.position	= float4(gbuf_pack_normal(norm), pos.z, gbuf_pack_hemi_mtl(norm.w, pos.w));
			res.C			= col;

			#if defined(ENGINE_RENDER_DWMBUFFER)
				res.dwmbuffer	= float4(0, 0, 0, 0);
			#endif

			return res;
		}
	#else
		void pack_gbuffer(out f_deffer O, in float4 norm, in float4 pos, in float4 col)
		{
			O.C			= col;
			O.position	= float4(gbuf_pack_normal(norm.xyz), pos.z, gbuf_pack_hemi_mtl(norm.w, pos.w));

			#if defined(ENGINE_RENDER_DWMBUFFER)
				O.dwmbuffer	= float4(0, 0, 0, 0);
			#endif
		}
	#endif

	gbuffer_data gbuffer_load_data( float2 tc : TEXCOORD, float2 pos2d, int iSample )
	{
		gbuffer_data gbd;

		gbd.P = float3(0,0,0);
		gbd.hemi = 0;
		gbd.mtl = 0;
		gbd.C = 0;
		gbd.N = float3(0,0,0);

		float4 P	= s_position.Sample( smp_nofilter, tc );

		// 3d view space pos reconstruction math
		// center of the plane (0,0) or (0.5,0.5) at distance 1 is eyepoint(0,0,0) + lookat (assuming |lookat| ==1
		// left/right = (0,0,1) -/+ tan(fHorzFOV/2) * (1,0,0 ) 
		// top/bottom = (0,0,1) +/- tan(fVertFOV/2) * (0,1,0 )
		// lefttop		= ( -tan(fHorzFOV/2),  tan(fVertFOV/2), 1 )
		// righttop		= (  tan(fHorzFOV/2),  tan(fVertFOV/2), 1 )
		// leftbottom   = ( -tan(fHorzFOV/2), -tan(fVertFOV/2), 1 )
		// rightbottom	= (  tan(fHorzFOV/2), -tan(fVertFOV/2), 1 )
		gbd.P  = float3( P.z * ( pos2d * pos_decompression_params.zw - pos_decompression_params.xy ), P.z );

		// reconstruct N
		gbd.N = gbuf_unpack_normal( P.xy );

		// reconstruct material
		gbd.mtl	= gbuf_unpack_mtl( P.w );

	    // reconstruct hemi
	    gbd.hemi = gbuf_unpack_hemi( P.w );

		gbd.C		= gbuf_get_diffuse( tc );
		gbd.gloss	= s_diffuse.Sample( smp_nofilter, tc ).w;

		#if defined(ENGINE_RENDER_DWMBUFFER)
			gbd.DWM = gbuf_get_dwmbuffer(tc);
		#endif

		return gbd;
	}

	gbuffer_data gbuffer_load_data( float2 tc : TEXCOORD, float2 pos2d )
	{
	   return gbuffer_load_data( tc, pos2d, 0 );
	}

	gbuffer_data gbuffer_load_data_offset( float2 tc : TEXCOORD, float2 OffsetTC : TEXCOORD, float2 pos2d )
	{
		float2  delta	  = ( ( OffsetTC - tc ) * pos_decompression_params2.xy );

		return gbuffer_load_data( OffsetTC, pos2d + delta, 0 );
	}

	gbuffer_data gbuffer_load_data_offset( float2 tc : TEXCOORD, float2 OffsetTC : TEXCOORD, float2 pos2d, uint iSample )
	{
	   float2  delta	  = ( ( OffsetTC - tc ) * pos_decompression_params2.xy );

	   return gbuffer_load_data( OffsetTC, pos2d + delta, iSample );
	}

#endif
