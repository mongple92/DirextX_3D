// 전역
// 결합된 뷰와 투영 행렬을 보관하기 위한 전역 변수
// 이 변수 값은 애플리케이션에서 초기화 하였다.
float4 view_proj_matrix;
float4 texture_matrix0;

// 출력 구조체는 쉐이더가 출력으로 이용할 구조체를 정의한다.
struct VS_OUTPUT
{
	float4 Pos : POSITION;
	float3 Pshade : TEXCOORD0;
};

// 메인 진입점, 메인 함수는 인자를 통해
// 입력 버텍스의 복사본을 받으며, 계산된 출력 버텍스를 리턴한다.
VS_OUTPUT main(float4 vPosition : POSITION)
{
	// 출력 멤버를 0으로 초기화
	VS_OUTPUT Out = (VS_OUTPUT)0;

	// 뷰 스페이스와 투영으로 변환
	Out.Pos = mul(view_proj_matrix, vPosition);

	// 버텍스 난반사 컬러를 파란색으로 지정
	Out.Pshade = mul(texture_matrix0, vPosition);

	// 투영되고 색상이 지정된 버텍스를 리턴한다.
	return Out;
}