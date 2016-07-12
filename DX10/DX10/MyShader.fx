float4x4 g_World;
float4x4 g_View;
float4x4 g_Proj;
// ���_�V�F�[�_
float4 VS(float4 Pos : IN_POSITION) : SV_POSITION
{
	// �ˉe��Ԃ֕ϊ�
	Pos = mul(Pos, g_World);
	Pos = mul(Pos, g_View);
	Pos = mul(Pos, g_Proj);
	return Pos; 
}

// �s�N�Z���V�F�[�_
float4 PS(float4 Pos : SV_POSITION) : SV_Target
{
	// ���F���F�œh��
	return float4(1.0f, 1.0f, 0.0f, 1.0f);
}

// �e�N�j�b�N
technique10 SimpleRender
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_4_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, PS()));
	}
}