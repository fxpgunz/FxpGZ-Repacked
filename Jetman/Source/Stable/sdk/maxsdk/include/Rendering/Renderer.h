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

#include "../MaxHeap.h"
#include "../Object.h"

#pragma warning(push)
#pragma warning(disable:4100)

class RendParamDlg;
class IRendParams;
class ITargetedIO;
class IInteractiveRender;

/*!
Describes a default light. An array of these default lights is
passed into the method Renderer::Open(). \n\n
Note: In 3ds Max 3.0 the production renderer has been modified so that if a
DefaultLight is passed into Renderer::Open() with a transformation
matrix that is all zeros, the renderer will interpret this to mean that on each
frame it should create a light located at the view point, pointing in the view
direction. This allows the implementation of the new viewport 1-light option so
that it tracks the camera during an animated camera move.
\see  Class Renderer, Class Matrix3, Structure LightState.
*/
class DefaultLight : public MaxHeapOperators
{
public:
    LightState ls;	//!< Describes the properties of the light.
    Matrix3 tm;		//!< The transformation of the light that controls its position in the scene.
};


//==============================================================================
// class Renderer
//
//! \brief This is the base class for any renderer plugin.
/*! 
The main entry points for this class are methods Open(), Render(), and Close().
Any render operation works as follows:
1. Open() is called exactly once.
2. Render() is called zero or more times.
3. Close() is called exactly once.
*/
/*! \sa  Class ReferenceTarget, Class FrameRendParams, Class RendProgressCallback, Class IRendParams, Class INode, Class ViewParams, Class RendParamDlg, Class RendParams, Class DefaultLight.\n\n
\par Description:
This is the base class for the creation of plug-in renderers. There are five
methods that need to be implemented: Open(), Render(),
Close(), CreateParamDialog() and ResetParams().
*/
class Renderer : public ReferenceTarget
{
public:

	//! \brief Returns the super class ID RENDERER_CLASS_ID.
	SClass_ID SuperClassID() {return RENDERER_CLASS_ID;}
	
	//! \brief Called once and only once per render operation; used to initialize the renderer
	//! before rendering a sequence of frames.
	/*! This gives a chance to the renderer to build any data structures which it will need
		in the rendering phase. If this call returns 0, then the caller is not required to call
		Close(), so Open() should do any necessary cleanup before returning 0.
		\param[in] scene - The root node of the scene to render. Note: If you are rendering 
		in the Materials Editor, you'll instead get a pointer to the INode that is in the 
		sample slot - not the root node of the scene.
		\param[in] vnode - The view node. This may be a camera, a light, or NULL.
		\param[in] viewPar - View parameters for rendering orthographic or user viewports. 
		This is used if vnode is NULL.
		\param[in] rp - This class contains a set of common renderer parameters.
		\param[in] hwnd - The owner window for messages.
		\param[in] defaultLights - An array of default lights if there are no user created lights in the scene.
		\param[in] numDefLights - Number of lights in defaultLights array.
		\param[in] prog - A callback used to allow the renderer to update the progress dialog.
		\return Nonzero for success, zero for failure.
	*/
	virtual int Open(
		INode *scene,
		INode *vnode,
		ViewParams *viewPar,
		RendParams &rp,
		HWND hwnd,
		DefaultLight* defaultLights=NULL,
		int numDefLights=0,
		RendProgressCallback* prog = NULL
	) = 0;
	
	//! \brief Called to render a single frame.
	/*! This may be called zero or more times, in between calls to Open() and Close(), to
		render a sequence of frames, at a series of time values which are not necessarily
		sequential.
		\param[in] t - The time at which this frame is to be rendered.
		\param[in] tobm - The bitmap to which the image is to be rendered.
		\param[in] frp - A set of frame dependent parameters.
		\param[in] hwnd - The owner window handle.
		\param[in] prog - A callback used to allow the renderer to update the progress dialog.
		\param[in] viewPar - This parameter allows one to specify a different view transformation 
		on each render call. For example, one may render a given scene at a given time from many 
		different viewpoints, without calling Render::Open() for each one.
		\return Nonzero for success, zero for failure.
	*/
	virtual int Render(
		TimeValue t,
   		Bitmap *tobm,
		FrameRendParams &frp,
		HWND hwnd,
		RendProgressCallback *prog=NULL,
		ViewParams *viewPar=NULL
	)=0;

	//! \brief Called once and only once per render operation; used to free any resources
	//! allocated by Open() or Render().
	/*! This method needs to be called whenever Open() was called and returned a non-zero value.
		The renderer should free any allocated resources, returning in a state identical to the one
		before Open() was called.
		\param[in] hwnd - The owner window handle.
		\param[in] prog - A callback used to allow the renderer to update the progress dialog.
		\return Nonzero for success, zero for failure.
	*/
	virtual void Close(	HWND hwnd, RendProgressCallback* prog = NULL )=0;	

