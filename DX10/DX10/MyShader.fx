float4x4 g_World;
float4x4 g_View;
float4x4 g_Proj;
// 頂点シェーダ
float4 VS(float4 Pos : IN_POSITION) : SV_POSITION
{
	// 射影空間へ変換
	Pos = mul(Pos, g_World);
	Pos = mul(Pos, g_View);
	Pos = mul(Pos, g_Proj);
	return Pos; 
}

// ピクセルシェーダ
float4 PS(float4 Pos : SV_POSITION) : SV_Target
{
	// 黄色い色で塗る
	return float4(1.0f, 1.0f, 0.0f, 1.0f);
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