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
float4x4 g_WVP   : WorldViewProjection;
float g_ZBias : DepthPriority;
int g_Time : time;

struct motion_in {
	float3 pos: POSITION;
	float4 color: COLOR;
};

struct motion_out {
	float4 instanceColor : COLOR;
	float3 instancePosition : POSITION;
};

motion_out Motion_vs_Main( motion_in IN ) {
    motion_out OUT = (motion_out)0;
	//transform marker to screen space
	float2 sc;
	sincos(IN.pos.y + IN.pos.z * g_Time / 4800.0f, sc.x, sc.y);
	float3 fTransPos = float3(IN.pos.x * sc.x, IN.pos.x * sc.y, 0.0f);
	float4 fHPos = mul(float4(fTransPos, 1.0f), g_WVP);
	//adjust z bias
	fHPos.z -= abs(g_ZBias) * sign(fHPos.w) ;
	fHPos /= fHPos.w;
	OUT.instancePosition = fHPos.xyz;
	OUT.instanceColor = IN.color;
    return OUT;
}

VertexShader vsStream = CompileShader(vs_5_0, Motion_vs_Main());
GeometryShader vsStreamSO = ConstructGSWithSO(vsStream, "0:Color.rgba;0:Position.xyz;", NULL, NULL, NULL, 0);

technique11 MotionShader
{
	pass P0
	{
		SetVertexShader( vsStream );
        SetGeometryShader( vsStreamSO );
        SetPixelShader( NULL );
	}
}