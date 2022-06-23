//////////////////////////////////////////////////////////////////////////////
//
//  Copyright 2009 Autodesk, Inc.  All rights reserved.
//  Copyright 2003 Character Animation Technologies.
//
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.   
//
//////////////////////////////////////////////////////////////////////////////

//**
//	A General Class ID sharing file
//	Helps in the creation of CAT objects
//	using the CreateInstance function
//**

#pragma once

// FootControllers
#define FOOTBEND_CLASS_ID		Class_ID(0x74b50fd8, 0x7ad665e)
#define FOOTLIFT_CLASS_ID		Class_ID(0x2a234435, 0x750465f5)
#define FOOTSEGTRANS_CLASS_ID	Class_ID(0x16c80ef7, 0x47746810)
#define FOOTSLAP_CLASS_ID		Class_ID(0x790d76a1, 0x13886f1b)
#define FOOTTRANS_CLASS_ID		Class_ID(0x234d6d19, 0x15d1f49)
#define HEELLIFT_CLASS_ID		Class_ID(0x7b012d81, 0x45742f74)
#define KNEEANGLE_CLASS_ID		Class_ID(0x40534bdc, 0x3ae34b58)
#define LIFTPLANTMOD_CLASS_ID	Class_ID(0x46606abf, 0x2d3676)
#define LEGWEIGHT_CLASS_ID		Class_ID(0x7c125bf5, 0x3a880ece)
#define STEPGRAPH_CLASS_ID		Class_ID(0x17202d99, 0x223572fe)
#define STEPSHAPE_CLASS_ID		Class_ID(0x18e02664, 0x21d309c3)
#define WEIGHTSHIFT_CLASS_ID	Class_ID(0x60da17a5, 0x133c03c1)
#define WEIGHTLIFT_CLASS_ID		Class_ID(0x5f702d1a, 0x62675e10)
#define LIFTOFFSET_CLASS_ID		Class_ID(0x3c2e5b6c, 0x73235cd8)

// Other IDs we need
#define CATFLOAT_CLASS_ID		Class_ID(0x2ba14f79, 0x5914ef13)
#define CATSCALE_CLASS_ID		Class_ID(0x339b423d, 0x34c81d7e)
#define OPPOSEGRAPH_CLASS_ID	Class_ID(0x301666fa, 0xe25784f)
#define STEPEASE_CLASS_ID		Class_ID(0x6fbc52ee, 0x71536264)
#define PIVOTPOS_CLASS_ID		Class_ID(0x7fa7544b, 0x711a51ea)
#define PIVOTROT_CLASS_ID		Class_ID(0x63785366, 0x6d970288)
#define MONOGRAPH_CLASS_ID		Class_ID(0x4a420b7, 0x7a142aee)

#define CATWEIGHT_CLASS_ID			Class_ID(0x265e07a8, 0xd281541)

#define CATMOTIONROT_CLASS_ID		Class_ID(0x67a81cf9, 0x79c62151)
#define CATMOTIONHUB_CLASS_ID		Class_ID(0x72b3ae5, 0x46583588)
#define CATMOTIONBEND_CLASS_ID		Class_ID(0x1e564bbd, 0x63d333dd)
#define CATMOTIONPLATFORM_CLASS_ID	Class_ID(0x2de70807, 0x3973713a)
#define CATMOTIONTAIL_CLASS_ID		Class_ID(0x275c1ac1, 0x8b45ee2)
#define CATMOTIONTAIL_ROT_CLASS_ID	Class_ID(0x1d1a667c, 0x206521f1)
#define CATP3_CLASS_ID				Class_ID(0x8b7645d, 0x34eb08f5)
#define CATMOTIONLIMB_CLASS_ID		Class_ID(0xb5e496b, 0x15363635)

