//**************************************************************************/
// Copyright (c) 2014 Autodesk, Inc.
// All rights reserved.
// 
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information written by Autodesk, Inc., and are
// protected by Federal copyright law. They may not be disclosed to third
// parties or copied or duplicated in any form, in whole or in part, without
// the prior written consent of Autodesk, Inc.
//**************************************************************************/
// DESCRIPTION: Appwizard generated plugin
// AUTHOR: 
//***************************************************************************/

#include "PopulateExample.h"

#define PopulateExample_CLASS_ID	Class_ID(0x8ec7cc50, 0x4906825c)

using namespace MaxSDK::Populate;

class PopulateExample : public UtilityObj 
{
public:

	//Constructor/Destructor
	PopulateExample();
	virtual ~PopulateExample();

	virtual void DeleteThis() { }

	virtual void BeginEditParams(Interface *ip,IUtil *iu);
	virtual void EndEditParams(Interface *ip,IUtil *iu);

	virtual void Init(HWND hWnd);
	virtual void Destroy(HWND hWnd);

	// Singleton access
	static PopulateExample* GetInstance() { 
		static PopulateExample thePopulateExample;
		return &thePopulateExample; 
	}

	// Populate example methods:
	void CreatePopulateData();
	void CreatePopulateIdleArea();
	void CreatePopulateSeat();

private:

	static INT_PTR CALLBACK DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	HWND   hPanel;
	IUtil* iu;
};


class PopulateExampleClassDesc : public ClassDesc2 
{
public:
	virtual int IsPublic() 							{ return TRUE; }
	virtual void* Create(BOOL /*loading = FALSE*/) 	{ return PopulateExample::GetInstance(); }
	virtual const TCHAR *	ClassName() 			{ return GetString(IDS_CLASS_NAME); }
	virtual SClass_ID SuperClassID() 				{ return UTILITY_CLASS_ID; }
	virtual Class_ID ClassID() 						{ return PopulateExample_CLASS_ID; }
	virtual const TCHAR* Category() 				{ return GetString(IDS_CATEGORY); }

	virtual const TCHAR* InternalName() 			{ return _T("PopulateExample"); }	// returns fixed parsable name (scripter-visible name)
	virtual HINSTANCE HInstance() 					{ return hInstance; }					// returns owning module handle


};


ClassDesc2* GetPopulateExampleDesc() { 
	static PopulateExampleClassDesc PopulateExampleDesc;
	return &PopulateExampleDesc; 
}




//--- PopulateExample -------------------------------------------------------
PopulateExample::PopulateExample()
	: hPanel(nullptr)
	, iu(nullptr)
{

}

PopulateExample::~PopulateExample()
{

}

void PopulateExample::BeginEditParams(Interface* ip,IUtil* iu) 
{
	this->iu = iu;
	hPanel = ip->AddRollupPage(
		hInstance,
		MAKEINTRESOURCE(IDD_PANEL),
		DlgProc,
		GetString(IDS_PARAMS),
		0);
}

void PopulateExample::EndEditParams(Interface* ip,IUtil*)
{
	this->iu = nullptr;
	ip->DeleteRollupPage(hPanel);
	hPanel = nullptr;
}

void PopulateExample::Init(HWND /*handle*/)
{

}

void PopulateExample::Destroy(HWND /*handle*/)
{

}

INT_PTR CALLBACK PopulateExample::DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) 
	{
	case WM_INITDIALOG:
		PopulateExample::GetInstance()->Init(hWnd);
		break;

	case WM_DESTROY:
		PopulateExample::GetInstance()->Destroy(hWnd);
		break;

	case WM_COMMAND:
		// react to button presses:
		switch(LOWORD(wParam)){
		case IDC_BUTTON1:
			PopulateExample::GetInstance()->CreatePopulateData();

			break;
		case IDC_BUTTON2:
			PopulateExample::GetInstance()->CreatePopulateIdleArea();
			break;

		case IDC_BUTTON3:
			PopulateExample::GetInstance()->CreatePopulateSeat();
			break;
		}
		break;

	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_MOUSEMOVE:
		GetCOREInterface()->RollupMouseMessage(hWnd,msg,wParam,lParam);
		break;

	default:
		return 0;
	}
	return 1;
}

