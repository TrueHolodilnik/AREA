
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
#define VIS									// Enables/disables the visor reflection effect
#define VIS_XFACTOR float(2.0)
#define VIS_YFACTOR float(1.3)
#define VIS_CRUNCHINTENSITY float (0.25)	// Intensity of the peripheral/radial crunch effect in the visor
#define VIS_MIRRORINTENSITY float(0.15)		// Intensity of the mirroring effect in the visor
#define VIS_RADIALCOEFF float(1.0)			// Increment power of radial effect towards periphery (1.0 = linearly incrementing)
#define VIS_BREAK float(1.4)				// Was 0.9
#define VIS_MIRRORMINDIST float (2.50)		// Objects nearer than this values won't be mirror. Use 2.5 to prevent the player's weapon to be mirrored too much.
#define VIS_SCREENRATIO float(0.625)		// Screen Ratio; use 0.75 for 4:3 (normalscreen) and 0.625 for 16:10 (widescreen) resolution
#define VIS_SATURATION float(1.0)			// Level of color saturation before applying visor mirror color (if other than (1,1,1)). 0 is fully desaturated, 1 is original color
#define VIS_SAT_SUN_COEF float(0.01)			// How much influence sun lighting has on the visor mirror color saturation.
#define VIS_CONTRAST float(1.0)				// Contrast ratio of mirrored color (1.0 = normal, 0.0 = none).
#define VIS_COLOR float4(1,1,1,1)			// Color filter applied to visor ("sunglass effect") (RGBA)
#define VIS_COLORCOEFF float(2.0)			// Power of color filter of mirrored object (1.0 = linear, 2.0 = square)

// visor border settings
#define VIS_CENTER float2(0.0,0.0)		// Y-axix center of border black out.
#define VIS_BLACKASPECT float2(1.5,0.1)		// Aspect ratio of border black out.
#define VIS_BLACKRADIUSMIN float2(0,0)	// Screen radius at which border black out begins.
#define VIS_BLACKRADIUSMAX float2(0,0)	// Screen radius at which border black out is complete.


float4 t1(float2 s,float4 V,float e,int f)
{
	float4 R=0;
	float2 r=float2(.5,.5),i=float2(s.x,(s.y-.5)*VIS_SCREENRATIO+.5);
	float x=length(i.xy-float2(.5,.5)),y=10,t=.35,F=.05,T=2.,O=.8,z=.15,c=5.2;
	if(x>=t)
	{
		float p=saturate((x/t-1)*T);
		for(int L=0;L<y;L++)
		{
			float N=1-p/O+z*(L/(y-1))*p;
			float2 m=(float2(.5,.5)-s.xy)*-N+r;
			R+=tex2Dlod(s_image,float4(m,0,0));
		}
		R*=1/y;
		R*=1-saturate((x-t-F)*c);
	}
	else R=tex2Dlod(s_image,float4(s,0,0));
	
	return(V+VIS_CRUNCHINTENSITY*R)/(1.f+VIS_CRUNCHINTENSITY);
}

float4 t2(float2 s,float4 V,float e,int f)
{
	float4 R=0;
	float r=f==1?VIS_CENTER.x:VIS_CENTER.y,O=f==1?VIS_BLACKASPECT.x:VIS_BLACKASPECT.y,F=f==1?VIS_BLACKRADIUSMIN.x:VIS_BLACKRADIUSMIN.y,y=f==1?VIS_BLACKRADIUSMAX.x:VIS_BLACKRADIUSMAX.y,z=0,t=0,x=0,i=0,L=0,T=0,c=0;
	z=VIS_XFACTOR*(s.x-.5);
	t=VIS_YFACTOR*(s.y-.5);
	x=sqrt(z*z*O+t*t);
	L=x;
	i=x;
	if(f!=1)
	{
		float p=VIS_XFACTOR*(s.x-.5),N=-(p-.5)+.5;
		p-=.3;
		N-=1.3;
		x=sqrt(p*p*tan(p+r*t)+t*t*cos(t));
		L=sqrt(N*N*tan(N+r*t)+t*t*cos(t));
	}
	T=1-saturate((x-F)/(y-F));
	c=1-saturate((L-F)/(y-F));
	x=i;
	static const float p=3.14159f;
	float2 N=float2(s.x,(s.y-.5)*VIS_SCREENRATIO+.5);
	float m=length(N.xy-float2(.5,.5)),l=x?-x/tan(x)/.4:0.,o=l*z,d=l*t,n=fmod(o/(VIS_BREAK*p)+.5,1.f),Y=d/(VIS_BREAK*p)+.5;
	float2 D=float2(n,Y);
	float v=tex2Dlod(s_position,float4(D,0,0)).z,K=saturate(1-(VIS_MIRRORMINDIST-v)/VIS_MIRRORMINDIST)*VIS_MIRRORINTENSITY;
	
	float4 X=tex2Dlod(s_image,float4(D,0,0));
	
	//float C=dot(X.xyz,float3(.3,.59,.11));
	//X.xyz=lerp(float3(C,C,C),X.xyz,VIS_SATURATION+dot(Ldynamic_color.xyz,float3(1,1,1))/3*VIS_SAT_SUN_COEF);
	float3 A=X.xyz*X.xyz;
	X.xyz=lerp(X.xyz,A,VIS_CONTRAST);
	R=V+K*pow(m,VIS_RADIALCOEFF)*X*pow(VIS_COLOR,VIS_COLORCOEFF);
	
	return R;
}

float4 visor_reflect(float2 s,float4 f,float V)
{
	int t=(float)((e_weights.x-.99f)*1000.f);
	float4 R = 0;
	R=t2(s,f,V,t);
	R=t1(s,R,V,t);
	return R*VIS_COLOR;
}