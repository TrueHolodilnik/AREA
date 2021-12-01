#ifndef	LMODEL_H
#define LMODEL_H

#define ALBEDO_AMOUNT 1.00

#define ROUGHNESS_LOW 0.3
#define ROUGHNESS_HIGH 0.9

#define SPECULAR_RANGE 1
#define SPECULAR_POW 1

#include "common.h"

#define SSSS_HLSL_4 1
#include "SeparableSSS.h"

static const float	pi								= 3.14159267;
static const float	specular_normalization_factor	= 1.f/(2.f*pi);


//////////////////////////////////////////////////////////////////////////////////////////
// Lighting formulas			// 
float G1V(float dotNV, float k)
{
	return 1.0f/(dotNV*(1.0f-k)+k);
}

float GGX(float3 dotNL, float3 dotNH, float3 dotNV, float3 dotLH, float3 F0, float Roughness)
{
	
	float alpha = Roughness*Roughness;

	float F, D, vis;

	// D
	float alphaSqr = alpha*alpha;
	float pi = 3.14f;
	float denom = dotNH * dotNH *(alphaSqr-1.0) + 1.0f;
	D = alphaSqr/(pi * denom * denom);

	// F
	float dotLH5 = pow(1.0f-dotLH,5);
	F = F0 + (1.0-F0)*(dotLH5);

	// V
	float k = alpha/2.0f;
	vis = G1V(dotNL,k)*G1V(dotNV,k);

	float specular = dotNL * D * F * vis;
	return specular;
}

float G_Smith(float m2, float nDotX)
{
	//m2 = m2*0.5; //remapping for GGX
	m2 = m2 * 0.797884560802865; //remapping for beckmann/blinn (sqrt(2/pi))
	return nDotX / (nDotX * (1 - m2) + m2);
}

float3 Blinn(float nDotL, float nDotH, float nDotV, float lDotH, float3 f0, float rough)
{
	float gloss = 1-rough;
	gloss = gloss*gloss;
	gloss = exp2(gloss*10);
	
	float d = pow(nDotH, gloss);
	d *= (gloss+8)/(8); //blinn normalized without pi, (n+6)/8 might be better?
	
	float m2 = rough * rough;
	float v1i = G_Smith(m2, nDotL);
	float v1o = G_Smith(m2, nDotV);
	float vis = v1i * v1o;
	
	//float vis = ceil(nDotL); //don't bother with vis
	
	float3 f = lerp(f0, 1.0, pow(1-lDotH, 5));

	return d * f * vis;
}

float SSS(float3 N, float3 V, float3 L)
{
	const float SSS_DIST = 0.125; //Scattering distortion
	const float SSS_POW = 4; //Scattering power
	const float SSS_SCALE = 1; //Scattering scale
	const float SSS_AMB = 0.5; //Scattering ambient
		
    float3 SSS_vector = L + N * SSS_DIST;
    float SSS_light = pow(saturate(dot(V, -SSS_vector)),SSS_POW); //DICE translucency
    SSS_light  *= (SSS_POW+8)/(8); //blinn normalize
	
    SSS_light  = SSS_light * SSS_SCALE;
    SSS_light  += saturate(dot(N,-L)) * SSS_AMB; //only apply ambient to shaded areas
    return SSS_light;
}