	//! \brief This method is called to apply the render effects at the specified time value. 
	/*! It should be called between the Open() and Close()
	methods.\n\n
	This can be used during a multi-pass rendering, in order to apply the
	render effects to the final, blended bitmap.
	\par Parameters:
	\param t - The time to apply the render effects.
	\param pBitmap - Points to the bitmap.
	\param updateDisplay - Passing true indicates that Bitmap's display should be refreshed by the
		renderer; false indicates it should not be.
	\return  Returns true if the effects were successfully applied; otherwise false. */
	virtual bool ApplyRenderEffects(TimeValue t, Bitmap* pBitmap, bool updateDisplay=true) { return false; }

	//! \brief This method is called to create and return a pointer to an instance of the
	//! RendParamDlg class.
	/*! The renderer can add rollup page(s) to the renderer configuration dialog 
	using the IRendParams interface passed into this method.
	\param ir - An interface that provides methods for use in displaying parameters, for
	example this class has methods for adding rollup pages.
	\param prog - If TRUE then the rollup page should just display the parameters so the user
	has them for reference while rendering, they should not be editable.
	\return  A pointer to an instance of the <b>RendParamDlg</b> class. This
	class will be deleted using RendParamDlg::DeleteThis(). */
	virtual RendParamDlg *CreateParamDialog(IRendParams *ir,BOOL prog=FALSE)=0;

	//! This method simply sets all the parameters to their default values.
	virtual void ResetParams()=0;
	virtual int	GetAAFilterSupport(){ return 0; } // point sample, no reconstruction filter

	//! \brief Renderers which support texture baking should override this method to return true. 
	/*! It is checked when the Render to Texture dialog is opened;
		if the current renderer does not support texture baking, then a warning message 
		is displayed and the user will not be able to press the Render button in the dialog.
		\return By default this will return false */
	virtual bool SupportsTexureBaking() { return false; }

	//! \brief A renderer should override this method to return true if it supports any custom preset categories beyond the standard categories.
	/*! \return By default this will returns false */
	virtual bool SupportsCustomRenderPresets() { return false; }

	//! \brief Return a number indicating the current version of the renderer's custom preset.
	/*! The number can be arbitrary 
		\return By default this will return -1 */
	virtual int  RenderPresetsFileVersion() { return -1; }

	//! \brief Return true if the renderer can load presets of the indicated version.
	/*! \param version The version to test compatibility against
		\return By default this will return false. */
	virtual BOOL RenderPresetsIsCompatible( int version ) { return false; }

	//! \brief Returns the UI name of a supported custom category
	/*! \param catIndex - The custom preset index to return the name of.  catIndex will be a number between 
		RENDER_PRESETS_CUSTOM_CATEGORY_INDEX_BEGIN and RENDER_PRESETS_CATEGORY_COUNT.
		\return If the ID is a supported custom category, return the name of the category to be displayed in the UI. Otherwise return NULL */
	virtual const MCHAR* RenderPresetsMapIndexToCategory( int catIndex ) { return NULL; }

	//! \brief Returns the index of a supported custom category
	/*! \param category - The UI name of a custom category
		\return If the input is the name of a custom category supported by the renderer, return the ID number of the category. Otherwise returns 0 */
	virtual int RenderPresetsMapCategoryToIndex( const MCHAR* category )  { return 0; }

	//! \brief called before a render preset is saved. 
	/*! For each custom category supported use root->AddSaveTarget() passing the object with the 
		parameters for that category if the corresponding bit is set in the saveCategories, 
		The object will be saved along with the preset. 
		\par Example Implementation:
		\code
		{
			// Iterate through all our possible custom preset indices
			for (int i = RENDER_PRESETS_CUSTOM_CATEGORY_INDEX_BEGIN; i < RENDER_PRESETS_CUSTOM_CATEGORY_INDEX_BEGIN + MY_CUSTOM_PRESET_COUNT; i++)
			{
				// Should the preset be saved?
				if (saveCategories[i])
				{
					// p_myOptions is a class derived from ReferenceTarget that contains our options.
					root->AddSaveTarget(i, p_myOptions);
				}
			}
		}
		\endcode
		\param root An instance of an ITargetedIO class to be used to save any custom presets specified
		\param saveCategories Species the custom preset categories to be saved	*/
	virtual int RenderPresetsPreSave( ITargetedIO * root, BitArray saveCategories ) { return -1; }

	//! \brief called after a preset is saved. 
	/*! No specific action is required from the renderer at this time.
		\sa RenderPresetsPreSave */
	virtual int RenderPresetsPostSave( ITargetedIO * /*root*/, BitArray /*loadCategories*/ ) { return -1; }

