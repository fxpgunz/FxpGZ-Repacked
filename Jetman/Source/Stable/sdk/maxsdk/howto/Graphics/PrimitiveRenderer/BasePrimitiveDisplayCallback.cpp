/**********************************************************************
 *<
	FILE: DemoCallback.cpp

	DESCRIPTION:  A demo class to use displayCallback
	              to display primitives.

	CREATED BY: Tingzhu Zhou

	HISTORY: created 12/8/11

 *>	Copyright (c) 2011, All Rights Reserved.
 **********************************************************************/
#include "BasePrimitiveDisplayCallback.h"


namespace MaxSDK { namespace Graphics {


BasePrimitiveDisplayCallback::BasePrimitiveDisplayCallback() : m_primitiveType(PrimitiveTriangleList)
{};

BasePrimitiveDisplayCallback::~BasePrimitiveDisplayCallback()
{
	m_positions.clear();
	m_normals.clear();
	m_indices.clear();
	m_textureCoords.clear();
}

void BasePrimitiveDisplayCallback::SetPrimitiveType(PrimitiveType type)
{
	m_primitiveType = type;
}
// texture coordinate functions
void BasePrimitiveDisplayCallback::AddTextureCoord(const Point3& coord)
{
	m_textureCoords.push_back(coord);
}
void BasePrimitiveDisplayCallback::ClearTextureCoords()
{
	m_textureCoords.clear();
}

// position functions
void BasePrimitiveDisplayCallback::AddPosition(const Point3& p)
{
	m_positions.push_back(p);
}
void BasePrimitiveDisplayCallback::ClearPositions()
{
	m_positions.clear();
}
// normal functions
void BasePrimitiveDisplayCallback::AddNormal(const Point3& n)
{
	m_normals.push_back(n);
}
void BasePrimitiveDisplayCallback::ClearNormals()
{
	m_normals.clear();
}
// vertex index functions
void BasePrimitiveDisplayCallback::AddVertexIndex(const int index)
{
	m_indices.push_back(index);
} 
void BasePrimitiveDisplayCallback::ClearVertexIndices()
{
	m_indices.clear();
} 

const Point3* BasePrimitiveDisplayCallback::GetPositionBuffer() const
{
	if (m_positions.size() == 0)
	{
		return NULL;
	}
	return &m_positions[0];
}
const Point3* BasePrimitiveDisplayCallback::GetNormalBuffer() const
{
	if (m_normals.size() == 0)
	{
		return NULL;
	}
	return &m_normals[0];
}
const int* BasePrimitiveDisplayCallback::GetVertexIndexBuffer() const
{
	if (m_indices.size() == 0)
	{
		return NULL;
	}
	return &m_indices[0];
}
const float* BasePrimitiveDisplayCallback::GetTextureCoordBuffer() const
{
	if (m_textureCoords.size() == 0)
	{
		return NULL;
	}
	return &m_textureCoords[0][0];
}

void BasePrimitiveDisplayCallback::DoDisplay(TimeValue t, IPrimitiveRenderer& renderer, const DisplayCallbackContext& displayContext)
{
	return;
}

} } // namespaces