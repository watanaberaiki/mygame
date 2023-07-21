#include "PostEffectTest.hlsli"

Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);

float4 main(VSOutput input) : SV_TARGET
{
	//float4 texcolor=tex.Sample(smp,input.uv);

	//uvずらし
	float2 uv=(0,0);
	float4 texcolor=tex.Sample(smp,input.uv+uv);


	//明度の変更
	//return float4(texcolor.rgb*2.0f,1);

	////色反転
	//float4 color=(255,255,255,1);
	//return float4(color.rgb-texcolor.rgb,1);

	return float4(texcolor.rgb,1);

}
