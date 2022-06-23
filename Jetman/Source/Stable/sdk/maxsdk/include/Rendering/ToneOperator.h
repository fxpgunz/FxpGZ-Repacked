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

#include "../sfx.h"

// The tone operator uses the standard Special Effects
// parameter dialog class for its UI.
typedef SFXParamDlg ToneOpParamDlg;

class RenderMapsContext;

/*! \brief This is the base class for plugins which perform <b>tone mapping</b> operations in 3ds Max.

\par The tone mapping process
\n
Renderers typically perform lighting simulation in <b>physical space</b>, using units like candelas or watts to represent the intensity of light. If they
do not strictly use physical units, they will at least perform calculations in <b>linear space</b>. Rendered images, however, are typically expressed
in <b>RGB space</b> (usually sRGB), which is usually non-linear. The purpose of the <b>tone operator</b> (or <b>tone mapper</b>) is to transform linear, physical
quantities into (potentially non-linear) RGB values. This process is similar to what happens when photographic film or sensors convert photons into
colors (either chemically or electronically).

\par The physical scale
\n
The physical scale is used to reconcile physical and non-physical light sources
in a scene; it defines the equivalence between a photometric light that emits in candelas and a standard light that emits in arbtrary units.
For example, a physical scale of 1500 would cause a non-physical emissive surface with a value of PI (3.1416) to emit 1500 cd/m^2.
The physical scale affects environment lighting as well; non-physical envrionment lighting will be affected in the same way.
\n\n
The tone operator expects that all physical values be pre-divided by the physical scale. That is, any value fed to ScaledToRGB() is expected
to be expressed in candela/physical_scale. The physical scale therefore adjusts physical values, rather than non-physical ones, in order to
preserve compatibility with legacy features that were implemented before the concept of physical units was introduced to 3ds Max. The
ToneOperator will multiply any values fed to ScaledToRGB() by the physical scale to undo this division.

\par Inverse tone mapping
\n
The process of converting an RGB value back into physical quantities is called inverse tone mapping. This process is especially useful when rendering
with a backgorund or environment image which is already expressed in non-linear, RGB values. The image in question must first be inverse-tone-mapped
to ensure that, once it goes through the rendering and tone mapping processes, it looks exactly as it did originally. Ideally, all environment images
fed to the renderer should be expressed in physical, linear quantities - but that is not always possible. */
class ToneOperator : public SpecialFX 
{
public:

    //! The constructor initializes internal data.
	ToneOperator();

    //! This default implementation does nothing.
	RefResult NotifyRefChanged(const Interval& changeInt, RefTargetHandle hTarget,  PartID& partID, RefMessage message, BOOL propagate) override;
    //! Returns the super class ID of ToneOperator
	SClass_ID SuperClassID() override final;
    //! Saves internal data and must be called by the derived method.
	IOResult Save(ISave *isave) override;
    //! Loads internal data and must be called by the derived method.
	IOResult Load(ILoad *iload) override;
    /*! Returns whether the ToneOperator is currently active.
        \see SetActive() */
	virtual BOOL Active(TimeValue t) override;

	/*! This method indicates whether the tone operator is active. 
    \remark The default implementation does not use the <b>TimeValue t</b>. The result of the default implementation can be retrieved 
        using <b>SpecialFX::GetActive</b>. If you override this method and change the mechanism for storing this state, you should also 
        override <b>SpecialFX::GetActive</b> so the correct state is returned.
	\param active A boolean indicating if the tone operator is active.
	\param t The time at which the active check is made. */
	virtual void SetActive(bool active, TimeValue t);

	/*! Returns the state of <b>A_TONEOP_PROCESS_BG</b>, indicating
	whether the ToneOperator will be processing the background. */
	bool GetProcessBackground() const;
	/*! This method allows you to set <b>A_TONEOP_PROCESS_BG</b>.
	\param active TRUE to activate, FALSE to deactivate. */
	void SetProcessBackground(bool active);

    //! Returns whether tone mapping should apply to indirect lighting only. 
	bool GetIndirectOnly() const;
    //! Sets whether tone mapping should apply to indirect lighting only.
	void SetIndirectOnly(bool active);

