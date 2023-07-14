#include "Basic.hlsli"


[maxvertexcount(2)]
void main(
	line VSOutput input[2] : SV_POSITION, 
	inout LineStream< GSOutput > output
)
{
	for(uint i=0;i<(uint)scale.x;i++){
	GSOutput element;
	//��ڂ̐�
	element.svpos=input[0].svpos+float4(0.1,0.0,0,0)*i;
	element.normal=input[0].normal;
	element.uv=input[0].uv;
	output.Append(element);

	//�����̏I�_
	element.svpos=input[1].svpos+float4(0.1,0.0,0,0)*i;
	element.normal=input[1].normal;
	element.uv=input[1].uv;
	output.Append(element);
	}
	output.RestartStrip();
	for(uint i=0;i<scale.x;i++){
	GSOutput element;
	//��ڂ̐�
	element.svpos=input[0].svpos-float4(0.1,0.0,0,0)*i;
	element.normal=input[0].normal;
	element.uv=input[0].uv;
	output.Append(element);

	//�����̏I�_
	element.svpos=input[1].svpos-float4(0.1,0.0,0,0)*i;
	element.normal=input[1].normal;
	element.uv=input[1].uv;
	output.Append(element);
	}

}