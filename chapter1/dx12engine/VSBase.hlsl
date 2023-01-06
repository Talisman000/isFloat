cbuffer ShaderProperty : register(b0)
{
	float4x4 World; // ���[���h�s��
	float4x4 View; // �r���[�s��
	float4x4 Proj; // ���e�s��
}

// �萔�o�b�t�@���瑗���Ă�����
struct VSInput
{
	float3 pos : POSITION; // ���_���W
	float3 normal : NORMAL; // �@��
	float2 uv : TEXCOORD; // UV
	float3 tangent : TANGENT; // �ڋ��
	float4 color : COLOR; // ���_�F
};

// �s�N�Z���V�F�[�_�[�ɏo�͂�����
struct VSOutput
{
	float4 svpos : SV_POSITION; // �ϊ����ꂽ���W
	float4 color : COLOR; // �ϊ����ꂽ�F
	float2 uv : TEXCOORD;
};

VSOutput vert(VSInput input)
{
	VSOutput output = (VSOutput)0;

	const float4 localPos = float4(input.pos, 1.0f); // ���_���W
	const float4 worldPos = mul(World, localPos); // ���[���h���W�ɕϊ�
	const float4 viewPos = mul(View, worldPos); // �r���[���W�ɕϊ�
	const float4 projPos = mul(Proj, viewPos); // ���e�ϊ�
	float3 nor;
	float  col;
	nor = mul(input.normal, World).xyz;
	nor = normalize(nor);
	const float3 Light = normalize(float3(-1.0f, 0.5f, 1.0f));
	col = saturate(dot(nor, (float3)Light));
	col = col * 0.3f + 0.7f;


	output.svpos = projPos; // ���e�ϊ����ꂽ���W���s�N�Z���V�F�[�_�[�ɓn��

	output.color = input.color * float4(col, col, col, 1); // ���_�F�����̂܂܃s�N�Z���V�F�[�_�[�ɓn��
	output.uv = input.uv;
	return output;
}