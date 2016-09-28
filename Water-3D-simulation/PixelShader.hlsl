Texture2D Texture : register(t0);
Texture2D reflectionTexture : register(t1);

SamplerState Sampler : register(s0);

struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float3 norm : NORMAL;
	float2 tex : TEXCOORD0;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
	float4 textureColor;
	float4 reflectionColor;
	float4 color;


	// Sample the texture pixel at this location.
	color = Texture.Sample(Sampler, input.tex);

	// Sample the texture pixel from the reflection texture using the projected texture coordinates.
	reflectionColor = reflectionTexture.Sample(Sampler, input.tex);

	// Do a linear interpolation between the two textures for a blend effect.
	//color = lerp(textureColor, reflectionColor, 0.15f);

	//color.a = 0.2;

	//float4 color = Texture.Sample(Sampler, input.tex);


	return color;
}