/**********************************************************************
 *<
	FILE: ModCustAttrib.cpp

	DESCRIPTION:	Defines the Modifier/BaseObject Custom Attribute

	CREATED BY:		Nikolai Sander

	HISTORY:		Created:  5/26/00
					Turnd into Sample Neil Hazzard - DCG: 12/5/00

 *>	Copyright (c) 2000, All Rights Reserved.
 **********************************************************************/
#include "max.h"
#include "DynPBlock.h"
#include "CASample.h"


static SimpleAttribClassDesc SimpleAttribDesc;

ClassDesc2* GetSimpleAttribDesc() {return &SimpleAttribDesc;}

static SwatchAttribClassDesc SwatchAttribDesc;

ClassDesc2* GetSwatchAttribDesc() {return &SwatchAttribDesc;}

static NodeAttribClassDesc NodeAttribDesc;

ClassDesc2* GetNodeAttribDesc() {return &NodeAttribDesc;}

enum { simple_params};
enum { simple_check, simple_spin };

enum { swatch_params};
enum { swatch_color};

enum { node_params};
enum { node_pick};



const int MIN_SPIN = 0;
const int MAX_SPIN = 100;


ParamBlockDesc2 simpleblk( simple_params, _T("Simple Parameters"),  0, &SimpleAttribDesc, P_AUTO_CONSTRUCT+ P_AUTO_UI,0,
		//rollout
		IDD_DYNPARAM, IDS_DYNPARAMS, 0, 0, NULL,
		// params
		simple_check, 	_T("checkbox"),		TYPE_BOOL, 		P_ANIMATABLE,				IDS_CHECK,
		p_default, 		FALSE, 
		p_ui, 			TYPE_SINGLECHECKBOX, 	IDC_CHECK, 
		p_end, 
		simple_spin, 	_T("spinner"), 			TYPE_INT, 		P_ANIMATABLE, 	IDS_SPIN, 
		p_default, 		4, 
		p_range, 		MIN_SPIN, MAX_SPIN, 
		p_ui, 			TYPE_SPINNER, EDITTYPE_INT, IDC_EDIT, IDC_EDITSPIN, 0.05f, 
		p_end, 
		p_end
);

ParamBlockDesc2 swatchblk( swatch_params, _T("Swatch Parameters"),  0, &SwatchAttribDesc, P_AUTO_CONSTRUCT+ P_AUTO_UI,0,
		//rollout
		IDD_SWATCH, IDS_SWATCHPARAMS, 0, 0, NULL,
		swatch_color,	_T("swatchcolor"),TYPE_RGBA,P_ANIMATABLE,IDS_SWATCHCOLOR,  // a fixed tab
		p_ui,			TYPE_COLORSWATCH,IDC_COLORS, 
		p_default,		Color(1,0,0),
		p_end,
		p_end

);

ParamBlockDesc2 nodeblk( node_params, _T("Node Parameters"),  0, &NodeAttribDesc, P_AUTO_CONSTRUCT+ P_AUTO_UI,0,
		//rollout
		IDD_NODE, IDS_NODEPARAMS, 0, 0, NULL,
		node_pick,	_T("nodePicker"),TYPE_INODE,0,IDS_NODEPICK,  // a fixed tab
		p_ui,			TYPE_PICKNODEBUTTON,IDC_NODEPICK, 
		p_end,
		p_end

);

SimpleCustAttrib::SimpleCustAttrib()
{
	pblock = NULL;
	SimpleAttribDesc.MakeAutoParamBlocks(this);
}


void SimpleCustAttrib::BeginEditParams(IObjParam *ip,ULONG flags,Animatable *prev)
{
	SimpleAttribDesc.BeginEditParams(ip,this,flags,prev);
}

void SimpleCustAttrib::EndEditParams(IObjParam *ip, ULONG flags, Animatable *next)
{
	SimpleAttribDesc.EndEditParams(ip,this,flags,next);
}


ReferenceTarget *SimpleCustAttrib::Clone(RemapDir &remap)
{
	SimpleCustAttrib *pnew = new SimpleCustAttrib;
	pnew->ReplaceReference(0,remap.CloneRef(pblock));
	BaseClone(this, pnew, remap);
	return pnew;
}


SwatchCustAttrib::SwatchCustAttrib()
{
	pblock = NULL;
	SwatchAttribDesc.MakeAutoParamBlocks(this);
}


void SwatchCustAttrib::BeginEditParams(IObjParam *ip,ULONG flags,Animatable *prev)
{
	SwatchAttribDesc.BeginEditParams(ip,this,flags,prev);
}

void SwatchCustAttrib::EndEditParams(IObjParam *ip, ULONG flags, Animatable *next)
{
	SwatchAttribDesc.EndEditParams(ip,this,flags,next);
}


ReferenceTarget *SwatchCustAttrib::Clone(RemapDir &remap)
{
	SwatchCustAttrib *pnew = new SwatchCustAttrib;
	pnew->ReplaceReference(0,remap.CloneRef(pblock));
	BaseClone(this, pnew, remap);
	return pnew;
}


NodeCustAttrib::NodeCustAttrib()
{
	pblock = NULL;
	NodeAttribDesc.MakeAutoParamBlocks(this);
}


void NodeCustAttrib::BeginEditParams(IObjParam *ip,ULONG flags,Animatable *prev)
{
	NodeAttribDesc.BeginEditParams(ip,this,flags,prev);
}

void NodeCustAttrib::EndEditParams(IObjParam *ip, ULONG flags, Animatable *next)
{
	NodeAttribDesc.EndEditParams(ip,this,flags,next);
}


ReferenceTarget *NodeCustAttrib::Clone(RemapDir &remap)
{
	NodeCustAttrib *pnew = new NodeCustAttrib;
	pnew->ReplaceReference(0,remap.CloneRef(pblock));
	BaseClone(this, pnew, remap);
	return pnew;
}
