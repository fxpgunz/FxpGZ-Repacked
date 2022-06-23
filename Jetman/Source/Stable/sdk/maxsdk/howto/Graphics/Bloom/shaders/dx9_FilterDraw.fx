// ------------------------------------------------------------ 
// Uniforms 
// ------------------------------------------------------------ 
// Built-in Uniforms
float4x4 amg_matWorldViewProj : WorldViewProjection;

float4x4 amg_matWorld : World;

// User Input Uniforms
Texture2D texturen2;
sampler2D sampler_texturen2 = sampler_state
{
    Texture = <texturen2>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = LINEAR;
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
    float4 color : COLOR;
};
amg_output_struct pixel_main(AMG_PixelIn fs_in)
{
    float3 amg_positionWorld = fs_in.amg_positionWorld;
	float3 amg_uvw = fs_in.amg_uvw;

    amg_output_struct fs_out;
	fs_out.color = tex2D(sampler_texturen2, float2(amg_uvw.x, amg_uvw.y));
	float intensity = (fs_out.color.r + fs_out.color.g + fs_out.color.b) / 3;
	if (intensity < 0.8f)
	{
		fs_out.color.rgb *= 0.3f;
	}
	else if (intensity < 0.9f)
	{
		fs_out.color.rgb *= 0.75f;
	}
	else if (intensity < 0.95f)
	{
		fs_out.color.rgb *= 0.85f;
	}
	else
	{
		fs_out.color.rgb *= 1.0f;
	}
	fs_out.color.a = 1;
    return fs_out;
}
technique T0
{
    pass P0
    {
        VertexShader = compile vs_3_0 vertex_main();
        ZEnable = false;
		ZWriteEnable = false;
        CullMode = None;
		
		/*
		AlphaBlendEnable = true;
		BlendOp = Add;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;

		AlphaTestEnable = true;
		AlphaRef = 1;
		AlphaFunc = LessEqual;
		*/

        PixelShader = compile ps_3_0 pixel_main();
    }
}
