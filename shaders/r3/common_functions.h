#ifndef	common_functions_h_included
#define	common_functions_h_included

	float SRGBToLinear(float gammaPre)
	{
		/*
		float Low = gammaPre / 12.92;
		float High = pow((gammaPre + 0.055) / 1.055, 2.4);
		return(gammaPre <= 0.04045) ? Low : High;
		*/
		//return (gammaPre <= 0.04045) ? gammaPre * 0.07739938080495356037151702786378 : pow((gammaPre + 0.055) * 0.94786729857819905213270142180095, 2.4);
		//return gammaPre * (gammaPre * (gammaPre * 0.305306011 + 0.682171111) + 0.012522878);
	
		//Cheap sRGB doesn't cause clipping
		return pow(gammaPre, 2.2);
	}

	float3 SRGBToLinear(float3 gammaPre)
	{
		gammaPre= max(0.0, gammaPre);
		float3 gammaPost = float3(
		SRGBToLinear(gammaPre.r),
		SRGBToLinear(gammaPre.g),
		SRGBToLinear(gammaPre.b));
		return gammaPost;
	}

	float LinearTosRGB(float gammaPre)
	{
		/*
		float Low = gammaPre * 12.92;
		float High = (pow(gammaPre, 1.0 / 2.4) * 1.055) - 0.055;
		return (gammaPre <= 0.0031308) ? Low : High;
		*/
		//return (gammaPre <= 0.00313080495356037151702786377709) ? gammaPre * 12.92 : (1.055 * pow(gammaPre, 0.41666666666666666666666666666667) - 0.055);
		//return max(1.055 * pow(gammaPre, 0.416666667) - 0.055, 0.0);
	
		//Cheap sRGB doesn't cause clipping
		return pow(gammaPre, 1/2.2);
	}

	float3 LinearTosRGB(float3 gammaPre)
	{
		gammaPre= max(0.0, gammaPre);
		float3 gammaPost = float3(
		LinearTosRGB(gammaPre.r),
		LinearTosRGB(gammaPre.g),
		LinearTosRGB(gammaPre.b));
		return gammaPost;
	}

	float square		( float value )
	{
		return	value*value;
	}
	
	float3 uv_to_eye(float2 uv, float eye_z)
	{
		uv = (uv * float2(2.0, 2.0) - float2(1.0, 1.0));
		return float3(uv * pos_decompression_params.xy * eye_z, eye_z);
	}

	float4 get_full_image(float2 texcoord)
	{
		return s_image.Sample(smp_rtlinear, texcoord);
	}

	float3 get_na_image(float2 texcoord)
	{
		return get_full_image(texcoord).xyz;
	}

	float to_float(float2 input)
	{
		return (input.x+input.y)/2;
	}

	float to_float(float3 input)
	{
		return (input.x+input.y+input.z)/3;
	}

	float invert_saturated(float input_value)
	{
		return float(1) - float(saturate(input_value));
	}

	float2 invert_saturated(float2 input_value)
	{
		return float(1).xx - float2(saturate(input_value));
	}

	float3 invert_saturated(float3 input_value)
	{
		return float(1).xxx - float3(saturate(input_value));
	}

	float4 invert_saturated(float4 input_value)
	{
		return float(1).xxxx - float4(saturate(input_value));
	}

	float3 get_na_gblurred_image(float2 texcoord, float dx, float dy)
	{
		float3 	img  = get_na_image(texcoord);
				img += get_na_image(texcoord +  float2(dx, 0));
				img += get_na_image(texcoord +  float2(dy, 0));
				img += get_na_image(texcoord +  float2(-dx, 0));
				img += get_na_image(texcoord +  float2(-dx, -dy));
				img += get_na_image(texcoord +  float2(dx, -dy));

		return 	img/6;
	}

	void filter_sky(inout float3 img, out float3 only_sky, in float depth)
	{
		// Объявим новый индификатор и присвоим ему входную картинку
		float3 not_sky = img;

		// Вытащил такой метод из SSDO в моде OGSE
		float depth_s = depth * (1 - step(0.001, abs(depth - 10000)));

		// Отржем небо от картинки:

		// Если это небо
		if(depth==depth_s)
		{
			not_sky = float3(0,0,0); // Картинка "без неба" равна 0 на небе
			only_sky = img; // Картинка "неба" равна картинке
		}

		// Если это НЕ небо
		else
		{
			not_sky = img; // Картинка "без неба" равна картинке
			only_sky = float3(0,0,0); // Картинка "неба" равна нулю
		}

		img = not_sky;
	}

	float2 calc_2d_distort_tc(Texture2D f_input, sampler you_sampler, float2 texcoord, float offset_factor)
	{
		float2 dudv_tex = f_input.Sample(you_sampler, texcoord).xy;

		float2 	offset			= (dudv_tex-.5h)*offset_factor;

		return float2(texcoord+offset);
	}

	float2 calc_tc_proj(float3 tcxyw)
	{
		return tcxyw.xy/tcxyw.z;
	}

	float normalize_depth(float depth)
	{
		return (saturate(depth/FARPLANE));
	}

	float GetRandom(float2 co)
	{
		return frac(sin(dot(co, float2(12.9898, 78.233))) * 43758.5453);
	}

	float3 GetRandomVector(float2 vTexCoord) 
	{
		return 2 * normalize(float3(GetRandom(vTexCoord - 0.5f),
				   GetRandom(vTexCoord + 0.5f),
				   GetRandom(vTexCoord))) - 1;
	}

	#ifndef SKY_WITH_DEPTH
	half is_sky(float depth)		{return step(depth, SKY_EPS);}
	half is_not_sky(float depth)	{return step(SKY_EPS, depth);}
	#else
	half is_sky(float depth)		{return step(abs(depth - SKY_DEPTH), SKY_EPS);}
	half is_not_sky(float depth)	{return step(SKY_EPS, abs(depth - SKY_DEPTH));}
	#endif


	float blend_soft_light(float a, float b) 
	{
		float3 c = 2 * a * b + a * a * (1 - 2 * b);
		float3 d = sqrt(a) * (2 * b - 1) + 2 * a * (1 - b);
		return (b < 0.5) ? c : d;
	}

	float3 blend_soft_light(float3 a, float3 b) 
	{
		float3 c = 2 * a * b + a * a * (1 - 2 * b);
		float3 d = sqrt(a) * (2 * b - 1) + 2 * a * (1 - b);
		return (b < 0.5) ? c : d;
	}


