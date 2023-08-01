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
	//一つ目の頂点
	element.svpos=input[i].svpos+float4(scale.x/2,0.0f,0.0f,0.0f);
	element.normal=input[i].normal;
	element.uv=input[i].uv;
	output.Append(element);
	}
	
	//二つ目の頂点
	{
	GSOutput element;
	element.svpos=input[i].svpos+float4(-scale.x/2,0.0f,0.0f,0.0f);
	element.normal=input[i].normal;
	element.uv=input[i].uv;
	output.Append(element);
	}
	//三つ目の頂点
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