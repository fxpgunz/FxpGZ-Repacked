//
// Copyright [2012] Autodesk, Inc.  All rights reserved. 
//
// This computer source code and related instructions and comments are the
// unpublished confidential and proprietary information of Autodesk, Inc. and
// are protected under applicable copyright and trade secret law.  They may
// not be disclosed to, copied or used by any third party without the prior
// written consent of Autodesk, Inc.
//
//

float2 g_TargetSize : ViewportPixelSize;
Texture2D g_MarkerTexture;

#define MARKER_SIZE 4

SamplerState MarkerSampler
{
    AddressU  = WRAP;        
    AddressV  = WRAP;
    AddressW  = WRAP;
	Filter = MIN_MAG_MIP_POINT;
};

struct vs_out {
	float4 Color : COLOR;
	float4 Position	: SV_POSITION;
	float2 UV	: TEXCOORD;
};

struct app_in {
	float2 quadVert : POSITION;
	float4 instanceColor : COLOR;
	float3 instancePosition : TEXCOORD0;
};

vs_out Marker_vs_main(app_in IN) {
    vs_out OUT = (vs_out)0;
	float3 fHPos = IN.instancePosition;
	//expand the marker to quad
	fHPos.xy *= g_TargetSize;
	fHPos.xy += IN.quadVert * MARKER_SIZE;
	fHPos.xy /= g_TargetSize;

	OUT.Position = float4(fHPos, 1.0f); 
	OUT.Color = IN.instanceColor;
	OUT.UV = max(IN.quadVert, float2(0,0));
    return OUT;
}

float4 Marker_ps_main(vs_out vsIn) : SV_Target
{
	//Color.a is marker type. Each marker's size is MARKER_SIZE.
	return vsIn.Color; //g_MarkerTexture.Sample(MarkerSampler, vsIn.UV) * vsIn.Color;
}

technique11 MarkerShader
{
	pass P0
	{
		SetVertexShader( CompileShader( vs_5_0, Marker_vs_main() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, Marker_ps_main() ) );
	}
}