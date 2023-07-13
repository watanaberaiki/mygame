#include "Basic.hlsli"


[maxvertexcount(2)]
void main(
	line VSOutput input[2] : SV_POSITION, 
	inout LineStream< GSOutput > output
)
{
GSOutput element;
	element.svpos=input[0].svpos;
	element.normal=input[0].normal;
	element.uv=input[0].uv;
	output.Append(element);

	//線分の終点
	element.svpos=input[1].svpos;
	element.normal=input[1].normal;
	element.uv=input[1].uv;
	output.Append(element);


}