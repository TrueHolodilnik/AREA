
function normal		(shader, t_base, t_second, t_detail)
	shader	: begin	("effects_rain","effects_rain")
			: zb	(true,false)
			: blend	(true,blend.srcalpha,blend.invsrcalpha)
--	TODO: DX10: implement aref for this shader
			: aref 		(true,0)
			: fog       (true)

	shader: dx10texture ("s_base", t_base)
	shader: dx10texture ("s_distort", "effects\\rain_distort")
	shader: dx10texture	("s_nmap", "water\\water_normal_empty")
	shader:dx10texture	("s_frame",	"$user$genericlf")

	shader: dx10sampler ("smp_base")
	shader: dx10sampler ("smp_rtlinear")
end