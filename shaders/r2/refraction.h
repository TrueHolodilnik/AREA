#ifndef REFRACTIONMODUL
#define REFRACTIONMODUL
#include "noise_3D.h"
//sampler2D input : register(s0);
//sampler2D Texture1 : register(s1);

// new HLSL shader
// modify the comment parameters to reflect your shader parameters

/// <summary>Explain the purpose of this variable.</summary>
/// <minValue>0/minValue>
/// <maxValue>1</maxValue>
/// <defaultValue>0.1</defaultValue>
//#define Amplitude float(0.1)
/// <summary>Explain the purpose of this variable.</summary>
/// <minValue>0/minValue>
/// <maxValue>256</maxValue>
/// <defaultValue>30</defaultValue>
//#define Frequency float(30.0)
/// <summary>Explain the purpose of this variable.</summary>
/// <minValue>0/minValue>
/// <maxValue>64</maxValue>
/// <defaultValue>3</defaultValue>
//#define Period int(3)
/// <summary>Explain the purpose of this variable.</summary>
/// <minValue>0/minValue>
/// <maxValue>100</maxValue>
/// <defaultValue>0</defaultValue>
//float RandomNumber : register(C3);


/// <summary>
/// Shader to refract all pixels with their alpha channel set to 0.
/// </summary>


#ifdef GL_ES
    precision highp float;
#endif


/// <summary>
/// Uniform variables.
/// <summary>
#define TexelSize float2(0.5/1024.f,0.5/768.f)








/// <summary>
/// Compute the normal using a sobel filter on the adjacent noise pixels.
///
/// Normally you would output the noise to a texture first and then calculate
/// the normals on that texture to improve performance; however everthing is
/// kept in this shader as a single process to help illustrate what's going on.
/// <summary>
/// <returns>A normal floattor.</returns>
float3 GetNormal (float2 vUv, float Amplitude, float Frequency, float Period, float factor, float time)
{
	//float Amplitude = e_barrier.x*10;
	//float Frequency = e_barrier.y*1000;
	//float Period = e_barrier.z*100;
	
    // Get Sobel values
    float2 uv = vUv * Frequency / factor;
    //float z = (time * Period * factor + time * Period)/2.f;
	float z = time * Period;
    
    float tl = snoise(float3(uv.x - TexelSize.x, uv.y - TexelSize.y, z));
    float t = snoise(float3(uv.x, uv.y - TexelSize.y, z));
    float tr = snoise(float3(uv.x + TexelSize.x, uv.y - TexelSize.y, z));
    float l = snoise(float3(uv.x - TexelSize.x, uv.y, z));
    float r = snoise(float3(uv.x + TexelSize.x, uv.y, z));
    float bl = snoise(float3(uv.x - TexelSize.x, uv.y + TexelSize.y, z));
    float b = snoise(float3(uv.x, uv.y + TexelSize.y, z));
    float br = snoise(float3(uv.x + TexelSize.x, uv.y + TexelSize.y, z));

    // Sobel filter
    float3 normal = float3((-tl - l * 2.0 - bl) + (tr + r * 2.0 + br),
                (-tl - t * 2.0 - tr) + (bl + b * 2.0 + br),
                1.0 / Amplitude / factor);
                        
    // Return normalized floattor
    return normalize(normal);
}

float ramp(float x)
{
    return 1.0f - 2.0f * (x-floor(x));
}

float2 xrefract(float2 vUv, float Amplitude, float Frequency, float Period, float dist_factor, float time) 
{ 
	
	//float4 color; 
	
	//float4 normal = tex2Dlod(Texture1, float4(vUv,0,0));
	
	//bool horizontal = normal.g > 0.5 && normal.r < 0.5 && normal.b < 0.5;

    // Refract only tagged pixels (that is, the alpha channel has been set)
    float2 offset;
    float factor = 0;
    //if ( tex2Dlod(input, float4(vUv,0,0)).w == 0.0 )
    //if(vUv.y > 0.5 && horizontal)    
	if (true)
    {
    		factor = dist_factor; //pow(1/depth,1);
        // Method 1: Use noise as the refraction angle.
        // Fast and good results for some scenarios.
        //const float pi = 3.141592;
        //float noise = snoise(float3((vUv * Frequency), RandomNumber * Period)) * pi;
        //offset = float2(cos(noise), sin(noise)) * Amplitude * TexelSize;
        
        // Method 2: Get the normal from an animating normalmap to use as the refracted floattor.
        // Slower, but better results.
        float3 normal = GetNormal(vUv, Amplitude, Frequency, Period, factor, ramp(time));
        offset = normal.xy;
    }
    
    // Use the colour at the specified offset into the texture
    
    //float4 origColor = tex2Dlod(s_image, float4(vUv,0,0));
    
    //color.xyz = tex2Dlod(s_image, float4(vUv + offset*factor,0,0)).xyz;
    //color.w = 1.0;
    //return origColor + 0.5*color;
	//return color;
	return vUv + offset*factor; 
}
#endif