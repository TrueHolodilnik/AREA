#include "stdafx.h"
#pragma hdrstop

#include "blender_SSLR.h"

CBlender_sslr::CBlender_sslr() { description.CLS = 0; }
CBlender_sslr::~CBlender_sslr() { }

void CBlender_sslr::Compile(CBlender_Compile& C)
{
	IBlender::Compile(C);
	
	switch (C.iElement)
	{
		case 0:
			C.r_Pass("stub_notransform_aa_AA", "wet\\perform_reflections", FALSE, FALSE, FALSE);

			if (!RImplementation.o.dx10_msaa) C.r_dx10Texture("s_frame", "$user$generic0");
			else C.r_dx10Texture("s_frame", r2_RT_generic0_r);
			C.r_dx10Texture("s_position", "$user$position");
			C.r_dx10Texture("sky_s0", "$user$sky0");
			C.r_dx10Texture("sky_s1", "$user$sky1");
			C.r_dx10Texture("s_accumulator", "$user$accum");
			C.r_dx10Texture("s_puddles", "shaders\\puddles");
		
			C.r_dx10Sampler ("smp_nofilter");
			C.r_dx10Sampler ("smp_rtlinear");
			C.r_End();
		break;

		case 1:
			C.r_Pass("stub_notransform_aa_AA", "wet\\apply_and_filtering_reflections", FALSE, FALSE, FALSE);

			C.r_dx10Texture("s_position", "$user$position");
			if (!RImplementation.o.dx10_msaa) C.r_dx10Texture("s_frame", "$user$generic0");
			else C.r_dx10Texture("s_frame", r2_RT_generic0_r);
			C.r_dx10Texture("s_dtssr", "$user$genericpp");
			C.r_dx10Texture("s_material", "$user$material");
			C.r_dx10Texture("s_pptemp", r2_RT_LF);
			C.r_dx10Texture("s_puddles", "shaders\\puddles");

			C.r_dx10Sampler("smp_nofilter");
			C.r_dx10Sampler("smp_base");
			C.r_dx10Sampler("smp_rtlinear");
			C.r_End();
		break;
	}

C.r_End();
}