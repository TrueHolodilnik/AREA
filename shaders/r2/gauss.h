#ifndef GAUSSBLUR_H_INCLUDED
#define GAUSSBLUR_H_INCLUDED
//uniform float4				 screen_res;

half4 Gauss(sampler2D inputSampler, float2 texCoord, int factor, bool optimize)
{
	half4 outColor;
	
	if(!optimize)
	{


		float dx = factor*.5f/screen_res.x;
		float dy = factor*.5f/screen_res.y;
		
		outColor =
		(
		1.f * tex2Dlod(inputSampler,float4(texCoord,0,0)).rgba +
		1.f * tex2Dlod(inputSampler,float4(texCoord+float2(dx,0),0,0)).rgba +
		1.f * tex2Dlod(inputSampler,float4(texCoord+float2(dy,0),0,0)).rgba +
		1.f * tex2Dlod(inputSampler,float4(texCoord+float2(-dx,0),0,0)).rgba +
		1.f * tex2Dlod(inputSampler,float4(texCoord+float2(-dx,-dy),0,0)).rgba +
		1.f * tex2Dlod(inputSampler,float4(texCoord+float2(dx,-dy),0,0)).rgba
		)/6.f;
	}
	else
	{

		const float delta = factor * (.5f/screen_res.x);
		
		outColor =
		(
		tex2Dlod(inputSampler,float4(texCoord+delta,0,0)).rgba +
		tex2Dlod(inputSampler,float4(texCoord-delta,0,0)).rgba
		)*(1.f/2.f);
	}
	
	return outColor;
}

float4 gaussblur(float2 center, float factor, bool optimize) { return Gauss(s_image, center, factor, optimize); }

float4 gaussbloom(float2 center, float factor, bool optimize) { return Gauss(s_bloom, center, factor, optimize); }

#endif//GAUSSBLUR_H_INCLUDED