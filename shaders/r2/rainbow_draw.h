#include "ShaderSettingsRainbows.h"
float4 ___z523(float f,bool R) {

	f+=.333;
	f=-(f-.5)+.5;
	float i=(int)(f*6%6)+5*(f<0),p=f*6%1+(f<0),s=1-p,b,z,T;
	if(i==0)b=1,z=p,T=0;
	else if(i==1)b=s,z=1,T=0;
	else if(i==2)b=0,z=1,T=p;
	else if(i==3)b=0,z=s,T=1;
	else if(i==4)b=p,z=0,T=1;
	else if(i==5)b=1,z=0,T=s;
	if(R) return float4(1,1,1,1);
	else return float4(b,z,T,1);
	
}

float4 draw_rainbow(float2 s,float2 R,float i,bool f,bool T) {

	float4 b=float4(0,0,0,0);
	float z=.16,p=.1;
	float2 x=1.3,X=x+z,Y=1.6,r=Y+p;
	if(T)z=.18,x=.9,p=.75,Y=1.2,R.y+=.2;
	float2 N=s;
	
	#ifdef RB_WIDESCREEN
	N.y*=10/16.f;
	#endif
	
	float O = distance(R,N),o=(N.x-R.x)/x,a=(N.y-R.y)/x,t=(N.x-R.x)/Y,l=(N.y-R.y)/Y;
	if(f&&N.x<R.x) {
		float y=0,e=0;
		float4 d=float4(0,0,0,0);
		float B=abs(RB_ENDXMAX-RB_ENDXMIN),E=RB_ENDXMIN+i*B;
		if (O>=x.x&&O<=X.x) {
			y=(O-x)/(X-x);
			d=___z523(pow(y*RB_COLRATIO-RB_COLSHIFT,RB_COLPOWER),T);
			e=1*pow(1-abs(y-.5),RB_DECAY);
			float C=saturate(RB_COFX-pow(o,E)),w=saturate(RB_COFY-pow(a,RB_ENDY));
			e*=C*w;
		}
		else if (O>=Y.x&&O<=r.x) {
			y=(O-Y)/(r-Y);
			d=___z523(1-pow(y*RB_COLRATIO-RB_COLSHIFT,RB_COLPOWER),T);
			e=RB_SECONDARY*pow(1-abs(y-.5),RB_DECAY);
			float C=saturate(RB_COFX-pow(t,E)),w=saturate(RB_COFY-pow(l,RB_ENDY));
			e*=C*w;
		}
		float C=dot(d.xyz,float3(.3,.59,.11));
		d.xyz=lerp(float3(C,C,C),d.xyz,RB_SATURATION);
		b=(1-RB_INTENSITY*e)*b+RB_INTENSITY*e*d;
	}
return b;
}