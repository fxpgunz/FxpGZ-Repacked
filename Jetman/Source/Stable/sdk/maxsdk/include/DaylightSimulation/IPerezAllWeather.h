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

#include "../ifnpub.h"

//! Interface identifier for class IPerezAllWeather.
#define PEREZ_ALL_WEATHER_INTERFACE_ID	Interface_ID(0x57a47c50, 0x1fb32e46)


//! \brief Interface for accessing functionality that exists in the Perez All Weather Model.
/*! The Perez All-Weather model is used to calculate the luminance of the sky based on various sets of parameters, which may
    all be measured or derived from each other:
        - Illuminances for the sky (diffuse horizontal) and in the direction of the sun (direct normal).
        - Irradiances for the sky (diffuse horizontal) and in the direction of the sun (direct normal).
        - Sky brightness and clearness.
    \remark A pointer to this interface may retrieved by calling IPerezAllWeather::GetInterface().
*/
class IPerezAllWeather : public FPStaticInterface
{
public:

	//! Data structure for storing the ABCDE coefficients of the Perez sky model.
	struct PerezParams
	{
		float a,b,c,d,e;
	};

    //! Returns the pointer to this interface.
    static IPerezAllWeather* GetInterface();

    //! \brief Calculates the Perez coefficients based on measured illumiance values.
    /*! \param dayNumber The day of the year, index from 1.
        \param atmWaterContent The atmospheric water content (maybe calculated through GetAtmosphericPrecipitableWaterContent()).
        \param solarZenithAngle The angle between the sun and the zenith (0,0,1), in radians.
        \param diffuseHorizontalIllumiance The measured diffuse horizontal illuminance.
        \param directNormalIlluminance The measured direct normal illuminance.
        \param [out] param The Perez coefficients are returned through this. */
	virtual void GetPerezParamsFromIlluminances(int dayNumber,float atmWaterContent,float solarZenithAngle,float diffuseHorizontalIllumiance, float directNormalIlluminance, IPerezAllWeather::PerezParams &param)=0;
    //! \brief Calculates the Perez coefficients based on measured irradiance values.
    /*! \param dayNumber The day of the year, index from 1.
        \param solarZenithAngle The angle between the sun and the zenith (0,0,1), in radians.
        \param diffuseHorizontalIrradiance The measured diffuse horizontal irradiance.
        \param directNormalIrradiance The measured direct normal irradiance.
        \param [out] param The Perez coefficients are returned through this. */
	virtual void GetPerezParamsFromIrradiances(int dayNumber,float solarZenithAngle,float diffuseHorizontalIrradiance, float directNormalIrradiance,IPerezAllWeather::PerezParams &param)=0;
    //! \brief Calculates the Perez coefficients based on measured sky brightness and clearness values.
    /*! \param skyBrightness The measured sky brightness (may be calculated through CalcSkyBrightness()).
        \param skyClearness The measured sky clearness (may be calculated through CalcSkyClearness()).
        \param solarZenithAngle The angle between the sun and the zenith (0,0,1), in radians.
        \param [out] param The Perez coefficients are returned through this. */
	virtual void CalcModelCoefficients(float skyBrightness,float skyClearness, float solarZenithAngle, IPerezAllWeather::PerezParams &param)=0;

    //! \brief Derives the sky brightness from measured irradiance values.
    /*! \param dayNumber The day of the year, index from 1.
        \param solarZenithAngle The angle between the sun and the zenith (0,0,1), in radians.
        \param diffuseHorizontalIrradiance The measured diffuse horizontal irradiance.
        \param normalDirectIrradiance The measured direct normal irradiance. 
        \return The calculated brightness value. */
	virtual float CalcSkyBrightness(int dayNumber,float solarZenithAngle,float diffuseHorizontalIrradiance, float normalDirectIrradiance)=0;
    //! \brief Derives the sky clearness from measured irradiance values.
    /*! \param solarZenithAngle The angle between the sun and the zenith (0,0,1), in radians.
        \param diffuseHorizontalIrradiance The measured diffuse horizontal irradiance.
        \param normalDirectIrradiance The measured direct normal irradiance. 
        \return The calculated clearness value. */
	virtual float CalcSkyClearness(float solarZenithAngle,float diffuseHorizontalIrradiance, float normalDirectIrradiance)=0;

