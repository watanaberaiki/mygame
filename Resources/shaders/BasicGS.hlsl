#include "Basic.hlsli"


[maxvertexcount(113)]
void main(
	line VSOutput input[2] : SV_POSITION, 
	inout TriangleStream< GSOutput > output
)
{
	for(uint i=0;i<2;i++){
	{
	GSOutput element;
	//��ڂ̒��_
	element.svpos=input[i].svpos+float4(scale.x/2,0.0f,0.0f,0.0f);
	element.normal=input[i].normal;
	element.uv=input[i].uv;
	output.Append(element);
	}
	
	//��ڂ̒��_
	{
	GSOutput element;
	element.svpos=input[i].svpos+float4(-scale.x/2,0.0f,0.0f,0.0f);
	element.normal=input[i].normal;
	element.uv=input[i].uv;
	output.Append(element);
	}
	//�O�ڂ̒��_
	{
	GSOutput element;
	if(i==0){
	element.svpos=input[1].svpos+float4(scale.x/2,0.0f,0.0f,0.0f);
	element.normal=input[1].normal;
	element.uv=input[1].uv;
	}else{
	element.svpos=input[0].svpos+float4(-scale.x/2,0.0f,0.0f,0.0f);
	element.normal=input[0].normal;
	element.uv=input[0].uv;
	}
	output.Append(element);
	}
	output.RestartStrip();
	}

}