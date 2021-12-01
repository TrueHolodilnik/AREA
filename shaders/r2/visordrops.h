#ifndef VSD_MOUNT
#define VSD_MOUNT
#include "common.h"
#include "noise_2D.h"
#include "ShaderSettingsVisor.h"

float4 convert_to_screen_space(float4 proj)
{
	float4 screen;
	screen.x = (proj.x + proj.w)*0.5;
	screen.y = (proj.w - proj.y)*0.5;
	screen.z = proj.z;
	screen.w = proj.w;
	return screen;
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
		sunpos = convert_to_screen_space(sun_pos_projected)/-abs(sun_pos_projected.w);	
	else
		sunpos = convert_to_screen_space(sun_pos_projected)/abs(sun_pos_projected.w);	
	
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

float2 l_jh2(float2 f,float4 s,float l)
{
	float2 x=s.xy,V=s.zw;
	float y=snoise_2D(f*float2(VSD_TURBSIZE,VSD_TURBSIZE))*.5;
	float4 r=float4(y,y,y,1);
	r.xy=float2(r.x+r.z/4,r.y+r.x/2);
	r-=.5;r*=l;return saturate(f+(x+V)*r.xy);
}
float3 bt9_q(float2 f,float2 y,float s,float3 V,float3 l,float3 r)
{
	float3 x=V;
	float p=s/2.;
	float2 a=(f.xy-y)/(s/2);
	float i=sqrt(p*p-a.x*a.x-a.y*a.y);
	i*=p;
	float3 E=normalize(float3(a.x,a.y,i));
	float e=clamp(pow(max(0.,dot(E,normalize(l))),2),.4,.95),z=clamp(pow(max(0.,dot(E,normalize(r))),2),.4,.95);
	e=abs(e-.68)<abs(z-.68)?e:z;
	float3 S=float3(e,e,e);V=lerp(V,V*S*1.35,.5);
	return lerp(x,V,VSD_LIGHT);
}
float2 z6g(float2 s,float2 f,float y)
{
	float2 l=f-s,V=l;V.y/=VSD_ASPECT;
	float x=length(V),S=saturate(1-x/(abs(-sin(VSD_RADIUS*y*8)*VSD_RADIUS*y)+1e-08F));
	float2 z=s+l*S*VSD_STRENGTH;
	return z;
}
float3 ud__(float4 f,float4 s,float2 y,float2 l,float2 V,float2 S,float3 e,float3 r)
{
	float2 x=z6g(l,V,S);
	if(length(x-l)>0)
	{
		float3 E=s.xyz-f.xyz;
		float z=VSD_RADIUS*S*.601,a=smoothstep(0,1,saturate(length(x-V)/z));
		float3 p=tex2Dlod(s_image,float4(x,0,0)).xyz;
		return!(f.x!=p.x||f.y!=p.y||f.z!=p.z)?s:lerp(s,bt9_q(x,V-(x-y),S,p+E,e,r),lerp(1,a,VSD_SMOOTH)*VSD_MIX);
	}
	else return s;
}

float dist(float a, float b, float c, float d){
	return sqrt((a - c) * (a - c) + (b - d) * (b - d));
}

float2 wavewarp(float2 uv, float wsize, float wtime, float wval1, float wval2, float wval3, float wval4)
{
	float f = sin(dist(uv.x + wtime, uv.y, wval1, wval1)*wsize)
                  + sin(dist(uv.x, uv.y, wval2, wval2)*wsize)
                  + sin(dist(uv.x, uv.y + wtime / wval4, wval3, wval2)*wsize);
	uv.xy = uv.xy+((f/wsize));
	return uv;
}


float2 wavewarp2(float2 f)
{
	return wavewarp(f,VSD_WSIZE,VSD_WTIME,VSD_WVAL1,VSD_WVAL2,VSD_WVAL3,VSD_WVAL4);
}
float ff35(float f)
{
	return frac(sin(f)*43758.6);
}

float4 visor_drops(float2 f,float4 s,float4 V,float3 y,float3 x,float l)
{
	float4 z=s;
	f=lerp(f,l_jh2(f,VSD_TURBSHIFT,VSD_TURBTIME),VSD_TURBCOF);
	s=tex2Dlod(s_image,float4(f,0,0));
	float2 e=f,a=0;
	float r=0;
	float2 p=0,S=float2(48,36),i=1./S;
	float E=0;1.25;
	float2 m=f;
	bool T=floor(m.y/i.y)%2.>=1.;
	if(T)m.x+=E*i.x/2.;
	float2 w=floor(m*S)/S+i*.5f,t=wavewarp2(w);
	float F=pow(length(t-float2(.5,.5)),3)*10,n=(.5+length(t-w)*20*F)*1.2f;
	if(abs(t.x-w.x)*n>.00475||abs(t.y-w.y)*n>.00361)n=.5f;
	float2 h=t;h.x-=.5;
	float H=h.y*h.x+h.x,d=ff35(H);
	if(d>=1-l)V.xyz=ud__(s,V,f,e,t,n,y,VSD_FIXLIGHT);
	return V;
}




#endif