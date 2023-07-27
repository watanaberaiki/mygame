#include "PostEffectTest.hlsli"

Texture2D<float4> tex0: register(t0);
Texture2D<float4> tex1 : register(t1);

SamplerState smp : register(s0);

float4 main(VSOutput input) : SV_TARGET
{
	//float4 texcolor=tex.Sample(smp,input.uv);

	//uvずらし
	//float2 uv=(0,0);
	float4 colortex0=tex0.Sample(smp,input.uv);
	float4 colortex1=tex1.Sample(smp,input.uv);

	

	//明度の変更
	//return float4(texcolor.rgb*2.0f,1);

	//色反転
	//colortex0=(1-tex0.Sample(smp,input.uv).xyz,1);


	////平均ぼかし
	//float2 uv={input.uv.x+1280/1,input.uv.y+720/1};
	////float v=(input.uv.y+720/1);
	//float4 blur={0,0,0,0};
	////縦移動
	//for(int i=0;i<1280;i++){
	////横移動
	//	for(int i=0;i<720;i++){
	//		blur=tex1.Sample(smp,uv);
	//	}
	//}
	//colortex1=blur/(10*10);
	//マルチテクスチャ
	float4 color=colortex0;
	if(fmod(input.uv.y,0.1)<0.05f){
		color =colortex1;
	}

	return float4(color.rgb,1);

}
