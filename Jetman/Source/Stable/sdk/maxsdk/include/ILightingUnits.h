/**********************************************************************
 *<
	FILE: ILightingUnits.h

	DESCRIPTION: Function-published interface to access the lighting units display system.

	CREATED BY: David Cunningham

	HISTORY: October 15, 2001 file created

 *>	Copyright (c) 2001, All Rights Reserved.
 **********************************************************************/

#pragma once


#include "maxheap.h"
#include "iFnPub.h"
#define ILIGHT_UNITS_FO_INTERFACE Interface_ID(0x7246dc, 0x4384dc)

//==================================================================================================
// class ILightingUnits
//
//! \brief Provides access to the lighting unit system for converting and displaying luminance and illuminance values.
class ILightingUnits : public FPStaticInterface {

public:

    //! Enumerates the available unit system for displaying lighting quantities.
    enum LightingSystems {
        //! The SI system used cd?m² and lux
        DISPLAY_INTERNATIONAL,
        //! The AS system uses cd?f² and footcandles
        DISPLAY_AMERICAN,
        DISPLAY_TYPE_COUNT
    };

    //! Encapsulates the information related to a unit system.
    struct LightDispInfo : public MaxHeapOperators 
    {
        //! The unit system being represented.
        LightingSystems lightingSystem;
        //! The unit suffix for luminance values.
        MSTR luminanceUnits;
        //! The unit suffix for illuminance values.
        MSTR illuminanceUnits;
    };

    //! For internal use
    virtual const MCHAR   *GetFirstLightingSystemName() const = 0;
    //! For internal use
    virtual const MCHAR   *GetNextLightingSystemName() const = 0;   

    //! Returns the unit system currently being used for display.
    virtual LightingSystems GetLightingSystem() const = 0;
    //! Sets the unit system to be used for display.
    virtual void SetLightingSystem(LightingSystems system) = 0;

    //! Returns the unit suffix as a text string, to be used for displaying luminance values.
    virtual const MCHAR* GetLuminanceUnits() const = 0;
    //! Returns the unit suffix as a text string, to be used for displaying illuminance values.
    virtual const MCHAR* GetIlluminanceUnits() const = 0;

    //! Converts a luminance or illuminance unit, from one system to the other.
    virtual double ConvertValue(const double value, const LightingSystems from_system, const LightingSystems to_system) const = 0;
    //! Converts a luminance or illuminance unit, from the given system to the one used for display.
    virtual double ConvertToDisplay(const double value, const LightingSystems from_system) const = 0;
    //! Converts a luminance or illuminance unit, from the display system to the given one.
    virtual double ConvertFromDisplay(const double value, const LightingSystems to_system) const = 0;
    
    //! \deprecated Use ConvertToDisplay().
    virtual double	ConvertLuminanceToCurrSystem(double luminanceValue) const = 0;
    //! \deprecated Use ConvertToDisplay().
    virtual double	ConvertLuminanceToCurrSystem(double luminanceValue, int inputType) const = 0;
    //! \deprecated Use ConvertValue().
    virtual double  ConvertLuminanceToSI(double AS_LuminanceValue) const = 0;
    //! \deprecated Use ConvertValue().
    virtual double  ConvertLuminanceToAS(double SI_LuminanceValue) const = 0;
    //! \deprecated Use ConvertToDisplay().
    virtual double	ConvertIlluminanceToCurrSystem(double illuminanceValue) const = 0;
    //! \deprecated Use ConvertToDisplay().
    virtual double	ConvertIlluminanceToCurrSystem(double illuminanceValue, int inputType) const = 0;
    //! \deprecated Use ConvertValue().
    virtual double  ConvertIlluminanceToSI(double AS_IlluminanceValue) const = 0;
    //! \deprecated Use ConvertValue().
    virtual double  ConvertIlluminanceToAS(double SI_IlluminanceValue) const = 0;

    //! Retrieves the display unit currently in use for lighting quantities.
    virtual LightDispInfo GetLightingUnitDisplayInfo() const = 0;
};




