#ifndef        HMODEL_H
#define HMODEL_H

#include "common.h"
#include "lmodel.h"

//uniform samplerCUBE         env_s0                ;
//uniform samplerCUBE         env_s1                ;
//uniform samplerCUBE         sky_s0                ;
//uniform samplerCUBE         sky_s1                ;

TextureCube		env_s0;
TextureCube		env_s1;
TextureCube		sky_s0;
TextureCube		sky_s1;

uniform float4	env_color;        // color.w  = lerp factor
uniform float3x4	m_v2w;

#define CUBE_MIPS 5 //mipmaps for ambient shading and specular
static const float Pi = 3.14159265359;

float3 CubeDiffuse (float rough, TextureCube cubeMap, float3 nw)
{
	float3 nwRemap = nw;
	float3 vnormabs    = abs(nwRemap);
	float  vnormmax    = max(vnormabs.x, max(vnormabs.y, vnormabs.z));
        nwRemap      /= vnormmax;

        if (nwRemap.y < 0.999)    
        nwRemap.y= nwRemap.y*2-1;     // fake remapping 

	nwRemap = normalize(nwRemap);
	
	float3 nSquared = nw * nw;
	const float Epsilon = 0.000001;
	float3 DiffuseColor = 0;
	DiffuseColor += nSquared.x * cubeMap.SampleLevel(smp_rtlinear, float3(nwRemap.x, Epsilon, Epsilon), CUBE_MIPS);
	DiffuseColor += nSquared.y * cubeMap.SampleLevel(smp_rtlinear, float3(Epsilon, nwRemap.y, Epsilon), CUBE_MIPS); 
	DiffuseColor += nSquared.z * cubeMap.SampleLevel(smp_rtlinear, float3(Epsilon, Epsilon, nwRemap.z), CUBE_MIPS);
	
	return DiffuseColor;
}

float3 CubeSpecular (float rough, TextureCube cubeMap, float3 vreflect)
{
	float RoughMip = rough * CUBE_MIPS;
	float3 SpecularColor = cubeMap.SampleLevel(smp_rtlinear, vreflect, RoughMip); 

	return SpecularColor;
}

float3 EnvBRDFApprox(float3 SpecularColor, float Roughness, float NoV )
{
	const float4 c0 = { -1, -0.0275, -0.572, 0.022 };
	const float4 c1 = { 1, 0.0425, 1.04, -0.04 };
	float4 r = Roughness * c0 + c1;
	float a004 = min(r.x * r.x, exp2(-9.28 * NoV ) ) * r.x + r.y;
	float2 AB = float2(-1.04, 1.04 ) * a004 + r.zw;
	return SpecularColor * AB.x + AB.y;
}

void hmodel
(
	out float3 hdiffuse, out float3 hspecular, 
	float m, float h, float4 alb_gloss, float3 Pnt, float3 normal
)
{
	normal = normalize(normal);	
	float3 albedo = alb_gloss.rgb;
	float foliage = saturate(20*(abs(m-0.47)-0.05)); //foliage
	//gloss *= foliage;
	float material_ID = m * foliage;
	float3 specular = SPECULAR_BASE; //base fresnel to tweak
	specular *= SPECULAR_RANGE * max(0,((0.5+material_ID) * (0.5+alb_gloss.a)) - 0.25); //0.0 - 2.0 fresnel multiplier
	specular = pow(specular, SPECULAR_POW);
	float3 f0 = saturate(specular);
	
	float rough = lerp(ROUGHNESS_HIGH, ROUGHNESS_LOW, pow(alb_gloss.a, ROUGHNESS_POW)); //gloss to roughness
	rough = pow(rough, 1/(1.01-Ldynamic_color.w)); //gloss factor
	
	float roughCube = pow(rough, 0.5); //cubemap mipmaps (brdf too?)
// hscale - something like diffuse reflection
	float3	nw		= mul( m_v2w, normal );
	float	hscale	= h;	//. *        (.5h + .5h*nw.y);

#ifdef         USE_GAMMA_22
			hscale	= (hscale*hscale);        // make it more linear
#endif

	// reflection vector
	float3	v2PntL	= normalize( Pnt );
	float3	v2Pnt	= mul( m_v2w, v2PntL );
	v2Pnt = normalize(v2Pnt);
	nw = normalize(nw);
	float3	vreflect= reflect( v2Pnt, nw );
	vreflect = lerp(nw, vreflect, (1-rough) * (sqrt(1-rough) + rough)); //dice normal blend

	float3 vreflectabs = abs(vreflect);
	float vreflectmax = max(vreflectabs.x, max(vreflectabs.y, vreflectabs.z));
	vreflect	  /= vreflectmax;
	if (vreflect.y < 0.999) 
	vreflect.y = vreflect.y*2-1;	//fake remapping 
	vreflect = normalize(vreflect);

	float3 nwRemap = nw;
	
	float	hspec	= .5h + .5h * dot( vreflect, v2Pnt );
	float4	light	= s_material.SampleLevel( smp_material, float3( hscale, hspec, m ), 0 ).xxxy;

	float3 e0d = CubeDiffuse(roughCube, env_s0, nwRemap.xyz ); 		//Valve AmbientCube method
	float3 e1d = CubeDiffuse(roughCube, env_s1, nwRemap.xyz ); 		//could probably be optimized to not sample the cubemap so much if mips worked
	float3	e0s = CubeSpecular(roughCube, env_s0, vreflect.xyz ); //Sample mipmapped cubemap
	float3	e1s = CubeSpecular(roughCube, env_s1, vreflect.xyz);  
	float3 env_col = env_color.rgb; 
	float3 amb_col = L_ambient.rgb;		
	float3 env_d = lerp(e0d, e1d, env_color.w);
	float3 env_s = lerp(e0s, e1s, env_color.w);
	env_d *= env_col; 		 
	env_s *= env_col; 
	env_d *= env_col; 		 
	env_s *= env_col; 

	//squaring? disabled for PBR cubemaps, it makes the colors inaccurate
	env_d *= env_d;
	env_s *= env_s;//+(rain_params.x/10.0);

	hscale	= s_material.SampleLevel( smp_material, float3( hscale, 0, m ), 0 ).x;
	env_d *= hscale; 		
	env_s *= hscale; 
	env_d += amb_col; 		
	// specular color
	
	env_d = SRGBToLinear(env_d); 		
	env_s = SRGBToLinear(env_s); 

	float nDotV = max(0, dot(-v2Pnt, nw));
	
	float3 diffuse_term = env_d * albedo; 
	
	float3 specular_term = env_s * EnvBRDFApprox(f0, roughCube, nDotV );
	
	hdiffuse = diffuse_term + specular_term;

	float luminance = 1-dot( light.rgb, float3(LUMINANCE_VECTOR) );

	hspecular	= light.w*luminance*env_color.w*env_s*2*(10 - 9*pow(m, 3));
}

#endif