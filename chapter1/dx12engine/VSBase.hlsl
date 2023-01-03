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

	output.svpos = projPos; // ���e�ϊ����ꂽ���W���s�N�Z���V�F�[�_�[�ɓn��
	output.color = input.color; // ���_�F�����̂܂܃s�N�Z���V�F�[�_�[�ɓn��
	output.uv = input.uv;
	return output;
}