	/*! Put up a modal dialog that lets the user edit the plug-ins parameters.
	\remarks This method creates the rollup pages in the render panel that
	    lets the user edit the tone operator's parameters. You can use
	    <b>IRendParams::AddRollupPage</b> and <b>IRendParams::DeleteRollupPage</b>
	    to manage your rollup pages directly. Or, if your parameters are stored in
	    a ParamBlock2 object, you can use <b>CreateRParamMap2</b> and
	    <b>DestroyRParamMap2</b> to manage the rollups. You may return NULL, if no
	    UI is required.
	\param ip Points to the render parameter dialog interface. You may call the methods of that class using this pointer.
	\return Pointer to the tone operator dialog. */
	virtual ToneOpParamDlg* CreateParamDialog(IRendParams* ip);

	/*! Implement this if you are using the ParamMap2 AUTO_UI system and the 
	   atmosphere has secondary dialogs that don't have the effect as their 'thing'.
	   Called once for each secondary dialog for you to install the correct thing.
	   Return TRUE if you process the dialog, false otherwise.
	\remarks Implement this if you are using the ParamMap2 AUTO_UI system and the
	    atmosphere has secondary dialogs that don't have the effect as their
	    'thing'. Called once for each secondary dialog for you to install the
	    correct thing.
	\param dlg Points tot he tone operator dialog.
	\return Return TRUE if you process the dialog, FALSE otherwise. */
	virtual BOOL SetDlgThing(ToneOpParamDlg* dlg);

    //! \name Initialization
    //@{
    /*! \brief Initializes the tone operator at the given time, and with the given camera & view options.
        This method needs to be called before any ToneOperator functionality is used - for example, before ScaledToRGB() is called. It enables
        the ToneOperator to initialize itself with the given view parameters, as well as cache data which would be costly to compute for every pixel.
        Using the tone operator without previously calling this method will result in incorrect tone mapping: the parameters being used may not be
        updated for the correct time or camera.
        \param t The time at which the tone operator is to be evaluated.
        \param cameraNode The camera node being used for rendering. The tone operator is free to affect the tone mapping process based on the camera parameters.
        \param renderResolution The resolution being rendered, used to determine the position in raster space when calling ScaleToRGB().
        \param pixelAspectRatio The pixel aspect ratio, which can be used to derive an image aspect ratio.
        \param [in, out] validity The validity of the parameters which were evaluated by the call. */
    virtual void Update(const TimeValue t, INode* const cameraNode, const IPoint2& renderResolution, const float pixelAspectRatio, Interval& validity) = 0;
    /*! \brief Initializes the tone operator at the given time, but without any information w.r.t. the view and camera. Consequently, some features
        may not be available.
        \par
        Instead, whenever possible, use: virtual void Update(const TimeValue t, INode* const cameraNode, const IPoint2& renderResolution, const float pixelAspectRatio, Interval& validity) */
    virtual void Update(TimeValue t, Interval& valid) = 0;
    //@}

	/*! This method is called for the operator to do any work it needs
	   to do prior to rendering. Rendering using the RenderMapsContext
	   uses the identity tone operator.
	\remarks This method is called for the operator to do any work it
	    needs to do prior to rendering. You may use this method to perform a
	    subrender to sample the rendered output for histogramming or automatic
	    exposure.
	\param t The time at which the rendering is taking place.
	\param rmc The context of the map rendering.
	\return True means this method succeeded. False means it didn't. This
	    method should return false if it the sub-render fails or if it can't
	    allocate memory or some other error occurs. If BuildMaps returns false, the
	    render is aborted. */
	virtual bool BuildMaps(TimeValue t, RenderMapsContext& rmc);

	/*! This method is called during subrenders to give the tone operator
	   a chance to sample the image with full dynamic range. If your operator
	   needs to sample the image, you can set a flag so you know when you are
	   sampling. */
	virtual void SubRenderSample(float energy[3]);

