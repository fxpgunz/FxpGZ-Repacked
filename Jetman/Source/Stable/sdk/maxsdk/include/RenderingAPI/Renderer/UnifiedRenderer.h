//////////////////////////////////////////////////////////////////////////////
//  Copyright 2014 by Autodesk, Inc.  All rights reserved.
//
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.
//////////////////////////////////////////////////////////////////////////////
#pragma	once

#include "../RenderingAPI.h"

// Max SDK
#include <iparamb2.h>
#include <render.h>
#include <IMtlRender_Compatibility.h>
#include <ITabDialog.h>
#include <rendering/IStoppableRenderer.h>
#include "IRenderSessionContext.h"
// std
#include <memory>

#pragma warning(push)
#pragma warning(disable: 4275 4251)

namespace MaxSDK
{;
namespace RenderingAPI
{;

class IOfflineRenderSession;
class IInteractiveRenderSession;
class IRenderSessionContext;

//==================================================================================================
// class UnifiedRenderer
//
/*! Unifies functionality for, both, interactive (ActiveShade) and offline (Production) rendering.

    This is the base class for any renderer plugin that wishes to use the Rendering API - which, ideally, should be the case for all renderer
    plugins going forward. This class implements most of the core rendering functionality, which is common to all renderers, and exposes a simplified
    mechanism based around <i>render sessions</i>. The responsibilities are as follows:
        \li class UnifiedRenderer: file I/O, user interface, and misc. application-side behaviour.
        \li class IOfflineRenderSession: rendering for offline (non-interactive, Production) purposes; render-engine-side behaviour.
        \li class IInteractiveRenderSession: rendering for interactive (ActiveShade) purposes; render-engine-side-behaviour.    

    Many of the methods, inherited from class Renderer and others, are made explicitly <i>final</i>, as overriding them would jeopardize the 
    foundations of this class' implementation. Other methods are made explicitly pure virtual to enforce their implementation by the plugin.

    \remark The <i>class descriptor</i> of a plugin deriving from UnifiedRenderer must derive from UnifiedRenderer::ClassDescriptor.
*/
class RenderingAPIExport UnifiedRenderer :	
    public Renderer,
    public ITabDialogObject
{
public:

    class ClassDescriptor;

	UnifiedRenderer();
	virtual ~UnifiedRenderer();

    /*! Returns a reference to the class descriptor that defines this renderer plugin.
        The class descriptor must inherit from class UnifiedRenderer::ClassDescriptor, and must be implemented as a singleton. */
    virtual ClassDescriptor& GetClassDescriptor() const = 0;

    //! \name Render Session Creation
    //@{    
    /*! Creates a new render session, for offline (non-interactive) rendering.
        \param sessionContext The context in which the render session operates. This reference is guaranteed to be valid for the lifetime of the
            session, but may not be used beyond this.
        \remark As the render session is returned through a std::unique_ptr, the caller takes ownership of the session and will destroy it
            once it is no longer needed.
        \return A new offline render session, or null to abort the render operation entirely. */
    virtual std::unique_ptr<IOfflineRenderSession> CreateOfflineSession(IRenderSessionContext& sessionContext) = 0;
    /*! Creates a new render session, for interactive (ActiveShade) rendering.
        \param sessionContext The context in which the render session operates. This reference is guaranteed to be valid for the lifetime of the
            session, but may not be used beyond this.
        \remark As the render session is returned through a std::unique_ptr, the caller takes ownership of the session and will destroy it
            once it is no longer needed.
        \remark If interactive rendering is <b>not supported</b> by the renderer plugin, then this method should return null. 
        \return A new interactive render session, or null to abort the render operation entirely.
        \see SupportsInteractiveRendering() */
    virtual std::unique_ptr<IInteractiveRenderSession> CreateInteractiveSession(IRenderSessionContext& sessionContext) = 0;
    /*! Returns whether this renderer supports interactive, that is: if an interactive render session can be created with CreateInteractiveSession().
        This affects whether this renderer plugin will be offered as a choice of ActiveShade renderer.
        \see CreateInteractiveSession() */
    virtual bool SupportsInteractiveRendering() const = 0;
    //@}

    //! \name File I/O
    //@{    
    /*! Replaces ReferenceMaker::Load() for loading plugin-specific data.
        ReferenceMaker::Load() is overridden and marked <i>final</i> to enforce that UnifiedRenderer be loaded in a certain way. This method is called
        by the implementation of Load() an should behave in an identical manner to ReferenceMaker::Load().
        \see ReferenceMaker::Load() */
    virtual IOResult Load_UnifiedRenderer(ILoad& iload) = 0;
    /*! Replaces ReferenceMaker::Save() for loading plugin-specific data.
        ReferenceMaker::Save() is overridden and marked <i>final</i> to enforce that UnifiedRenderer be saved in a certain way. This method is called
        by the implementation of Save() an should behave in an identical manner to ReferenceMaker::Save().
        \see ReferenceMaker::Save() */
    virtual IOResult Save_UnifiedRenderer(ISave& isave) const = 0;
    //@}

    //! \name Interface Access
    //@{        
    /*! Replaces Animatable::GetInterface(ULONG), which is implemented as <i>final</i> by UnifiedRenderer to enforce correct behaviour.
        \see Animatable::GetInterface(ULONG) */
    virtual void* GetInterface_UnifiedRenderer(ULONG id) = 0;
    /*! Replaces InterfaceServer::GetInterface(Interface_ID), which is implemented as <i>final</i> by UnifiedRenderer to enforce correct behaviour.
        \see InterfaceServer::GetInterface(Interface_ID) */
    virtual BaseInterface* GetInterface_UnifiedRenderer(Interface_ID id) = 0;
    //@}

    //! \name Scene Options
    //@{        
    /*! Enables the renderer to override the behaviour of per-object motion blur options.
        class INode exposes properties to control how motion blur is to be processed for that object: image based, object based, or disabled entirely. 
        Certain renderers may want to override these properties, especially if they do not support image-based motion blur - or simply to expose
        a global switch to avoid having to change the properties of all nodes in the scene.
        \return True if object-based motion blur is to be used for all objects in the scene, regardless of the per-object proeprties. False if the 
            node properties are to be respected. */
    virtual bool MotionBlurIgnoresNodeProperties() const = 0;
    //@}

    //! \name Offline Rendering
    //@{        
    //! This method is <i>final</i> to enforce usage of the render session. All the data passed to this method is exposed through class IRenderSessionContext.
    virtual int	Open(INode* scene, INode* vnode, ViewParams* viewPar, RendParams& rp, HWND hwnd, DefaultLight* defaultLights, int numDefLights, RendProgressCallback* prog) override final;
    //! This method is <i>final</i> to enforce usage of the render session. All the data passed to this method is exposed through class IRenderSessionContext.
    virtual int	Render(TimeValue t, Bitmap* tobm, FrameRendParams& frp, HWND hwnd, RendProgressCallback* prog, ViewParams* viewPar) override final;
    //! This method is <i>final</i> to enforce usage of the render session. All the data passed to this method is exposed through class IRenderSessionContext.
    virtual void Close(HWND	hwnd, RendProgressCallback* prog) override final;
    //! This method is <i>final</i> to enforce usage of the render session. It applies the render effects that are active in the current render session.
    virtual bool ApplyRenderEffects(TimeValue t, Bitmap* pBitmap, bool updateDisplay) override final;
    //@}

    //! \name Interactive Rendering
    //@{        
    //! \brief Internally implements the IInteractiveRenderer interface.
    /*! \remark Will return null if SupportsInteractiveRendering() returns false. */
    virtual IInteractiveRender* GetIInteractiveRender() override final;
    //@}

    //! \name Process Control
    //@{        
    //! Returns whether the render supports the <i>stop</i> command (and button). See Renderer::IsStopSupported().
    virtual bool IsStopSupported() const override = 0;
    //! Returns whether the render supports the <i>pause</i> command (and button). See Renderer::IsPauseSupported().
    virtual PauseSupport IsPauseSupported() const override = 0;
    //! This method forwards to the render session. See IOfflineRenderSession::StopRendering().
    virtual void StopRendering() override final;
    //! This method forwards to the render session. See IOfflineRenderSession::PauseRendering().
    virtual void PauseRendering() override final;
    //! This method forwards to the render session. See IOfflineRenderSession::ResumeRendering().
    virtual void ResumeRendering() override final;
    //@}

    //! \name Parameter Access
    //@{    
    //! See Animatable::NumParamBlocks().
    virtual int NumParamBlocks() override = 0;
    //! See Animatable::GetParamBlock(int).
    virtual IParamBlock2* GetParamBlock(int i) override = 0;
    //! See Animatable::GetParamBlockByID(BlockID).
    virtual IParamBlock2* GetParamBlockByID(BlockID id) override = 0;
    //! See Renderer::ResetParams(). The default implementation resets all the parameter blocks.
    virtual void ResetParams() override;
    //@}

    //! \name Tabbed Dialog
    //@{    
    //! Made pure virtual to force implementation. The implementation should create at least one tab, in which the render settings UI will be displayed.
    virtual	void AddTabToDialog(ITabbedDialog* dialog, ITabDialogPluginTab*	tab) override = 0;
    //! Default implementation refuses radiosity and ray-tracer tabs, to enable only showing the renderer's own tabs.
    virtual	int	AcceptTab(ITabDialogPluginTab* tab) override;
    //@}

    //! \name Implementation of classes ReferenceTarget / ReferenceMaker
    //@{    
    /*! Creates a new renderer plugin, cloned/copied from this one.
        The default implementation creates a new instance of the renderer through the class descriptor, and clones all the references.
        The plugin may need to specialize this method, for example if it has data members to copy. */
    virtual RefTargetHandle	Clone( RemapDir	&remap ) override;
    //! This default implementation ignores all reference messages, as renderers typically don't need to react to them.
    virtual RefResult NotifyRefChanged(const Interval& changeInt, RefTargetHandle hTarget, PartID& partID, RefMessage message, BOOL propagate) override;
    //! This method is <i>final</i> to enable UnifiedRenderer to load its own data. Use Load_UnifiedRenderer() to load data in the sub-class.
    virtual IOResult Load(ILoad	*iload) override final;
    //! This method is <i>final</i> to enable UnifiedRenderer to load its own data. Use Save_UnifiedRenderer() to load data in the sub-class.
    virtual IOResult Save(ISave	*isave) override final;
    //! Made pure virtual to enforce implementation.
    virtual int NumRefs() override = 0;
    //! Made pure virtual to enforce implementation.
    virtual RefTargetHandle	GetReference(int i) = 0;
    //! Made pure virtual to enforce implementation.
    virtual void SetReference(int i, RefTargetHandle rtarg) = 0;
    //@}

    //! \name Implementation of class Animatable, misc.
    //@{    
    //! This method is <i>final</i> to enable UnifiedRenderer to expose its own interfaces correctly. 
    //! Use GetInterface_UnifiedRenderer(ULONG) to expose interfaces in the sub-class.
    virtual void* GetInterface(ULONG id) override final;
    //! This method is <i>final</i> to enable UnifiedRenderer to expose its own interfaces correctly. 
    //! Use GetInterface_UnifiedRenderer(Interface_ID) to expose interfaces in the sub-class.
    virtual BaseInterface* GetInterface(Interface_ID id) override final;
    //! Call forwarded to class descriptor (see UnifiedRenderer::ClassDescriptor).
    virtual Class_ID ClassID() override final;	
    //! Returns RENDERER_CLASS_ID
    virtual SClass_ID SuperClassID() override final;
    //! Call forwarded to class descriptor (see UnifiedRenderer::ClassDescriptor).
    virtual void GetClassName(TSTR&	s) override final;
    //! This default implementation returns the number of parameter blocks.
    virtual int NumSubs() override;
    //! This default implementation returns the name of the parameter block at index i.
    virtual TSTR SubAnimName(int i) override;
    //! This default implementation returns the param block at index i.
    virtual Animatable*	SubAnim(int	i) override;
    //! Calls: "delete this" and performs cleanup
    virtual void DeleteThis() override final;
    //@}

protected:

    //! \brief Returns whether the renderer supports interactive material editor sessions.
    /*! Material editor previews are rendered using offline render sessions. But to optimize the preview renders
        by avoiding a full re-initialization of the session and re-translation of the scene, the system implements
        a level of semi-interactive rendering. This is performed by keeping the render session alive between MEdit preview
        renders, allowing the re-use of translated scene elements from the last render, as well as removing the need
        for re-initializing the renderer. The translation mechanism works exactly the same as with an interactive render
        session, but is performed through the offline render session interface. Scene changes can therefore only occur
        between renders - no asynchronous changes may occur. */
    virtual bool GetEnableInteractiveMEditSession() const = 0;

private:

    // Private implementation details
    class Implementation;
    std::unique_ptr<Implementation> m_impl;
};

//==================================================================================================
// class UnifiedRenderer::ClassDescriptor
//
/*! Base class for all class descriptors of plugins that derive from UnifiedRenderer.

    This class implements functionality and interfaces necessary for plugins that derive from UnifiedRenderer.
*/
class RenderingAPIExport UnifiedRenderer::ClassDescriptor :
    public ClassDesc2,
    public IMtlRender_Compatibility_Renderer,
    public MaxSDK::Util::Noncopyable
{
public:

    //! The constructor initializes IMtlRender_Compatibility_Renderer
    ClassDescriptor();
    virtual ~ClassDescriptor();

    /*! Returns a new instance of the renderer plugin.
        \param loading True if the plugin is being created for loading purposes, in which case the plugin may skip any initialization steps
            that would be superfluous. */
    virtual UnifiedRenderer* CreateRenderer(const bool loading) = 0;

    //! This default implementation returns true.
    virtual	int	IsPublic() override;
    //! This method is <i>final</i> to enforce going through CreateRenderer().
    virtual	void* Create(BOOL loading) override final;
    //! Returns RENDERER_CLASS_ID
    virtual	SClass_ID SuperClassID() override final;
    //! This default implementation returns "Renderer"
    virtual	const TCHAR* Category() override;
    //! Made pure virtual to enforce implementation.
    virtual	const TCHAR* InternalName() override = 0;
    //! Made pure virtual to enforce implementation.
    //! \remark Consider calling MaxSDK::GetHInstance().
    virtual	HINSTANCE HInstance() override = 0;

protected:

    /*! Returns whether the given material class descriptor is that of a "wrapper" material, which would get resolved
        by BaseTranslator_INode::GetMaterial(). This is may be used by the implementation of IMtlRender_Compatibility_Renderer::IsCompatibleWithMtlBase() to flag
        wrapper materials as compatible.*/
    virtual bool IsCompatibleWrapperMaterial(ClassDesc& mtlBaseClassDesc) const;

private:

    // Private implementation details
    class Implementation;
    std::unique_ptr<Implementation> m_impl;
};


}}  // namespace

#pragma warning(pop)