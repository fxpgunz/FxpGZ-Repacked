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

#include "../Object.h"

namespace MaxSDK 
{;

//==================================================================================================
// class ISunPositioner
//
//! \brief Access to functionality and properties of the Sun Positioner object.
/*! The Sun Positioner is a light object which is used for the sole purpose of positioning/orienting
    a sun in a scene. It is meant to be connected with a Sun & Sky environment map - where the environment
    map does the shading, but uses the Sun Positioner to determine the direction of the sun.

    The Sun Positioner also provides access to weather measurement data which may be provided by a weather
    data file. 
    
    To retrieve a pointer to this interface, perform a dynamic_cast on a LightObject.
*/
class ISunPositioner :
    public LightObject
{
public:

    struct WeatherMeasurements;

    //! \brief Returns the class ID for the Sun Positioner object plugin.
    static Class_ID GetClassID();

    //! \brief Returns the direction towards the sun.
    /*! Returns a unit vector pointing towards the sun, in <b>object space</b>. 
        \param t The time at which to evaluate the parameters.
        \param validity The interval into which the validity of the evaluated parameters is intersected. */
    virtual Point3 GetSunDirection(const TimeValue t, Interval& validity) const = 0;

    //! \brief Returns weather measurement data from the weather data file, if present.
    /*! \param [out] measurements The measurements are returned through this argument.
        \param t The time at which to evaluate the parameters.
        \param validity The interval into which the validity of the evaluated parameters is intersected.
        \return True if weather data file was successfully obtained from a weather data file; false if a weather data file
        is not specified, missing, or does contain valid weather measurements. */
    virtual bool GetWeatherMeasurements(WeatherMeasurements& measurements, const TimeValue t, Interval& validity) const = 0;

    //! \brief Returns the day of the year provided by the user, or specified by the weather data file.
    /*! The day of the year may be necessary for interpreting the measurements within the Perez All-Weather sky model.
        \param [out] day_of_year The 1-based index of the day within the year (i.e. a value in [1, 366])
        \param t The time at which to evaluate the parameters.
        \param validity The interval into which the validity of the evaluated parameters is intersected.
        \return False if no date is available (e.g. the sun is manually positioned); true otherwise. */
    virtual bool GetDayOfYear(int& day_of_year, const TimeValue t, Interval& validity) const = 0;
};

//==================================================================================================
// class ISunPositioner::WeatherMeasurements
//
//! \brief Encapsulates weather measurements provided by a weather data file.
struct ISunPositioner::WeatherMeasurements
{
    WeatherMeasurements();

    //! The illuminance measurement on a horizontal plane, excluding the solar disc area.
    float diffuse_horizontal_illuminance;
    //! The illuminance measured while facing the sun directly.
    float direct_normal_illuminance;
    //! The dew point temperature in celsius. Only to be used if dew_point_temperature_valid is true.
    float dew_point_temperature;
    //! Whether a valid dew_point_temperature was provided.
    float dew_point_temperature_valid;  
};


}   // namespace

#include "ISunPositioner.inline.h"