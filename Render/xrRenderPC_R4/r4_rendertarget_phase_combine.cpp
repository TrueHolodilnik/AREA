#include "stdafx.h"
#include "../../xrEngine/igame_persistent.h"
#include "../../xrEngine/environment.h"

#include "../xrRender/dxEnvironmentRender.h"

#define STENCIL_CULL 0

void CRenderTarget::DoAsyncScreenshot()
{
	//	Igor: screenshot will not have postprocess applied.
	//	TODO: fox that later
	if (RImplementation.m_bMakeAsyncSS)
	{
		HRESULT hr;

		ID3DTexture2D*	pBuffer;
		hr = HW.m_pSwapChain->GetBuffer( 0, __uuidof( ID3D10Texture2D ), (LPVOID*)&pBuffer );
		HW.pContext->CopyResource( t_ss_async, pBuffer );
		

		RImplementation.m_bMakeAsyncSS = false;
	}
}

float	hclip(float v, float dim)		{ return 2.f*v/dim - 1.f; }
void	CRenderTarget::phase_combine	()
{
	PIX_EVENT(phase_combine);

	//	TODO: DX10: Remove half poxel offset
	bool	_menu_pp	= g_pGamePersistent?g_pGamePersistent->OnRenderPPUI_query():false;

	u32			Offset					= 0;
	Fvector2	p0,p1;

	//*** exposure-pipeline
	u32			gpu_id	= Device.dwFrame%HW.Caps.iGPUNum;
	if (Device.m_SecondViewport.IsSVPActive()) //--#SM+#-- +SecondVP+
	{
		// clang-format off
		gpu_id = (Device.dwFrame - 1) % HW.Caps.iGPUNum;	// Фикс "мерцания" tonemapping (HDR) после выключения двойного рендера. 
															// Побочный эффект - при работе двойного рендера скорость изменения tonemapping (HDR) падает в два раза
															// Мерцание связано с тем, что HDR для своей работы хранит уменьшенние копии "прошлых кадров"
															// Эти кадры относительно похожи друг на друга, однако при включЄнном двойном рендере
															// в половине кадров оказывается картинка из второго рендера, и поскольку она часто может отличатся по цвету\яркости
															// то при попытке создания "плавного" перехода между ними получается эффект мерцания
		// clang-format on
	}
	{
		t_LUM_src->surface_set		(rt_LUM_pool[gpu_id*2+0]->pSurface);
		t_LUM_dest->surface_set		(rt_LUM_pool[gpu_id*2+1]->pSurface);
	}

    if( RImplementation.o.ssao_hdao && RImplementation.o.ssao_ultra)
    {
        if( ps_r_ssao > 0 )
        {
		    phase_hdao();
        }
    }
    else
    {
        if (RImplementation.o.ssao_opt_data)
        {
            phase_downsamp();
            //phase_ssao();
        } 
        else if (RImplementation.o.ssao_blur_on)
            phase_ssao();
    }

	// low/hi RTs
	if( !RImplementation.o.dx10_msaa ) {
		FLOAT ColorRGBA[4] = {0.0f, 0.0f, 0.0f, 0.0f};
		HW.pContext->ClearRenderTargetView(rt_Generic_0->pRT, ColorRGBA);
		HW.pContext->ClearRenderTargetView(rt_Generic_1->pRT, ColorRGBA);
		u_setrt				( rt_Generic_0,rt_Generic_1,0,0,HW.pBaseZB );
	}
	else
	{
		FLOAT ColorRGBA[4] = {0.0f, 0.0f, 0.0f, 0.0f};
		HW.pContext->ClearRenderTargetView(rt_Generic_0_r->pRT, ColorRGBA);
		HW.pContext->ClearRenderTargetView(rt_Generic_1_r->pRT, ColorRGBA);
		u_setrt(rt_Generic_0_r, rt_Generic_1_r, 0, 0, RImplementation.Target->rt_MSAADepth->pZRT);
	}
	RCache.set_CullMode	( CULL_NONE );
	RCache.set_Stencil	( FALSE		);

	BOOL	split_the_scene_to_minimize_wait			= FALSE;
	if (ps_r2_ls_flags.test(R2FLAG_EXP_SPLIT_SCENE))	split_the_scene_to_minimize_wait=TRUE;

	// draw skybox
	if (1)
	{
		//	Moved to shader!
		//RCache.set_ColorWriteEnable					();
		//	Moved to shader!
		//RCache.set_Z(FALSE);
		g_pGamePersistent->Environment().RenderSky	();

		//	Igor: Render clouds before compine without Z-test
		//	to avoid siluets. HOwever, it's a bit slower process.
		g_pGamePersistent->Environment().RenderClouds	();

		//	Moved to shader!
		//RCache.set_Z(TRUE);
	}

	// 
	//if (RImplementation.o.bug)	{
		RCache.set_Stencil					(TRUE,D3DCMP_LESSEQUAL,0x01,0xff,0x00);	// stencil should be >= 1
		if (RImplementation.o.nvstencil)	{
			u_stencil_optimize				(CRenderTarget::SO_Combine);
			RCache.set_ColorWriteEnable		();
		}
	//}

	// calc m-blur matrices
	Fmatrix		m_previous, m_current;
	Fvector2	m_blur_scale;
	{
		static Fmatrix		m_saved_viewproj;
		
		// (new-camera) -> (world) -> (old_viewproj)
		Fmatrix	m_invview;	m_invview.invert	(Device.mView);
		m_previous.mul		(m_saved_viewproj,m_invview);
		m_current.set		(Device.mProject)		;
		m_saved_viewproj.set(Device.mFullTransform)	;
		float	scale		= ps_r2_mblur/2.f;
		m_blur_scale.set	(scale,-scale).div(12.f);
	}

	// Draw full-screen quad textured with our scene image
	if (!_menu_pp)
	{
		PIX_EVENT(combine_1);
		// Compute params
		Fmatrix		m_v2w;			m_v2w.invert				(Device.mView		);
		CEnvDescriptorMixer& envdesc= *g_pGamePersistent->Environment().CurrentEnv		;
		const float minamb			= 0.001f;
		Fvector4	ambclr = { std::max(envdesc.ambient.x * 2, minamb), std::max(envdesc.ambient.y * 2, minamb), std::max(envdesc.ambient.z * 2, minamb), 0 };
					ambclr.mul		(ps_r2_sun_lumscale_amb);

//.		Fvector4	envclr			= { envdesc.sky_color.x*2+EPS,	envdesc.sky_color.y*2+EPS,	envdesc.sky_color.z*2+EPS,	envdesc.weight					};
		Fvector4	envclr			= { envdesc.hemi_color.x*2+EPS,	envdesc.hemi_color.y*2+EPS,	envdesc.hemi_color.z*2+EPS,	envdesc.weight					};

		Fvector4	fogclr			= { envdesc.fog_color.x,	envdesc.fog_color.y,	envdesc.fog_color.z,		0	};
					envclr.x		*= 2*ps_r2_sun_lumscale_hemi; 
					envclr.y		*= 2*ps_r2_sun_lumscale_hemi; 
					envclr.z		*= 2*ps_r2_sun_lumscale_hemi;
		Fvector4	sunclr,sundir;

		float		fSSAONoise = 2.0f;
					fSSAONoise *= tan(deg2rad(67.5f/2.0f));
					fSSAONoise /= tan(deg2rad(Device.fFOV/2.0f));

		float		fSSAOKernelSize = 150.0f;
					fSSAOKernelSize *= tan(deg2rad(67.5f/2.0f));
					fSSAOKernelSize /= tan(deg2rad(Device.fFOV/2.0f));


		// sun-params
		{
			light*		fuckingsun		= (light*)RImplementation.Lights.sun_adapted._get()	;
			Fvector		L_dir,L_clr;	float L_spec;
			L_clr.set					(fuckingsun->color.r,fuckingsun->color.g,fuckingsun->color.b);
			L_spec						= u_diffuse2s	(L_clr);
			Device.mView.transform_dir	(L_dir,fuckingsun->direction);
			L_dir.normalize				();

			sunclr.set				(L_clr.x,L_clr.y,L_clr.z,L_spec);
			sundir.set				(L_dir.x,L_dir.y,L_dir.z,0);
		}

		/*
		// Fill VB
		//float	_w					= float(Device.dwWidth);
		//float	_h					= float(Device.dwHeight);
		//p0.set						(.5f/_w, .5f/_h);
		//p1.set						((_w+.5f)/_w, (_h+.5f)/_h );
		//p0.set						(.5f/_w, .5f/_h);
		//p1.set						((_w+.5f)/_w, (_h+.5f)/_h );

		// Fill vertex buffer
		Fvector4* pv				= (Fvector4*)	RCache.Vertex.Lock	(4,g_combine_VP->vb_stride,Offset);
		//pv->set						(hclip(EPS,		_w),	hclip(_h+EPS,	_h),	p0.x, p1.y);	pv++;
		//pv->set						(hclip(EPS,		_w),	hclip(EPS,		_h),	p0.x, p0.y);	pv++;
		//pv->set						(hclip(_w+EPS,	_w),	hclip(_h+EPS,	_h),	p1.x, p1.y);	pv++;
		//pv->set						(hclip(_w+EPS,	_w),	hclip(EPS,		_h),	p1.x, p0.y);	pv++;
		pv->set						(-1,	1,	0, 1);	pv++;
		pv->set						(-1,	-1,	0, 0);	pv++;
		pv->set						(1,		1,	1, 1);	pv++;
		pv->set						(1,		-1,	1, 0);	pv++;
		RCache.Vertex.Unlock		(4,g_combine_VP->vb_stride);
		*/

		// Fill VB
		float	scale_X				= float(Device.dwWidth)	/ float(TEX_jitter);
		float	scale_Y				= float(Device.dwHeight)/ float(TEX_jitter);

		// Fill vertex buffer
		FVF::TL* pv					= (FVF::TL*)	RCache.Vertex.Lock	(4,g_combine->vb_stride,Offset);
		pv->set						(-1,	1,	0, 1, 0, 0,			scale_Y	);	pv++;
		pv->set						(-1,	-1,	0, 0, 0, 0,			0		);	pv++;
		pv->set						(1,		1,	1, 1, 0, scale_X,	scale_Y	);	pv++;
		pv->set						(1,		-1,	1, 0, 0, scale_X,	0		);	pv++;
		RCache.Vertex.Unlock		(4,g_combine->vb_stride);

		dxEnvDescriptorMixerRender &envdescren = *(dxEnvDescriptorMixerRender*)(&*envdesc.m_pDescriptorMixer);

		// Setup textures
		ID3DBaseTexture*	e0	= _menu_pp?0:envdescren.sky_r_textures_env[0].second->surface_get();
		ID3DBaseTexture*	e1	= _menu_pp?0:envdescren.sky_r_textures_env[1].second->surface_get();
		t_envmap_0->surface_set		(e0);	_RELEASE(e0);
		t_envmap_1->surface_set		(e1);	_RELEASE(e1);
	
		// Draw
		RCache.set_Element			(s_combine->E[0]	);
		//RCache.set_Geometry			(g_combine_VP		);
		RCache.set_Geometry			(g_combine		);

		RCache.set_c				("m_v2w",			m_v2w	);
		RCache.set_c				("L_ambient",		ambclr	);

		RCache.set_c				("Ldynamic_color",	sunclr	);
		RCache.set_c				("Ldynamic_dir",	sundir	);

		RCache.set_c				("env_color",		envclr	);
		RCache.set_c				("fog_color",		fogclr	);

		RCache.set_c("rain_drops_params0", envdesc.rain_density, droplets_power_debug, envdesc.far_plane, envdesc.fog_distance);
		RCache.set_c("wenv_params", envdesc.fog_density, envdesc.wind_velocity, envdesc.m_fSunShaftsIntensity, envdesc.wind_direction);
		RCache.set_c("control_params", ps_mask_control, ps_drops_control, ps_script_wetness, ps_sslr_control);
		RCache.set_c("pp_control_params", ps_postprocess_control, ps_chrom_abberation_control, ps_bloom_control, ps_dof_chrabb);
		RCache.set_c("sky_water_control_params", ps_sky_rainbow, ps_water_ripples, ps_water_mudfoam, ps_sslr_control);
		RCache.set_c("pp_control_params1", ps_metall_refls, ps_fake_tonemap, ps_vignette, ps_vignette_power);

		RCache.set_c("technicolor_params1", ps_techni_amount, ps_techni_power, ps_techni_red, ps_techni_green);
		RCache.set_c("technicolor_params2", ps_techni_blue, ps_luma_power, ps_luma_offset, 0);
		RCache.set_c("chrom_hdr_luma_params", ps_chrom_abb_factor, ps_hdr_factor, ps_luma_factor, ps_luma_pattern);
		RCache.set_c("dpx_params1", ps_dpx_red, ps_dpx_green, ps_dpx_blue, ps_dpx_color_gamma);
		RCache.set_c("dpx_params2", ps_dpx_saturation, ps_dpx_power, 0, 0);
		RCache.set_c("curves_sslr_params", ps_curves_formula, ps_curves_contrast, ps_sslr_samples, ps_chrom_abb_power);

		RCache.set_c				("ssao_noise_tile_factor",	fSSAONoise	);
		RCache.set_c				("ssao_kernel_size",		fSSAOKernelSize	);

      if( !RImplementation.o.dx10_msaa )
		   RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
      else
      {
         RCache.set_Stencil( TRUE, D3DCMP_EQUAL, 0x01, 0x81, 0 );
         RCache.Render		( D3DPT_TRIANGLELIST,Offset,0,4,0,2);
         if( RImplementation.o.dx10_msaa_opt )
         {
            RCache.set_Element( s_combine_msaa[0]->E[0]	);
            RCache.set_Stencil( TRUE, D3DCMP_EQUAL, 0x81, 0x81, 0 );
            RCache.Render		( D3DPT_TRIANGLELIST,Offset,0,4,0,2);
         }
         else
         {
            for( u32 i = 0; i < RImplementation.o.dx10_msaa_samples; ++i )
            {
               RCache.set_Element		   ( s_combine_msaa[i]->E[0]	);
               StateManager.SetSampleMask ( u32(1) << i  );
               RCache.set_Stencil         ( TRUE, D3DCMP_EQUAL, 0x81, 0x81, 0 );
               RCache.Render				   ( D3DPT_TRIANGLELIST,Offset,0,4,0,2);
            }
            StateManager.SetSampleMask( 0xffffffff );
         }
         RCache.set_Stencil( FALSE, D3DCMP_EQUAL, 0x01, 0xff, 0 );
      }  
   }

   //phase_combine_refl();

	if (!RImplementation.o.dx10_msaa)
		HW.pContext->CopyResource(rt_LF->pTexture->surface_get(), rt_Generic_0->pTexture->surface_get());
	else
		HW.pContext->CopyResource(rt_LF->pTexture->surface_get(), rt_Generic_0_r->pTexture->surface_get());

	// Forward rendering
	{
		PIX_EVENT(Forward_rendering);
		if( !RImplementation.o.dx10_msaa )
			u_setrt(rt_Generic_0, 0, 0, 0, HW.pBaseZB);		// LDR RT
		else
			u_setrt(rt_Generic_0_r, 0, 0, 0, RImplementation.Target->rt_MSAADepth->pZRT);		// LDR RT
		RCache.set_CullMode				(CULL_CCW);
		RCache.set_Stencil				(FALSE);
		RCache.set_ColorWriteEnable		();
		//	TODO: DX10: CHeck this!
		//g_pGamePersistent->Environment().RenderClouds	();
		RImplementation.render_forward	();
		if (g_pGamePersistent)	g_pGamePersistent->OnRenderPPUI_main()	;	// PP-UI
	}

	if (ps_r2_ls_flags.test(R3FLAG_DYN_WET_SURF)) {

		if (!RImplementation.o.dx10_msaa)
			u_setrt(rt_PPTemp, rt_LF, NULL, NULL, HW.pBaseZB);
		else
			u_setrt(rt_PPTemp, rt_LF, NULL, NULL, RImplementation.Target->rt_MSAADepth->pZRT);

		RCache.set_CullMode(CULL_NONE);
		RCache.set_Stencil(FALSE);

		{
			struct v_aa {
				Fvector4	p;
				Fvector2	uv0;
				Fvector2	uv1;
				Fvector2	uv2;
				Fvector2	uv3;
				Fvector2	uv4;
				Fvector4	uv5;
				Fvector4	uv6;
			};

			float _w, _h, ddw, ddh;

			_w = float(Device.dwWidth);
			_h = float(Device.dwHeight);

			ddw = 1.f / _w;
			ddh = 1.f / _h;

			p0.set(.5f / _w, .5f / _h);
			p1.set((_w + .5f) / _w, (_h + .5f) / _h);

			v_aa* pv;
			pv = (v_aa*)RCache.Vertex.Lock(4, g_aa_AA->vb_stride, Offset);
			pv->p.set(EPS, float(_h + EPS), EPS, 1.f); pv->uv0.set(p0.x, p1.y); pv->uv1.set(p0.x - ddw, p1.y - ddh); pv->uv2.set(p0.x + ddw, p1.y + ddh); pv->uv3.set(p0.x + ddw, p1.y - ddh); pv->uv4.set(p0.x - ddw, p1.y + ddh); pv->uv5.set(p0.x - ddw, p1.y, p1.y, p0.x + ddw); pv->uv6.set(p0.x, p1.y - ddh, p1.y + ddh, p0.x); pv++;
			pv->p.set(EPS, EPS, EPS, 1.f); pv->uv0.set(p0.x, p0.y); pv->uv1.set(p0.x - ddw, p0.y - ddh); pv->uv2.set(p0.x + ddw, p0.y + ddh); pv->uv3.set(p0.x + ddw, p0.y - ddh); pv->uv4.set(p0.x - ddw, p0.y + ddh); pv->uv5.set(p0.x - ddw, p0.y, p0.y, p0.x + ddw); pv->uv6.set(p0.x, p0.y - ddh, p0.y + ddh, p0.x); pv++;
			pv->p.set(float(_w + EPS), float(_h + EPS), EPS, 1.f); pv->uv0.set(p1.x, p1.y); pv->uv1.set(p1.x - ddw, p1.y - ddh); pv->uv2.set(p1.x + ddw, p1.y + ddh); pv->uv3.set(p1.x + ddw, p1.y - ddh); pv->uv4.set(p1.x - ddw, p1.y + ddh); pv->uv5.set(p1.x - ddw, p1.y, p1.y, p1.x + ddw); pv->uv6.set(p1.x, p1.y - ddh, p1.y + ddh, p1.x); pv++;
			pv->p.set(float(_w + EPS), EPS, EPS, 1.f); pv->uv0.set(p1.x, p0.y); pv->uv1.set(p1.x - ddw, p0.y - ddh); pv->uv2.set(p1.x + ddw, p0.y + ddh); pv->uv3.set(p1.x + ddw, p0.y - ddh); pv->uv4.set(p1.x - ddw, p0.y + ddh); pv->uv5.set(p1.x - ddw, p0.y, p0.y, p1.x + ddw); pv->uv6.set(p1.x, p0.y - ddh, p0.y + ddh, p1.x); pv++;
			RCache.Vertex.Unlock(4, g_aa_AA->vb_stride);
		}

		RCache.set_Element(s_SSLR->E[0]);

		CEnvDescriptorMixer& envdesc = *g_pGamePersistent->Environment().CurrentEnv;
		RCache.set_c("ssr_rain_drops_params0", envdesc.rain_density, droplets_power_debug, envdesc.far_plane, envdesc.fog_distance);
		RCache.set_c("ssr_control_params", ps_mask_control, ps_drops_control, ps_script_wetness, ps_sslr_control);

		RCache.set_Geometry(g_aa_AA);
		RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

		if (!RImplementation.o.dx10_msaa)
			u_setrt(rt_SSR, NULL, NULL, NULL, HW.pBaseZB);
		else
			u_setrt(rt_SSR, NULL, NULL, NULL, RImplementation.Target->rt_MSAADepth->pZRT);

		RCache.set_CullMode(CULL_NONE);
		RCache.set_Stencil(FALSE);

		RCache.set_c("ssr_rain_drops_params0", envdesc.rain_density, droplets_power_debug, envdesc.far_plane, envdesc.fog_distance);
		RCache.set_c("ssr_control_params", ps_mask_control, ps_drops_control, ps_script_wetness, ps_sslr_control);

		RCache.set_Element(s_SSLR->E[1]);
		RCache.set_Geometry(g_aa_AA);
		RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

		if (!RImplementation.o.dx10_msaa){
			HW.pContext->CopyResource(rt_Generic_0->pTexture->surface_get(), rt_SSR->pTexture->surface_get());
			HW.pContext->CopyResource(rt_LF->pTexture->surface_get(), rt_Generic_0->pTexture->surface_get());
		}
		else {
			HW.pContext->CopyResource(rt_Generic_0_r->pTexture->surface_get(), rt_SSR->pTexture->surface_get());
			HW.pContext->CopyResource(rt_LF->pTexture->surface_get(), rt_Generic_0_r->pTexture->surface_get());
		}


	}


	//	Igor: for volumetric lights
	//	combine light volume here
	if (m_bHasActiveVolumetric)
		phase_combine_volumetric();

	// Perform blooming filter and distortion if needed
	RCache.set_Stencil	(FALSE);

   if( RImplementation.o.dx10_msaa )
   {
      // we need to resolve rt_Generic_1 into rt_Generic_1_r
      HW.pContext->ResolveSubresource( rt_Generic_1->pTexture->surface_get(), 0, rt_Generic_1_r->pTexture->surface_get(), 0, DXGI_FORMAT_R8G8B8A8_UNORM );
      HW.pContext->ResolveSubresource( rt_Generic_0->pTexture->surface_get(), 0, rt_Generic_0_r->pTexture->surface_get(), 0, DXGI_FORMAT_R8G8B8A8_UNORM );
   }

   // for msaa we need a resolved color buffer - Holger
	phase_bloom			( );												// HDR RT invalidated here

	//RImplementation.rmNormal();
	//u_setrt(rt_Generic_1,0,0,HW.pBaseZB);

	// Distortion filter
	BOOL	bDistort	= RImplementation.o.distortion_enabled;				// This can be modified
	{
		if ((0==RImplementation.mapDistort.size()) && !_menu_pp)		
			bDistort= FALSE;
		if (bDistort)		
		{
			PIX_EVENT(render_distort_objects);
			FLOAT ColorRGBA[4] = { 127.0f/255.0f, 127.0f/255.0f, 0.0f, 127.0f/255.0f};
			if( !RImplementation.o.dx10_msaa )
			{
				u_setrt(rt_Generic_1, 0, 0, 0, HW.pBaseZB);		// Now RT is a distortion mask
				HW.pContext->ClearRenderTargetView( rt_Generic_1->pRT, ColorRGBA);
			}
			else {
				u_setrt(rt_Generic_1_r, 0, 0, 0, RImplementation.Target->rt_MSAADepth->pZRT);		// Now RT is a distortion mask
				HW.pContext->ClearRenderTargetView( rt_Generic_1_r->pRT, ColorRGBA);
			}
			RCache.set_CullMode			(CULL_CCW);
			RCache.set_Stencil			(FALSE);
			RCache.set_ColorWriteEnable	();
			//CHK_DX(HW.pDevice->Clear	( 0L, NULL, D3DCLEAR_TARGET, color_rgba(127,127,0,127), 1.0f, 0L));
			RImplementation.r_dsgraph_render_distort	();
			if (g_pGamePersistent)	g_pGamePersistent->OnRenderPPUI_PP()	;	// PP-UI
		}
	}

    RCache.set_Stencil(FALSE);
  

    //FXAA
    if (ps_r2_fxaa){
        PIX_EVENT(FXAA);
        phase_fxaa();
        RCache.set_Stencil(FALSE);
    }   
  
    // PP enabled ?
    //	Render to RT texture to be able to copy RT even in windowed mode.
    //BOOL	PP_Complex = u_need_PP() | (BOOL)RImplementation.m_bMakeAsyncSS;
    //#GIPERION: WTF IS HOLGER HACK?!!!
    //HOLGER HACK
    BOOL	PP_Complex = TRUE;

	if (!_menu_pp)
	{
        if (RImplementation.o.sunshaft_screenspace && ps_r_sun_shafts > 0)
            phase_ogse_sunshafts();
        else if (RImplementation.o.sunshaft_mrmnwar && ps_r_sun_shafts > 0)
            phase_SunShafts();
		
        //phase_rain_drops();
   }
   
	// Combine everything + perform AA
   if( RImplementation.o.dx10_msaa )
   {
	   if (PP_Complex)	u_setrt(rt_Generic, 0, 0, 0, HW.pBaseZB);			// LDR RT
	   else					   u_setrt		( Device.dwWidth,Device.dwHeight,HW.pBaseRT,NULL,NULL,HW.pBaseZB);
   }
   else
   {
	   if (PP_Complex)	u_setrt(rt_Color, 0, 0, 0, HW.pBaseZB);			// LDR RT
      else					   u_setrt		( Device.dwWidth,Device.dwHeight,HW.pBaseRT,NULL,NULL,HW.pBaseZB);
   }
	//. u_setrt				( Device.dwWidth,Device.dwHeight,HW.pBaseRT,NULL,NULL,HW.pBaseZB);
	RCache.set_CullMode		( CULL_NONE )	;
	RCache.set_Stencil		( FALSE		)	;


	if (1)
	{
		PIX_EVENT(combine_2);
		// 

		{
			struct v_aa	{
				Fvector4	p;
				Fvector2	uv0;
				Fvector2	uv1;
				Fvector2	uv2;
				Fvector2	uv3;
				Fvector2	uv4;
				Fvector4	uv5;
				Fvector4	uv6;
			};

			float	_w = float(Device.dwWidth);
			float	_h = float(Device.dwHeight);
			float	ddw = 1.f / _w;
			float	ddh = 1.f / _h;
			p0.set(.5f / _w, .5f / _h);
			p1.set((_w + .5f) / _w, (_h + .5f) / _h);

			// Fill vertex buffer
			v_aa* pv = (v_aa*)RCache.Vertex.Lock(4, g_aa_AA->vb_stride, Offset);
			pv->p.set(EPS, float(_h + EPS), EPS, 1.f); pv->uv0.set(p0.x, p1.y); pv->uv1.set(p0.x - ddw, p1.y - ddh); pv->uv2.set(p0.x + ddw, p1.y + ddh); pv->uv3.set(p0.x + ddw, p1.y - ddh); pv->uv4.set(p0.x - ddw, p1.y + ddh); pv->uv5.set(p0.x - ddw, p1.y, p1.y, p0.x + ddw); pv->uv6.set(p0.x, p1.y - ddh, p1.y + ddh, p0.x); pv++;
			pv->p.set(EPS, EPS, EPS, 1.f); pv->uv0.set(p0.x, p0.y); pv->uv1.set(p0.x - ddw, p0.y - ddh); pv->uv2.set(p0.x + ddw, p0.y + ddh); pv->uv3.set(p0.x + ddw, p0.y - ddh); pv->uv4.set(p0.x - ddw, p0.y + ddh); pv->uv5.set(p0.x - ddw, p0.y, p0.y, p0.x + ddw); pv->uv6.set(p0.x, p0.y - ddh, p0.y + ddh, p0.x); pv++;
			pv->p.set(float(_w + EPS), float(_h + EPS), EPS, 1.f); pv->uv0.set(p1.x, p1.y); pv->uv1.set(p1.x - ddw, p1.y - ddh); pv->uv2.set(p1.x + ddw, p1.y + ddh); pv->uv3.set(p1.x + ddw, p1.y - ddh); pv->uv4.set(p1.x - ddw, p1.y + ddh); pv->uv5.set(p1.x - ddw, p1.y, p1.y, p1.x + ddw); pv->uv6.set(p1.x, p1.y - ddh, p1.y + ddh, p1.x); pv++;
			pv->p.set(float(_w + EPS), EPS, EPS, 1.f); pv->uv0.set(p1.x, p0.y); pv->uv1.set(p1.x - ddw, p0.y - ddh); pv->uv2.set(p1.x + ddw, p0.y + ddh); pv->uv3.set(p1.x + ddw, p0.y - ddh); pv->uv4.set(p1.x - ddw, p0.y + ddh); pv->uv5.set(p1.x - ddw, p0.y, p0.y, p1.x + ddw); pv->uv6.set(p1.x, p0.y - ddh, p0.y + ddh, p1.x); pv++;
			RCache.Vertex.Unlock(4, g_aa_AA->vb_stride);

		}

		//	Set up variable
		Fvector2	vDofKernel;
		vDofKernel.set(0.5f/Device.dwWidth, 0.5f/Device.dwHeight);
		vDofKernel.mul(ps_r2_dof_kernel_size);

		// Draw COLOR
      if( !RImplementation.o.dx10_msaa )
      {
		   if (ps_r2_ls_flags.test(R2FLAG_AA))			RCache.set_Element	(s_combine->E[bDistort?3:1]);	// look at blender_combine.cpp
		   else										RCache.set_Element	(s_combine->E[bDistort?4:2]);	// look at blender_combine.cpp
      }
      else
      {
         if (ps_r2_ls_flags.test(R2FLAG_AA))			RCache.set_Element	(s_combine_msaa[0]->E[bDistort?3:1]);	// look at blender_combine.cpp
         else										RCache.set_Element	(s_combine_msaa[0]->E[bDistort?4:2]);	// look at blender_combine.cpp
      }
		RCache.set_c				("e_barrier",	ps_r2_aa_barier.x,	ps_r2_aa_barier.y,	ps_r2_aa_barier.z,	0);
		RCache.set_c				("e_weights",	ps_r2_aa_weight.x,	ps_r2_aa_weight.y,	ps_r2_aa_weight.z,	0);
		RCache.set_c				("e_kernel",	ps_r2_aa_kernel,	ps_r2_aa_kernel,	ps_r2_aa_kernel,	0);
		RCache.set_c				("m_current",	m_current);
		RCache.set_c				("m_previous",	m_previous);
		RCache.set_c				("m_blur",		m_blur_scale.x,m_blur_scale.y, 0,0);
		Fvector3					dof;
		g_pGamePersistent->GetCurrentDof(dof);
		RCache.set_c				("dof_params",	dof.x, dof.y, dof.z, ps_r2_dof_sky);
//.		RCache.set_c				("dof_params",	ps_r2_dof.x, ps_r2_dof.y, ps_r2_dof.z, ps_r2_dof_sky);
		RCache.set_c				("dof_kernel",	vDofKernel.x, vDofKernel.y, ps_r2_dof_kernel_size, 0);

		CEnvDescriptorMixer& envdesc = *g_pGamePersistent->Environment().CurrentEnv;
		RCache.set_c("rain_drops_params0", envdesc.rain_density, droplets_power_debug, envdesc.far_plane, envdesc.fog_distance);
		RCache.set_c("wenv_params", envdesc.fog_density, envdesc.wind_velocity, envdesc.m_fSunShaftsIntensity, envdesc.wind_direction);
		RCache.set_c("control_params", ps_mask_control, ps_drops_control, ps_script_wetness, ps_sslr_control);
		RCache.set_c("pp_control_params", ps_postprocess_control, ps_chrom_abberation_control, ps_bloom_control, ps_dof_chrabb);
		RCache.set_c("sky_water_control_params", ps_sky_rainbow, ps_water_ripples, ps_water_mudfoam, ps_sslr_control);
		RCache.set_c("pp_control_params1", ps_metall_refls, ps_fake_tonemap, ps_vignette, ps_vignette_power);

		RCache.set_c("technicolor_params1", ps_techni_amount, ps_techni_power, ps_techni_red, ps_techni_green);
		RCache.set_c("technicolor_params2", ps_techni_blue, ps_luma_power, ps_luma_offset, 0);
		RCache.set_c("chrom_hdr_luma_params", ps_chrom_abb_factor, ps_hdr_factor, ps_luma_factor, ps_luma_pattern);
		RCache.set_c("dpx_params1", ps_dpx_red, ps_dpx_green, ps_dpx_blue, ps_dpx_color_gamma);
		RCache.set_c("dpx_params2", ps_dpx_saturation, ps_dpx_power, 0, 0);
		RCache.set_c("curves_sslr_params", ps_curves_formula, ps_curves_contrast, ps_sslr_samples, ps_chrom_abb_power);
		
		RCache.set_Geometry			(g_aa_AA);
		RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
	}
	RCache.set_Stencil		(FALSE);

	//	if FP16-BLEND !not! supported - draw flares here, overwise they are already in the bloom target
	/* if (!RImplementation.o.fp16_blend)*/	g_pGamePersistent->Environment().RenderFlares	();	// lens-flares

	//	PP-if required
	if (PP_Complex)		
	{
		PIX_EVENT(phase_pp);
		phase_pp		();
	}

	//	Re-adapt luminance
	RCache.set_Stencil		(FALSE);

	//*** exposure-pipeline-clear
	{
		std::swap					(rt_LUM_pool[gpu_id*2+0],rt_LUM_pool[gpu_id*2+1]);
		t_LUM_src->surface_set		(NULL);
		t_LUM_dest->surface_set		(NULL);
	}

#ifdef DEBUG
	RCache.set_CullMode	( CULL_CCW );
	static	xr_vector<Fplane>		saved_dbg_planes;
	if (bDebug)		saved_dbg_planes= dbg_planes;
	else			dbg_planes		= saved_dbg_planes;
	if (1) for (u32 it=0; it<dbg_planes.size(); it++)
	{
		Fplane&		P	=	dbg_planes[it];
		Fvector		zero	;
		zero.mul	(P.n,P.d);
		
		Fvector             L_dir,L_up=P.n,L_right;
		L_dir.set           (0,0,1);                if (_abs(L_up.dotproduct(L_dir))>.99f)  L_dir.set(1,0,0);
		L_right.crossproduct(L_up,L_dir);           L_right.normalize       ();
		L_dir.crossproduct  (L_right,L_up);         L_dir.normalize         ();

		Fvector				p0,p1,p2,p3;
		float				sz	= 100.f;
		p0.mad				(zero,L_right,sz).mad	(L_dir,sz);
		p1.mad				(zero,L_right,sz).mad	(L_dir,-sz);
		p2.mad				(zero,L_right,-sz).mad	(L_dir,-sz);
		p3.mad				(zero,L_right,-sz).mad	(L_dir,+sz);
		RCache.dbg_DrawTRI	(Fidentity,p0,p1,p2,0xffffffff);
		RCache.dbg_DrawTRI	(Fidentity,p2,p3,p0,0xffffffff);
	}

	static	xr_vector<dbg_line_t>	saved_dbg_lines;
	if (bDebug)		saved_dbg_lines	= dbg_lines;
	else			dbg_lines		= saved_dbg_lines;
	if (1) for (u32 it=0; it<dbg_lines.size(); it++)
	{
		RCache.dbg_DrawLINE		(Fidentity,dbg_lines[it].P0,dbg_lines[it].P1,dbg_lines[it].color);
	}
#endif

#ifdef DEBUG
	dbg_spheres.clear	();
	dbg_lines.clear		();
	dbg_planes.clear	();
#endif
}

