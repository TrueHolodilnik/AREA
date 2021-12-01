/* */
/* wet_relection.h - Wet reflection shader (C)2011-2013 by Meltac */
/* */

#define WET								// Defines whether to use dynamic wet surfaces. Comment out to disable.	
	#define WET_ZOOM						// Defines whether to use dynamic wet surfaces also when zooming gun/binoc.
	#define WET_FX .5f/1024.f				// Just the X-axis factor
	#define WET_FY .5f/768.f				// Just the Y-axis factor
	//#define WET_DEBUG						// Enables debug output. Leave disabled.
	
	// Basic Settings
	#define WET_BASE half(0.5)				// Amount of full wet ground
	#define WET_REF half(0.5)				// Amount of full wet reflections
	#define WET_REF_BUILD half (0.5)		// Amount (fixed) of diffuse reflection in buildings that are defined as being reflective
	#define WET_NORMAL half(0.001)			// Verticality / normality barrier for outdoor/terrain
	#define WET_NORMALIN half(0.001) // 0.5		// Verticality / normality barrier for indoor/buildings
	#define WET_NORMALFLOOD half(0.75)		// Verticality / normality threshold for "flooded" wet ground/floors
	//#define WET_BUILDINGREF					// Enables/disables building's floors being always reflective. Default=disabled.
	#define WET_MAXALPHA half(0.01)		// Alpha barrier (reduce if encountering rendering artifacts in the sky). Change only if experiencing shader ghosting.
	#define WET_HEMILUM half (0.7)			// Hemisphere luminance level at which the color recognition changes (to prevent false positives) - do not change normally
	#define WET_MAXCOF half(3.0) //1.0		// Maximum  amount of wet reflection on normal ground (asphalt etc.)	
	#define WET_MAXCOF_METAL half (0.25)	// Maximum  amount of wet reflection on metallic structures
	#define WET_MAXCOF_VEHIC half (0.5)	// Maximum  amount of wet reflection on vehicles
	#define WET_MAXCOF_BUILD half (0.5)	// Maximum  amount of wet reflection on/in buildings and concretes	
	#define WET_CONTRAST half (0.0) // 0.7 // Maximum constrast coefficient (larger values make wet surfaces look darker). Can be disabled by commenting out. CAUTION: Causes black artifacts - disabled for now.
	#define WET_CONTRASTBLUE half (0.7)	// How much blue colors (e.g. asphalt) will be more affected by contrast change than other colors.
	#define WET_MINLUM half(0.085)			// Minimum luminance (brightness) of ground to apply wet reflection effect	
	#define WET_MAXDIST half(375.0)		// If not commented out, maximum distance for the wet refelction effect to be applied
	#define WET_FADEMIN half(10.0)			// Distance at which the wet effect begins to fade.
	#define WET_FADEMAX half(375.0)		// Distance at which the wet effect has completely faded out.
	//#define WET_ADVANCEDBLUR half(1.0)	// If not commented out, blur amount to smoothen area between wet and dry (caution: very slow compilation!). Does not work correctly.
	#define WET_BLUR half(3.0)				// If not commented out, blur amount applied to the entire wet/reflective area
	#define WET_BLURQUALITY int(12)			// Quality of blur of the wet/reflective area
	#define WET_REFBLUR half(0.5)			// If not commented out, blur amount applied to the reflection. Decreases effect of WET_BLOOM and WET_RAINBLOOM.
	#define WET_REFBLURQUALITY int(12)		// Quality of blur of the reflection	
	#define WET_BLOOM_REF half(0.0)		// amount of reflected color bloom to add to the reflection color (makes it brighter as well)
	#define WET_BLOOM_COL half(0.0)		// amount of ground color bloom to add to the reflection color (makes it brighter as well)
	#define WET_WAVE half(0) // 0.01)			// inverse amplitude of sinus wave distorting the water reflection effect (lower means larger wave amplitude) - CAUTION: Breaks rain modulation!
	#define WET_WAVENEG half(0) //0.00007)		// inverse amplitude of sinus wave distorting the water reflection effect - same as above but for negative ref values - CAUTION: Breaks rain modulation!	
	#define WET_LUM half(17)				// How much luminosity affects wet reflections (color factor). Do not change normally. See also next line.
	#define WET_LUMCOF half(3)				// How much luminosity affects wet reflections (power coefficient). See also previous line.		
	
	#define WET_BLOOM half(0.24)			// How much wetness affects bloom/blur of reflections. Higher values = more bloom. Effect depends on your r2_ls_bloom settings (in weather_control.script). Adjust WET_REF when changing this.
	#define WET_RAINBLOOM half(0.05)		// How much rain density affects bloom/blur of reflections. Higher values = more bloom. Effect depends on your r2_ls_bloom settings.
	
	#define WET_YCORR half(1.3)			// View correction on Y-axis. Do not change normally.

	// Object Reflection Settings
	#define WET_OBJREFLECT					// Enables/disables dynamic object reflections (increases compile time and impacts FPS heavily!)
	#define WET_OBJDEPTH half(0.1)			// Object depth difference when using dynamic object reflections	
	#define WET_OBJSAMPLES int(40)			// Number of samples for dynamic object reflections
	#define WET_OBJSTEP int(20)	 // 6			// Step size between samples
	#define WET_OBJMAXDISTCOL half(50.0)	// Maximal distance of reflective pixel to apply dynamic object reflections
	#define WET_OBJMAXDISTREF half(100.0)	// Maximal distance of reflected  pixel to apply dynamic object reflections	
	#define WET_OBJMAXCOL half(0.56)		// Maximum amount of reflective color
	#define WET_OBJMAXREF half(0.55)		// Maximum amount of reflected  color
	#define WET_OBJFADE int(3)	// 6		// Amount of object fading. Normal is about 8.
	#define WET_OBJDSTCORR half(1)			// Depth correction value for object reflections. Normally do not change.
	#define WET_OBJWAVE half(0.00)			// inverse amplitude of sinus wave distorting the water reflection effect fore object reflections (see also WET_WAVE).
	
	// Rain distortion/modulation effect Settings
	#define WET_DISAMP half(  0.10)// 1.00	// Maximum distortion amplitude
	#define WET_DISFRQ half( 20.00)// 10.0	// Maximum distortion frequency
	#define WET_DISPER half( 2.00)// 2.00	// Maximum distortion period
	// #define WET_DISAMP half(  0.35)// 1.00	// Maximum distortion amplitude
	// #define WET_DISFRQ half( 50.00)// 10.0	// Maximum distortion frequency
	// #define WET_DISPER half(100.00)// 2.00	// Maximum distortion period
	#define WET_RAINTHR half(0.2)			// Rain density threshold beyond which an alternative reflection is used.
	
	// Asphalt Color Settings
	#define WET_BLOOM_MIX half(0.3)		// How much bloom impacts wetness detection. 0=none, 1=full - larger looks smoother but is more prone to false detection. Decrease upon wetness issues.
	#define WET_COLDETECT					// Enable/disable asphalt detection by color. Disable only if not using vanilla asphalt textures and not seeing any wet reflections.
	#define WET_SUNLUM half(0.1)			// Brightness/luminance of the sun below which color detection will not be used. Change only if not seeing any wet reflections.
	#define WET_BLUEDIFF half(-0.1)		// Minimum color difference between blue and red/green terrain color. Might need changing if using custom textures and/or shaders and/or weathers.
	#define WET_BLUECORR half(0.4)			// Corrected amount of blue color component. 1 = original blue, 0 = no blue. Change only if not using DS 2.0 asphalt textures.	
	#define WET_SAT half(1)				// Wet color (de)saturation factor. 1=no change / keep original colors.
	
	#define WET_COLFILTER half4(2,1.6,0,1)	// Enables/disables and sets color filter for wet surfaces/buildings. Experimental.
	
	// Debug Settings
	#define WET_DEBUGVAL half(0.0)			// Increase in 0.1 steps for debugging purposes. Leave at 0.0 for normal use.	
	