	//! \brief called before a preset is loaded. 
	/*! For each custom category supported, if the corresponding bit is not set
		in the loadCategories, use root->Store() passing the object with the parameters for that category. The object 
		will be preserved, so the renderer can refer to it after the preset is loaded.
		\sa RenderPresetsPreSave, RenderPresetsPostLoad
		\param root An instance of an ITargetedIO class to be used to store any custom presets specified
		\param saveCategories Lists the custom preset categories to be loaded.  Any categories not being loaded should
			be stored on root. */
	virtual int RenderPresetsPreLoad( ITargetedIO * root, BitArray saveCategories ) { return -1; }

	//! \brief called after a preset is loaded. 
	/*! For each custom category supported...\n\n
		If the bit is set in the loadCategories: use root->GetSaveTarget() to retrieve the loaded object, 
		and update the renderer's active parameters to match this object\n\n
		If the bit is not set in the loadCategories: use root->Retrieve() to retrieve the object that was
		stored during pre-load. Update the renderer's active parameters to match this object. 
		This is important in case a certain category was held in the file and loaded, but the user did 
		not choose to load that category. In this case the renderer must restore its parameters to their former value. 
		\sa RenderPresetsPreLoad
		\param root An instance of an ITargetedIO class to be used to retrieve any custom presets stored pre-load.
		\param loadCategories Lists the custom preset categories that have been loaded. */
	virtual int RenderPresetsPostLoad( ITargetedIO * root, BitArray loadCategories ) { return -1; }

    //! \name Stopping and Pausing a Render
    //! See \ref stopping_pausing_renderer
    /*! \page stopping_pausing_renderer Stopping and pausing a renderer
        <b>Stopping functionality</b> is meant for progressive renderers which may be stopped manually at any time. Renderers such as path tracers
        may be set to render indefinitely, in which case the only way to stop rendering is through this functionality. Stopping a renderer differs
        from canceling/aborting in that a stopped render is considered successful, finished, complete. Stopping a progressive renderer is equivalent
        to reaching a pre-set termination criterion, such as time or number of iterations. Following a stop, the rendered image will be saved, the
        render elements will be processed, and the next frame of the animation sequence will be rendered - all things that do not happen upon canceling
        a render.

        <b>Pausing functionality</b> enables a user to temporarily interrupt the rendering process, freeing computer resources to perform other tasks.
        3ds Max has long supported the pausing of renderers through a wait loop in the call to RendProgressCallback:Progress(), however this functionality
        is not well suited for all renderers. These methods below supersede the legacy functionality; a renderer that supports pausing through this
        interface will not see the progress callback perform a wait loop when the renderer is paused. */
    //@{
    /*! Returns true if and only the renderer supports the stop functionality, in which case the Stop button will be displayed in the render progress
        dialog, and StopRendering() may be called. 
        \remark This method may only be called from the main thread. 
        \sa \ref stopping_pausing_renderer */
    virtual bool IsStopSupported() const = 0;
    /*! Called to instruct the renderer to stop rendering. The renderer is expected to stop rendering at its earliest convenience. If, for example, 
        the renderer is in the process of rendering an iteration, it may finish rendering this iteration (or not), as it wishes - so long as 
        it outputs a valid rendered image representing the progress rendered so far.
        \remark \li This method may only be called from the main thread. 
                \li The renderer should take special care of supporting a stop while the renderer is paused; the user should not need to manually resume
                    for a stop to function properly. 
                \li This method is asynchronously called, and should return immediately; it must not wait for the rendering process to actually be stopped.  
        \sa \ref stopping_pausing_renderer */
    virtual void StopRendering() = 0;
    /*! Returns whether pause is supported by this renderer.
        \remark \li This method may only be called from the main thread.
        \sa \ref stopping_pausing_renderer */
    enum class PauseSupport
    {
        //! This renderer does not support pausing, in which case the Pause button will be disabled or hidden.
        None,
        //! Full support for pausing, in which case pressing the Pause button will result in PauseRendering() being called.
        Full,
        //! Legacy support for pausing, which involves the progress callback hanging to forcibly pause the renderer. This only
        //! works if the renderer uses the progess callback in a synchronous manner. PauseRendering() may still be called.
        Legacy
    };
    virtual PauseSupport IsPauseSupported() const = 0;
    /*! Requests that the renderer pauses itself, at which point it should stop rendering and free CPU resources until a resume is requested.
        \remark \li This method may only be called from the main thread. 
                \li Abort or Stop events may be received while the renderer is paused. Those should be processed correctly. 
                \li It is possible for this method to be called while the renderer is already paused. The implementation should handle that correctly.
        \sa \ref stopping_pausing_renderer */
    virtual void PauseRendering() = 0;
    /*! Requests that the renderer resumes rendering, following a call to PauseRendering().
        \remark \li This method may only be called from the main thread. 
                \li It is possible for this method to be called while the renderer is not currently paused. The implementation should handle that correctly.
        \sa \ref stopping_pausing_renderer */
    virtual void ResumeRendering() = 0;
    //@}

