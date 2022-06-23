/**********************************************************************
 *<
	FILE: DrawIndexedPrimitiveCallback.cpp

	DESCRIPTION:  A demo class to use displayCallback
	              to display primitives.

	CREATED BY: Tingzhu Zhou

	HISTORY: created 12/8/11

 *>	Copyright (c) 2011, All Rights Reserved.
 **********************************************************************/
#include "DrawIndexedPrimitiveCallback.h"


namespace MaxSDK { namespace Graphics {

DrawIndexedPrimitiveCallback::DrawIndexedPrimitiveCallback()
{
	hSolidColorMaterial.Initialize();	
}

DrawIndexedPrimitiveCallback::~DrawIndexedPrimitiveCallback()
{};

void DrawIndexedPrimitiveCallback::DoDisplay(TimeValue t, IPrimitiveRenderer& renderer, const DisplayCallbackContext& displayContext)
{
	SimpleVertexStream vertexStreams;
	vertexStreams.Positions = const_cast<Point3 *>(GetPositionBuffer());
	vertexStreams.Normals = const_cast<Point3 *>(GetNormalBuffer());
	vertexStreams.TextureStreams[0].Dimension = 3;
	vertexStreams.TextureStreams[0].Data = const_cast< float * >((const float*)GetTextureCoordBuffer());
	SetSolidColorMaterial(renderer);
	renderer.DrawIndexedPrimitiveUP(
		m_primitiveType, 
		vertexStreams, 
		(int)m_indices.size() / PRIMITIVE_DEGREE, 
		GetVertexIndexBuffer(), 
		(int)m_indices.size());
}

void DrawIndexedPrimitiveCallback::SetSolidColorMaterial(IPrimitiveRenderer& renderer)
{	
	float r = (float)rand() / (float)RAND_MAX;
	float g = (float)rand() / (float)RAND_MAX;
	float b = (float)rand() / (float)RAND_MAX;
	hSolidColorMaterial.SetColor(AColor(r, g, b, 1.0f));	
	renderer.SetMaterial(hSolidColorMaterial);
}

} } // namespaces