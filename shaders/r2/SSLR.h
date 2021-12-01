#include "common.h"
#include "gauss.h"

half3 SSLR(half3 vreflect, half2 tc, half3 norm, half3 v2point)
{
	float3 pos = getWpos(tc);
	
	float fresnel = 0.0 + saturate(pow(1 - dot(-1 * v2point, norm),1));
	
	float2 refl_tc = float2(0.f,0.f);
	float L = SSLR_params.x;
	float sample_depth = 0.f;
	float depth = 0.f;
	for (int i = 0; i < 10; i++)
	{
		float3 Npos = pos.xyz + vreflect*L;
		
		float4 Npos_proj = mul(m_VP,float4(Npos,1.f));
		sample_depth = Npos_proj.z;
		Npos_proj.xyz /= Npos_proj.w;
		float2 sample_tc = float2(0.f,0.f);
		sample_tc.x = (Npos_proj.x + 1.f) * .5f;
		sample_tc.y = 1.f - ((Npos_proj.y + 1.f) * .5f);
		
		float3 Hpos = getWpos(sample_tc);
		depth = Hpos.z;
		L = length(Hpos-pos);
		refl_tc = sample_tc;
	}
	
	float error = abs(depth - sample_depth) < SSLR_params.y ? 1 : 0;
	//refl_tc = get_6x6_image(refl_tc);
	float4 final = Gauss(s_image, refl_tc, error - 1, true);
	final *= fresnel;
	final.a = 1;
//	img *= error;
	
	return sat(final, 0.75);
}
/*
half3 SSLR(half3 refl_vec, half2 tc)
{	
	half3 pos = getWpos(tc);
	half2 refl_tc = half2(0.f,0.f);//текстурные координаты отражённой геометрии
	half3 Hpos = getWpos(refl_tc);
	half L = SSLR_params.x;// начальная длина луча
  
	for(int i = 0; i < 10; i++)// как показали тесты 6 проходов вполне достаточно для получения приемлимого результата
	{
		half3 new_pos = pos.xyz + refl_vec*L; // получаем новую позицию
  
		half4 new_pos_proj = mul(m_VP,half4(new_pos,1));//переводим её в скрин спейс
  		new_pos_proj.xyz /= new_pos_proj.w; //нормализуем
  
  		half2 sample_tc = half2(0,0);// создаём новую переменную в теле цикла, куда будем толкать новые текстурные координаты
	  //мы получаем позицию пикселя в диапазоне от -1 до 1, это дело нам как то нужно перевести в диапазон от 0 до 1
	  //вот так
	  	sample_tc.x = (new_pos_proj.x+1)*0.5;
	  	sample_tc.y = 1-((new_pos_proj.y+1)*0.5);
  
	  	half3 hit_pos = getWpos(sample_tc);//каждый проход цикла мы будем получать позицию, но уже с новыми текстурными координатами
	  	L=length(Hpos-pos);
  	
	  	refl_tc=sample_tc;//отправим полученные текстурные координаты на выход
	}  
	half3 refl_img=tex2D(s_image,refl_tc);
	return refl_img;	  
} */