float4 proj2screen(float4 Project)
{
	float4	Screen;
			Screen.x = (Project.x + Project.w) * 0.5f;
			Screen.y = (Project.w - Project.y) * 0.5f;
			Screen.z = Project.z;
			Screen.w = Project.w;
			
	return Screen;
}

	//	contrast function
	float Contrast(float Input, float ContrastPower)
	{
		 //piecewise contrast function
		 bool IsAboveHalf = Input > 0.5 ;
		 float ToRaise = saturate(2*(IsAboveHalf ? 1-Input : Input));
		 float Output = 0.5*pow(ToRaise, ContrastPower); 
		 Output = IsAboveHalf ? 1-Output : Output;
		 return Output;
	}


#include "tonemap_srgb.h"
void tonemap( out float4 low, out float4 high, float3 rgb, float scale)
{
	rgb =    	rgb*scale;

	const float fWhiteIntensity = 11.2;

	low =   float4(tonemap_sRGB(rgb, fWhiteIntensity ), 0);
	high = 	half4(rgb/def_hdr, 0);
}

void        tonemap_sky              (out float4 low, out float4 high, float3 rgb, float scale)
{
	rgb		=	rgb*scale;
	low		=	( (rgb*(1+rgb/1.7689)) / (rgb+1) ).xyzz;
	high	=	rgb.xyzz/3;	// 8x dynamic range
}


float3 Vibrance( float3 i, half val )
{
	float luminance = dot( float3( i.rgb ), LUMINANCE_VECTOR );
	return float3( lerp( luminance, float3( i.rgb ), val ));
}

float3 compute_colored_ao(float ao, float3 albedo)
{ //https://www.activision.com/cdn/research/s2016_pbs_activision_occlusion.pptx
    float3 a = 2.0404 * albedo - 0.3324;
    float3 b = -4.7951 * albedo + 0.6417;
    float3 c = 2.7552 * albedo + 0.6903;

    return max(ao, ((ao * a + b) * ao + c) * ao);
}