//////////////////////////////////////////////////////////////////////////
// ClassID's of various CATNodeControl implementations
#define HUB_CLASS_ID			Class_ID(0x1cbb1d2f, 0x30ea5264)
#define FOOTTRANS2_CLASS_ID		Class_ID(0x71af1a7d, 0x71c05de1)
#define PALMTRANS2_CLASS_ID		Class_ID(0x4edf6dde, 0x3d77551c)
#define COLLARBONE_CLASS_ID		Class_ID(0x6b7d574a, 0x7805231e)

//////////////////////////////////////////////////////////////////////////
// ClassID's of various CATControl implementations
#define LIMBDATA2_CLASS_ID					Class_ID(0x6e4253a3, 0x7ffa3373)
#define TAILDATA2_CLASS_ID		Class_ID(0xbc975dd, 0x6e1a1ae5)

//////////////////////////////////////////////////////////////////////////
// these are the Class IDs used in CATMuscle
#define HDLOBJ_CLASS_ID				Class_ID(0x141e37a3, 0x2b362406)
#define MUSCLEPATCH_CLASS_ID		Class_ID(0x750c5c2c, 0x1d4b0de0)
#define MUSCLEBONES_CLASS_ID		Class_ID(0x17ef46d8, 0x1c0e6acd)
#define MUSCLESTRAND_CLASS_ID		Class_ID(0x7cf3882, 0x47f01642)
#define HDLTRANS_CLASS_ID	Class_ID(0x14fc0b12, 0x7b402f92)
#define SEGTRANS_CLASS_ID	Class_ID(0x52f0baa, 0x38e71f53)

//////////////////////////////////////////////////////////////////////////
// ClassID's of various CATParent implementations
#define CATPARENT_CLASS_ID	 Class_ID(0x56ae72e5, 0x389b6659)

/////////////////////////////////////////////////////////////////////
// Max Classsids that we may use
#define IPOS_CONTROL_CLASS_ID		Class_ID(0x118f7e02,0xffee238a)
#define IPOINT3_CONTROL_CLASS_ID	Class_ID(0x118f7e02,0xfeee238b)
#define ISCALE_CONTROL_CLASS_ID		Class_ID(0x118f7c01,0xfeee238b)
#define ICOLOR_CONTROL_CLASS_ID		Class_ID(0x118f7c01,0xfeee238a)

#define PARAMETERBLOCK_SUPERCLASS_ID	0x0009008
#define PARAMETERBLOCK_CLASS_ID			Class_ID(0x072b3ae5, 0x46583588)

/////////////////////////////////////////////////////////////////////
//	C++ objects the rig is made of
#define CATHUB_CLASS_ID						Class_ID(0x73dc4833, 0x65c93caa)
// Hum object's parameter block id
#define HUBOBJ_PARAMBLOCK_ID				0
// enum for C++ Hub object ParamBlock
enum {
	HUBOBJ_PB_HUBTRANS,
	HUBOBJ_PB_CATUNITS,
	HUBOBJ_PB_LENGTH,
	HUBOBJ_PB_WIDTH,
	HUBOBJ_PB_HEIGHT,
	HUBOBJ_PB_PIVOTPOSY,
	HUBOBJ_PB_PIVOTPOSZ
};

#define CATBONE_CLASS_ID					Class_ID(0x2e6a0c09, 0x43d5c9c0)
enum {
	BONEOBJ_PB_BONETRANS,
	BONEOBJ_PB_CATUNITS,
	BONEOBJ_PB_LENGTH,
	BONEOBJ_PB_WIDTH,
	BONEOBJ_PB_DEPTH
};

#define CATPLATFORM_CLASS_ID				Class_ID(0x3ade442a, 0x10d3291f)
enum {
	FOOTOBJ_PB_PLATFORMTRANS,
	FOOTOBJ_PB_CATUNITS,
	FOOTOBJ_PB_LENGTH,
	FOOTOBJ_PB_WIDTH,
	FOOTOBJ_PB_HEIGHT,
};

//------------------------------------------------------
#define IKTARGET_OBJECT_CLASS_ID	Class_ID(0x67a52f20, 0x2eac1b81)
