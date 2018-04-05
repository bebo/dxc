struct VOut
{
	float4 position : SV_POSITION;
	float2 texCoord: TEXCOORD;
};

Texture2D Texture;
SamplerState ss;

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