// Lighting formulas			// 
float4 plight_infinity( float m, float3 pnt, float3 normal, float4 c_tex, float3 light_direction, float2 tc, float4 tangent, uint iSample = 0)
{
	float mat = m;
  	float3 N			= normal;							// normal 
  	float3 V = -normalize(pnt);					// vector2eye
  	float3 L = -light_direction;						// vector2light
  	float3 H = normalize(L+V);						// float-angle-vector 
	
	float4 smat = s_material.SampleLevel( smp_material, float3( saturate(dot(L,N)), saturate(dot(H,N)), m ), 0 );

	float3 albedo = c_tex.xyz;
	float gloss = c_tex.w;

	//float3 F0 = (smat.y*0.7 + smat.w*0.3)*(1 - m);
	float foliage = saturate(20*(abs(m - 0.47)-0.05)); //foliage
	//gloss *= foliage;
	m *= foliage;

	float3 F0 = (smat.y*0.7 + smat.w*0.3); //base fresnel to tweak
	F0 *= SPECULAR_RANGE * (m*0.25 + gloss*4); //0.0 - 2.0 fresnel multiplier
	F0 = pow(F0, SPECULAR_POW);
	F0 = saturate(F0);
	
	float roughness = lerp(ROUGHNESS_HIGH, ROUGHNESS_LOW, gloss); //gloss to roughness
		  roughness = pow(roughness, 1/(1.01-Ldynamic_color.w)) * (1.0 - m * 0.7) + (1 - m*0.25 + gloss*4);
	//Lambert diffuse
	
	float nDotL = max(0, dot(N, L));
	float nDotH = max(0, dot(N, H));
	float nDotV = max(0, dot(N, V));
	float lDotH = max(0, dot(L, H));
	
	//float3 diffuse_term = pow(nDotL, lerp(1.0, 0.75, roughness) * 1.5) * albedo;
	//diffuse_term *= albedo;
	
	////Phong infinity
	//float3 viewer 		= -V;
	//// Compute the reflection vector
	//float3 reflection   = reflect( L, N );
	//// Compute the angle between the reflection and the viewer
	//float  RdotV        = max( -dot( reflection, viewer ), 0.0f );
	//// Compute the specular color
	////float specular		= RdotV/( RdotV + (20*m) * ( 1 - RdotV ));
	//float specular_term		= pow( RdotV, 0.5 );
	//
	//float pure_diffuse	= saturate( nDotL );
	//float diffuse_powered = pure_diffuse/( pure_diffuse + ( 1 - pure_diffuse ));
	////float diffuse_powered = pure_diffuse;
	//// Compute the diffuse term for the Phong equation
	//float diffuse_term      = diffuse_powered + saturate( 0.4 - nDotL );
	
	//Modified Phong BRDF
	//float specular_power = 1 + (m*0.25 + gloss*4) * 2;
	//float diffuse_term				= max(0, dot(-L, N));
	//float3 reflected_light_direction	= reflect( L, N );
	//float specular_term				= 
	//	pow(
	//		max( dot( reflected_light_direction, V ), 0.f ),
	//		clamp( specular_power * (1.f - dot( L, N )) / 2.f, 0.001f, 255.0f) 
	//	) * (specular_power + 2.f) * 1.f/(2.f*3.14159267);
	
	//Hoffman
	//float light_intensity_on_surface	= max( 0.f, dot( L, N ) );
	//float diffuse_term      	= light_intensity_on_surface;
	//float view_dot_halfway		= dot( V, halfway_direction );
	//float fresnel_factor		= F0 + (1.f - F0)*pow( 1.f - view_dot_halfway, 5.f );
	//float specular_level		= 100.f;
	//float specular_term			= ((specular_level + 2.f) / 8) * pow( nDotH, specular_level) * fresnel_factor * light_intensity_on_surface;
	
	////Blinn-Phong
	//float diffuse_term 				= nDotL;
	//float specular_power = 1 + (m*0.25 + gloss*4) * 2;
	//float specular_term = pow(nDotH, clamp(specular_power, 0.1f, 255.0f)) * (specular_power + 2.f) * 1.f/(2.f*3.14159267);


	float alpha_model_parameter					 	= 1;
	
	float2 isotropic_roughness_surface_factor		= max(float2(m*0.25 + gloss*4, m*0.25 + gloss*4), (0.01f).xx);
	float anisotropic_roughness_surface_factor_x	= isotropic_roughness_surface_factor.x;
	float anisotropic_roughness_surface_factor_y	= isotropic_roughness_surface_factor.y;
	
	// we use Schlicks approximation formula
	// where f0 - is cumulative zero degree material fresnel factor
	float3 halfway_direction	= H;
	float view_dot_halfway		= dot(V, halfway_direction );
	float view_dot_halfway_plus	= max( view_dot_halfway, 0.f );
	float fresnel_factor		= F0 + (1.f - F0)*pow( 1.f - view_dot_halfway_plus, 5.f );

	float light_dot_normal		= dot(N, L);
	float light_dot_normal_plus	= pow( max( light_dot_normal, 0.f ), 1.5);
	
	// Half Lambert
	//result.diffuse			= (light_dot_normal/2 + 0.5) * max(1.f - fresnel_factor, 0);
	float diffuse_term 				= light_dot_normal_plus* ( 1.f - fresnel_factor );;// * ( 1.f - fresnel_factor );
	
	// compute normalized distribution functions
	// Beckmann distribution function in case of isotropic materials
	float cosine_theta			= dot( N, halfway_direction );
	float square_cosine_theta	= square( cosine_theta );
	float square_sine_theta		= 1.f - square_cosine_theta;
	float square_tangent_theta	= square_sine_theta/square_cosine_theta;
	
	float3 surface_tangent		= tangent.xyz * 2.0f - 1.0f; //t_tangents.Sample(s_accumulator, texture_coordinates).xyz * 2.0f - 1.0f;
	float3 half_projection		= normalize(halfway_direction - N * dot(N, halfway_direction));
	float square_cosine_phi		= saturate(square( dot(half_projection, surface_tangent) ));
	float square_sine_phi		= 1.0f - square_cosine_phi;
	
	float distribution_function	=
		 exp(
			 -square_tangent_theta
			 *(
				 square_cosine_phi / square( anisotropic_roughness_surface_factor_x )
				 +
				 square_sine_phi / square( anisotropic_roughness_surface_factor_y )
			 )
		 );
	//float distribution_function	= exp( -square_tangent_theta / square( isotropic_roughness_surface_factor ) );
	
	float normalized_distribution_function	=
		distribution_function /
		(
			pi * anisotropic_roughness_surface_factor_x * anisotropic_roughness_surface_factor_y * square(square_cosine_theta)
		);
	
	float specular					=
		light_dot_normal > 0.f
		?
			light_dot_normal_plus * fresnel_factor * normalized_distribution_function /
			(
				4.f * view_dot_halfway_plus *
				pow(
					abs(
						light_dot_normal *
						dot(V, N)
					),
					alpha_model_parameter
				)
			)
		:
			0.f;
	float specular_term				= clamp(specular, 0.0f, 4.0f);
	
	
	//float specular_term = 0;
	//if ((mat + 0.25) / 4.f >= 0.22) specular_term = GGX(nDotL, nDotH, nDotV, lDotH, F0, roughness);
	//else specular_term = Blinn(nDotL, nDotH, nDotV, lDotH, F0, roughness);
	
	float4 light = float4(diffuse_term*albedo + specular_term + smat.x, specular_term);
	
	//float4 subsurface = SSSSBlurPS(tc, 1.0, float2(1.0, 0.0), false, iSample);
	light.rgb += SSS(N, V, L);
	
	return light;		// output (albedo.gloss)
}

