function l_special  (shader, t_base, t_second, t_detail)
  shader:begin  ("model_distort4glass","particle_distort")
      : sorting   (3, true)
      : blend     (true,blend.srcalpha,blend.invsrcalpha)
      : zb        (true,false)
      : fog       (false)
      : distort   (true)
--  shader:sampler  ("s_base")      :texture  (t_base)
--  shader:sampler  ("s_distort")   :texture  ("pfx\\pfx_dist_glass")
	shader: dx10texture ("s_base", t_base)
	shader: dx10texture ("s_distort", "pfx\\pfx_dist_glass") --:texture  (t_base) -- ("pfx\\pfx_dist_glass2")
	shader: dx10sampler ("smp_linear")
end

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
	shader:	dx10texture ("s_env", "sky\\sky_5_cube")

	shader: dx10sampler ("smp_base")
	shader: dx10sampler ("smp_rtlinear")
end