int mcls(half mcol)
{
	mcol = (mcol + 0.25) / 4.f;
	if      (mcol < 0.14)	// RGB 28 - Concrete walls, stone/brick buildings, NPCs, mutants, suits
		return 1;
	else if (mcol < 0.16)	// RGB 37 - Terrain, grass, water, metal cases, wood crates, signs, plastics, medkits, food, TVs, bulbs/lamps, some doors
		return 2;
	else if (mcol < 0.22)	// RGB 52 - Wood, bushes, trees, glass bottles
		return 3;
	else if (mcol < 0.29)	// RGB 70 - Weapons, binoc, ammo, ammo boxes
		return 4;
	else if (mcol < 0.33)	// RGB 82 - Vehicles, steel boxes, gas cans, fuel cans, barrels
		return 5;
	else if (mcol < 0.36)	// RGB 87 - Metallic structures/objects
		return 6;
	else
		return -1;
}

#include "refraction.h"
//#include "lightdetect.h"
#include "gauss.h"

half4 decolor(half4 Color) : COLOR
{	
	Color.r *= 5;	
	half sat = 0.7;
	//half sun = COLOR_SAT_SUN_COEF;
	half sun = 0.6;	
	half gray=dot(Color.xyz,half3(0.3,0.59,0.11));
	Color.xyz=lerp(half3(gray,gray,gray),Color.xyz,sat+(dot(Ldynamic_color.xyz,half3(1,1,1))/3)*sun);	
	return Color;
}


