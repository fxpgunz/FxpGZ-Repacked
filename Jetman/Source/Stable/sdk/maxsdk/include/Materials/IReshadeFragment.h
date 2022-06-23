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

#include "../BaseInterface.h"
#include "../AColor.h"

class Color32;

/*! \sa  Class InterfaceServer, Class IReshading, Class Point2,  Class Point3\n\n
\par Description:
The Reshade Fragment interface is the materials/shaders/textures interface to
reshading fragments. This interface is only concerned with saving and
retrieving values in the fragments variable sized cache. Values are always
saved in multiples of 32 bit words, and they can be a variety of types, colors,
float's, int's, etc. Unit vectors are compressed to a single 32 bit word.
Point2 and Point3 data types save multiple floats.\n\n
Color channels will be compressed to a 32 bit rgba color and saved in the
cache.\n\n
Color32 channels are only used internally. This is the 'raw' form of what's
stored, other forms may be accessed from this class.\n\n
Float and integer channels are stored uncompressed, they are stored directly in
the 32 bits.\n\n
The Point2 \& Point3 channels are shells that store multiple float channels so
these values will be uncompressed. These should be used with great caution, as
this memory is stored per fragment, in a scene with 100,000 fragments of a
given material, a point3 channel will add 1.2 megabytes to the storage for the
reshading buffer.  */
class IReshadeFragment : public InterfaceServer 
{
public:
	 /*! \remarks This method returns the number of textures for the fragment.
	 */
	 virtual int NChannels()=0;
	 /*! \remarks This method returns the index of the first channel. */
	 virtual int NFirstChannel()=0; 
	 /*! \remarks This method returns the number of textures. */
	 virtual int NTextures()=0;

	 /*! \remarks This method returns the specified channel.
	 \par Parameters:
	 <b>int nChan</b>\n\n
	 The channel you wish to return. */
	 virtual Color32   Channel( int nChan )=0;
	 /*! \remarks This method allows you to set a channel in raw mode to a new
	 value.
	 \par Parameters:
	 <b>int nChan</b>\n\n
	 The channel you wish to set.\n\n
	 <b>Color32 c</b>\n\n
	 The new value to set. */
	 virtual void SetChannel( int nChan, Color32 c )=0;
	 /*! \remarks This method allows you to add a new color channel in raw mode
	 to the end of the cache.
	 \par Parameters:
	 <b>Color32 c</b>\n\n
	 The channel value you wish to set. */
	 virtual void AddChannel( Color32 c )=0;

	 /*! \remarks This method allows you to add a new color channel to the end
	 of the cache. The color will be compressed to 32bits.
	 \par Parameters:
	 <b>RGBA tex</b>\n\n
	 The color channel value you wish to add.\n\n
		 */
	 virtual void AddColorChannel( RGBA tex )=0;
	 /*! \remarks This method allows you to add a float channel in the fragment
	 cache.
	 \par Parameters:
	 <b>float f</b>\n\n
	 The channel value to add. */
	 virtual void AddFloatChannel( float f )=0;
	 /*! \remarks This method allows you to add an integer channel in the
	 fragment cache.
	 \par Parameters:
	 <b>int i</b>\n\n
	 The integer value to add. */
	 virtual void AddIntChannel( int i )=0;
	 /*! \remarks This method allows you to add a unit vector channel in the
	 fragment cache, compressed to 32-bits.
	 \par Parameters:
	 <b>Point3 v</b>\n\n
	 The unit vector to add. */
	 virtual void AddUnitVecChannel( Point3 v )=0;
	 /*! \remarks This method allows you to add a Point2 channel in the fragment
	 cache, uncompressed.
	 \par Parameters:
	 <b>Point2 p</b>\n\n
	 The Point2 to add. */
	 virtual void AddPoint2Channel( Point2 p )=0;
	 /*! \remarks This method allows you to add a Point3 channel in the fragment
	 cache, uncompressed.
	 \par Parameters:
	 <b>Point3 p</b>\n\n
	 The Point3 to add. */
	 virtual void AddPoint3Channel( Point3 p )=0;

	 /*! \remarks This method sets the existing color channel number
	 <b>nChan</b> to the new value.
	 \par Parameters:
	 <b>int nChan</b>\n\n
	 The color channel number.\n\n
	 <b>RGBA text</b>\n\n
	 The new color value. */
	 virtual void SetColorChannel( int nChan, RGBA tex )=0;
	 /*! \remarks This method allows you to set a float channel in the fragment
	 cache to a new value.
	 \par Parameters:
	 <b>int nChan</b>\n\n
	 The float channel number.\n\n
	 <b>float f</b>\n\n
	 The new value to set. */
	 virtual void SetFloatChannel( int nChan, float f )=0;
	 /*! \remarks This method allows you to set an int channel in the fragment
	 cache to a new value.
	 \par Parameters:
	 <b>int nChan</b>\n\n
	 The int channel number.\n\n
	 <b>int i</b>\n\n
	 The new value to set. */
	 virtual void SetIntChannel( int nChan, int i )=0;
	 /*! \remarks This method allows you to set a unit vector channel in the
	 fragment cache to a new value.
	 \par Parameters:
	 <b>int nChan</b>\n\n
	 The unit vector channel number.\n\n
	 <b>Point3 v</b>\n\n
	 The new value to set. */
	 virtual void SetUnitVecChannel( int nChan, Point3 v )=0;
	 /*! \remarks This method allows you to set a Point2 channel in the fragment
	 cache to a new value.
	 \par Parameters:
	 <b>int nChan</b>\n\n
	 The Point2 channel number.\n\n
	 <b>Point2 p</b>\n\n
	 The new value to set. */
	 virtual void SetPoint2Channel( int nChan, Point2 p )=0;
	 /*! \remarks This method allows you to set a Point3 channel in the fragment
	 cache to a new value.
	 \par Parameters:
	 <b>int nChan</b>\n\n
	 The Point3 channel number.\n\n
	 <b>Point3 p</b>\n\n
	 The new value to set. */
	 virtual void SetPoint3Channel( int nChan, Point3 p )=0;

	 /*! \remarks This method will return an existing color channel, expanded to
	 float RGBA.
	 \par Parameters:
	 <b>int nChan</b>\n\n
	 The color channel you wish to return. */
	 virtual RGBA GetColorChannel( int nChan )=0;
	 /*! \remarks This method will return an existing float channel.
	 \par Parameters:
	 <b>int nChan</b>\n\n
	 The float channel you wish to return. */
	 virtual float GetFloatChannel( int nChan )=0;
	 /*! \remarks This method will return an existing int channel.
	 \par Parameters:
	 <b>int nChan</b>\n\n
	 The int channel you wish to return. */
	 virtual int GetIntChannel( int nChan )=0;
	 /*! \remarks This method will return an existing unit vector channel.
	 \par Parameters:
	 <b>int nChan</b>\n\n
	 The unit vector channel you wish to return. */
	 virtual Point3 GetUnitVecChannel( int nChan )=0;
	 /*! \remarks This method will return an existing Point2 channel.
	 \par Parameters:
	 <b>int nChan</b>\n\n
	 The Point2 channel you wish to return. */
	 virtual Point2 GetPoint2Channel( int nChan )=0;
	 /*! \remarks This method will return an existing Point3 channel.
	 \par Parameters:
	 <b>int nChan</b>\n\n
	 The Point3 channel you wish to return. */
	 virtual Point3 GetPoint3Channel( int nChan )=0;
};