void PopulateExample::CreatePopulateData(){

	// From Populate Flow doc sample:
	//! [Create Populate Flow]
	// Create Populate Flow
	Interface* ip = GetCOREInterface();
	Object *flowObj = (Object*) ip->CreateInstance(GEOMOBJECT_CLASS_ID,POPULATE_FLOW_CLASS_ID);
	INode *flowNode = ip->CreateObjectNode(flowObj);
	IPopulateFlow  *flowIface = IPopulateFlow::GetPopulateFlowInterface(flowObj);
	// Add points to the flow
	flowIface->AddPoint(Point3(-400.0f,0.0f,0.0f));
	flowIface->AddPoint(Point3( 400.0f,0.0f,0.0f));
	flowIface->SetPathWidth(230.0f);
	flowIface->ComputeSides();
	// Set some flow parameters
	flowIface->SetDensity(0,0.7f);
	flowIface->SetSpeed(0,0.8f);
	// Add flow to Populate
	IPopulate *iPopulate = IPopulate::GetInstance();
	iPopulate->AddFlow(flowNode);
	//! [Create Populate Flow]

	//! [Create Populate]
	// Set the figure display type and number of frames
	iPopulate->SetNumFrames(500);
	iPopulate->SetDisplayType(IPopulate::eDisplayTexturedSkin);
	// Simulate
	iPopulate->Simulate();
	// select all characters
	INode *node = NULL;
	INodeTab nodes;
	for (int i=0; i<iPopulate->NumStandingIdlers(); i++)
	{
		node = iPopulate->GetStandingIdler(i);
		if (node) nodes.Append(1,&node);
	}
	for (int i=0; i<iPopulate->NumSeatedIdlers(); i++)
	{
		node = iPopulate->GetSeatedIdler(i);
		if (node) nodes.Append(1,&node);
	}
	for (int i=0; i<iPopulate->NumPedestrians(); i++)
	{
		node = iPopulate->GetPedestrian(i);
		if (node) nodes.Append(1,&node);
	}
	ip->SelectNodeTab(nodes,1,1);
	// change the resolution of the characters to high resolution
	iPopulate->SwitchSelectedCharactersResolution();
	//! [Create Populate]

}

void PopulateExample::CreatePopulateSeat(){

	// From IPopulateSeat reference sample:
	//! [Create Populate Seat]
	// Create Populate seat
	Interface* ip = GetCOREInterface();
	Object *seatObj = (Object*) ip->CreateInstance(GEOMOBJECT_CLASS_ID,POPULATE_SEAT_CLASS_ID);
	INode *seatNode = ip->CreateObjectNode(seatObj);
	IPopulateSeat *seatIface = IPopulateSeat::GetPopulateSeatInterface(seatObj);
	// Set seat gender
	seatIface->SetGender(IPopulateSeat::eFemaleSeat);
	// Place and rotate the seat using standard Max controllers
	Control *posControl = seatNode->GetTMController()->GetPositionController();
	Point3 pos(0.0f,150.0f,0.0f);
	posControl->SetValue(0,(void*)(&pos));
	Point3 axis(0.0f,0.0f,1.0f);
	AngAxis aa(axis,180.0f * .017453277f); // convert to radians
	Quat quat(aa);
	Control *rotControl = seatNode->GetTMController()->GetRotationController();
	rotControl->SetValue(0,(void*)(&quat));
	// Add seat to Populate
	IPopulate *iPopulate = IPopulate::GetInstance();
	iPopulate->AddSeat(seatNode);
	//! [Create Populate Seat]

}


void PopulateExample::CreatePopulateIdleArea(){

	// From the PopulateIdleArea reference sample:
	//! [Create Populate Idle Area]
	// need these includes:
	// #include <iepoly.h> // necessary to edit the geometry of the idle area
	// #include <mnmesh.h>
	// Also need to add mnmath.lib to project libraries
	
	Interface* ip = GetCOREInterface();

	// Create Idle Area
	Object *iaObj = (Object*) ip->CreateInstance(GEOMOBJECT_CLASS_ID,POPULATE_IDLEAREA_CLASS_ID);
	INode *iaNode = ip->CreateObjectNode(iaObj);
	// To set the idle area geometry, get the Edit Poly Object at the base of the Idle Area Object
	EPoly *cd = NULL; MNMesh* mm = NULL;
	Object* base = iaNode->GetObjectRef();
	if (base->ClassID() == POPULATE_IDLEAREA_CLASS_ID)
	{
		ReferenceMaker* rmak = (ReferenceMaker*) base;
		for (int i=0;i<rmak->NumRefs();i++)
		{
			ReferenceTarget* rtarg = rmak->GetReference(i);
			cd = (EPoly *)(rtarg->GetInterface(EPOLY_INTERFACE));
			if (cd)
			{
				mm = cd->GetMeshPtr();
				break;
			}
		}
	}
	// set the idle area to be a rectangle
	float width = 400.0f;
	float length = 400.0f;
	int faceVerts[4];
	Point3 corner0(-(width*0.5f),-(length*0.5f),0.0f);
	Point3 corner1(width*0.5f,-(length*0.5f),0.0f);
	Point3 corner2(width*0.5f,length*0.5f,0.0f);
	Point3 corner3(-(width*0.5f),length*0.5f,0.0f);
	faceVerts[0] = mm->NewVert(corner0);
	faceVerts[1] = mm->NewVert(corner1);
	faceVerts[2] = mm->NewVert(corner2);
	faceVerts[3] = mm->NewVert(corner3);
	cd->EpfnCreateFace(faceVerts,4);
	// get the interface to the idle area
	IPopulateIdleArea *IpopIA= new IPopulateIdleArea();
	IpopIA->SetIdleAreaObjInstance((ReferenceTarget*) iaObj);
	// Set some parameters
	IpopIA->SetDensity(0.7f);
	IpopIA->SetGroups(0.33f);
	// Add area to Populate
	IPopulate *iPopulate = IPopulate::GetInstance();
	iPopulate->AddIdleArea(iaNode);
	
	//! [Create Populate Idle Area]
}

