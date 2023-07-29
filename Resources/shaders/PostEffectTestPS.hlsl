#include "PostEffectTest.hlsli"

Texture2D<float4> tex0: register(t0);
Texture2D<float4> tex1 : register(t1);

SamplerState smp : register(s0);

float4 main(VSOutput input) : SV_TARGET
{
	//float4 texcolor=tex.Sample(smp,input.uv);

	//uv���炵
	//float2 uv=(0,0);
	float4 colortex0=tex0.Sample(smp,input.uv);
	float4 colortex1=tex1.Sample(smp,input.uv);

	

	//���x�̕ύX
	//return float4(texcolor.rgb*2.0f,1);

	//�F���]
	colortex0=(1-tex0.Sample(smp,input.uv));


	//���ςڂ���
	float u=(input.uv.x+1/(1280/2));
	float v=(input.uv.y+1/(720/2));
	float4 blur={0,0,0,0};

	////����
	//blur+=tex1.Sample(smp,input.uv+float2(-u,-v));
	////����
	//blur+=tex1.Sample(smp,input.uv+float2(0,-v));
	////�E��s�N�Z��
	//blur+=tex1.Sample(smp,input.uv+float2(+u,-v));
	////����
	//blur+=tex1.Sample(smp,input.uv+float2(-u,0));
	////����
	//blur+=tex1.Sample(smp,input.uv+float2(0,0));
	////�E��
	//blur+=tex1.Sample(smp,input.uv+float2(+u,0));
	////����
	//blur+=tex1.Sample(smp,input.uv+float2(-u,+v));
	////����
	//blur+=tex1.Sample(smp,input.uv+float2(0,+v));
	////�E��
	//blur+=tex1.Sample(smp,input.uv+float2(+u,+v));



	//const int vertical=5:
	//const int beside=5;

	//�c�ړ�
	for(int i=0;i<11;i++){
	//���ړ�
		for(int j=0;j<11;j++){
			blur+=tex1.Sample(smp,input.uv)+tex1.Sample(smp,float2(u+j,v+i));
		}
	}
	colortex1=blur/(11*11);

	////�}���`�e�N�X�`��
	//float4 color=colortex0;
	//if(fmod(input.uv.y,0.1)<0.05f){
	//	color =colortex1;
	//}

	return float4(colortex1.rgb,1);

}
