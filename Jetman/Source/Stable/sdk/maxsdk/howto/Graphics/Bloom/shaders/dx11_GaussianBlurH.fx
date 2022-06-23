// ------------------------------------------------------------ 
// Uniforms 
// ------------------------------------------------------------ 
// Built-in Uniforms
float4x4 amg_matWorldViewProj : WorldViewProjection;

float4x4 amg_matWorld : World;

float3 amg_viewSize;

// User Input Uniforms
Texture2D texturen2;
SamplerState sampler_texturen2
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Clamp;
    AddressV = Clamp;
};

// ------------------------------------------------------------ 
// Shader body 
// ------------------------------------------------------------ 
// Vertex Shader Input
struct AMG_VertexIn
{
    float3 position : POSITION;
	float3 uvw : TEXCOORD0;
};
// Pixel Shader Input
struct AMG_VertexOut
{
    float4 hpos : SV_POSITION;
    float3 amg_positionWorld : TEXCOORD0;
	float3 amg_uvw : TEXCOORD2;
};

struct AMG_PixelIn
{
    float4 hpos : SV_POSITION;
    float3 amg_positionWorld : TEXCOORD0;
	float3 amg_uvw : TEXCOORD2;
};
// ------------------------------------------------------------ 
// Main Vertex Shader
// ------------------------------------------------------------ 
AMG_VertexOut vertex_main(AMG_VertexIn vs_in)
{
    AMG_VertexOut vs_out;
    vs_out.hpos = mul(float4(vs_in.position,1.0),amg_matWorldViewProj);
    vs_out.amg_positionWorld = mul(float4(vs_in.position,1.0),amg_matWorld).xyz;
	vs_out.amg_uvw = vs_in.uvw;
    return vs_out;
}
// ------------------------------------------------------------ 
// Main Pixel Shader
// ------------------------------------------------------------ 
struct amg_output_struct
{
    float4 color : SV_Target0;
};
amg_output_struct pixel_main(AMG_PixelIn fs_in)
{
    float3 amg_positionWorld = fs_in.amg_positionWorld;
	float3 amg_uvw = fs_in.amg_uvw;

	float3 sum = float3(0, 0, 0);
	float blurSize = 1.0f / amg_viewSize.x;
 
   // blur in y (vertical)
   // take nine samples, with the distance blurSize between them
   sum += (texturen2.Sample(sampler_texturen2, float2(amg_uvw.x - 4.0*blurSize, amg_uvw.y)) * 0.05).rgb;
   sum += (texturen2.Sample(sampler_texturen2, float2(amg_uvw.x - 3.0*blurSize, amg_uvw.y)) * 0.09).rgb;
   sum += (texturen2.Sample(sampler_texturen2, float2(amg_uvw.x - 2.0*blurSize, amg_uvw.y)) * 0.12).rgb;
   sum += (texturen2.Sample(sampler_texturen2, float2(amg_uvw.x - blurSize, amg_uvw.y)) * 0.15).rgb;
   sum += (texturen2.Sample(sampler_texturen2, float2(amg_uvw.x, amg_uvw.y)) * 0.16).rgb;
   sum += (texturen2.Sample(sampler_texturen2, float2(amg_uvw.x + blurSize, amg_uvw.y)) * 0.15).rgb;
   sum += (texturen2.Sample(sampler_texturen2, float2(amg_uvw.x + 2.0*blurSize, amg_uvw.y)) * 0.12).rgb;
   sum += (texturen2.Sample(sampler_texturen2, float2(amg_uvw.x + 3.0*blurSize, amg_uvw.y)) * 0.09).rgb;
   sum += (texturen2.Sample(sampler_texturen2, float2(amg_uvw.x + 4.0*blurSize, amg_uvw.y)) * 0.05).rgb;

    amg_output_struct fs_out;
    fs_out.color.rgb = sum;
	fs_out.color.a = 1;
	//fs_out.color = float4(amg_uvw.x, amg_uvw.y, 1, 0.5);
    return fs_out;
}

RasterizerState RasterizerStateDefault
{
	FillMode					= Solid;
	CullMode					= None;
	FrontCounterClockwise		= FALSE;
	DepthBias					= 0;
	DepthBiasClamp				= 0.0f;
	SlopeScaledDepthBias		= 0.0f;
	DepthClipEnable				= TRUE;
	ScissorEnable				= FALSE;
	MultisampleEnable			= FALSE;
	AntialiasedLineEnable		= FALSE;
};

DepthStencilState DepthStencilStateDefault
{
	DepthEnable = FALSE;
	DepthWriteMask = Zero;
	StencilEnable = FALSE;
};

technique11 T0
{
    pass P0
    {
		SetRasterizerState( RasterizerStateDefault );
		SetDepthStencilState( DepthStencilStateDefault, 0 );
        SetVertexShader( CompileShader( vs_5_0, vertex_main()));
        SetPixelShader( CompileShader( ps_5_0, pixel_main()));
    }
}
