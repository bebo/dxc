struct VOut
{
	float4 position : SV_POSITION;
	float2 texCoord: TEXCOORD;
};

Texture2D Texture;
SamplerState ss;

static const float3x3 yuvCoef = {
	1.164f,  1.164f, 1.164f,
	0.000f, -0.392f, 2.017f,
	1.596f, -0.813f, 0.000f };

VOut VShader(float4 position : POSITION, float2 texCoord : TEXCOORD)
{
	VOut output;

	output.position = position;
	output.texCoord = texCoord;

	return output;
}

float4 PShader(float4 position : SV_POSITION, float2 texCoord : TEXCOORD) : SV_TARGET
{
	return Texture.Sample(ss, texCoord);
}