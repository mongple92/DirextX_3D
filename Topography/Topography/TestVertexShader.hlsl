// 용책에 있는 쉐이더 튜토리얼
// 뷰와 투영 변환으로 버텍스를 변환하고 버텍스 컬러를 파란색으로 지정하는 버텍스 쉐이더

// 전역
// 결합된 뷰와 투영 행렬을 보관하기 위한 전역 변수
// 이 변수 값은 애플리케이션에서 초기화 하였다.
matrix ViewProjMatrix;

// 파란색 컬러 벡터를 초기화
vector Blue = { 0.0f, 0.0f, 1.0f, 1.0f };

// 구조체
// 입력 구조체는 쉐이더에 입력으로 이용 될 구조체를 정의한다.
// 여기에서 입력 버텍스는 위치 성분만을 포함한다.
struct VS_INPUT
{
	vector position : POSITION;
};

// 출력 구조체는 쉐이더가 출력으로 이용할 구조체를 정의한다.
// 여기에서 출력 버텍스는 위치와 컬러 성분을 포함한다.
struct VS_OUTPUT
{
	vector position : POSITION;
	vector diffuse	: COLOR;
};

// 메인 진입점, 메인 함수는 인자를 통해
// 입력 버텍스의 복사본을 받으며, 계산된 출력 버텍스를 리턴한다.
VS_OUTPUT main(VS_INPUT input)
{
	// 출력 멤버를 0으로 초기화
	VS_OUTPUT output = (VS_OUTPUT)0;

	// 뷰 스페이스와 투영으로 변환
	output.position = mul(input.position, ViewProjMatrix);

	// 버텍스 난반사 컬러를 파란색으로 지정
	output.diffuse = Blue;

	// 투영되고 색상이 지정된 버텍스를 리턴한다.
	return output;
}