// ���_�V�F�[�_�[���瑗���Ă�����
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