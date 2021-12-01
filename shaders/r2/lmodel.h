#ifndef	LMODEL_H
#define LMODEL_H

#define ECB_LL_DIST 1.2f                            // дальность освещени€ ламп и костров
#define ECB_LL_BRIGHTNESS 1.5f                      // €ркость освещени€ ламп и костров

#include "common.h"

#define LIGHT_MUL1 0.7//.7f
#define LIGHT_MUL2 0.0//.5f

//////////////////////////////////////////////////////////////////////////////////////////
// Lighting formulas			// 
half4 	plight_infinity		(half m, half3 point, half3 normal, half3 light_direction)       				{
  half3 N		= normal;							// normal 
  half3 V 		= -normalize	(point);					// vector2eye
  half3 L 		= -light_direction;						// vector2light
  half3 H		= normalize	(L+V);						// half-angle-vector 
  half4 ret;
  half4 light = tex3D 		(s_material,	half3( dot(L,N), dot(H,N), m ) );		// sample material
  ret = (light*LIGHT_MUL1+half4(light.www*(Ldynamic_color.xyz*2),light.w)*Ldynamic_color.w/2*LIGHT_MUL2);

  ret.rgb = Vibrance(ret.rgb,2);
  
  return ret;
}
half4 	plight_infinity2	(half m, half3 point, half3 normal, half3 light_direction)       				{
  	half3 N		= normal;							// normal 
  	half3 V 	= -normalize		(point);		// vector2eye
  	half3 L 	= -light_direction;					// vector2light
 	half3 H		= normalize			(L+V);			// half-angle-vector 
	half3 R     = reflect         	(-V,N);
	half 	s	= saturate(dot(L,R));
			s	= saturate(dot(H,N));
	half 	f 	= saturate(dot(-V,R));
			s  *= f;
	half4	r	= tex3D 			(s_material,	half3( dot(L,N), s, m ) );	// sample material
			r.w	= pow(saturate(s),4);
  	return	r	;
}
half4 	plight_local		(half m, half3 point, half3 normal, half3 light_position, half light_range_rsq, out float rsqr)  {
  half3 N		= normal;							// normal 
  half3 L2P 	= point-light_position;                         		// light2point 
  half3 V 		= -normalize	(point);					// vector2eye
  half3 L 		= -normalize	((half3)L2P);					// vector2light
  half3 H		= normalize	(L+V);						// half-angle-vector
		rsqr	= dot		(L2P,L2P);					// distance 2 light (squared)
  half  att 	= saturate	(1 - rsqr*light_range_rsq*L_RANGE)*L_BRIGHT;			// q-linear attenuate
  half4 light	= tex3D		(s_material, half3( dot(L,N), dot(H,N), m ) ); 	// sample material
  half4 ret = (att*light*LIGHT_MUL1+att*float4(light.www*(Ldynamic_color.xyz*2),light.w)*Ldynamic_color.w/2*LIGHT_MUL2);
  
  
  ret.rgb = Vibrance(ret.rgb,2);
  
  return ret;
}
float4 	plight_local_torch		(float m, float3 _point, float3 normal, float3 light_position, float light_range_rsq, float angle_cos)  {
  float3 N		= normal;							// normal 
  float3 L2P 	= _point-light_position;                         		// light2point 
  float3 V 		= -normalize	(_point);					// vector2eye
  float3 L 		= -normalize	((float3)L2P);					// vector2light
  float3 H		= normalize	(L+V);						// float-angle-vector
  float rsqr	= dot		(L2P,L2P);					// distance 2 light (squared)
  float  att 	= saturate	(1 - rsqr*light_range_rsq*ECB_LL_DIST)*ECB_LL_BRIGHTNESS;			// q-linear attenuate
  float4 light	= tex3D		(s_material, half3( dot(L,N), dot(H,N), m ) ); 	// sample material
  //new light model start;
  return float4(att*light.xxx,0)+att*float4(light.www*(Ldynamic_color.xyz*Ldynamic_color.xyz),light.w);
  //new light model end;
}
half4	blendp	(half4	value, float4 	tcp)    		{
	#ifndef FP16_BLEND  
		value 	+= (half4)tex2Dproj 	(s_accumulator, tcp); 	// emulate blend
	#endif
	return 	value	;
}
half4	blend	(half4	value, float2 	tc)			{
	#ifndef FP16_BLEND  
		value 	+= (half4)tex2D 	(s_accumulator, tc); 	// emulate blend
	#endif
	return 	value	;
}

#endif
