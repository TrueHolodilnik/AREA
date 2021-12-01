local tex_nmap                	= "water\\pure_waters_4"


--[[
function normal_hq(shader, t_base, t_second, t_detail)
  shader:begin    ("model_env_hq","model_env_hq")
      : fog       (true)
      : zb        (true,false)
      : blend     (true,blend.srcalpha,blend.invsrcalpha)
      : aref      (true,0)
      : sorting   (3,true)
  shader:sampler  ("s_base")       :texture    (t_base)
  shader:sampler  ("s_env")        :texture    ("sky\\sky_5_cube") : clamp()
  shader:sampler  ("s_lmap")       :texture  ("$user$projector")
    : clamp    ()
    : f_linear   ()
    : project     (true)
end
]]

function normal   (shader, t_base, t_second, t_detail)
  shader:begin    ("model_env_lq","model_env_lq")
      : fog       (true)
      : zb        (true,false)
      : blend     (true,blend.srcalpha,blend.invsrcalpha)
      : aref      (true,0)
      : sorting   (3,true)
--  shader:sampler  ("s_base")       :texture    (t_base)
--  shader:sampler  ("s_env")        :texture    ("sky\\sky_5_cube") : clamp()
	shader: dx10texture ("s_base", t_base)
	shader:dx10texture	("s_nmap",		tex_nmap)
	shader:	dx10texture ("s_env", "$user$sky0")
	shader:dx10texture	("s_pptemp", "$user$genericlf")

	shader:dx10texture	("s_position",	"$user$position")
	shader: dx10sampler ("smp_base")
	shader:dx10sampler	("smp_nofilter")
	shader: dx10sampler ("smp_rtlinear")
	shader:dx10sampler	("smp_linear")
end

function l_special  (shader, t_base, t_second, t_detail)
  shader:begin  ("model_distort4glass","particle_distort")
			:sorting        (2, true)
      : blend     (true,blend.srcalpha,blend.invsrcalpha)
      : zb        (true,false)
      : fog       (false)
      : distort   (true)
	shader: dx10color_write_enable( true, true, true, false)
--  shader:sampler  ("s_base")      :texture  (t_base)
--  shader:sampler  ("s_distort")   :texture  ("pfx\\pfx_dist_glass")
	shader: dx10texture ("s_base", t_base)
	shader:dx10texture	("s_position",	"$user$position")
	shader: dx10texture ("s_distort", "pfx\\pfx_dist_glass6") --:texture  (t_base) -- ("pfx\\pfx_dist_glass2")
	shader:dx10sampler	("smp_base")
	shader: dx10sampler ("smp_linear")
	shader:dx10sampler	("smp_nofilter")
end
