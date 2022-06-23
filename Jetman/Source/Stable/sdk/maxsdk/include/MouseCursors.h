#pragma once
//
// Copyright 2015 Autodesk, Inc.  All rights reserved. 
//
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.
//
//


/*! \file MouseCursors.h
 *	\brief Centralized dll for all cursors in 3dsmax, consists of LoadMouseCursor function and enum of all Cursor names.
 */


#include "windows.h"
#include "export.h"


/*! \brief Namespace UI (Userinterface) Containing the namespace,
 *	        EMouseCursors containing the function to load the cursors.
 */
namespace UI{	
	namespace MouseCursors{


		enum ECursors{
				cursor0 = 0,		//!< \image html 00p.png
				cursor5,			//!< \image html 05p.png
				cursor10,			//!< \image html 10p.png
				cursor15,			//!< \image html 15p.png
				cursor20,			//!< \image html 20p.png
				cursor25,			//!< \image html 25p.png
				cursor30,			//!<  \image html 30p.png
				cursor35,			//!<  \image html 35p.png
				cursor40,			//!<  \image html 40p.png
				cursor45,			//!<  \image html 45p.png
				cursor50,			//!<  \image html 50p.png
				cursor55,			//!<  \image html 55p.png
				cursor60,			//!<  \image html 60p.png
				cursor65,			//!<  \image html 65p.png
				cursor70,			//!<  \image html 70p.png
				cursor75,			//!<  \image html 75p.png
				cursor80,			//!<  \image html 80p.png
				cursor85,			//!<  \image html 85p.png
				cursor90,			//!<  \image html 90p.png
				cursor95,			//!<  \image html 95p.png
				cursor100,			//!<  \image html 100p.png
				Circle1,			//!<  \image html cursor22.png
				Circle2,			//!< \image html cursorSmall.png
				Circle3,			//!< \image html cursor3.png
				Circle4,			//!< \image html cursor4.png
				Circle5,			//!< \image html cursor5.png
				Circle6,			//!< \image html cursor6.png
				Circle7,			//!< \image html cursor7.png
				Circle8,			//!< \image html cursor8.png
				Circle9,			//!< \image html cursor9.png
				Circle10,			//!< \image html cursor10.png
				Circle11,			//!< \image html cursor11.png
				Circle12,			//!< \image html cursor12.png
				Circle13,			//!< \image html cursor13.png
				Circle14,			//!< \image html bigCircle.png
				Circle15,			//!< \image html cursor16.png
				Circle16,			//!< \image html cursor17.png
				Align,				//!< \image html align_ds.png
				AlignHit,			//!< \image html align_h.png
				AlignHighlight,		//!< \image html align_hi.png
				Animate,			//!< \image html animarro.png
				AnimateHit,			//!< \image html animcurs.png
				ArrowFrz,			//!< \image html unfrzmd.png
				Attach,				//!< \image html attach.png
				AttachSymbol,		//!< \image html attach_symbol.png
				Bevel,				//!< \image html bevel.png			
				BGPlane,			//!< \image html bgplane.png
				BindWSM,			//!< \image html bindwsm.png
				BipPlaceHit,		//!< \image html mxbiphcu.png 
				Blank,				//!< \image html swblank.png
				BoolIntersection,	//!< \image html boolinte.png
				BoolSubtraction,	//!< \image html boolsubt.png
				BoolUnion,			//!< \image html boolunio.png
				Bubble,				//!< \image html bubble.png
				Camera,				//!< \image html align_vw.png
				CameraPan,			//!< \image html cam_pan.png
				Chamfer,			//!< \image html chamfer.png
				ChamferE,			//!< \image html echamfer.png
				ChamferV,			//!< \image html vchamfer.png
				ClipSideMove,		//!< \image html mxclipsi.png
				Coffee,				//!< \image html coffee.png
				ColorHighlight,		//!< \image html color_hi.png
				ColorMo,			//!< \image html color_mo.png
				Copy,				//!< \image html copy_tan.png
				CreateVertex,		//!< \image html addvertc.png
				CreateWeld,			//!< \image html createwe.png
				Crosshair,			//!< \image html crosshr.png
				CrossInsert,		//!< \image html crossins.png
				CrossSection,		//!< \image html cross_se.png	
				Cui,				//!< \image html cui_cur.png
				CurveHit,			//!< \image html mxwtcrea.png
				CutEdge,			//!< \image html cut_edge.png
				CutFace,			//!< \image html cut_face.png
				CutVert,			//!< \image html cut_vert.png
				CutWire,			//!< \image html cutwire.png
				Diagram,			//!< \image html diagram.png
				Dock,				//!< \image html dock_cur.png
				Dolly,				//!< \image html dolly.png
				DragOkArrow,		//!< \image html dragokar.png
				DropNo,				//!< \image html dropno.png
				DropOkay,			//!< \image html dropokay.png
				DropPlus,			//!< \image html plus.png
				Dropper,			//!< \image html DropCurs.png
				DropperFull,		//!< \image html dropper.png
				Edge,				//!< \image html edgecurs.png
				EdgeExt,			//!< \image html edge_ext.png
				EditFlowCursor,		//!< \image html editflowcursor.png
				Extend,				//!< \image html extendflowcursor.png
				Extract,			//!< \image html extract.png
				ExtractFromEdge,	//!< \image html extract_from_edge.png
				Extrude,			//!< \image html extrudec.png
				ExtrudeVertex,		//!< \image html extrude_vertex.png
				Falloff,			//!< \image html falloff.png
				Fill,				//!< \image html VCFillGizmo.png
				Fillet,				//!< \image html fillet.png
				FieldOfView,		//!< \image html fov.png
				Float,				//!< \image html flt_cur.png
				FloatPlaceHit,		//!< \image html mxflthcu.png 
				ForbiddenBindWSM,	//!< \image html bindwsmn.png
				Forbidden,			//!< \image html forbidden.png
				ForbiddenLink,		//!< \image html linknopi.png
				GetFit,				//!< \image html th_getfi.png
				GetPath,			//!< \image html getpath.png
				GetShape,			//!< \image html getshape.png
				HandPointer,		//!< \image html linkselect.png
				HitAdd,				//!< \image html th_addcm.png
				HitCam,				//!< \image html align_hv.png
				HitFrz,				//!< \image html unfrzhit.png
				HitHighlight,		//!< \image html align_hl.png
				HitNormal,			//!< \image html align_sr.png
				HitQA,				//!< \image html align_qh.png
				HitSubtract,		//!< \image html th_subcm.png
				HitCursor,			//!< \image html thselcur.png
				HitPivot,			//!< \image html hitpivot.png
				HorizontalAxis,		//!< \image html horaxis.png
				HorizontalSnap,		//!< \image html horisnap.png
				HorizontalMove,		//!< \image html mxhormov.png
				HotSpot, 			//!< \image html hotspot.png
				Insert,				//!< \image html insert.png
				InsertIntoEdge,		//!< \image html insert_into_edge.png
				InsertJT,			//!< \image html jtinsert.png
				InsertLK,			//!< \image html lkinsert.png
				InsideCircle,		//!< \image html incircle.png
				LeftFoot,			//!< \image html LFOOT.png
				LeftHitRB, 			//!< \image html cur00001.png
				Link,				//!< \image html LINKCURS.png
				Linker,				//!< \image html svlinkcur.png
				LinkTo,				//!< \image html LINKTOCU.png
				Lod,				//!< \image html lodcurs.png
				Magnify,			//!< \image html magnify.png
				MagnifyAll,			//!< \image html magall.png
				MagnifyOut,			//!< \image html zoomout.png
				MiddleHitRB,		//!< \image html ivcursor.png
				MinusArrow, 		//!< \image html minusarr.png
				MinusSelection,		//!< \image html selcursorminus.png
				MoveCursor,			//!< \image html mxmovecu.png
				MoveH, 				//!< \image html weldcur1.png
				MoveW,				//!< \image html weld.png
				MultipleCopy,		//!< \image html multcopy.png
				MultipleDrop,		//!< \image html multdrop.png
				MoveSnap,			//!< \image html movecursnap.png
				NewNode,			//!< \image html NEWNODEC.png
				NotBipPlaceHit,		//!< \image html mxbipvcu.png
				NotFloatPlaceHit, 	//!< \image html mxfltvcu.png
				Normal,				//!< \image html align_nr.png
				NoSpray,			//!< \image html spraynot.png
				NotTrackHit,		//!< \image html mxtrackv.png
				Nuscale,			//!< \image html nuscale.png
				Orbit,				//!< \image html orbit.png
				OutsideCircle,		//!< \image html rotcur.png
				Outline,			//!< \image html outline.png
				Paint,				//!< \image html paintcur.png
				PanHand,			//!< \image html panhand.png
				PanHandGrab,		//!< \image html pando.png
				PanHandPointer,		//!< \image html edgecurs.png
				PanHandReady,		//!< \image html panready.png
				Paste,				//!< \image html paste_tan.png
				PasteOkay,			//!< \image html mxclipok.png
				Pencil,				//!< \image html pencil.png
				Perspective,		//!< \image html persp.png
				PickBindWSM,		//!< \image html bindwsmp.png
				PickLink,			//!< \image html linkpick.png	
				PickPivot,			//!< \image html pickpivot.png
				PickShape,			//! <image html pickshap.png
				Pinch,				//!< \image html pinch.png
				PlusArrow,			//!< \image html plusarro.png
				PlusMinusArrow,		//!< \image html PLUSMINUSARROW.png
				Question,			//!< \image html whattodo.png
				QuickAlign,			//!< \image html align_q.png
				QuickSlice,			//!< \image html quickslice.png
				Region,				//!< \image html region.png
				ResArrow,			//!< \image html arrow.png
				RightFoot,			//!< \image html RFOOT.png
				RightHitRB,			//!< \image html cur00002.png
				Roll,				//!< \image html roll.png
				RotateView,			//!< \image html rotview.png
				Scale,				//!< \image html scale.png
				ScaleX,				//!< \image html scale_x.png
				ScaleY,				//!< \image html scale_y.png
				SegmentBreak,		//!< \image html segbreak.png
				SegmentRefine,		//!< \image html segrefin.png
				Selection,			//!< \image html selection.png
				Skeleton,			//!< \image html skl.png
				SketchPick,								//!< \image html sketchcu.png
				Slider,								//!< \image html slider.png
				Spinner,									//!< \image html spinner.png
				Spray,								//!< \image html spray.png
				Squash,									//!< \image html squash.png
				SVLink,									//!< \image html svLinkcur.png
				SVNoPick,									//!< \image html svLinknopi.png	
				Tack,								//!< \image html pincurso.png
				TransLK,									//!< \image html lktrans.png
				UniCopy,								//!< \image html unicopy.png
				UniDrop,								//!< \image html unidrop.png
				UScale,								//!< \image html uscalecu.png
				TrackDown,									//!< \image html cur00005.png
				TrackHit,								//!< \image html mxtrackh.png
				Trim,								//!< \image html trim.png
				VerticalAxis,									//!< \image html vertaxis.png
				VerticalConnect,							//!< \image html vertconn.png
				VerticalInsert,						//!< \image html vinsert.png
				VerticalMove,							//!< \image html mxvermov.png	
				ViewChange,							//!< \image html vchangec.png
				Wired,								//!< \image html wired.png
				WiredO,									//!< \image html wiredo.png
				Wireing,									//!< \image html wireing.png
				Recycle,				 //!< \image html to_recyc.png
				VertSnap,								//!< \image html vertsnap.png
				HoriSnap,								//!< \image html horisnap.png
				InsideSnap,								//!< \image html inside_c.png
				OutsideSnap,								//!< \image html outside_.png
				SWLook,							//!< \image html SWLook.png
				SWOrbit,									//!< \image html SWOrbit.png
				SWWalk,								//!< \image html SWWalk.png
				SWWalkBack,									//!< \image html SWWalkB.png
				SWWalkBackLeft,								//!< \image html SWWalkBL.png
				SWWalkBackRight,							//!< \image html SWWalkBR.png
				SWWalkForward,						//!< \image html SWWalkF.png
				SWWalkForwardLeft,							//!< \image html SWWalkFL.png
				SWWalkForwardRight,						//!< \image html SWWalkFR.png
				SWWalkLeft,						//!< \image html SWWalkL.png
				SWWalkRight,								//!< \image html SWWalkR.png
				SWWalkFastB,							//!< \image html SWWalkFastB.png
				SWWalkFastBL,								//!< \image html SWWalkFastBL.png
				SWWalkFastBR,								//!< \image html SWWalkFastBR.png
				SWWalkFastF,								//!< \image html SWWalkFastF.png
				SWWalkFastFL,								//!< \image html SWWalkFastFL.png
				SWWalkFastFR,								//!< \image html SWWalkFastFR.png
				SWWalkFastL,								//!< \image html SWWalkFastL.png
				SWWalkFastR,								//!< \image html SWWalkFastR.png
				BlendCurveArrow,								//!< \image html blndcur.png
				BlendCurveHit,							//!< \image html blndcur2.png
				BlendSurfaceArrow,								//!< \image html blndsur.png
				BlendSurfaceHit,							//!< \image html blndsur2.png
				CapSurfaceArrow,							//!< \image html capsurf.png
				CapSurfaceHit,							//!< \image html capsurf2.png
				CurveBreakArrow,								//!< \image html cbreak.png
				CurveBreakHit,							//!< \image html cbreak2.png
				CurveExtendArrow,								//!< \image html ccvext.png
				CurveExtendHit,							//!< \image html ccvext2.png
				CurveFuseArrow,								//!< \image html ccvfuse.png
				CurveFuseHit,								//!< \image html ccvfuse2.png
				CurveRefineArrow,				//!< \image html ccvref.png
				CurveRefineHit,							//!< \image html ccvref2.png
				CurveMakeFirstArrow,								//!< \image html cfirst.png
				CurveMakeFirstHit,						//!< \image html cfirst2.png
				CurveMakeFirstHitAdd,							//!< \image html cfirstp.png
				ChamferAlternate,						//!< \image html chamferalt.png 
				ChamferAlternateHit,							//!< \image html chamfer2.png
				CurveJoinArrow,						//!< \image html cjoin.png
				CurveJoinHit,								//!< \image html cjoin2.png
				CurvePointArrow,								//!< \image html curvpt.png
				CurvePointHit,							//!< \image html curvpt2.png
				CurveCVCH,								//!< \image html cvcurve.png
				CurveIntersectArrow,									//!< \image html cvcvpt.png
				CurveIntersectHit,						//!< \image html cvcvpt2.png
				CVSurf,							//!< \image html cvsurf.png
				CosArrow,										//!< \image html cv_cos_acur.png
				CosHit,									//!< \image html cv_cos_icur.png
				ExtrudeAlternateArrow,										//!< \image html extrude.png
				ExtrudeAlternateHit,						//!< \image html extrude2.png
				FilletAlternateArrow,						//!< \image html fillet.png
				FilletAlternateHit,						//!< \image html fillet2.png
				FilletSurfArrow,							//!< \image html fil_srf.png
				FilletSurfHit,							//!< \image html fil_srf2.png
				CurveFitArrow,								//!< \image html fitcurv.png
				CurveFitHit,								//!< \image html fitcurv2.png
				FitPointArrow,								//!< \image html fitpt.png
				FitPointHit,								//!< \image html fitpt2.png
				LatheArrow,								//!< \image html lathe.png
				LatheHit,									//!< \image html lathe2.png
				MirrorCurveArrow,									//!< \image html mirrcur.png
				MirrorCurveHit,							//!< \image html mirrcur2.png
				NBLDArrow,								//!< \image html nbld_acur.png
				NBLDHit,									//!< \image html nbld_icur.png
				TrimNArrow,									//!< \image html ntrim_acur.png
				TrimNHit,									//!< \image html ntrim_icur.png
				OffCurveArrow,									//!< \image html offscur.png
				OffCurveHit,								//!< \image html offscur2.png
				Offset,								//!< \image html offset_s.png
				SurfOffArrow,										//!< \image html offsurf.png
				SurfOffHit,								//!< \image html offsurf2.png
				ExtendArrow,									//!< \image html pextend.png
				ExtendHit,								//!< \image html pextend2.png
				FuseArrow,									//!< \image html pfuse.png
				FuseHit,									//!< \image html pfuse2.png
				PointCosArrow,									//!< \image html pnt_cos_acur.png
				PointCosHit,								//!< \image html pnt_cos_icur.png
				PointRefineArrow,								//!< \image html prefcur.png
				PointRefineHit,							//!< \image html prefcur2.png
				SurfColumnArrow,								//!< \image html prefsco.png
				SurfColumnHit,							//!< \image html prefsco2.png
				SurfBothArrow,								//!< \image html prefsrc.png
				SurfBothHit,								//!< \image html prefsrc2.png
				SurfRowArrow,								//!< \image html prefsrw.png
				SurfRowHit,								//!< \image html prefsrw2.png
				ProjectNormArrow,									//!< \image html proj_norm_acur.png
				ProjectNormHit,							//!< \image html proj_norm_icur.png
				ProjectVectArrow,								//!< \image html proj_vect_acur.png
				ProjectVectHit,							//!< \image html proj_vec_icur.png
				Point,								//!< \image html pt.png
				PointCurve,										//!< \image html ptcurve.png
				PointSurf,									//!< \image html ptsurf.png
				RulSurfArrow,									//!< \image html rulsurf.png
				RulSurfHit,								//!< \image html rulsurf2.png
				SurfBreakBothArrow,									//!< \image html sbrkbth.png
				SurfBreakBothHit,							//!< \image html sbrkbth2.png
				SurfBreakColArrow,							//!< \image html sbrkcol.png
				SurfBreakColHit,							//!< \image html sbrkcol2.png
				SurfBreakRowArrow,							//!< \image html sbrkrow.png
				SurfBreakRowHit,							//!< \image html sbrkrow2.png
				SurfExtendArrow,							//!< \image html sextend.png
				SurfExtendHit,							//!< \image html sextend2.png
				SurfFuseCVArrow,								//!< \image html sfusecv.png
				SurfFuseCVHit,							//!< \image html sfusecv2.png
				SurfJoinArrow,								//!< \image html sjoin.png
				SurfJoinHit,								//!< \image html sjoin2.png
				SurfRefineBothArrow,								//!< \image html srefbth.png
				SurfRefineBothHit,						//!< \image html srefbth2.png
				SurfRefineColArrow,							//!< \image html srefcol.png
				SurfRefineColHit,							//!< \image html srefcol2.png
				SurfRefineRowArrow,							//!< \image html srefrow.png
				SurfRefineRowHit,							//!< \image html srefrow2.png
				Surf1RailArrow,							//!< \image html srf_1rail_acur.png
				Surf1RailHit,								//!< \image html srf_1rail_icur.png
				Surf2RailArrow,								//!< \image html srf_2rail_acur.png
				Surf2RailHit,								//!< \image html srf_2rail_icur.png
				SurfEdgeArrow,								//!< \image html srf_edge.png
				SurfEdgeHit,								//!< \image html srf_edge2.png
				SurfOffCurveArrow,								//!< \image html srf_off_crv_acrv.png
				SurfOffCurveHit,							//!< \image html srf_off_crv_icur.png
				SurfMirrorArrow,							//!< \image html surfmir.png
				SurfMirrorHit,							//!< \image html surfmir2.png
				SurfPointArrow,								//!< \image html surfpt.png
				SurfPointHit,								//!< \image html surfpt2.png
				SurfCurveArrow,								//!< \image html surf_crv_acur.png
				SurfCurveHit,								//!< \image html surf_crv_icur.png
				SurfSurfArrow,								//!< \image html surf_surf_acur.png
				SurfSurfHit,								//!< \image html surf_surf_icur.png
				TransCurveArrow,								//!< \image html trancur.png
				TransCurveHit,							//!< \image html trancur2.png
				Transform,								//!< \image html transfor.png
				TransSurfArrow,									//!< \image html transur.png
				TransSurfHit,								//!< \image html transur2.png
				UIsoArrow,								//!< \image html uiso.png
				UIsoHit,									//!< \image html uiso2.png
				ULoftArrow,									//!< \image html uloft.png
				ULoftHit,									//!< \image html uloft2.png
				UVLoftArrow,									//!< \image html uvlft_acur.png
				UVLoftHit,								//!< \image html uvlft_icur.png
				VIsoArrow,									//!< \image html viso.png
				VIsoHit,									//!< \image html viso2.png
				Xform,									//!< \image html xform.png
				North,										//!< \image html cursor_N.png
				NorthEast,										//!< \image html cursor_NE.png
				East,									//!< \image html cursor_E.png
				SouthEast,										//!< \image html cursor_SE.png
				South,									//!< \image html cursor_S.png
				SouthWest,										//!< \image html cursor_SW.png
				West,									//!< \image html cursor_W.png
				NorthWest,										//!< \image html cursor_NW.png
				Compass,									//!< \image html cursor_rot.png
				XHair,									//!< \image html crosshai.png
				Inset,									//!< \image html Inset.png
				SelectionPlus,							//!< \image html SelectionPlus.png
				Invalid						
		};

/*! \brief Function that loads 3dsMax Cursors, using an id as parameter and LoadMouseCursor function. (id consisting of an enum, to locate a cursor within an array).
 *
 */
		DllExport HCURSOR LoadMouseCursor(ECursors id);


	}
}


