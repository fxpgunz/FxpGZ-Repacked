/**********************************************************************
 *<
	FILE: BasePrimitiveDisplayCallback.h

	DESCRIPTION:	Includes for Plugins

	CREATED BY:

	HISTORY:

	*>	Copyright (c) 2011, All Rights Reserved.
	**********************************************************************/
#ifndef __BasePrimitiveDisplayCallback__H
#define __BasePrimitiveDisplayCallback__H

#include <Graphics/IDisplayCallback.h>
#include <Graphics/IPrimitiveRenderer.h>
#include <vector>


namespace MaxSDK { namespace Graphics {

const int PRIMITIVE_DEGREE = 3;

class BasePrimitiveDisplayCallback : public IDisplayCallback
{
public:
	BasePrimitiveDisplayCallback();	
	virtual ~BasePrimitiveDisplayCallback();
	
public:
	void SetPrimitiveType(PrimitiveType type);
	// texture coordinate functions
	void AddTextureCoord(const Point3& coord);
	void ClearTextureCoords();
	// position functions
	void AddPosition(const Point3& p);
	void ClearPositions();
	// normal functions
	void AddNormal(const Point3& n);
	void ClearNormals();
	// vertex index functions
	void AddVertexIndex(const int index);
	void ClearVertexIndices();

	/*! \remarks This method returns the position buffer of the parameter 'm_positions'.
		\par Parameters:		
		\return  Pointer to positions.*/
	const Point3* GetPositionBuffer() const;
	/*! \remarks This method returns the normal buffer of the parameter 'm_normals'.
		\par Parameters:		
		\return  Pointer to normals.*/
	const Point3* GetNormalBuffer() const;
	/*! \remarks This method returns the vertex index buffer of the parameter 'm_indices'.
		\par Parameters:		
		\return  Pointer to vertex index.*/
	const int* GetVertexIndexBuffer() const;
	/*! \remarks This method returns the normal buffer of the parameter 'm_textureCoords'.
		\par Parameters:		
		\return  Pointer to textureCoords.*/
	const float* GetTextureCoordBuffer() const;

protected:
	/* From IDisplayCallback.
	\param[in] t Current time
	\param[in] renderer Instance of IPrimitiveRender. User can use this class 
		to draw geometries, markers, and texts.
	\param[in] displayContext The display context. 
	*/
	virtual void DoDisplay(
		TimeValue t, 
		IPrimitiveRenderer& renderer, 
		const DisplayCallbackContext& displayContext);

	// Attributes
	PrimitiveType m_primitiveType;
	std::vector<Point3> m_positions;
	std::vector<Point3> m_normals;
	std::vector<Point3> m_textureCoords;
	std::vector<int> m_indices;
};

typedef RefPtr<BasePrimitiveDisplayCallback> BasePrimitiveDisplayCallbackPtr;

} } // namespaces
#endif