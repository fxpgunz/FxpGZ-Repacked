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

#include "Renderer.h"

/// \defgroup Interface_ID Interface ID
//@{
#define SCANLINE_RENDERER_INTERFACE  Interface_ID(0x342323, 0x551665)
//@}

#define GetScanRendererInterface(obj) ((IScanRenderer2*)obj->GetInterface(SCANLINE_RENDERER_INTERFACE)) 

//--------------------------------------------------------------------------
// Interface into the default scanline renderer, Class_ID(SREND_CLASS_ID,0)
//---------------------------------------------------------------------------
class FilterKernel;
/*! 
This class provides an interface into the standard 3ds Max scanline renderer.
This renderer has a Class_ID of <b>Class_ID(SREND_CLASS_ID,0)</b>. All methods of this class are
implemented by the system.  
\see  Class Renderer.*/
class IScanRenderer: public Renderer {
	public:
	/*! Sets the 'Anti-Aliasing' setting of the renderer to the value
	passed.
	\par Parameters:
	<b>BOOL b</b>\n\n
	TRUE for on; FALSE for off. */
	virtual void SetAntialias(BOOL b) = 0;
	/*! Returns TRUE if anti-aliasing is on; FALSE if off. */
	virtual BOOL GetAntialias() = 0;

	/*! Sets the 'Filter Maps' setting to the value passed.
	\par Parameters:
	<b>BOOL b</b>\n\n
	TRUE for on; FALSE for off. */
	virtual void SetFilter(BOOL b) = 0;

	/*! Returns TRUE if Filter Maps is on; FALSE if off. */
	virtual BOOL GetFilter() = 0;

	/*! Sets the 'Shadows' setting to the value passed.
	\par Parameters:
	<b>BOOL b</b>\n\n
	TRUE for on; FALSE for off. */
	virtual void SetShadows(BOOL b) = 0;
	/*! Returns TRUE if the rendering of shadow is on; FALSE if off.
	*/
	virtual BOOL GetShadows() = 0;
	/*! Sets the 'Mapping' setting to the value passed.
	\par Parameters:
	<b>BOOL b</b>\n\n
	TRUE for on; FALSE for off. */
	virtual void SetMapping(BOOL b) = 0;
	/*! Returns TRUE if the Mapping setting is on; FALSE if off. */
	virtual BOOL GetMapping() = 0;
	/*! Sets the 'Force Wireframe' setting to the value passed.
	\par Parameters:
	<b>BOOL b</b>\n\n
	TRUE for on; FALSE for off. */
	virtual void SetForceWire(BOOL b) = 0;
	/*! Returns TRUE if Force Wire is on; FALSE if off. */
	virtual BOOL GetForceWire() = 0;
	/*! Sets the 'Auto-Reflect/Refract and Mirrors' setting to the
	value passed.
	\par Parameters:
	<b>BOOL b</b>\n\n
	TRUE for on; FALSE for off. */
	virtual	void SetAutoReflect(BOOL b)=0;
	/*! Returns TRUE if auto reflect is on; FALSE if off. */
	virtual	BOOL GetAutoReflect()=0;


	/*! Sets Object Motion Blur to on or off.
	\par Parameters:
	<b>BOOL b</b>\n\n
	TRUE for on; FALSE for off. */
	virtual void SetObjMotBlur(BOOL b) = 0;

	/*! Returns TRUE if object motion blur is on; FALSE if off. */
	virtual BOOL GetObjMotBlur() = 0;


	/*! Sets Velocity Motion Blur to on or off.
	\par Parameters:
	<b>BOOL b</b>\n\n
	TRUE for on; FALSE for off. */
	virtual void SetVelMotBlur(BOOL b) = 0;

	/*! Returns TRUE if velocity motion blur is on; FALSE if off. */
	virtual BOOL GetVelMotBlur() = 0;

	//! This method is deprecated in 3ds Max 2017. It was previously flagged as obsolete, for over 15 years.
    MAX_DEPRECATED void SetPixelSize(float size);
	
