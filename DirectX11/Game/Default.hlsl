// 사용법
// 1. 컴파일해서 .cso 파일을 만들고 파일을 가져와서 사용
// 2. 프로그램 시작시 동적으로 셰이더 파일을 읽어서 사용 
// -> 셰이더 내용이 많아지면 안좋아짐, 문법이나 오타 확인이 안됨.
// 둘다 사용할 수도 있음. 
//
// Vertex Shader와 Pixel Shader를 나누어 제작해도 됨
// 
// 렌더링 파이프 단계
// IA(입력) - VS(정점) - RS(보간) - PS(픽셀) - OM(병합)

struct VS_INPUT
{
    float4 position : POSITION;
    // float4 color : COLOR;
    float2 uv : TEXCOORD;
};


struct VS_OUTPUT
{
    // SV(System Value) : 
    // - 3차원 이미지를 2차원 이미지로 변환하는 과정에서 사용되는 좌표 시스템과 행렬
    // - 렌더링 파이프라인의 다른 단계에서도 유효함
    float4 position : SV_POSITION;
	// float4 color : COLOR;
    float2 uv : TEXCOORD;
};

// Vertex Shader
// VS_INPUT 타입을 인자로 받아서 VS_OUTPUT타입으로 반환함
// 정점 단위로 실행됨(정점 마다 개별로 실행됨)
VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;
    output.position = input.position;
    // output.color = input.color;
    output.uv = input.uv;
    
    return output;
}

// Rasterizer
// 각 정점이 연결하고 연결된 영역을 확인(카메라 밖에 있는 영역은 제거)하고 
// 각 정점이 가진 색상, 등 을 가지고 영역 내에 위치에 맞는 값을 보간값을 계산함

// Pixel Shader
// t0의 레지스터에 texture2D타입의 texture0를 사용할 것 이다.
Texture2D       texture0 : register(t0);
Texture2D       texture1 : register(t1);
// s0의 레지스터에 SamplerState타입의 sampler0를 사용할 것이다
// 맵핑 할 때 사용함
SamplerState    sampler0 : register(s0);

// Rasterizer에서 적용된 값을 각 픽셀에 적용함
float4 PS(VS_OUTPUT input) : SV_Target // PS의 결과물을 SV_Target(RenderTarget)에 사용함
{
    // float4 output = input.color;
    // sampler0에 정해진 규정에 따라 
    // input.uv좌표 값에 맞는 texture0좌표 색상을 가져옴
    float4 color = texture1.Sample(sampler0, input.uv);
    
    // output += float4(1.f, 1.f, 1.f, 1.f);
    return color;
}