half4 checkwet(half2 uv, bool what) : COLOR
{     	
	half4 Color;
	Color = tex2Dlod( s_image , half4(uv.xy,0,0));
	half3 viewspace_P = tex2Dlod(s_position, half4(uv,0,0)).xyz; // half3(uv.x, uv.y, -uv.y);
	half4 check_col = tex2Dlod(s_bloom, half4(uv,0,0));
	half lum = (Color.r + Color.g + Color.b) / 3;
	
	if (what)
		check_col /= L_hemi_color;
	else		
		check_col = (((check_col / L_hemi_color) / Ldynamic_color) / half4(L_sun_color, 1))  / 3;
	
	half m = 0.08;   // e_barrier.x * 10; //0.3;    
    half g = 0.30;	  // e_barrier.y * 10; //0.99;
    half r = 0.86;	  // e_barrier.z * 10; //1.0;
    half col_ratio = check_col.b / ( (check_col.r + check_col.g + check_col.b) / 3);

    if (check_col.b > m && check_col.r < g && check_col.g < g && col_ratio > r
	&& viewspace_P.z < WET_MAXDIST
	&& lum > WET_MINLUM)
    {	
		half amount = saturate(((check_col.b-m)+0.01) * ((g-check_col.r)+0.01) * ((g-check_col.g)+0.01) * ((col_ratio-r)+0.01) * (0.5-0.4) * 100000);		
		Color.a = 0.1;
	}
	else	
		Color.a = 1.0;
	
	return Color;
}


half4 decolorize(half2 uv, bool what) : COLOR
{ 
	half4 Color = checkwet(uv, what);
	if (Color.a == 0.1)
		Color = decolor(Color);
	return Color;
}


half4 normalize_blue(half4 Color) : COLOR
{
 return half4(Color.r / Color.b, Color.g / Color.b, 1, Color.a);
}