float4 sat( float4 i, half val )
{
	float luminance = dot( float3( i.rgb ), LUMINANCE_VECTOR );
	return float4( lerp( luminance, float3( i.rgb ), val ), i.w );
}
float4 combine_bloom( float3  low, float4 high)	
{
        return float4(low + high * high.a, 1.f );
}


	// От 0 до 1
	float smooth_out_depth(in float depth, in float max_length, float contrast)
	{
		depth  = clamp(depth, 0, max_length);
		depth  = max_length - depth;
		depth /= max_length;
		depth *= contrast;

		return  saturate(depth);
	}

	float3 transform_n_to_d(float3 n)
	{
		return float(1).xxx - n;
	}

	float3 sky_fake_reflection(TextureCube skytex1, TextureCube skytex2, float3 vreflect)
	{
		float3	env0	= skytex1.SampleLevel(smp_base, vreflect.xyz, 0).xyz;
		float3	env1	= skytex2.SampleLevel(smp_base, vreflect.xyz, 0).xyz;

		return lerp(env0,env1,L_ambient.w);
	}

	float3 true_remapping(float3 vreflect)
	{
		float3 s_vreflect = vreflect / max(abs(vreflect.x), max(abs(vreflect.y), abs(vreflect.z)));

		if(s_vreflect.y < 0.99) {s_vreflect.y = s_vreflect.y * 2 - 1;}

		return s_vreflect;
	}

	float3 sky_true_reflection(TextureCube skytex1, TextureCube skytex2, float3 vreflect)
	{
		return sky_fake_reflection(skytex1, skytex2, true_remapping(vreflect));
	}

float calc_fogging( float4 w_pos )      
{
	return dot(w_pos,fog_plane);         
}

#ifdef SM_5_0
//Swartz27: I can't remember if all DX11 cards support doubles.
//if not revert this.
double2 unpack_tc_base( float2 tc, float du, float dv )
#else
float2 unpack_tc_base( float2 tc, float du, float dv )
#endif
{
	return (tc.xy + float2	(du,dv))*(32.f/32768.f); 
}

float3 calc_sun_r1( float3 norm_w )    
{
	return L_sun_color*saturate(dot((norm_w),-L_sun_dir_w));                 
}

float3 calc_model_hemi_r1( float3 norm_w )    
{
 return max(0,norm_w.y)*L_hemi_color;
}

float3 calc_model_lq_lighting( float3 norm_w )    
{
	return L_material.x*calc_model_hemi_r1(norm_w) + L_ambient + L_material.y*calc_sun_r1(norm_w);
}

float3 	unpack_normal( float3 v )	{ return 2*v-1; }
float3 	unpack_bx2( float3 v )	{ return 2*v-1; }
#ifdef SM_5_0
double3 	unpack_bx4( float3 v )	{ return 4*v-2; } //Swartz27: Add "precise" to front if using DirectXCompiler (the open source one)
#else
float3 	unpack_bx4( float3 v )	{ return 4*v-2; } 
#endif
float2 	unpack_tc_lmap( float2 tc )	{ return tc*(1.f/32768.f);	} // [-1  .. +1 ] 
float4	unpack_color( float4 c ) { return c.bgra; }
float4	unpack_D3DCOLOR( float4 c ) { return c.bgra; }
float3	unpack_D3DCOLOR( float3 c ) { return c.bgr; }

float3   p_hemi( float2 tc )
{
	float4	t_lmh = s_hemi.Sample( smp_rtlinear, tc);
	return	t_lmh.a;
}

float   get_hemi( float4 lmh)
{
	return lmh.a;
}

float   get_sun( float4 lmh)
{
	return lmh.g;
}

float3	v_hemi(float3 n)
{
	return L_hemi_color*(.5f + .5f*n.y);                   
}

float3	v_sun(float3 n)                        	
{
	return L_sun_color*dot(n,-L_sun_dir_w);                
}

float3	calc_reflection( float3 pos_w, float3 norm_w )
{
    return reflect(normalize(pos_w-eye_position), norm_w);
}

float4 proj_to_screen(float4 proj)
{
	float4 screen = proj;
	screen.x = (proj.x + proj.w);
	screen.y = (proj.w - proj.y);
	screen.xy *= 0.5;
	screen.z = proj.z;
	screen.w = proj.w;
	return screen;
}
float4 screen_to_proj(float2 screen, float z)
{
	float4 proj;
	proj.w = 1.0;
	proj.z = z;
	proj.x = screen.x*2 - proj.w;
	proj.y = -screen.y*2 + proj.w;
	return proj;
}
float is_in_range(float3 args)
{
	float mn = (args.x > args.y) ? 1: 0;
	float mx = (args.z > args.x) ? 1: 0;
	return mn*mx;
}