    //! \name Physical to RGB conversion
    //! \brief These methods convert physical (energy) values into RGB space.
    //@{
    /*! \brief Scales a value from physical space to RGB space.
        Rendered images, as directly output by renderers, will store values in physical units (candelas per meter-squared) which must be transformed
        into RGB space before they can be viewed on a monitor. This method performs this transformation.
        \param [in, out] energy The physical RGB value, which gets converted in-place into an RGB value.
        \param xyCoord The x/y coordinate of the pixel being scaled, the valid range being [0, renderResolution] (renderResolution having been passed to Update()). */
    virtual void ScaledToRGB(float energy[3], const Point2& xyCoord) const = 0;
    /*! \brief Scales a <b>grayscale</b> value from physical space to RGB space. 
        \see void ScaledToRGB(energy[3], xyCoord) const
        \param [in, out] energy The physical RGB <b>grayscale</b> value, which gets converted in-place into an RGB <b>grayscale</b> value.
        \param xyCoord The x/y coordinate of the pixel being scaled, the valid range being [0, renderResolution] (renderResolution having been passed to Update()). */
	virtual float ScaledToRGB(float energy, const Point2& xyCoord) const = 0;
    /*! Equivalent of virtual void ScaledToRGB(float energy[3], const Point2& xyCoord) const, but without any support for view-dependent tone mapping. */
    virtual void ScaledToRGB(float color[3]) const = 0;
    /*! Equivalent of virtual void ScaledToRGB(float energy, const Point2& xyCoord) const, but without any support for view-dependent tone mapping. */
    virtual float ScaledToRGB(float energy) const = 0;
    //@}

    //! \name Physical scale
    //@{
    //! \brief Returns the physical scale to be used when rendering.
    /*! For a full description of the physical scale, see the details description for class ToneOperator.
        \param t The time at which to fetch the phyisical scale.
        \param valid The validity of the physical scale value is intersected into this interval. */
	virtual float GetPhysicalUnit(TimeValue	t, Interval& valid) const = 0;
    //! \brief Sets the physical scale to be used when rendering.
    /*! For a full description of the physical scale, see the details description for class ToneOperator.
        \param value The new physical scale.
        \param t The time at which to set the new physical scale. */
	virtual void SetPhysicalUnit(float value, TimeValue t) = 0;
	/*! Scale physical values so they can be used in the renderer. The
	    first version of the method converts a color value and the second
	   converts a monochrome value. The converted color value is stored
	   in <i>energy</i>. The converted monochrome value is returned.
	   This method assumes that Update has been called to cache the
	   various values needed by the tone operator.
	   By using a float array to pass in color values, we can use the same
	   routine to handle the various classes used to store color information,
	   for example, Color, AColor and Point3. The red, green and blue
	   components are stored in that order in the array.
	\remarks This method is used to scale a physical color value so it may be used in
	    the renderer.\n\n
	    This method assumes that Update has been called to cache the various values
	    needed by the tone operator.\n\n
	    Note: By using a float array to pass in color values, we can use the same
	    routine to handle the various classes used to store color information, for
	    example, Color, AColor and Point3.
	\param energy The input and output (converted) color value. The colors are stored as red=energy[0], green=energy[1], and blue=energy[2]. */
	virtual void ScalePhysical(float energy[3]) const = 0;
	/*! Grayscale version of virtual void ScalePhysical(float energy[3]) const */
	virtual float ScalePhysical(float energy) const = 0;
	/*! Scale RGB values, just supplied to invert ScalePhysical. The first
	   version of the method converts a color value and the second
	   converts a monochrome value. The converted color value is stored
	   in <i>energy</i>. The converted monochrome value is returned.
	   This method assumes that Update has been called to cache the
	   various values needed by the tone operator.
	   By using a float array to pass in color values, we can use the same
	   routine to handle the various classes used to store color information,
	   for example, Color, AColor and Point3. The red, green and blue
	   components are stored in that order in the array.
	\remarks This method is called to scale RGB values (the inverse of
	    ScalePhysical()).\n\n
	    This method assumes that Update has been called to cache the various values
	    needed by the tone operator.
	\param color The input values to scale and storage for the output scaled values as well. The colors are stored as red=energy[0], green=energy[1], and
	    blue=energy[2]. The output values are in the range 0-1. */
	virtual void ScaleRGB(float color[3]) const = 0;
    /*! Grayscale version of virtual void ScaleRGB(float color[3]) const */
	virtual float ScaleRGB(float color) const = 0;
    //@}

