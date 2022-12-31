// 頂点シェーダーから送られてくる情報
struct VSOutput
{
	float4 svpos : SV_POSITION;
	float4 color : COLOR;
	float2 uv : TEXCOORD;
};

float4 pixel(VSOutput input) : SV_TARGET
{
	//return input.color;
	return float4(input.uv.xy, 1, 1);
}