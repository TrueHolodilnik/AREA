function normal		(shader, t_base, t_second, t_detail)
  shader:begin  	("model_def_lplanes","base_lplanes")
      : fog    		(false)
      : zb     		(true,false)
      : blend   	(true,blend.srcalpha,blend.one)
      : aref    	(true,0)
      : sorting		(2, true)

	shader:dx10texture	("s_base",	t_base)
	shader:dx10texture  ("s_position"	,"$user$position")
	
	shader:dx10sampler	("smp_base")
	shader:dx10sampler	("smp_nofilter")
end
