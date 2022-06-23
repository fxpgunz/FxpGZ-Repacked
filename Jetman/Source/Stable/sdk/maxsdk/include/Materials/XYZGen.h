//////////////////////////////////////////////////////////////////////////////
//
//  Copyright 2015 Autodesk, Inc.  All rights reserved.
//
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.   
//
//////////////////////////////////////////////////////////////////////////////
#pragma once

#include "MtlBase.h"

#pragma warning(push)
#pragma warning(disable:4100)

// indicate the channel id for object XYZ
#define OBJECT_XYZ_CHANNEL_ID -99

// This class generates Point3 coordinates based on the ShadeContext.
// A reference to one of these is referenced by all 3D texture maps.
/*! \sa  Class MtlBase, Class ShadeContext, Class Point3.\n\n
\par Description:
This class generates <b>Point3</b> coordinates based on the
<b>ShadeContext</b>. A reference to one of these is referenced by all 3D
texture maps. <b>XYZGen</b> does for 3D Texmaps what <b>UVGen</b> does for 2D
Texmaps. It puts up the 3D "Coordinates" rollup, and supplies the 3D Texmap
with transformed 3D coordinates. All methods of this class are implemented by
the system.  */
class XYZGen: public MtlBase {
	 public:
			// Get texture coords and derivatives for antialiasing
			/*! \remarks This method retrieves coordinates and derivatives for
			antialiasing. It returns the coordinates in the "Object Coordinate
			System", scaled and rotated by the parameters given in the Coordinates
			rollup. The default transformation is the identity transform, so you just
			get Object coordinates. For instance, if you have a sphere of radius 100,
			you will get coordinates ranging from -100 to +100.\n\n
			A developer might wonder why 3ds Max don't use some normalized
			coordinates space. This was considered, but it has some real problems.
			Say you make something by assembling several boxes, of varying sizes, and
			then apply a wood texture to the whole thing. You want the wood texture
			to be at the same scale for all the pieces, not smaller on the smaller
			pieces, and thus that doesn't work.
			\par Parameters:
			<b>ShadeContext\& sc</b>\n\n
			Describes the properties of the point to be shaded.\n\n
			<b>Point3\& p</b>\n\n
			The 3D point is returned here.\n\n
			<b>Point3\& dp</b>\n\n
			The derivative of the 3D point is returned here. */
			virtual void GetXYZ( ShadeContext& sc, Point3& p, Point3& dp)=0;
			SClass_ID SuperClassID() { return XYZGEN_CLASS_ID; }
			/*! \remarks			Sets the XYZGen rollup state to open or closed.
			\par Parameters:
			<b>BOOL open</b>\n\n
			TRUE for open; FALSE for closed. */
			virtual void SetRollupOpen(BOOL open)=0;
			/*! \remarks			Returns the open or closed state of the XYZGen rollup.
			\return  TRUE is open; FALSE is closed. */
			virtual BOOL GetRollupOpen()=0;
			virtual BOOL IsStdXYZGen() { return FALSE; } // class StdXYZGen implements this to return TRUE
			/*! \remarks			This method returns the transformed bump unit vectors for 3D bump
			mapping. The 3D textures need to use these in computing the gradient.
			\par Parameters:
			<b>ShadeContext\& sc</b>\n\n
			Describes the properties of the point to be shaded.\n\n
			<b>Point3* dP</b>\n\n
			The 3 unit vectors for computing differentials are returned here.
			\par Default Implementation:
			<b>{}</b>
			\par Sample Code:
			Here is a typical use of XYZGen-\>GetBumpDP() to compute the bump mapping
			for a 3D texture:\n\n
			\code
			Point3 Marble::EvalNormalPerturb(ShadeContext& sc) {
				float del,d;
				Point3 p,dp;
				if (!sc.doMaps) 
					return Point3(0,0,0);
				if (gbufID) 
					sc.SetGBufferID(gbufID);
				xyzGen->GetXYZ(sc,p,dp);
				if (size==0.0f) 
					size=.0001f;
				p *= FACT/size;
				d = MarbleFunc(p);
				del = 20.0f;
				Point3 np;
				Point3 M[3];
				xyzGen->GetBumpDP(sc,M);
				np.x = (MarbleFunc(p+del*M[0]) - d)/del;
				np.y = (MarbleFunc(p+del*M[1]) - d)/del;
				np.z = (MarbleFunc(p+del*M[2]) - d)/del;
				np *= 100.0f;
				return sc.VectorFrom(np,REF_OBJECT);
			}
			\endcode */
			virtual  void GetBumpDP( ShadeContext& sc, Point3* dP) {}; // returns 3 unit vectors for computing differentials
			CoreExport SvGraphNodeReference SvTraverseAnimGraph(IGraphObjectManager *gom, Animatable *owner, int id, DWORD flags);
			CoreExport MSTR SvGetName(IGraphObjectManager *gom, IGraphNode *gNode, bool isBeingEdited)
	    {
			 return Animatable::SvGetName(gom, gNode, isBeingEdited);
		  }
};

CoreExport XYZGen* GetNewDefaultXYZGen();

#pragma warning(pop)
