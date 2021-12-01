local tex_base                = "water\\water_listya"
local tex_base_s                = "water\\water_water_r1"
local tex_nmap                	= "water\\pure_waters_4"
--local tex_nmap                = "water\\water_normal"
local tex_nmap2                	= "water\\water_normal_12345"
local tex_dist                = "water\\water_dudv"
local tex_env0                = "$user$sky0"         -- "sky\\sky_8_cube"
local tex_env1                = "$user$sky1"         -- "sky\\sky_8_cube"

--local tex_leaves              = "decal\\decal_listja"
--local tex_leaves              = "decal\\decal_listja_vetki"
local tex_leaves              = "water\\water_foam"
local tex_dudv                	= "water\\pure_waters_4_dudv"
local tex_env0                	= "$user$sky0"      
local tex_env1                	= "$user$sky1"        
local tex_mud                = "water\\water_studen_green_t"
local tex_leaves_nmap              = "water\\water_ryaska1_t"

function normal                (shader, t_base, t_second, t_detail)
	shader	:begin		("water_soft","water_soft")
    		:sorting	(2, false)
			:blend		(true,blend.srcalpha,blend.invsrcalpha)
			:zb			(true,false)
			:distort	(false)
			:fog		(true)


	shader:dx10texture	("s_base",		tex_base)
	shader:dx10texture	("s_base_static",	tex_base_s)
	shader:dx10texture	("s_nmap",		tex_nmap)
	shader:dx10texture	("s_nmap_leaves",		tex_leaves_nmap)
	shader:dx10texture	("s_nmap_blurred", tex_nmap2)
	shader:dx10texture	("s_env0",		tex_env0)
	shader:dx10texture	("s_env1",		tex_env1)
	shader:dx10texture	("sky_s0",		tex_env0)
	shader:dx10texture	("sky_s1",		tex_env1)
	shader:dx10texture	("s_position",	"$user$position")
	shader:dx10texture	("s_dudv_water_map", tex_dudv)

	shader:dx10texture	("s_nmap_noise", tex_nmap2)
	shader:dx10texture	("s_leaves",	tex_leaves)
	shader:dx10texture	("s_mud",	tex_mud)

	shader:dx10texture	("s_pptemp", "$user$genericlf")
	shader:dx10sampler	("smp_base")
	shader:dx10sampler	("smp_nofilter")
	shader:dx10sampler	("smp_rtlinear")
	shader:dx10sampler	("smp_linear")
end

function l_special        (shader, t_base, t_second, t_detail)
	shader	:begin                ("waterd_soft","waterd_soft")
			:sorting        (2, true)
			:blend                (true,blend.srcalpha,blend.invsrcalpha)
			:zb                (true,false)
			:fog                (false)
			:distort        (false)

	shader: dx10color_write_enable( true, true, true, false)

--  shader:sampler        ("s_base")       :texture  (tex_base)
--  shader:sampler        ("s_distort")    :texture  (tex_dist)
--  shader:sampler        ("s_position")       :texture  ("$user$position")

	shader:dx10texture	("s_base",		tex_base)
	shader:dx10texture	("s_distort",	tex_dist)
	shader:dx10texture	("s_position",	"$user$position")

	shader:dx10sampler	("smp_base")
	shader:dx10sampler	("smp_nofilter")	
end