float hash(float2 intro)
{
return frac(1.0e4 * sin(17.0*intro.x + 0.1*intro.y) * (0.1 + abs(sin(13.0*intro.y + intro.x))));
}

float hash3D(float3 intro)
{
return hash(float2(hash(intro.xy),intro.z));
}

float hash12(float2 p)
{
	float3 p3  = frac(float3(p.xyx) * .1031);
    p3 += dot(p3, p3.yzx + 19.19);
    return frac((p3.x + p3.y) * p3.z);
}

float2 hash22(float2 p)
{
	float3 p3 = frac(float3(p.xyx) * float3(.1031, .1030, .0973));
    p3 += dot(p3, p3.yzx+19.19);
    return frac((p3.xx+p3.yz)*p3.zy);
}

float2 GetRandom2_10(in float2 uv) 
{
	float noiseX = (frac(sin(dot(uv, float2(12.9898,78.233) * 2.0)) * 43758.5453));
	float noiseY = sqrt(1 - noiseX * noiseX);
	return float2(noiseX, noiseY);
}

float2 scale_texcoord(float2 uv, float zoom)
{
	float2 add = float2(1-(1/zoom),1-(1/zoom));
	return (add / 2) + (uv / zoom);
}

inline float sunpos_screen()
{
    return dot(-eye_direction, normalize(L_sun_dir_w));
}

#define USABLE_BIT_1                uint(0x00002000)
#define USABLE_BIT_2                uint(0x00004000)
#define USABLE_BIT_3                uint(0x00008000)
#define USABLE_BIT_4                uint(0x00010000)
#define USABLE_BIT_5                uint(0x00020000)
#define USABLE_BIT_6                uint(0x00040000)
#define USABLE_BIT_7                uint(0x00080000)
#define USABLE_BIT_8                uint(0x00100000)
#define USABLE_BIT_9                uint(0x00200000)
#define USABLE_BIT_10               uint(0x00400000)
#define USABLE_BIT_11               uint(0x00800000)   // At least two of those four bit flags must be mutually exclusive (i.e. all 4 bits must not be set together)
#define USABLE_BIT_12               uint(0x01000000)   // This is because setting 0x47800000 sets all 5 FP16 exponent bits to 1 which means infinity
#define USABLE_BIT_13               uint(0x02000000)   // This will be translated to a +/-MAX_FLOAT in the FP16 render target (0xFBFF/0x7BFF), overwriting the 
#define USABLE_BIT_14               uint(0x04000000)   // mantissa bits where other bit flags are stored.
#define USABLE_BIT_15               uint(0x80000000)
#define MUST_BE_SET                 uint(0x40000000)   // This flag *must* be stored in the floating-point representation of the bit flag to store


// Holger Gruen AMD - I change normal packing and unpacking to make sure N.z is accessible without ALU cost
// this help the HDAO compute shader to run more efficiently
float2 gbuf_pack_normal( float3 norm )
{
    float2 res;
    res.x = atan2(norm.y, norm.x) / 3.14159f;
    res.y = norm.z;
    return res;
}

float3 gbuf_unpack_normal( float2 norm )
{
    float2 theta;
    sincos(norm.x * 3.14159f, theta.x, theta.y);
    float2 phi = float2(sqrt(1.0 - norm.y * norm.y), norm.y);
    return float3(theta.y * phi.x, theta.x * phi.x, phi.y);
}

float gbuf_pack_hemi_mtl( float hemi, float mtl )
{
   uint packed_mtl = uint( ( mtl / 1.333333333 ) * 31.0 );
//   uint packed = ( MUST_BE_SET + ( uint( hemi * 255.0 ) << 13 ) + ( ( packed_mtl & uint( 31 ) ) << 21 ) );
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
//   return float( ( asuint( mtl_hemi ) >> 13 ) & uint(255) ) * (1.0/255.0);
	return float( ( asuint( mtl_hemi ) >> 13 ) & uint(255) ) * (1.0/254.8);
}

