//
// Copyright 2015 Autodesk, Inc.  All rights reserved. 
//
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.
//
//

#pragma once

#include "object.h"
#include "Graphics/IObjectDisplay2.h"

#ifdef POLY_LIB_EXPORTING
#define PolyLibExport __declspec( dllexport )
#else
#define PolyLibExport __declspec( dllimport )
#endif

/** \class SimplePolyObject
	This is a base class for creating procedural objects compatible with Editable Poly.
	This class implements many of the methods required to create a procedural object.
	The only limitation for a procedural object using SimplePolyObject as a base
	class is that it must represent itself with an MNMesh.  These objects allow non-triangular
	faces and support edge and vertex creasing for OpenSubdiv and other subdivision systems.\n\n
	Procedural Object plug-ins which subclass off SimplePolyObject must implement some
	methods. These are noted, as are default implementations of other methods.\n\n
	Note: Methods of the base class refer to the protected data members. For example the
	base class implementations of the bounding box methods use the polyMesh
	data member. Therefore the plug-in derived from SimplePolyObject must use these
	same data members.
	\see GeomObject, IParamBlock2, MNMesh.
*/
class SimplePolyObject : public GeomObject
{
	protected:
		/** The parameter block for managing the object's parameters.
		*/
		IParamBlock2* pblock2;
		/** The MNMesh object that is built by BuildMesh()
		*/
		MNMesh polyMesh;
		/** The validity interval for the polyMesh. This interval is used to determine how
		BuildMesh() is called. If this interval is not set BuildMesh() will be called over and over
		as the system won't know when the mesh is valid or not. Make sure you set this interval
		to accurately reflect the validity interval for the mesh.
		*/
		Interval ivalid;
		/** If TRUE, this causes no snapping to occur. This is commonly used to prevent an
		object from snapping to itself when it is creating.
		*/
		BOOL suspendSnap;
		/** If non-null, this is the SimplePolyObject currently being edited.
		*/
		static SimplePolyObject* editOb;
	public:
		PolyLibExport SimplePolyObject();
		PolyLibExport ~SimplePolyObject();
		
		PolyLibExport void UpdateMesh(TimeValue t);				
		PolyLibExport void GetBBox(TimeValue t, Matrix3& tm, Box3& box );		
		virtual void MeshInvalid() {ivalid.SetEmpty();}
		

		// From BaseObject
		PolyLibExport void BeginEditParams( IObjParam  *ip, ULONG flags,Animatable *prev) override;
		PolyLibExport void EndEditParams( IObjParam *ip, ULONG flags,Animatable *next) override;
		PolyLibExport int HitTest(TimeValue t, INode* inode, int type, int crossing, int flags, IPoint2 *p, ViewExp *vpt) override;
		PolyLibExport void Snap(TimeValue t, INode* inode, SnapInfo *snap, IPoint2 *p, ViewExp *vpt) override;
		PolyLibExport int Display(TimeValue t, INode* inode, ViewExp *vpt, int flags) override;		
		PolyLibExport unsigned long GetObjectDisplayRequirement() const override;
		PolyLibExport bool PrepareDisplay(
			const MaxSDK::Graphics::UpdateDisplayContext& prepareDisplayContext) override;

		PolyLibExport bool UpdatePerNodeItems(
			const MaxSDK::Graphics::UpdateDisplayContext& updateDisplayContext,
			MaxSDK::Graphics::UpdateNodeContext& nodeContext,
			MaxSDK::Graphics::IRenderItemContainer& targetRenderItemContainer) override;
		// From Object
		PolyLibExport ObjectState Eval(TimeValue time) override;
		void InitNodeName(MSTR& s) override {s = GetObjectName();}
		PolyLibExport Interval ObjectValidity(TimeValue t) override;
		PolyLibExport int CanConvertToType(Class_ID obtype) override;
		PolyLibExport Object* ConvertToType(TimeValue t, Class_ID obtype) override;		
        PolyLibExport BOOL PolygonCount(TimeValue t, int& numFaces, int& numVerts) override;

		// From GeomObject
		PolyLibExport int IntersectRay(TimeValue t, Ray& ray, float& at, Point3& norm) override;		
		PolyLibExport void GetWorldBoundBox(TimeValue t, INode* inode, ViewExp* vpt, Box3& box ) override;
		PolyLibExport void GetLocalBoundBox(TimeValue t, INode* inode, ViewExp* vpt, Box3& box ) override;
		PolyLibExport void GetDeformBBox(TimeValue t, Box3& box, Matrix3 *tm, BOOL useSel ) override;
		PolyLibExport Mesh* GetRenderMesh(TimeValue t, INode *inode, View &view, BOOL& needDelete) override;

		// Animatable methods
		PolyLibExport void FreeCaches() override; 		
		void GetClassName(MSTR& s) override {s = GetObjectName();}		
		int NumSubs() override { return 1; }  
		Animatable* SubAnim(int i) override { UNREFERENCED_PARAMETER(i); DbgAssert(i == 0); return (Animatable*)pblock2; }
		PolyLibExport MSTR SubAnimName(int i) override;

		// from InterfaceServer
		PolyLibExport virtual BaseInterface* GetInterface(Interface_ID iid) override;
		PolyLibExport void* GetInterface(ULONG id) override;

		// From ref
		int NumRefs() override { return 1; }
		RefTargetHandle GetReference(int i) override { UNREFERENCED_PARAMETER(i); DbgAssert(i == 0); return (RefTargetHandle)pblock2; }
		int NumParamBlocks() override { return 1; }
		IParamBlock2* GetParamBlock(int i) override { UNREFERENCED_PARAMETER(i); DbgAssert(i == 0); return pblock2; } // return i'th ParamBlock
		IParamBlock2* GetParamBlockByID(short id) override { return pblock2 && (pblock2->ID() == id) ? pblock2 : NULL; } // return id'd ParamBlock
	protected:
		virtual void SetReference(int i, RefTargetHandle rtarg) override { UNREFERENCED_PARAMETER(i); DbgAssert(i == 0); pblock2=(IParamBlock2*)rtarg; }		
	public:
		PolyLibExport RefResult NotifyRefChanged(const Interval& changeInt,RefTargetHandle hTarget, 
		   PartID& partID, RefMessage message, BOOL propagate) override;

		/**	This method is called to build the mesh representation of the object
			using its parameter settings at the time passed. The plug-in should use
			the data member polyMesh to store the built mesh.\n\n
			This method must be implemented by the derived class.
			\param t - The time at which to build the mesh.
		*/
		virtual void BuildMesh(TimeValue t)=0;
		/** This method returns a BOOL to indicate if it is okay to draw the object
			at the time passed. Normally it is always OK to draw the object, so the
			default implementation returns TRUE. However for certain objects it
			might be a degenerate case to draw the object at a certain time
			(perhaps the size went to zero for example), so these objects could
			return FALSE.
			\param t - The time at which the object would be displayed.
			\return  TRUE if the object may be displayed; otherwise FALSE.
		*/
		virtual bool OkToDisplay(TimeValue t) { UNREFERENCED_PARAMETER(t); return true; }
		/** This is called if the user interface parameters needs to be updated
			because the user moved to a new time. The UI controls must display
			values for the current time.
		*/
		virtual void InvalidateUI() {}
	};

