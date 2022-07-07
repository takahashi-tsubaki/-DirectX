#include "Basic.hlsli"

Texture2D<float4> tex : register(t0);	//0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
SamplerState smp : register(s0);		//0�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[

float4 main(VSOutput input) : SV_TARGET
{

	//return float4(tex.Sample(smp,input.uv))*color;
	//return float4(input.normal,1);//RGB�����ꂼ��@����xyz�AA��1�ŏo��
	//return float4 (1,1,1,1);

	////���ʔ��ˌ�
	//float3 light = normalize(float3(1,-1,1));//�E�����@�����̃��C�g
	//float brightness = dot(-light, input.normal);//�����ւ̃x�N�g���Ɩ@���x�N�g���̓���
	//return float4(brightness, brightness, brightness, 1);//�P�x��RGB�ɑ�����ďo��

	//����
	float3 light = normalize(float3(1,-1,1));//�E�����@�����̃��C�g
	float diffuse = saturate(dot(-light, input.normal));//diffuse��[0,1]�͈̔͂�clamp����
	float brightness = diffuse + 0.3f;//�A���r�G���g����0.3�Ƃ��Čv�Z
	//return float4(brightness, brightness, brightness, 1);//�P�x��RGB�ɑ�����ďo��

	float4 texColor = float4(tex.Sample(smp, input.uv));
	return float4(texColor.rgb * brightness, texColor.a) * color;
}