	/*! Sets the Auto Reflect/Refract Maps Rendering Iterations
	setting to the value passed.
	\par Parameters:
	<b>int n</b>\n\n
	The value to set in the range of 1 to 10. */
	virtual void SetAutoReflLevels(int n) = 0;
	/*! Sets the ''Wire Thickness' setting to the value passed.
	\par Parameters:
	<b>float t</b>\n\n
	The value to set in the range of 0.0 to 10000.0. */
	virtual void SetWireThickness(float t) = 0;

	virtual void SetObjBlurDuration(float dur) = 0;
	virtual void SetVelBlurDuration(float dur) = 0;
	/*! Sets the Motion Blur Duration setting to the value passed.
	\par Parameters:
	<b>int n</b>\n\n
	The value to set in the range of 1 to 16. */
	virtual void SetNBlurFrames(int n) = 0;
	/*! Sets the Object Motion Blur Samples setting to the value
	passed.
	\par Parameters:
	<b>int n</b>\n\n
	The value to set in the range of 1 to 16. */
	virtual void SetNBlurSamples(int n) = 0;


	/*! Sets the '3ds Max Quadtree Depth' setting for Ray-Traced
	shadows to the value passed.
	\par Parameters:
	<b>int n</b>\n\n
	The value to set in the range of 0-10. */
	virtual void SetMaxRayDepth(int n) = 0;
	/*! \remarks	Returns the maximum ray depth. */
	virtual int GetMaxRayDepth() { return 7; }


	/*! \remarks	Sets the anti-aliasing filter to the one passed.
	\par Parameters:
	<b>FilterKernel *pKernel</b>\n\n
	Points to the filter to use. See Class FilterKernel. */
	virtual void SetAntiAliasFilter( FilterKernel * pKernel ) = 0;

	/*! \remarks	Returns a pointer to the anti-aliasing filter in use. See
	Class FilterKernel. */
	virtual FilterKernel * GetAntiAliasFilter() = 0;

	/*! \remarks	Sets the anti-aliasing filter size to the given value.
	\par Parameters:
	<b>float size</b>\n\n
	The size to set. */
	virtual void SetAntiAliasFilterSz(float size) = 0;

	/*! \remarks	Returns the anti-aliasing filter size. */
	virtual float GetAntiAliasFilterSz() = 0;


	/*! \remarks	Sets if sampling is enabled or disabled.
	\par Parameters:
	<b>BOOL on</b>\n\n
	TRUE for enabled (on); FALSE for disabled (off). */
	virtual void SetPixelSamplerEnable( BOOL on ) = 0;

	/*! \remarks	Returns TRUE if anti-aliasing is on; FALSE if off. */
	virtual BOOL GetPixelSamplerEnable() = 0;
};

//--------------------------------------------------------------------------
// Extended Interface into the default scanline renderer, Class_ID(SREND_CLASS_ID,0)
//---------------------------------------------------------------------------
class IScanRenderer2: public IScanRenderer, public FPMixinInterface {
	public:

	enum {  get_mapping, set_mapping,  
			get_shadows, set_shadows,  
			get_autoReflect, set_autoReflect,  
			get_forceWire, set_forceWire,  
			get_antialias, set_antialias,  
			get_filter, set_filter,  
			get_objMotBlur, set_objMotBlur,  
			get_velMotBlur, set_velMotBlur,  
			get_applyVelBlurEnv, set_applyVelBlurEnv,  
			get_velBlurTrans, set_velBlurTrans,  
			get_memFrugal, set_memFrugal,  
			get_pixelSamplerEnable, set_pixelSamplerEnable,  
			get_wireThickness, set_wireThickness,  
			get_objBlurDuration, set_objBlurDuration,  
			get_velBlurDuration, set_velBlurDuration,  
			get_antiAliasFilterSz, set_antiAliasFilterSz,  
			get_NBlurSamples, set_NBlurSamples,  
			get_NBlurFrames, set_NBlurFrames,  
			get_autoReflLevels, set_autoReflLevels,  
			get_colorClampType, set_colorClampType,  
			get_antiAliasFilter, set_antiAliasFilter,  
			get_enableSSE, set_enableSSE,  
			get_globalSamplerEnabled, set_globalSamplerEnabled, 
			get_globalSamplerClassByName, set_globalSamplerClassByName,
			get_globalSamplerSampleMaps, set_globalSamplerSampleMaps, 
			get_globalSamplerQuality, set_globalSamplerQuality, 
			get_globalSamplerAdaptive, set_globalSamplerAdaptive, 
			get_globalSamplerAdaptiveThresh, set_globalSamplerAdaptiveThresh,
			get_globalSamplerParam1, set_globalSamplerParam1,
			get_globalSamplerParam2, set_globalSamplerParam2,
		};