    //! \name Inversion (RGB to Physical conversion)
    //@{
    /*! Returns whether this ToneOperator supports the inversion functions. The inversion functions enable a ToneOperator to convert an RGB
        value back into physical units. This is especially useful for inverse tone-mapping the background map, should it not be affected by
        the tone operator. */
	virtual bool CanInvert() const = 0;
	/*! Convers an RGB value back into physical space. This is effectively the inverse of ScaledToRGB(). 
        \remark The ToneOperator must be initialized with Update() before this may be called. */
    virtual void InverseMap(float rgb[3], const Point2& xyCoord) const = 0;
	/*! Convers an RGB value back into physical space. This is effectively the inverse of ScaledToRGB(). 
        \remark The ToneOperator must be initialized with Update() before this may be called. */
    virtual float InverseMap(float rgb, const Point2& xyCoord) const = 0;
    /*! Equivalent of virtual void InverseMap(float rgb[3], const Point2& xyCoord) const, but without any support for view-dependent tone mapping. */
    virtual void InverseMap(float rgb[3]) const = 0;
    /*! Equivalent of virtual void InverseMap(float rgb, const Point2& xyCoord) const, but without any support for view-dependent tone mapping. */
    virtual float InverseMap(float rgb) const = 0;
    //! Alias for InverseMap()
    void RGBToScaled(float energy[3], const Point2& xyCoord) const;
    //! Alias for InverseMap()
    float RGBToScaled(float energy, const Point2& xyCoord) const;
    //! Alias for InverseMap()
    void RGBToScaled(float energy[3]) const;
    //! Alias for InverseMap()
    float RGBToScaled(float energy) const;
    //@}

    /*! \deprecated This method no longer served a purpose and has been removed. */
	MAX_DEPRECATED bool IsPhysicalSpace() const;

    //! \brief Enables processing a tone operator independently of class ToneOperator, for example in a multi-threaded manner.
    /*! As class ToneOperator derives from ReferenceTarget, it may only safely be used from the main thread. But tone operators can be expensive
        to process, making it desirable to process them from non-UI threads (or even from multiple threads). This interface encapsulates everything
        a tone operator needs to process itself, but doesn't derive from ReferenceTarget so may safely be used from arbitrary threads.
        The implementation of this interface <b>must allow for concurrent calls from arbitrary threads</b>.
        \see ToneOperator::AllocateThreadedProcessor() */
    class ThreadedProcessor : public MaxHeapOperators
    {
    public:
        virtual ~ThreadedProcessor() {}

        //! See virtual void ToneOperator::ScaledToRGB(float energy[3], const Point2& xyCoord)
        //! \remark This method is thread-safe, it may be called from multiple threads concurrently.
        virtual void ScaledToRGB(float energy[3], const Point2& xyCoord) const = 0;
        //! See virtual float ToneOperator::ScaledToRGB(float energy, const Point2& xyCoord)
        //! \remark This method is thread-safe, it may be called from multiple threads concurrently.
        virtual float ScaledToRGB(float energy, const Point2& xyCoord) const = 0;
        //! See virtual float ToneOperator::CanInvert()
        //! \remark This method is thread-safe, it may be called from multiple threads concurrently.
        virtual bool CanInvert() const = 0;
        //! See virtual float ToneOperator::InverseMap(float energy[3], const Point2& xyCoord)
        //! \remark This method is thread-safe, it may be called from multiple threads concurrently.
        virtual void InverseMap(float rgb[3], const Point2& xyCoord) const = 0;
        //! See virtual float ToneOperator::InverseMap(float energy, const Point2& xyCoord)
        //! \remark This method is thread-safe, it may be called from multiple threads concurrently.
        virtual float InverseMap(float rgb, const Point2& xyCoord) const = 0;
    };

    /*! \brief Allocates an interface which enables processing of the tone operator from arbitrary (or multiple) threads.
        \return A pointer to a newly allocated interface which is then owned exclusively by the caller. The caller is responsible for deleting
            the pointer once done. <b>May return null</b> if the tone operator doesn't implement support for the ThreadedProcessor interface,
            in which case the tone operator must be processed <b>from the main thread</b>, using ToneOperator::ScaledToRGB(). 
        \remark This method may only be called from the main thread - as every other method in in this class. 
        \remark The tone operator must be updated using ToneOperator::Update() <b>before calling this method</b>. The 
            processor to be returned will reflect the parameter values setup by the last call to Update(). */
    virtual const ThreadedProcessor* AllocateThreadedProcessor() const = 0;
};

#include "ToneOperator.inline.h"