float gbuf_unpack_mtl( float mtl_hemi )
{
   uint packed       = asuint( mtl_hemi );
   uint packed_hemi  = ( ( packed >> 21 ) & uint(15) ) + ( ( packed & USABLE_BIT_15 ) == 0 ? 0 : 16 );
   return float( packed_hemi ) * (1.0/31.0) * 1.333333333;
}

#ifndef EXTEND_F_DEFFER
f_deffer pack_gbuffer( float4 norm, float4 pos, float4 col )
#else
f_deffer pack_gbuffer( float4 norm, float4 pos, float4 col, uint imask )
#endif
{
	f_deffer res;

#ifndef GBUFFER_OPTIMIZATION
	res.position	= pos;
	res.Ne			= norm;
	res.C			   = col;
#else
	res.position	= float4( gbuf_pack_normal( norm ), pos.z, gbuf_pack_hemi_mtl( norm.w, pos.w ) );
	res.C			   = col;
#endif

#ifdef EXTEND_F_DEFFER
	res.mask = imask;
#endif

	return res;
}

#ifdef GBUFFER_OPTIMIZATION
gbuffer_data gbuffer_load_data( float2 tc : TEXCOORD, float2 pos2d, uint iSample )
{
	gbuffer_data gbd;

	gbd.P = float3(0,0,0);
	gbd.hemi = 0;
	gbd.mtl = 0;
	gbd.C = 0;
	gbd.N = float3(0,0,0);

#ifndef USE_MSAA
	float4 P = s_position.Sample( smp_nofilter, tc );
	float4 C = s_diffuse.Sample( smp_nofilter, tc );	
#else
	float4 P = s_position.Load( int3( pos2d, 0 ), iSample );
	float4 C = s_diffuse.Load( int3( pos2d, 0 ), iSample );
#endif

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

	gbd.C		= C.xyz;
	gbd.gloss	= C.w;

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

#else // GBUFFER_OPTIMIZATION
gbuffer_data gbuffer_load_data( float2 tc : TEXCOORD, int iSample )
{
	gbuffer_data gbd;

#ifndef USE_MSAA
	float4 P	= s_position.Sample( smp_nofilter, tc );
	float4 N	= s_normal.Sample( smp_nofilter, tc );
	float4	C	= s_diffuse.Sample(  smp_nofilter, tc );
#else
   float4 P	= s_position.Load( int3( tc * pos_decompression_params2.xy, 0 ), iSample );
   float4 N	= s_normal.Load( int3( tc * pos_decompression_params2.xy, 0 ), iSample );
   float4	C	= s_diffuse.Load( int3( tc * pos_decompression_params2.xy, 0 ), iSample );
#endif

	gbd.P		= P.xyz;
	gbd.mtl		= P.w;

	gbd.N		= N.xyz;
	gbd.hemi	= N.w;

	gbd.C		= C.xyz;
	gbd.gloss	= C.w;

	return gbd;
}

gbuffer_data gbuffer_load_data( float2 tc : TEXCOORD  )
{
   return gbuffer_load_data( tc, 0 );
}

gbuffer_data gbuffer_load_data_offset( float2 tc : TEXCOORD, float2 OffsetTC : TEXCOORD, uint iSample )
{
   return gbuffer_load_data( OffsetTC, iSample );
}

#endif // GBUFFER_OPTIMIZATION

//////////////////////////////////////////////////////////////////////////
//	Aplha to coverage code
#if ( defined( MSAA_ALPHATEST_DX10_1_ATOC ) || defined( MSAA_ALPHATEST_DX10_1 ) )

#if defined(MSAA_SAMPLES)

uint calc_atoc_level(float base, uint2 pos2d)
{
	float tc = pos2d.xy * pos_decompression_params.zw;
	float2 dx = ddx(tc);
	float2 dy = ddy(tc);
	float delta_max_sqr = max(dot(dx, dx), dot(dy, dy));
	float mip_new = max(0.0, 0.5 * log2(delta_max_sqr));
	float alpha = base + base * max(0, mip_new) * 0.25;
	float absdfull = abs(ddx(alpha)) + abs(ddy(alpha));
	alpha = (alpha - def_aref) / max(absdfull, 0.0001) + 0.5;
	alpha = saturate(alpha);
	uint levels = uint(alpha * MSAA_SAMPLES);
	uint mask = 0;
	for(uint i = 0; i < levels && i < MSAA_SAMPLES; i++) mask |= 1 << i;
	return mask;
}

#endif
#endif





#endif	//	common_functions_h_included
