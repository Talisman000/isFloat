struct VSOutput
{
	float4 svpos : SV_POSITION;
	float4 color : COLOR;
	float2 uv : TEXCOORD;
};

SamplerState smp : register(s0); // サンプラー
Texture2D _MainTex : register(t0); // テクスチャ

float4 pixel(VSOutput input) : SV_TARGET
{
	//return input.color;
	// return float4(input.uv.xy, 1, 1);
	return _MainTex.Sample(smp, input.uv);
}