float4 plight_local( float m, float3 pnt, float3 normal, float4 c_tex, float3 light_position, float light_range_rsq, float2 tc, out float rsqr, float4 tangent, uint iSample = 0)
{
	float mat = m;
  	float3 N		= normal;							// normal 
	float3 L2P 	= pnt - light_position;                         		// light2point 
	float3 V 		= -normalize	(pnt);					// vector2eye
	float3 L 		= -normalize	((float3)L2P);					// vector2light
	float3 H		= normalize	(L+V);						// half-angle-vector
		rsqr	= dot		(L2P,L2P);					// distance 2 light (squared)
	float  att 	= SRGBToLinear(saturate(1 - rsqr*light_range_rsq)); //light_range_rsq			// q-linear attenuate
	
	float4 smat = s_material.SampleLevel( smp_material, float3( saturate(dot(L,N)), saturate(dot(H,N)), m ), 0 );

	float3 albedo = c_tex.xyz;
	float gloss = c_tex.w;

	//float3 F0 = (smat.y*0.7 + smat.w*0.3)*(1 - m);
	float foliage = saturate(20*(abs(m - 0.47)-0.05)); //foliage
	//gloss *= foliage;
	m *= foliage;

	float3 F0 = (smat.y*0.7 + smat.w*0.3); //base fresnel to tweak
	F0 *= SPECULAR_RANGE * (m*0.25 + gloss*4); //0.0 - 2.0 fresnel multiplier
	F0 = pow(F0, SPECULAR_POW);
	F0 = saturate(F0);
	
	//float roughness = lerp(ROUGHNESS_HIGH, ROUGHNESS_LOW, gloss); //gloss to roughness
	//	  roughness = pow(roughness, 1/(1.01-Ldynamic_color.w)) * (1.0 - m * 0.7) + (1 - m*0.25 + gloss*4);
	//Lambert diffuse
	
	float nDotL = max(0, dot(N, L));
	float nDotH = max(0, dot(N, H));
	float nDotV = max(0, dot(N, V));
	float lDotH = max(0, dot(L, H));
	
	//float3 diffuse_term = pow(nDotL, lerp(1.0, 0.75, roughness) * 1.5) * albedo;
	//diffuse_term *= albedo;
	
	////Phong infinity
	//float3 viewer 		= -V;
	//// Compute the reflection vector
	//float3 reflection   = reflect( L, N );
	//// Compute the angle between the reflection and the viewer
	//float  RdotV        = max( -dot( reflection, viewer ), 0.0f );
	//// Compute the specular color
	////float specular		= RdotV/( RdotV + (20*m) * ( 1 - RdotV ));
	//float specular_term		= pow( RdotV, 0.5 );
	//
	//float pure_diffuse	= saturate( nDotL );
	//float diffuse_powered = pure_diffuse/( pure_diffuse + ( 1 - pure_diffuse ));
	////float diffuse_powered = pure_diffuse;
	//// Compute the diffuse term for the Phong equation
	//float diffuse_term      = diffuse_powered + saturate( 0.4 - nDotL );
	
	//Modified Phong BRDF
	//float specular_power = 1 + (m*0.25 + gloss*4) * 2;
	//float diffuse_term				= max(0, dot(-L, N));
	//float3 reflected_light_direction	= reflect( L, N );
	//float specular_term				= 
	//	pow(
	//		max( dot( reflected_light_direction, V ), 0.f ),
	//		clamp( specular_power * (1.f - dot( L, N )) / 2.f, 0.001f, 255.0f) 
	//	) * (specular_power + 2.f) * 1.f/(2.f*3.14159267);
	
	//Hoffman
	//float light_intensity_on_surface	= max( 0.f, dot( L, N ) );
	//float diffuse_term      	= light_intensity_on_surface;
	//float view_dot_halfway		= dot( V, halfway_direction );
	//float fresnel_factor		= F0 + (1.f - F0)*pow( 1.f - view_dot_halfway, 5.f );
	//float specular_level		= 100.f;
	//float specular_term			= ((specular_level + 2.f) / 8) * pow( nDotH, specular_level) * fresnel_factor * light_intensity_on_surface;
	
	////Blinn-Phong
	//float diffuse_term 				= nDotL;
	//float specular_power = 1 + (m*0.25 + gloss*4) * 2;
	//float specular_term = pow(nDotH, clamp(specular_power, 0.1f, 255.0f)) * (specular_power + 2.f) * 1.f/(2.f*3.14159267);


	float alpha_model_parameter					 	= 1;
	
	float2 isotropic_roughness_surface_factor		= max(float2(m*0.25 + gloss*4,m*0.25 + gloss*4), (0.01f).xx);
	float anisotropic_roughness_surface_factor_x	= isotropic_roughness_surface_factor.x;
	float anisotropic_roughness_surface_factor_y	= isotropic_roughness_surface_factor.y;
	
	// we use Schlicks approximation formula
	// where f0 - is cumulative zero degree material fresnel factor
	float3 halfway_direction	= H;
	float view_dot_halfway		= dot(V, halfway_direction );
	float view_dot_halfway_plus	= max( view_dot_halfway, 0.f );
	float fresnel_factor		= F0 + (1.f - F0)*pow( 1.f - view_dot_halfway_plus, 5.f );

	float light_dot_normal		= dot(N, L);
	float light_dot_normal_plus	= pow( max( light_dot_normal, 0.f ), 1.5);
	
	// Half Lambert
	//result.diffuse			= (light_dot_normal/2 + 0.5) * max(1.f - fresnel_factor, 0);
	float diffuse_term 				= light_dot_normal_plus* ( 1.f - fresnel_factor );;// * ( 1.f - fresnel_factor );
	
	// compute normalized distribution functions
	// Beckmann distribution function in case of isotropic materials
	float cosine_theta			= dot( N, halfway_direction );
	float square_cosine_theta	= square( cosine_theta );
	float square_sine_theta		= 1.f - square_cosine_theta;
	float square_tangent_theta	= square_sine_theta/square_cosine_theta;
	
	float3 surface_tangent		= tangent.xyz * 2.0f - 1.0f; //t_tangents.Sample(s_accumulator, texture_coordinates).xyz * 2.0f - 1.0f;
	float3 half_projection		= normalize(halfway_direction - N * dot(N, halfway_direction));
	float square_cosine_phi		= saturate(square( dot(half_projection, surface_tangent) ));
	float square_sine_phi		= 1.0f - square_cosine_phi;
	
	float distribution_function	=
		 exp(
			 -square_tangent_theta
			 *(
				 square_cosine_phi / square( anisotropic_roughness_surface_factor_x )
				 +
				 square_sine_phi / square( anisotropic_roughness_surface_factor_y )
			 )
		 );
	//float distribution_function	= exp( -square_tangent_theta / square( isotropic_roughness_surface_factor ) );
	
	float normalized_distribution_function	=
		distribution_function /
		(
			pi * anisotropic_roughness_surface_factor_x * anisotropic_roughness_surface_factor_y * square(square_cosine_theta)
		);
	
	float specular					=
		light_dot_normal > 0.f
		?
			light_dot_normal_plus * fresnel_factor * normalized_distribution_function /
			(
				4.f * view_dot_halfway_plus *
				pow(
					abs(
						light_dot_normal *
						dot(V, N)
					),
					alpha_model_parameter
				)
			)
		:
			0.f;
	float specular_term				= clamp(specular, 0.0f, 4.0f);
	
	
	//float specular_term = 0;
	//if ((mat + 0.25) / 4.f >= 0.22) specular_term = GGX(nDotL, nDotH, nDotV, lDotH, F0, roughness);
	//else specular_term = Blinn(nDotL, nDotH, nDotV, lDotH, F0, roughness);
	
	float4 light = float4(diffuse_term*albedo + specular_term + smat.x, specular_term);
	
	//float4 subsurface = SSSSBlurPS(tc, 1.0, float2(1.0, 0.0), false, iSample);
	light.rgb += SSS(N, V, L);
	
	return att*light;		// output (albedo.gloss)
}
#endif