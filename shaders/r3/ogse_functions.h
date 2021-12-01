#ifndef OGSE_FUNCTIONS_H
#define OGSE_FUNCTIONS_H

static const float2 poisson_disk[12] = {				// ���� ��������
	float2(0.2636057f, 0.4026764f),
	float2(0.8931927f, 0.3809958f),
	float2(-0.351227f, 0.9287762f),
	float2(-0.4619994f, -0.001132701f),
	float2(0.1122695f, 0.9784672f),
	float2(0.6323467f, -0.2056593f),
	float2(-0.08509368f, -0.2938382f),
	float2(-0.2061059f, 0.3703068f),
	float2(-0.5982098f, -0.5618949f),
	float2(-0.8395036f, 0.4914388f),
	float2(-0.9885221f, 0.02723012f),
	float2(0.1027245f, -0.8669642f),
};

// new uniform variables - OGSE Team
// global constants
//uniform float4 ogse_c_resolution;	// x - width, y - height, z - 1/width, w - 1/height
//uniform float4 ogse_c_screen;		// x - fFOV, y - fAspect, z - Zf/(Zf-Zn), w - Zn*tan(fFov/2)
//uniform float4 ogse_c_jitter;		// x - jitter u, y - jitter v. Test for smaa t2x, don't touch
uniform float4 ogse_c_rain;			// x - rmap dist, y - flood level, z - wet level, w - rain intensity
//uniform float4 ogse_c_various;		// x - moonroad flag from weather


#endif