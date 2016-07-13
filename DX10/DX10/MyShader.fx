float4x4 World;
float4x4 View;
float4x4 Proj;
Texture2D Texture;

//テクスチャの座標の設定
SamplerState samLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

struct VS_INPUT
{
	float4 Pos : IN_POSITION;
	float2 Tex : TEXCOORD;
};

struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD0;
};

// 頂点シェーダ
PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT)0;
	output.Pos = mul(input.Pos, World);
	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Proj);
	output.Tex = input.Tex;

	return output;
}

// ピクセルシェーダ
float4 PS(PS_INPUT input) : SV_Target
{
	return Texture.Sample(samLinear, input.Tex);
}

// テクニック
technique10 SimpleRender
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_4_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, PS()));
	}
}