half4 objreflect(half4 Color, half4 P, int mat, bool indoor_wet, bool indoor_ref, half2 uv : TEXCOORD, half dist_factor, half wind, half colAmount, half refAmount, half rain_density, half water_height) : COLOR
{
	//int WET_OBJSAMPLES = 40; // (int)e_weights.x*100;
	//int WET_OBJSTEP = (int)e_weights.y*100;
	//int WET_OBJFADE = (int)e_weights.z*100;
	//half WET_OBJDEPTH = e_weights.z;
	
	half yMax = 0.5;  	
	bool obj = false;
	half lastDepth = P.z;
	half expectedDepth = 0;
	half maxObjDepth = 0;
	half objDepth = 0;
	half4 objPos = 0;
	int objMat = mat;
	int sample = 0;
		
	if ( (mat == 2 || mat == 1 && (indoor_wet || indoor_ref) ) && P.z < WET_OBJMAXDISTCOL) // restrict object reflections on terrain
	{		
		half yF   = 1.f / 768.f;
		half yFac = WET_OBJSTEP / 768.f;		
		
		for (int i=1; i <= WET_OBJSAMPLES; i++)
		{
			if (!obj)
			{
				half testY = uv.y - yFac * i;
				half4 testPos = tex2Dlod( s_position , half4(uv.x, testY, 0, 0) );	
				
				bool above_water = false;
				
				if (water_height != 0)
				{
					half3 testPos_world     = mul(half4(testPos.xyz,1),(m_WV)).xyz+eye_position.xyz;	
					above_water = testPos_world.y > water_height;
					// half3 testPos_water     = half3(testPos_world.x, water_height, testPos_world.z);				
					// half4 testPos_projected = mul(m_VP, half4(testPos_water, 1));					
					// half4 testPos_screen    = convert_to_screen_space(testPos_projected)/abs(testPos_projected.w);	
					// testPos = tex2Dlod( s_position , half4(testPos_screen.x, testPos_screen.y, 0, 0) );			
				}				
				
				objMat = mcls(testPos.a);
				
				if (testPos.z < WET_OBJMAXDISTREF)
				{				
					expectedDepth = (lastDepth + WET_OBJDEPTH); // * (lastDepth/e_barrier.z*100);
					maxObjDepth = P.z; // - 2*WET_OBJDEPTH;
					lastDepth = testPos.z;
					
					// Primary check: Change of material - Secondary: Depth
					//if ( water_height != 0 && above_water  ||  objMat != mat ) // || (testPos.z < expectedDepth  && testPos.z > maxObjDepth) )
					if (testPos.z < expectedDepth  && testPos.z > maxObjDepth)
					{						
						yMax = testY;
						obj = true;
						objDepth = testPos.z;	
						objPos = testPos;
						sample = i;
					}
				}				
			}
		}
		
		int testMat = objMat;
		
		// reflection point found, so refine it
		if (obj) // && e_barrier.z > 0.1)
		{
			for (int i=0; i <= (WET_OBJSTEP+5); i++)
			{
				if (obj)
				{
					half testY = yMax + yF * i;
					half4 testPos = tex2Dlod( s_position , half4(uv.x, testY, 0, 0) );
					
					bool below_water = false;
					
					if (water_height != 0)
					{
						half3 testPos_world     = mul(half4(testPos.xyz,1),(m_WV)).xyz+eye_position.xyz;	
						below_water = testPos_world.y < water_height;
						// half3 testPos_water     = half3(testPos_world.x, water_height, testPos_world.z);				
						// half4 testPos_projected = mul(m_VP, half4(testPos_water, 1));					
						// half4 testPos_screen    = convert_to_screen_space(testPos_projected)/abs(testPos_projected.w);	
						// testPos = tex2Dlod( s_position , half4(testPos_screen.x, testPos_screen.y, 0, 0) );			
					}
				
					testMat = mcls(testPos.a);
					
					expectedDepth = (lastDepth - WET_OBJDEPTH/WET_OBJSTEP) * 1; //e_barrier.z*100;					
					lastDepth = testPos.z;

					//if (abs(testPos.z - objDepth) < e_barrier.z*100)
					//if ( water_height != 0 && below_water  ||  objMat != mat && testMat == mat ) // || testPos.z >= expectedDepth || testMat != objMat)
					if (testPos.z >= expectedDepth)
					{										
						yMax = testY;						
						objDepth = testPos.z;
						objPos = testPos;
					}
					else obj = false;
				}
			}
			obj = true;
		}
		
		half yOld = yMax;
		
		// NEW: Redefine Y according to depth		
		if (objDepth > 0)
		{
			
			// calc ground world pos
			
			// half3 pos_world = eye_position;
			// half4 pos_projected = mul(m_VP, half4(pos_world, 1));	
			// half4 pos_screen = convert_to_screen_space(pos_projected)/abs(pos_projected.w);		
			// half  pos_diff = pos_world.y - pos_screen.y;
			
			//half  pos_diff = P.y - normalize(eye_direction).y*WET_YCORR; // P.y;
			objPos.xyz = mul(objPos.xyz,(half3x3)m_WV).xyz;			
			//objDepth = objPos.z;
			half d4 = distance(half2(0,0), objPos.xz);
			
			half pos_diff = mul(P,(half3x3)m_WV).y;
			
			
			half WET_YCORR2 = 0.8;  // (e_barrier.z*10);
			half WET_YCORR3 = 4.0f;			
			//half WET_YCORR4 = (e_barrier.y-0.5)*100 + (e_barrier.z-0.5)*100 * eye_position.y + pos_diff; //0.15f
			//half WET_YCORR4 = (e_barrier.y-0.5)*100 + (e_barrier.z-0.5)*100 * (pos_diff); //;+ (e_barrier.z-0.5)*100*pos_diff; //0.15f
			half WET_YCORR4 = (0.49005-0.5)*100 + (0.4925-0.5)*100 * (pos_diff);
			//half WET_YCORR4 = (0.4915-0.5)*100 + (0.5075-0.5)*100 * eye_position.y; //0.15f
			yMax = (WET_YCORR4/objDepth) + 0.5f + 0.5f*saturate(0.5f/pow(objDepth,1))*WET_YCORR3;// ;
			
			half ny = normalize(eye_direction).y;
			
			if (ny <= -0.6)
				WET_YCORR2 = 0.88;	// 0.90
			else if (ny > -0.6 && ny <= -0.55)
				WET_YCORR2 = 0.85;	// 0.87
			else if (ny > -0.55 && ny <= -0.5)
				WET_YCORR2 = 0.83;	// 0.84
			else if (ny > -0.5 && ny <= -0.4)
				WET_YCORR2 = 0.80;  // 0.82;
			else if (ny > -0.4 && ny <= -0.2)
				WET_YCORR2 = 0.79; //0.80;
			else
				WET_YCORR2 = 0.78;			
			
			yMax = yMax + ny*WET_YCORR2;
			
			if (ny < 0)
			{
				//yMax = (yOld + yMax) * 0.5f;
				half oldFact = abs(ny)*1.5;
				//yMax = ( oldFact * yOld + (1-oldFact)*yMax ) * 0.5f;
				yMax = lerp(yMax,yOld,oldFact);
			}
			
			
			//yMax = yMax - (pos_diff-P.y);
			//yMax = mul(half3(P.x, yMax, P.z),(half3x3)m_WV).y;
			
			//if (normalize(eye_direction).y > e_barrier.z*10)
			//	yMax = yOld;
			
			
			//half4 pos4 = tex2Dlod(s_position,half4(center.xy,0,0));
			//half3 wp4 = mul(objPos.xyz,(half3x3)m_WV).xyz;
			//half d4 = distance(half2(0,0), wp4.xz);
			
		}
		
		//half wave = e_barrier.z;
		half wave = WET_OBJWAVE; //WET_WAVE;

		//half newY = -(uv.y-yMax)+yMax + (sin(uv.y*100 / pow(dist_factor, 2) )*0.03); 
		half newY = -(uv.y-yMax)+yMax + (sin(P.y*100 * wind * wave / pow(dist_factor, 2) )*0.03); 
		
		// ANTI-TEST 1: Do not reflect onto reflected object (requires NOT USING ABS() above)
		if (newY > yMax || newY < 0)
			obj = false;

		if (obj) 
		{
			half4 ref_col = 0;
			
			// refract
			//rain_density = 1;
			//rain_density *= 100;
			//half WET_DISAMP2 = e_barrier.x * 100;
			//half WET_DISFRQ2 = e_barrier.y * 100;
			//half WET_DISPER2 = e_barrier.z * 100;
			half ref_amp = rain_density * WET_DISAMP; // / 2.f;
			half ref_frq = rain_density * WET_DISFRQ;
			half ref_per = rain_density * WET_DISPER; 
			half2 refraction = xrefract(half2(uv.x, newY), ref_amp, ref_frq, ref_per, pow(dist_factor, 0.5), 0);
			refraction = xrefract(refraction, ref_amp, ref_frq, ref_per, pow(dist_factor, 0.5), timers.x);			
			uv.x = refraction.x;
			newY = refraction.y;
			
			half4 ref_bloom = tex2Dlod( s_bloom , half4(uv.x, newY, 0, 0 ));
			
			if (mat != 1 || indoor_wet)	// buildings only when defined as wet
			{
				ref_col = tex2Dlod( s_image , half4(uv.x, newY, 0, 0 ));
				// mix between bloom and clean image according to wet amount and rain density
				//half WET_BLOOM = e_barrier.z;
				ref_col	  = lerp(ref_bloom, ref_col, saturate((1-WET_BLOOM)*refAmount - WET_RAINBLOOM*rain_density) );
			}
			else // dry but reflective buildings
			{
				ref_col = ref_bloom;		// s_bloom does not look so good here, so use the same for the moment ???????
			}
									
			half4 ref_pos = tex2Dlod( s_position , half4(uv.x, newY, 0, 0 ));
			int ref_mat = mcls(ref_pos.a);			
			
			// ANTI-TEST 2: Do not reflect beyond reflected object			
			expectedDepth = lastDepth + WET_OBJDSTCORR;
			if (ref_pos.z > expectedDepth)
				obj = false;    

			if (obj && ref_mat != 3 && ref_mat != 4 && ref_col.a < WET_MAXALPHA)	 // don't reflect trees and weapons
			{      
				//half ref_dist = pow(yMax/uv.y,WET_OBJFADE);   
				half ref_dist = pow(saturate(1-(half)sample/(half)WET_OBJSAMPLES),WET_OBJFADE)*10;   
				//ref_col.g = pow(ref_col.r,2) * 20; // debug
				
				refAmount  = saturate(10*refAmount * dist_factor * ref_dist);
				if (refAmount > WET_OBJMAXREF) refAmount = WET_OBJMAXREF;
				colAmount *= saturate(1-refAmount) * 1.8;			
				
				Color = colAmount * Color + refAmount * ref_col;
								
				//Color = colAmount * Color + refAmount * ref_col * dist_factor * ref_dist;
				
			}
		}
	}
		
	// debug
	//if (obj && e_barrier.z > 0.1 && abs(uv.y-yMax)<0.02)
	//	Color = half4(1,0,0,1);

	return Color;
}

