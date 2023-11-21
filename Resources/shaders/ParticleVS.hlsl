#include "Particle.hlsli"

//VSOutput main(float4 pos : POSITION)
VSOutput main(float4 pos : POSITION, float scale : TEXCOORD,float rotation : ROTATION)
{
	VSOutput output; // ピクセルシェーダーに渡す値
	output.pos = mul(matBillboard, pos);
	output.scale = scale;
	output.rotation=rotation;
	return output;
}