// ------------------------------------------------------------ 
// Uniforms 
// ------------------------------------------------------------ 
// Built-in Uniforms
float4x4 amg_matWorldViewProj : WorldViewProjection;

float4x4 amg_matWorld : World;

// User Input Uniforms
float3 amg_viewSize;

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
    float4 hpos : POSITION;
    float3 amg_positionWorld : TEXCOORD0;
	float3 amg_uvw : TEXCOORD2;
};

struct AMG_PixelIn
{
    float4 hpos : POSITION;
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

float3 getMixedColor(float3 color1, float3 color2, float mixture)
{
	return color1 * mixture + color2 * (1 - mixture);
}

amg_output_struct pixel_main(AMG_PixelIn fs_in)
{
	float4 hPos = fs_in.hpos;
    float3 amg_positionWorld = fs_in.amg_positionWorld;
	float3 amg_uvw = fs_in.amg_uvw;

	float3 color1 = float3(0, 1, 0);
	float3 color2 = float3(1, 1, 0);

	float mixture = abs(amg_uvw.x * amg_uvw.y);
	mixture *= mixture;

    amg_output_struct fs_out;
	fs_out.color.rgb = getMixedColor(color1, color2, mixture);
	fs_out.color.a = 1;

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
		SetGeometryShader( NULL );
    }
}
