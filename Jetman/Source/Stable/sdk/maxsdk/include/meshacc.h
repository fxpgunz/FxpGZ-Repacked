/*****************************************************************************
 *<
	FILE: meshacc.h

	DESCRIPTION:  A class for getting at the private snap data of the mesh class

	CREATED BY: John Hutchinson		

	HISTORY: created 1/2/97

 *>	Copyright (c) 1994, All Rights Reserved.
 *****************************************************************************/
#pragma once
#include "maxheap.h"
#include "export.h"
#include "mesh.h"
#include "mnmesh.h"

class MeshAccess: public MaxHeapOperators {
private:
	Mesh *mpTriMesh;
	MNMesh *mpPolyMesh;
public:
	DllExport MeshAccess(Mesh *somemesh);
	DllExport MeshAccess(MNMesh *somemesh);
	DllExport int BuildSnapData(GraphicsWindow *gw,int verts,int edges);
	DllExport char* GetsnapV();
	DllExport char* GetsnapF();
};

