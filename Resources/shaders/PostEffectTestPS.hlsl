#include "PostEffectTest.hlsli"

Texture2D<float4> tex0 : register(t0);
Texture2D<float4> tex1 : register(t1);

SamplerState smp : register(s0);

float4 main(VSOutput input) : SV_TARGET
{
	//float4 texcolor=tex.Sample(smp,input.uv);

	//uvずらし
	//float2 uv=(0,0);
    float4 colortex0 = tex0.Sample(smp, input.uv);
    float4 colortex1 = tex1.Sample(smp, input.uv);

	

	//明度の変更
	//return float4(texcolor.rgb*2.0f,1);

	//色反転
    //colortex0 = (1 - tex0.Sample(smp, input.uv));


	//平均ぼかし
    float u =1 / 1280;
    float v =1 / 720;
    float4 blur = { 0, 0, 0, 0 };

	//縦移動
    for (int i = 0; i < 15; ++i)
    {
		//横移動
        for (int j = 0; j < 15; ++j)
        {
            float2 a = float2(j, i) * float2(u, v);
            blur += tex1.Sample(smp, input.uv + a);
        }
    }
    colortex1 = blur;
    colortex1 /= (15 * 15);
	
	////左上
 //   blur += tex1.Sample(smp, input.uv + float2(-u, -v));
	////中上
 //   blur += tex1.Sample(smp, input.uv + float2(0, -v));
	////右上ピクセル
 //   blur += tex1.Sample(smp, input.uv + float2(+u, -v));
	////左中
 //   blur += tex1.Sample(smp, input.uv + float2(-u, 0));
	////中中
 //   blur += tex1.Sample(smp, input.uv + float2(0, 0));
	////右中
 //   blur += tex1.Sample(smp, input.uv + float2(+u, 0));
	////左下
 //   blur += tex1.Sample(smp, input.uv + float2(-u, +v));
	////中下
 //   blur += tex1.Sample(smp, input.uv + float2(0, +v));
	////右下
 //   blur += tex1.Sample(smp, input.uv + float2(+u, +v));

 //   colortex1 = blur;
 //   colortex1 /= 9;

	//マルチテクスチャ
    float4 color = colortex0;
    if (fmod(input.uv.y, 0.1) < 0.05f)
    {
        color = float4(1,1,1,1)-colortex1;
    }

    return float4(color.rgb, 1);

}