	//Function Map For Mixin Interface
	//*************************************************
	#pragma warning(push)
	#pragma warning(disable:4238)
	BEGIN_FUNCTION_MAP
		PROP_FNS(get_mapping, GetMapping,						set_mapping, SetMapping, TYPE_BOOL);
		PROP_FNS(get_shadows, GetShadows,						set_shadows, SetShadows, TYPE_BOOL);
		PROP_FNS(get_autoReflect, GetAutoReflect,				set_autoReflect, SetAutoReflect, TYPE_BOOL);
		PROP_FNS(get_forceWire, GetForceWire,					set_forceWire, SetForceWire, TYPE_BOOL);
		PROP_FNS(get_antialias, GetAntialias,					set_antialias, SetAntialias, TYPE_BOOL);
		PROP_FNS(get_filter, GetFilter,							set_filter, SetFilter, TYPE_BOOL);
		PROP_FNS(get_objMotBlur, GetObjMotBlur,					set_objMotBlur, SetObjMotBlur, TYPE_BOOL);
		PROP_FNS(get_velMotBlur, GetVelMotBlur,					set_velMotBlur, SetVelMotBlur, TYPE_BOOL);
		PROP_FNS(get_applyVelBlurEnv, GetApplyVelBlurEnv,		set_applyVelBlurEnv, SetApplyVelBlurEnv, TYPE_BOOL);
		PROP_FNS(get_velBlurTrans, GetVelBlurTrans,				set_velBlurTrans, SetVelBlurTrans, TYPE_BOOL);
		PROP_FNS(get_memFrugal, GetMemFrugal,					set_memFrugal, SetMemFrugal, TYPE_BOOL);
		PROP_FNS(get_pixelSamplerEnable, GetPixelSamplerEnable,	set_pixelSamplerEnable, SetPixelSamplerEnable, TYPE_BOOL);

		PROP_FNS(get_wireThickness, GetWireThickness,			set_wireThickness, SetWireThickness, TYPE_FLOAT);
		PROP_FNS(get_objBlurDuration, GetObjBlurDuration,		set_objBlurDuration, SetObjBlurDuration, TYPE_FLOAT);
		PROP_FNS(get_velBlurDuration, GetVelBlurDuration,		set_velBlurDuration, SetVelBlurDuration, TYPE_FLOAT);
		PROP_FNS(get_antiAliasFilterSz, GetAntiAliasFilterSz,	set_antiAliasFilterSz, SetAntiAliasFilterSz, TYPE_FLOAT);

		PROP_FNS(get_NBlurSamples, GetNBlurSamples,				set_NBlurSamples, SetNBlurSamples, TYPE_INT);
		PROP_FNS(get_NBlurFrames, GetNBlurFrames,				set_NBlurFrames, SetNBlurFrames, TYPE_INT);
		PROP_FNS(get_autoReflLevels, GetAutoReflLevels,			set_autoReflLevels, SetAutoReflLevels, TYPE_INT);
		PROP_FNS(get_colorClampType, GetColorClampType,			set_colorClampType, SetColorClampType, TYPE_INT);

		PROP_FNS(get_antiAliasFilter, GetAntiAliasFilter,		set_antiAliasFilter, SetAntiAliasFilterRT, TYPE_REFTARG);

		PROP_FNS(get_enableSSE, IsSSEEnabled,					set_enableSSE, SetEnableSSE, TYPE_BOOL);

