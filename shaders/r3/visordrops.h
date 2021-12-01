
#include "common.h"

#define CRACKS_INTENSITY 0.25
#define VIGNETTE_INTENSITY 0.5
////////////////////////////
#define DUDV_DISTORT float(0.025f)
///////////////////////////
#define DROPS_TURBSIZE 15.f
#define DROPS_TURBSHIFT float4(0.35, 1, 0, 1)
#define DROPS_TURBTIME sin(0.1/3.f)
#define DROPS_TURBCOF 0.33
///////////////////////////
#define VIS_GLASS_NUM 16
#define VIS_GLASS_RADIUS 0.45
#define VIS_GLASS_INTENSITY 0.35

// Original shader: https://www.shadertoy.com/view/ltffzl
//Done by BigWIngs (thanks man!). Modified, and ported by LVutner (2019)

float2 mod289(float2 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
float3 mod289(float3 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
float4 mod289(float4 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
float3 permute(float3 x) { return mod289(((x*34.0)+1.0)*x); }
float4 permute(float4 x) { return mod289(((x*34.0)+1.0)*x); }

float snoise_2D (float2 v)
{
    const float4 C = float4(0.211324865405187,  // (3.0-sqrt(3.0))/6.0
                        0.366025403784439,  // 0.5*(sqrt(3.0)-1.0)
                        -0.577350269189626, // -1.0 + 2.0 * C.x
                        0.024390243902439); // 1.0 / 41.0

    // First corner
    float2 i  = floor(v + dot(v, C.yy) );
    float2 x0 = v -   i + dot(i, C.xx);

    // Other corners
    float2 i1;
    i1 = (x0.x > x0.y) ? float2(1.0, 0.0) : float2(0.0, 1.0);
    float4 x12 = x0.xyxy + C.xxzz;
    x12.xy -= i1;

    // Permutations
    i = mod289(i); // Avoid truncation effects in permutation
    float3 p = permute( permute( i.y + float3(0.0, i1.y, 1.0 ))
        + i.x + float3(0.0, i1.x, 1.0 ));

    float3 m = max(0.5 - float3(dot(x0,x0), dot(x12.xy,x12.xy), dot(x12.zw,x12.zw)), 0.0);
    m = m*m ;
    m = m*m ;

    // Gradients: 41 points uniformly over a line, mapped onto a diamond.
    // The ring size 17*17 = 289 is close to a multiple of 41 (41*7 = 287)

    float3 x = 2.0 * frac(p * C.www) - 1.0;
    float3 h = abs(x) - 0.5;
    float3 ox = floor(x + 0.5);
    float3 a0 = x - ox;

    // Normalise gradients implicitly by scaling m
    // Approximation of: m *= inversesqrt( a0*a0 + h*h );
    m *= 1.79284291400159 - 0.85373472095314 * ( a0*a0 + h*h );

    // Compute final noise value at P
    float3 g;
    g.x  = a0.x  * x0.x  + h.x  * x0.y;
    g.yz = a0.yz * x12.xz + h.yz * x12.yw;
    return 130.0 * dot(m, g);
}

float2 l_jh2(float2 f, float4 s, float l)
{
	float2 x = s.xy, V = s.zw;
	float y = snoise_2D(f * float2(DROPS_TURBSIZE, DROPS_TURBSIZE))*.5;
	float4 r = float4(y, y, y, 1);
	r.xy = float2(r.x + r.z/4, r.y + r.x/2);
	r -= 1.5;
	r *= l;
	return saturate(f + (x + V) *r.xy);
}


float3 N13(float p) {
   // from DAVE HOSKINS
   float3 p3 = frac(float3(p,p,p) * float3(.1031,.11369,.13787));
   p3 += dot(p3, p3.yzx + 19.19);
   return frac(float3((p3.x + p3.y)*p3.z, (p3.x+p3.z)*p3.y, (p3.y+p3.z)*p3.x));
}

float N(float t) {
    return frac(sin(t*12345.564)*7658.76);
}

float Saw(float b, float t) {
    return smoothstep(0., b, t)*smoothstep(1., b, t);
}

float2 DropLayer2(float2 uv, float t) 
{
    uv.y = 1. - uv.y;  
    float2 UV = uv;
  
    uv.y += t*0.75;
    float2 a = float2(9., 2.);
    float2 grid = a*2.;
    float2 id = floor(uv*grid);
    
    float colShift = N(id.x); 
    uv.y += colShift;
    
    id = floor(uv*grid);
    float3 n = N13(id.x*35.2+id.y*2376.1);
    float2 st = frac(uv*grid)-float2(.5, 0);
    
    float x = n.x-.5;
    
    float y = UV.y;
    float wiggle = sin(y+sin(y));
    x += wiggle*(.5-abs(x))*(n.z-.5);
    x *= .7;
    float ti = frac(t+n.z);
    y = (Saw(.85, ti)-.5)*.9+.5;
    float2 p = float2(x, y);
    
    float d = length((st-p)*a.yx);
    
    float mainDrop = smoothstep(.5, .0, d);
    
    float r = sqrt(smoothstep(1., y, st.y));
    float cd = abs(st.x-x);
    float trail = smoothstep(.23*r, .15*r*r, cd);
    float trailFront = smoothstep(-.02, .02, st.y-y);
    trail *= trailFront*r*r;
    
    y = UV.y;
    float trail2 = smoothstep(.2*r, .0, cd);
    float droplets = max(0., (sin(y*(1.-y)*120.)-st.y))*trail2*trailFront*n.z;
    y = frac(y*1.)+(st.y-.5); //1 is fucking trail 
    float dd = length(st-float2(x, y));
    droplets = smoothstep(.3, 0., dd);
    float m = mainDrop+droplets*r*trailFront;

    return float2(m, trail);
}

float StaticDrops(float2 uv, float t) 
{
	uv *= 40.;
    
    float2 id = floor(uv);
    uv = frac(uv)-.5;
    float3 n = N13(id.x*107.45+id.y*3543.654);
    float2 p = (n.xy-.5)*.7;
    float d = length(uv-p);
    
    float fade = Saw(.025, frac(t+n.z));
    float c = smoothstep(0.5, 0., d)*frac(n.z*10.)*fade;
    return c;
}

float2 Drops(float2 uv, float t, float l0, float l1, float l2) 
{
    float s = StaticDrops(lerp(uv, l_jh2(uv, DROPS_TURBSHIFT, DROPS_TURBTIME), DROPS_TURBCOF), t)*l0; 
    float2 m1 = DropLayer2(lerp(uv, l_jh2(uv, DROPS_TURBSHIFT, DROPS_TURBTIME), DROPS_TURBCOF), t)*l1;
    float2 m2 = DropLayer2(lerp(uv, l_jh2(uv, DROPS_TURBSHIFT, DROPS_TURBTIME), DROPS_TURBCOF)*1.75, t)*l2;
    
    float c = s+m1.x+m2.x;
    c = smoothstep(.3, 1., c);
    
    return float2(c, max(m1.y*l0, m2.y*l1));
}

float4 screen_sunpos(float3 sundir_w)
{	
	#define SSH_FARPLANE float(180.0)

	// Transform sun position to screen space
	float4 sunpos = 0; //float4(L_sun_dir_e,0);
	//float sunheightdiff = (abs(sundir_w.y)-SSH_POSHEIGHT);	
	
	
	// OGSE Sun Pos calculation (thanks)	
	// dist to the sun
	float sun_dist = SSH_FARPLANE / (sqrt(1 - sundir_w.y * sundir_w.y));
	// sun pos
	float3 sun_pos_world = sun_dist*sundir_w + eye_position;
	float4 sun_pos_projected = mul(m_VP, float4(sun_pos_world, 1));
	
	// hack for sun vs. artificial light shafts
	if (sundir_w.x == L_sun_dir_w.x && sundir_w.y == L_sun_dir_w.y && sundir_w.z == L_sun_dir_w.z)
		sunpos = proj_to_screen(sun_pos_projected)/-abs(sun_pos_projected.w);	
	else
		sunpos = proj_to_screen(sun_pos_projected)/abs(sun_pos_projected.w);	
	
	return sunpos;
}

float4 screen_sunpos_diff(float3 sundir_w, float4 sunpos, bool useCorr)
{	
	float xdiff = abs(sunpos.x-0.5);	
	
	float dot_prod_x  = dot(normalize(eye_direction.xz), normalize(sundir_w.xz));
	float dot_angle_x = degrees(acos(dot_prod_x));
	
	float dot_prod_y  = dot(normalize(float2(eye_direction.y,0)), normalize(float2(sundir_w.y,0)));
	float dot_angle_y = degrees(acos(dot_prod_y));	
		
	float xcorr = 1.f;
	float3 testcol = 1;
	
	//if (useCorr)
	if (true)
	{
		if (xdiff > 0.40)
		{
			xcorr = 1.75;
			testcol = float3(0,1,0);
		}
		else if (xdiff > 0.36)
		{
			xcorr = 1.53;
			testcol = float3(0,1,1);
		}	
		else if (xdiff > 0.33)
		{
			xcorr = 1.42;
			testcol = float3(0.5,1,0.5);
		}
		else if (xdiff > 0.30)
		{
			xcorr = 1.33;
			testcol = float3(1,1,0);
		}
		else if (xdiff > 0.24)
		{
			xcorr = 1.22;
			testcol = float3(0,1,1);
		}
		else if (xdiff > 0.18)
		{
			xcorr = 1.115;
			testcol = float3(0,1,0);
		}		
		else if (xdiff > 0.12)
		{
			xcorr = 1.07;
			testcol = float3(1,0,0.5);
		}
	}
		
	if (!useCorr)
	{		
		
		//float b = 180; //e_barrier.z*1000; // 159; //e_barrier.z*1000; // MAX  160 / 170
		//float c = 125; //e_barrier.y*1000; // MAX  120 / 130
		
		float b = 180;
		
		//float c = e_barrier.y*1000 + xoff * e_barrier.z*1000; ; 125;		110 ?
		
		float c = 100 + cos(dot_prod_x) * 15; // e_barrier.z*1000;
		
		if (xdiff >= 0.25)
		{
			float rel_xdiff = 1-saturate(3.333f*(0.4-xdiff)/0.4f); // is between 0 and 1
			c = 100 + cos(dot_prod_x) * (15+rel_xdiff*10);
		}		
					
		float view_min_angle = 70;
			
		if (dot_angle_x < view_min_angle)
		{
			sunpos.a = 0;
		}
		else if (dot_angle_x < c)
		{	
			sunpos.a = 1-saturate((dot_angle_x-c)/(view_min_angle-c));			
		}
	}	
	
	return sunpos;
}