void CRenderTarget::phase_wallmarks		()
{
	// Targets
	RCache.set_RT(NULL,2);
	RCache.set_RT(NULL,1);
   if( !RImplementation.o.dx10_msaa )
	   u_setrt(rt_Color, NULL, NULL, NULL, HW.pBaseZB);
   else
	   u_setrt(rt_Color, NULL, NULL, NULL, rt_MSAADepth->pZRT);
	// Stencil	- draw only where stencil >= 0x1
	RCache.set_Stencil					(TRUE,D3DCMP_LESSEQUAL,0x01,0xff,0x00);
	RCache.set_CullMode					(CULL_CCW);
	RCache.set_ColorWriteEnable			(D3DCOLORWRITEENABLE_RED|D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_BLUE);
}

void CRenderTarget::phase_combine_volumetric()
{
	PIX_EVENT(phase_combine_volumetric);
	u32			Offset					= 0;
	//Fvector2	p0,p1;

	//	TODO: DX10: Remove half pixel offset here

	//u_setrt(rt_Generic_0,0,0,HW.pBaseZB );			// LDR RT
	if( !RImplementation.o.dx10_msaa )
		u_setrt(rt_Generic_0,rt_Generic_1,0,0,HW.pBaseZB );
	else
		u_setrt(rt_Generic_0_r,rt_Generic_1_r,0,0,RImplementation.Target->rt_MSAADepth->pZRT );
	//	Sets limits to both render targets
	RCache.set_ColorWriteEnable(D3DCOLORWRITEENABLE_RED|D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_BLUE);
	{
		// Fill VB
		float	scale_X				= float(Device.dwWidth)	/ float(TEX_jitter);
		float	scale_Y				= float(Device.dwHeight)/ float(TEX_jitter);

		// Fill vertex buffer
		FVF::TL* pv					= (FVF::TL*)	RCache.Vertex.Lock	(4,g_combine->vb_stride,Offset);
		pv->set						(-1,	1,	0, 1, 0, 0,			scale_Y	);	pv++;
		pv->set						(-1,	-1,	0, 0, 0, 0,			0		);	pv++;
		pv->set						(1,		1,	1, 1, 0, scale_X,	scale_Y	);	pv++;
		pv->set						(1,		-1,	1, 0, 0, scale_X,	0		);	pv++;
		RCache.Vertex.Unlock		(4,g_combine->vb_stride);

		// Draw
		RCache.set_Element			(s_combine_volumetric->E[0]	);
		RCache.set_Geometry			(g_combine		);
		RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
	}
	RCache.set_ColorWriteEnable();
}
