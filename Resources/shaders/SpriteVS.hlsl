#include "Sprite.hlsli"

//VSOutput main(float4 pos : POSITION,float3 normal : NORMAL, float2 uv : TEXCOORD)
//{
//	VSOutput output;//�s�N�Z���V�F�[�_�ɓn���l
//	//output.svpos = pos;
//	output.svpos = mul(mat,pos);//���W�ɍs�P���v�Z
//	output.normal = normal;
//	output.uv = uv;
//
//	return output;
//}

VSOutput main(float4 pos : POSITION, float2 uv : TEXCOORD)
{
	VSOutput output;//�s�N�Z���V�F�[�_�ɓn���l
	/*output.svpos = pos;*/
	output.svpos = mul(mat, pos);//���W�ɍs�P���v�Z
	output.uv = uv;
	return output;
}