    //! \brief Derives the diffuse horizontal irradiance value from sky brightness.
    /*! \param dayNumber The day of the year, index from 1.
        \param skyBrightness The measured sky brightness.
        \param solarZenithAngle The angle between the sun and the zenith (0,0,1), in radians.
        \return The calculated diffuse horizontal irradiance value. */
	virtual float CalcDiffuseHorizontalIrradiance(int dayNumber,float skyBrightness,float solarZenithAngle)=0;
    //! \brief Derives the direct normal irradiance value from sky clearness.
    /*! \param diffuseHorizontalIrradiance The diffuse horizontal irradiance, which may be calculated with CalcDiffuseHorizontalIrradiance().
        \param solarZenithAngle The angle between the sun and the zenith (0,0,1), in radians.
        \param skyClearness The measured sky clearness.
        \return The calculated diffuse horizontal irradiance value. */
	virtual float CalcDirectNormalIrradiance(float diffuseHorizontalIrradiance, float solarZenithAngle,float skyClearness)=0;

    //! \brief Converts a radiance value to an illuminance.
    /*! \param diffuseHorizontalRadiance The measured diffuse horizontal radiance.
        \param skyBrightness The measured sky brightness (may be calculated through CalcSkyBrightness()).
        \param skyClearness The measured sky clearness (may be calculated through CalcSkyClearness()).
        \param solarZenithAngle The angle between the sun and the zenith (0,0,1), in radians.
        \param atmWaterContent The atmospheric water content (maybe calculated through GetAtmosphericPrecipitableWaterContent()). 
        \return The illuminance value, calculated from the inputs. */
	virtual float GetDiffuseHorizontalIlluminance(float diffuseHorizontalRadiance,float skyBrightness,float skyClearness, float solarZenithAngle, float atmWaterContent)=0;	
    //! \brief Converts a radiance value to an illuminance.
    /*! \param directNormalRadiance The measured direct normal radiance.
        \param skyBrightness The measured sky brightness (may be calculated through CalcSkyBrightness()).
        \param skyClearness The measured sky clearness (may be calculated through CalcSkyClearness()).
        \param solarZenithAngle The angle between the sun and the zenith (0,0,1), in radians.
        \param atmWaterContent The atmospheric water content (maybe calculated through GetAtmosphericPrecipitableWaterContent()). 
        \return The illuminance value, calculated from the inputs. */
	virtual float GetDirectNormalIlluminance(float directNormalRadiance,float skyBrightness,float skyClearness, float solarZenithAngle, float atmWaterContent)=0;	
    //! \brief Converts an illuminance value to an irradiance.
    /*! \param diffuseHorizontalIlluminance The measured diffuse horizontal illuminance.
        \param skyBrightness The measured sky brightness (may be calculated through CalcSkyBrightness()).
        \param skyClearness The measured sky clearness (may be calculated through CalcSkyClearness()).
        \param solarZenithAngle The angle between the sun and the zenith (0,0,1), in radians.
        \param atmWaterContent The atmospheric water content (maybe calculated through GetAtmosphericPrecipitableWaterContent()). 
        \return The irradiance value, calculated from the inputs. */
	virtual float GetDiffuseHorizontalIrradiance(float diffuseHorizontalIlluminance,float skyBrightness,float skyClearness, float solarZenithAngle, float atmWaterContent)=0;	
    //! \brief Converts an illuminance value to an irradiance.
    /*! \param directNormalIllumiance The measured direct normal illuminance.
        \param skyBrightness The measured sky brightness (may be calculated through CalcSkyBrightness()).
        \param skyClearness The measured sky clearness (may be calculated through CalcSkyClearness()).
        \param solarZenithAngle The angle between the sun and the zenith (0,0,1), in radians.
        \param atmWaterContent The atmospheric water content (maybe calculated through GetAtmosphericPrecipitableWaterContent()). 
        \return The irradiance value, calculated from the inputs. */
	virtual float GetDirectNormalIrradiance(float directNormalIllumiance,float skyBrightness,float skyClearness, float solarZenithAngle, float atmWaterContent)=0;	
    //! \brief Converts a dew point temperature to an atmospheric water content.
    /*! \param tempValid Indicates whether the value stored in surfaceDewPointTemp is valid and usable. If false, then the value stored in surfaceDewPointTemp
            is ignored, and a default value is returned instead.
        \param surfaceDewPointTemp The dew point temperature.
        \return The atmospheric water content, calculated from the input value. */
	virtual float GetAtmosphericPrecipitableWaterContent(BOOL tempValid,float surfaceDewPointTemp)=0;
};

inline IPerezAllWeather* IPerezAllWeather::GetInterface()
{
    return dynamic_cast<IPerezAllWeather*>(GetCOREInterface(PEREZ_ALL_WEATHER_INTERFACE_ID));
}