half4 wetreflect(half4 Color, half2 uv, half wet_amount, half rain_density, half water_height, bool indoor_wet, bool indoor_ref) : COLOR
{	
	half4 OrigColor = Color;
		
	indoor_ref = true;	
						
	half4 test_Color = tex2Dlod( s_image , half4(uv.xy,0,0));
	half4 viewspace_P = tex2Dlod(s_position, half4(uv,0,0));
	
	int mat = mcls(viewspace_P.a);
	
	// Freeze wet amount in reflective buildings (use constant value independently from rain)
	if (mat == 1 && indoor_ref) wet_amount = WET_REF_BUILD;
	
	half amount = 0;
	half ref_amount = 0;	
	//bool is_indoor_wet = (mat == 1 && indoor_wet);
	
	if (WET_DEBUGVAL > 0.0 || (test_Color.a < WET_MAXALPHA && wet_amount > 0) ) // alpha 0 = exclude sky! (plus particles, fences) -> avoid shader ghosting!
	{					
		half dist_factor = saturate(1/viewspace_P.z) * 1.0f; // wet amount = 1.0	
		
		//half wind = pow(wet_amount*10,3) * sin(timers.x * e_barrier.y * 1000) * pow(dist_factor, 3); // USE 0.0001 or lower // / dist_factor; // 1; //  ((e_kernel.x-0.4) * 100);		
		half wind = 1;		
		
		if (WET_DEBUGVAL > 0.1 || (mat == 2 || mat == 5 || mat == 6 || (mat == 1 && (indoor_wet || indoor_ref) ) ) ) // is_indoor_wet
		{	
			half calc_y        = uv.y - normalize(eye_direction).y*WET_YCORR;
			
			// compute normality / verticality			
			half3 P = half3(uv.x, calc_y , viewspace_P.z);	  /// UNPACK NORMALS ?????????????????????????????????????					

			half3 viewspace_N = tex2Dlod(s_normal, half4(uv,0,0)).xyz;
			half3 N = normalize(mul(viewspace_N,(half3x3)m_WV));

			half vert = pow( (1-N.r) * (1-N.b) * N.g, 2 ); // the more green the more vertical is the surface	
			
			// check color
			half4 check_col = lerp(tex2Dlod(s_image, half4(uv,0,0)), tex2Dlod(s_bloom, half4(uv,0,0)), WET_BLOOM_MIX);				// WAS only s_bloom before, looked better but did not render everything properly as wet (false negatives)			
			half lum = (test_Color.r + test_Color.g + test_Color.b) / 3;
			half check_lum = (check_col.r + check_col.g + check_col.b) / 3;
			
			//half col_factor = 1 ; e_barrier.z*10;
						
			//check_col = (((check_col / (L_hemi_color*col_factor)) / (Ldynamic_color*col_factor) ) / (half4(L_sun_color, 1)*col_factor))  / 3;	
			//check_col = check_col / (L_hemi_color*col_factor);
			
			half m = 0.08; //e_barrier.z;
			half s = 0.35; //e_barrier.y;
			half g = 0.35; //e_weights.z;
			half r = 0.46; //e_weights.y;
			
			half min_sun = 0.01;
			if (L_sun_color.r < min_sun && L_sun_color.g < min_sun && L_sun_color.b < min_sun)
			{
				//check_col = normalize(2 * L_hemi_color * check_col);
				check_col = (2 * L_hemi_color * check_col) * 9;
			}
			else
			{
				//check_col = normalize(2 * L_hemi_color * check_col / (e_barrier.z*10 * half4(L_sun_color, 1)) );
				//check_col = ( (2* check_col / (L_hemi_color)) / pow(half4(L_sun_color, 1), 0.5) );
				check_col = (2 * L_hemi_color * check_col) * 9 / pow(half4(L_sun_color, 1), 0.5) ;
				
				half hemi_lum = (L_hemi_color.r + L_hemi_color.g + L_hemi_color.b)/3;
				if (hemi_lum > WET_HEMILUM)
					r = 0.86;
				
				#define WET_COLOR_SATURATION half(0.8)  //Level of Grey. 0 is all grey (Black and White), 1 is no Grey (Why have this enabled?)					
				half gray=dot(check_col.xyz,half3(0.3,0.59,0.11));
				check_col.xyz=lerp(half3(gray,gray,gray),check_col.xyz,WET_COLOR_SATURATION+(dot(Ldynamic_color.xyz,half3(1,1,1))/3)*0);
			}
			
			//check_col.b *= e_barrier.z * 1000;
			check_col = normalize(normalize_blue(check_col))/2;
			
			half col_ratio = check_col.b / ( (check_col.r + check_col.g + check_col.b) / 3);
			
			half maxcof = (mat == 2) ? WET_MAXCOF : ( (mat == 1) ? WET_MAXCOF_BUILD : ( (mat == 5) ? WET_MAXCOF_VEHIC : WET_MAXCOF_METAL) );
			
			half amount_base = 1000 * maxcof * wet_amount; // TODO: Make configurable
			amount = saturate(((check_col.b-m)+0.01) * ((s-check_col.r)+0.01) * ((g-check_col.g)+0.01) * ((col_ratio-r)+0.01) * amount_base);
			
			half blue_amount = check_col.b * col_ratio * (1-check_col.r) * (1-check_col.g);				
			
			//half WET_NORMAL = 0;			
			//half WET_NORMALIN = e_barrier.z * 1000;
			//is_indoor_wet
			
			half min_vert = (mat == 1) ? WET_NORMALIN : WET_NORMAL;
			
			// add contrast	if enabled
			if (vert > min_vert && wet_amount > 0)
			{
				half3 highcontrast=Color.xyz*Color.xyz;
				Color.xyz=lerp(Color.xyz,highcontrast, WET_CONTRAST * maxcof * wet_amount * pow(blue_amount, WET_CONTRASTBLUE)); // * e_barrier.y * 1000 );
			}			
			
			half sunColLum = (L_sun_color.r+L_sun_color.g+L_sun_color.b)/3;
						
			// add reflections if no sun OR color is matching OR disabled
			bool colordetection = true;
			
			if (WET_DEBUGVAL > 0.2 || (!colordetection || sunColLum < WET_SUNLUM || (check_col.b > m && check_col.r < s && check_col.g < g && col_ratio > r && viewspace_P.z < WET_MAXDIST && lum > WET_MINLUM)	) )
			{					
				// screen area check				
				//if (check_y > 0.4)
				if (true)
				{
				
					if (e_kernel.x == 0.7)
						Color = decolor(Color); 					
						
					//if (e_barrier.z > 0)
					if (WET_DEBUGVAL > 0.3 || (vert > min_vert) )  // was: min_vert
					{
						half newX = uv.x;						
						half newY = (rain_density < WET_RAINTHR) ?
							   (-(P.y-0.5)+0.5 + (sin(P.y*100 * wind * WET_WAVE / pow(dist_factor, 2) )*0.03)) : 
							abs(-(P.y-0.5)+0.5 + (sin(P.y*100 * wind * WET_WAVE / pow(dist_factor, 2) )*0.03)) ; 
						if (newY < 0)	// Correction for negative reflection coordinates
						{
							newY = abs(newY + (sin(P.y*100 * pow(abs(newY*100),2) * WET_WAVENEG / pow(dist_factor, 2) )*0.03));
							amount -= pow(abs(newY),0.5);
						}
						if (newY > 1)
							newY = 2 - newY;
							
						// add refraction ONLY ON TERRAIN (exclude indoor stuff such as metallic tables, weapons etc.)
						if (mat == 2)
						{
							//half WET_DISAMP = e_barrier.x * 100;
							//half WET_DISFRQ = e_barrier.y * 100;
							//half WET_DISPER = e_barrier.z * 100;
							half ref_amp = rain_density * WET_DISAMP;
							half ref_frq = rain_density * WET_DISFRQ;
							half ref_per = rain_density * WET_DISPER; 
							half2 refraction = xrefract(half2(newX, newY), ref_amp, ref_frq, ref_per, pow(dist_factor, 0.5), timers.x);
							//wind = newY-refraction.y;		// used for object reflections later														
							//Color = tex2D(s_image, half2(uv.x+(refraction.x-newX), uv.y+(refraction.y-newY))); 
							//Color = tex2D(s_image, half2(refraction.x, refraction.y)); 
							newX = refraction.x;
							newY = refraction.y;
						}
						
						//half4 bloom_col = tex2Dlod(s_bloom, half4(uv,0,0));
						half4 ref_bloom = tex2Dlod(s_bloom, half4(newX, newY, 0, 0 )); 
						half4 ref_col = 0;
												
						if (mat != 1 || indoor_wet)	// buildings only when defined as wet
						{
							// sample reflection color for wet outdoor and wet indoor
							ref_col   = tex2Dlod(s_image, half4(newX, newY, 0, 0 )); 
							
							// mix between bloom and clean image according to wet amount and rain density
							//half WET_BLOOM = e_barrier.z;
							ref_col	  = lerp(ref_bloom, ref_col, saturate((1-WET_BLOOM)*wet_amount - WET_RAINBLOOM*rain_density) );
						}
						else // mat == 1 && indoor_ref
						{
							// sample reflection color for dry indoor - bloom only
							ref_col   = ref_bloom;
						}						
						
						if (mat != 1 || indoor_wet)	// buildings only when defined as wet    --> ??? IS THIS THE CORRECT LOGIC ??? WHAT ABOUT indoor_ref ???
						{
							half contrib=1.h;
							half total_blur = amount * WET_REFBLUR;
							half f=0.f;
							half inc=total_blur/WET_REFBLURQUALITY;
							half3 sum = 0;
							//for (int i=0;i<WET_REFBLURQUALITY;i++){sum+=gaussbloom(half2(newX, newY),f).xyz;contrib++;f+=inc;}
							for (int i=0;i<WET_REFBLURQUALITY;i++){sum+=gaussblur(half2(newX, newY),f, false).xyz;contrib++;f+=inc;}
							ref_col.rgb += (sum/contrib) - ref_col;  							
						}

						half ref_lum = (ref_col.r + ref_col.g + ref_col.b) * ref_col.a / 3;	
						
						half ref_vert = vert;

						if (mat != 1 || indoor_wet || indoor_ref)	// buildings only when defined as wet or reflective
						{
							// Set/add water color
							half3 water = Color.rgb * saturate(1-dist_factor * WET_BASE * 2); // What the hell did I intend with this??? --> Seems to darken ground the more wet it is  
							half3 water_color = amount * water + (1-amount) * Color.rgb;   
							Color.rgb = water_color;   

							// Make different use of verticality							
							if (wet_amount > WET_NORMALFLOOD && ref_vert < wet_amount) ref_vert = wet_amount;     // WHAT THE HELL??? LOOK AT THAT WEIRD LINE OF CODE!!!							
						}
						
						// Amplify reflected color according to wet amount, luminance, and verticality
						ref_amount = amount * WET_REF * saturate(pow(WET_LUM*check_lum, WET_LUMCOF)) * ref_vert; // vert;												
						
						// Apply amount to reflected color
						ref_col *= ref_amount * dist_factor;

						// add bloom if required (disabled by default)
						//half bloom = ((e_kernel.x-0.4) * 100); 
						if (vert > min_vert)  
							ref_col = (ref_col + WET_BLOOM_COL * ref_bloom + WET_BLOOM_REF * ref_bloom);
							
						// DEBUG - REMOVE THE FOLLOWING LINE AFTER TESTING OBJECT REFLECTIONS !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
						//ref_col = 0;

						if (mat == 1 && indoor_wet)	// buildings only when defined as wet
						{
							half gray=dot(Color.xyz,half3(0.3,0.59,0.11));							
							Color = 1 * half4(lerp(half3(gray,gray,gray),Color.xyz,0.1) * half3(0.1,0.1,0),1) +  ref_col * WET_COLFILTER;
						}
						else
							Color = 1 * Color  +  ref_col;
						
						// Add object reflections
						Color = objreflect(Color, viewspace_P, mat, indoor_wet, indoor_ref, uv, dist_factor, wind, WET_OBJMAXCOL, ref_amount, rain_density, water_height);
						
						// Add light reflections
						//Color += light_reflection(half2(newX, newY), ref_amount, ref_amp, ref_frq, ref_per, dist_factor);
						
					}										
					
					// Add blur
					half contrib=1.h;
					half total_blur = amount * WET_BLUR; // e_barrier.y * 1000; //1000 * (e_kernel.x-0.4); // --> 0.401
					half f=0.f;
					half inc=total_blur/WET_BLURQUALITY;
					half3 sum = 0;
					for (int i=0;i<WET_BLURQUALITY;i++){sum+=gaussblur(uv,f,false).xyz;contrib++;f+=inc;}
					Color.rgb += (sum/contrib) - tex2Dlod(s_image,half4(uv,0,0)).xyz;  
					
					Color.a = 0.1;  				
					
					 
				}
				else	// not inside affected screen area
				{
					Color.a = 1.0;			
				}		
			}
			else	// color doesn't match
			{
				Color.a = 1.0;
				//Color.r *= 5;
			}			
		}	
		else	// not affected material
		{			
			Color.a = 1.0; 
		}		
		
	}
	
	/* BLUE FILTER (POST PROCESS FOR WET SURFACES ASPHALT BLUE */
	if (ref_amount == 0)
	{
		if (test_Color.a < WET_MAXALPHA && mat == 2) // Apply to terrain only and not to sky
		{
			half blue_diff = test_Color.b*2 - test_Color.r - test_Color.g;
			if (blue_diff > WET_BLUEDIFF)
			{
				//Color.b = (test_Color.r + test_Color.g) / 2 + blue_diff * (10*(e_barrier.z-0.5));
				
				//if (ref_amount == 0)
					Color.b *= WET_BLUECORR;
				//else
					
			}
		}
	}
	else
	{
		// If wet, decrease saturation accordingly
		half gray=dot(Color.xyz,half3(0.3,0.59,0.11));
		//Color.xyz=lerp(half3(gray,gray,gray),Color.xyz,ref_amount*e_barrier.z*10+(dot(Ldynamic_color.xyz,half3(1,1,1))/3)*COLOR_SAT_SUN_COEF);
		Color.xyz=lerp(half3(gray,gray,gray),Color.xyz,WET_SAT);  // e_barrier.z*10
	}
	
	return Color;
}