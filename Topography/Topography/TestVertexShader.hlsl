// ����
// ���յ� ��� ���� ����� �����ϱ� ���� ���� ����
// �� ���� ���� ���ø����̼ǿ��� �ʱ�ȭ �Ͽ���.
float4 view_proj_matrix;
float4 texture_matrix0;

// ��� ����ü�� ���̴��� ������� �̿��� ����ü�� �����Ѵ�.
struct VS_OUTPUT
{
	float4 Pos : POSITION;
	float3 Pshade : TEXCOORD0;
};

// ���� ������, ���� �Լ��� ���ڸ� ����
// �Է� ���ؽ��� ���纻�� ������, ���� ��� ���ؽ��� �����Ѵ�.
VS_OUTPUT main(float4 vPosition : POSITION)
{
	// ��� ����� 0���� �ʱ�ȭ
	VS_OUTPUT Out = (VS_OUTPUT)0;

	// �� �����̽��� �������� ��ȯ
	Out.Pos = mul(view_proj_matrix, vPosition);

	// ���ؽ� ���ݻ� �÷��� �Ķ������� ����
	Out.Pshade = mul(texture_matrix0, vPosition);

	// �����ǰ� ������ ������ ���ؽ��� �����Ѵ�.
	return Out;
}