    //! \name Renderer Requirements
    //! Renderer requirements are special flags which the application may query to determine whether a renderer supports certain features,
    //! or to modify the behaviour of the application depending no the renderer's needs.
    //@{
    enum Requirement {
        // This flag is deprecated in 3ds Max 2017. It is replaced by Renderer::IsPauseSupported()
		//kRequirement_NoPauseSupport = 0,
		/** Indicates that the VFB shouldn't be popped-up after rendering, even if 
		 * "Show VFB" is ON in the common render parameters. This is useful for
		 * renderers which generate something other than an image.
		 * Note that this also affects render element VFBs.*/
		kRequirement_NoVFB = 1,
		/** Indicates that the rendered image shouldn't be saved after rendering, even
		 * if a file was specified in the common render parameters. This is useful for
		 * renderers which generate something other than an image.
		 * Note that this also affects render element outputs.*/
		kRequirement_DontSaveRenderOutput = 2,
		/** Indicates the renderer wants a 32bit floating-point RGBA frame buffer to be
		 * created for output. The render executer will query the renderer and will create a
		 * 32bit floating-point frame buffer, instead of a 16bit integer buffer, if the
		 * renderer returns true for this requirement.
		 * Note that there is no guarantee about the frame buffer type: even if the renderer
		 * returns true for this requirement, a 16bit integer buffer could still be created.*/
		kRequirement_Wants32bitFPOutput = 3,
		/** Indicates the renderer wants an object selection for rendering. 
		 * The render executer will throw an error message when the renderer indicates
		 * this requirement, and no objects are selected. */
		kRequirement_WantsObjectSelection = 4,
        /** Flags the renderer as not supporting GBuffers when processing the exposure control preview. By default, the exposure control preview
         * uses GBuffers to re-compose the image whenever the exposure parameters change. This enables it to effectively re-compose the background 
         * into the rendered image with good fidelity. But should this requirement be set, the exposure control preview will switch to an alternate
         * method which uses the alpha channel to extract the background from the rendered image. The results will not be as good but are generally
         * sufficient for the sake of previewing. */
        kRequirement_NoGBufferForToneOpPreview = 5,
        /** Enables the concurrent/parallel rendering of ActiveShade and material editor previews for this renderer. */
        kRequirement_SupportsConcurrentRendering = 6,
	};
	//! Returns true if the renderer has the given requirement, or returns false otherwise.
	virtual bool HasRequirement(Requirement requirement) = 0;
    //@}

    //! \name Renderer Element Support
    //@{
    //! Returns whether this renderer plugin is compatible with any render elements whatsoever.
    //! This affects whether the render elements tab is displayed at all.
    virtual bool CompatibleWithAnyRenderElement() const = 0;
    //! Returns whether this renderer plugin is compatible with the given render element.
    //! \param pIRenderElement The render element for which to verify compatibility.
    virtual bool CompatibleWithRenderElement(IRenderElement& pIRenderElement) const = 0;
    //@}

    //! \name Interactive Rendering (Active Shade)
    //@{
    //! \brief Returns the interface used for interactive rendering.
    /*! This method returns a pointer to an IInteractiveRender, the interface used to perform interactive rendering
        (aka Active Shade). Ownership of the interface remains with the Renderer, i.e. the Renderer is responsible
        for freeing this interface, if necessary, in its destructor. Consequently, every call to this method should
        return the same pointer value - there is a one-to-one association between classes IInteractiveRenderer and Renderer.
        \remark Should return null if interactive rendering is not supported by the renderer. */
    virtual IInteractiveRender* GetIInteractiveRender() = 0;
    //@}

    //! \name Renderer Information
    //@{
    //! \brief Get renderer name, provider, version, etc. 
    //! \remarks If the string is longer than 32-character, it will be trunked on Render Message Window
    //! \param info The returned string containing the vendor info.
    virtual void GetVendorInformation(MSTR& info) const = 0;
    //! \brief Platform information concerning the renderer. 
    //! \remarks It could be information of OS, CPU, GPU, etc. 
    //! If the string is longer than 32-character, it will be trunked on Render Message Window. 
    //! \param info The returned string containing the platform info.
    virtual void GetPlatformInformation(MSTR& info) const = 0;
    //@}
};

#pragma warning(pop)