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
	half2 refl_tc = half2(0.f,0.f);//���������� ���������� ��������� ���������
	half3 Hpos = getWpos(refl_tc);
	half L = SSLR_params.x;// ��������� ����� ����
  
	for(int i = 0; i < 10; i++)// ��� �������� ����� 6 �������� ������ ���������� ��� ��������� ����������� ����������
	{
		half3 new_pos = pos.xyz + refl_vec*L; // �������� ����� �������
  
		half4 new_pos_proj = mul(m_VP,half4(new_pos,1));//��������� � � ����� �����
  		new_pos_proj.xyz /= new_pos_proj.w; //�����������
  
  		half2 sample_tc = half2(0,0);// ������ ����� ���������� � ���� �����, ���� ����� ������� ����� ���������� ����������
	  //�� �������� ������� ������� � ��������� �� -1 �� 1, ��� ���� ��� ��� �� ����� ��������� � �������� �� 0 �� 1
	  //��� ���
	  	sample_tc.x = (new_pos_proj.x+1)*0.5;
	  	sample_tc.y = 1-((new_pos_proj.y+1)*0.5);
  
	  	half3 hit_pos = getWpos(sample_tc);//������ ������ ����� �� ����� �������� �������, �� ��� � ������ ����������� ������������
	  	L=length(Hpos-pos);
  	
	  	refl_tc=sample_tc;//�������� ���������� ���������� ���������� �� �����
	}  
	half3 refl_img=tex2D(s_image,refl_tc);
	return refl_img;	  
} */