		PROP_FNS(get_globalSamplerEnabled, GetGlobalSamplerEnabled,					set_globalSamplerEnabled, SetGlobalSamplerEnabled, TYPE_BOOL); 
		PROP_FNS(get_globalSamplerClassByName, GetGlobalSamplerClassByName,			set_globalSamplerClassByName, SetGlobalSamplerClassByName, TYPE_TSTR_BV);
		PROP_FNS(get_globalSamplerSampleMaps, GetGlobalSamplerSampleMaps,			set_globalSamplerSampleMaps, SetGlobalSamplerSampleMaps, TYPE_BOOL);
		PROP_FNS(get_globalSamplerQuality, GetGlobalSamplerQuality ,				set_globalSamplerQuality, SetGlobalSamplerQuality, TYPE_FLOAT);
		PROP_FNS(get_globalSamplerAdaptive, GetGlobalSamplerAdaptive,				set_globalSamplerAdaptive, SetGlobalSamplerAdaptive, TYPE_BOOL);
		PROP_FNS(get_globalSamplerAdaptiveThresh, GetGlobalSamplerAdaptiveThresh,	set_globalSamplerAdaptiveThresh, SetGlobalSamplerAdaptiveThresh, TYPE_FLOAT);
		PROP_FNS(get_globalSamplerParam1, GetGlobalSamplerParam1,					set_globalSamplerParam1, SetGlobalSamplerParam1, TYPE_FLOAT);
		PROP_FNS(get_globalSamplerParam2, GetGlobalSamplerParam2,					set_globalSamplerParam2, SetGlobalSamplerParam2, TYPE_FLOAT);

	END_FUNCTION_MAP
	#pragma warning(pop)

	FPInterfaceDesc* GetDesc();    // <-- must implement 
		//**************************************************

	void SetAntiAliasFilterRT(ReferenceTarget* op) {
		if (op && op->SuperClassID( ) == FILTER_KERNEL_CLASS_ID)
			SetAntiAliasFilter(static_cast< FilterKernel* >(op));
	}

	virtual float GetWireThickness() = 0;
	virtual void SetColorClampType (int i) = 0;
	virtual int GetColorClampType () = 0;
	virtual float GetObjBlurDuration() = 0;
	virtual int GetNBlurSamples() = 0;
	virtual int GetNBlurFrames() = 0;
	virtual float GetVelBlurDuration() = 0;
	virtual void SetApplyVelBlurEnv(BOOL b) = 0;
	virtual BOOL GetApplyVelBlurEnv() = 0;
	virtual void SetVelBlurTrans(BOOL b) = 0;
	virtual BOOL GetVelBlurTrans() = 0;
	virtual int GetAutoReflLevels() = 0;
	virtual void SetMemFrugal(BOOL b) = 0;
	virtual BOOL GetMemFrugal() = 0;
	virtual void SetEnableSSE(BOOL b) = 0;
	virtual BOOL IsSSEEnabled() = 0;

	virtual BOOL GetGlobalSamplerEnabled() = 0;
	virtual void SetGlobalSamplerEnabled(BOOL enable) = 0;
	virtual MSTR GetGlobalSamplerClassByName() = 0;
	virtual void SetGlobalSamplerClassByName(const MSTR) = 0;
	virtual BOOL GetGlobalSamplerSampleMaps() = 0;
	virtual void SetGlobalSamplerSampleMaps(BOOL enable) = 0;
	virtual float GetGlobalSamplerQuality() = 0;
	virtual void SetGlobalSamplerQuality(float f) = 0;
	virtual BOOL GetGlobalSamplerAdaptive() = 0;
	virtual void SetGlobalSamplerAdaptive(BOOL enable) = 0;
	virtual float GetGlobalSamplerAdaptiveThresh() = 0;
	virtual void SetGlobalSamplerAdaptiveThresh(float f) = 0;
	virtual float GetGlobalSamplerParam1() = 0;
	virtual void SetGlobalSamplerParam1(float f) = 0;
	virtual float GetGlobalSamplerParam2() = 0;
	virtual void SetGlobalSamplerParam2(float f) = 0;
};