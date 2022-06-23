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
#include "../Color.h"
#include "../CoreExport.h"

class Shader;
class Mtl;

#ifndef USE_LIMITED_STDMTL
#define STD2_NMAX_TEXMAPS  24
#else
#define STD2_NMAX_TEXMAPS  12
#endif

/*! \sa  Class Shader, Class Point3,  Class Color.\n\n
\par Description:
This class allows the Standard material to get the parameters from a Shader
plug-in. It also allows the Shader to supply its computed color data back to
the Standard material. The Standard material will handle the texturing but it
needs to know the color before the texturing is done.\n\n
The shader object holds some raw parameters. But these parameters may be
textured. So for each pixel that will be shaded, the 3ds Max shade routine asks
the shader to fill in the raw values. Then it applies the texture over the top
of any of the values which are textured.
\par Data Members:
<b>Shader* pShader;</b>\n\n
This parameter is available in release 4.0 and later only.\n\n
A pointer to the shader.\n\n
<b>Mtl* pMtl;</b>\n\n
This parameter is available in release 4.0 and later only.\n\n
A pointer to the material being shaded or NULL if it's a background.\n\n
<b>Color channels[32];</b>\n\n
A color channel for each of the possible parameters that may be textured. Note
that these channels don't have specific meanings. They are defined by each
Shader. The Shader tells the Standard material what data is in these channels
using its <b>ChannelType()</b> and <b>StdIDToChannel()</b> methods.\n\n
<b>float falloffOpac;</b>\n\n
Shaders using standard opacity can ignore this data member. This is available
for certain Shaders which don't use the standard 3ds Max transparency equation.
This allows these Shaders to simulate the 3ds Max version. This is the
pre-computed textured opacity with standard falloff applied. The value here is
pre-computed to consider all the standard opacity settings of falloff
direction, etc. The standard transparency computation then uses this after
shading. So a shader could modify this value if it wanted to to affect the
transparency.\n\n
Note: The regular opacity can be obtained from the channel data.\n\n
<b>float kR;</b>\n\n
This value is used when there is a reflection and an atmosphere present.
Normally if there is no atmosphere (for instance no Fog in the scene) then the
transparency of the reflection is 100% (it is unaffected). However, if there is
an atmosphere present it will impart some level of opacity. This opacity (alpha
value) is not available via the reflection color. Thus this data member is here
to provide this opacity information to the Shader.\n\n
This value is the alpha which is returned by the reflection query that has the
transparency of the atmosphere contained within it, which is then multiplied by
the Amount spinner. This can then be used either by the Shader or by the
standard handling for reflection maps.\n\n
<b>ULONG hasComponents;</b>\n\n
These are the bits for the active components of bump, reflection, refraction
and opacity mapping. If the bit is set that component is active. This provides
a quick way for a Shader to check if they're used (as opposed to looking
through the <b>channels</b> array searching for these channel types).\n\n
<b>HAS_BUMPS</b> -- If bump mapping is present this is set.\n\n
<b>HAS_REFLECT</b> -- If there is any kind of reflection (raytraced, etc) then
this is set.\n\n
<b>HAS_REFRACT</b> -- If there is any kind of refraction then this is set..\n\n
<b>HAS_OPACITY</b> -- If opacity mapping is used this is set.\n\n
<b>HAS_REFLECT_MAP</b> -- If there is a reflection map only this is set. This
is used by the Strauss shader for example. If it sees a reflection map present
it dims the diffuse channel.\n\n
<b>HAS_REFRACT_MAP</b> -- If there is a refraction map only then this is
set.\n\n
<b>ULONG stdParams;</b>\n\n
The standard parameter bits. See \ref Standard_Parameter_flags.
This is filled in by the Standard material.\n\n
<b>Color ambIllumOut;</b>\n\n
This is the ambient output from the <b>Illum()</b> method.\n\n
<b>Color diffIllumOut;</b>\n\n
This is the diffuse output from the <b>Illum()</b> method.\n\n
<b>Color transIllumOut;</b>\n\n
This is the transparency output from the <b>Illum()</b> method.\n\n
<b>Color selfIllumOut;</b>\n\n
This is the self illumination output from the <b>Illum()</b> method.\n\n
<b>Color specIllumOut;</b>\n\n
This is the specular illumination output from the <b>Illum()</b> method.\n\n
<b>Color reflIllumOut;</b>\n\n
This is the reflection output from the <b>Illum()</b> method. Certain shaders
may wish to store the reflection output here (as opposed to providing it in the
<b>channels</b> array). This is the '"raw " color from the direction of
reflection (unattenuated). Some combiner implementations
(<b>Shader::CombineComponents()</b>)can get the reflection data here, others
may get it from the channels.\n\n
<b>float diffIllumIntens;</b>\n\n
Used only by reflection dimming, intensity of diffIllum prior to color
multiply.\n\n
<b>float finalAttenuation;</b>\n\n
The final attenuation for combining components.\n\n
<b>float finalOpac;</b>\n\n
This is the final opacity value used for combining components together in
<b>Shader::CombineComponents()</b>.\n\n
<b>Color finalC;</b>\n\n
This is the final output color that the <b>Shader::CombineComponents()</b>
composites together.\n\n
<b>Color finalT;</b>\n\n
This is the final Shader transparency color output.  */
class IllumParams : public BaseInterfaceServer {
public:
	 // these are the inputs to the shader, mostly textured channels
// ULONG mtlFlags;
	 Shader* pShader;  // for render elements to call the shader & mtl again, may be null
	 Mtl* pMtl;        // max mtl being shaded, null if bgnd
// Point3 N, V;
	 Color channels[ STD2_NMAX_TEXMAPS ];   //inputs, to be textured

	 float finalOpac;     // textured opacity w/ stdfalloff (reg opac in channels)
	 float reflectAmt;          // combined reflection.a * reflectAmt 
	 float refractAmt;          // combined reflection.a * amt 
	 ULONG hasComponents; // bits for active components(e.g.currently has active refl map)
	 ULONG stdParams;
	 int*  stdIDToChannel;   // combined shader & mtl. Note: may be NULL (for example, matte/shadow)

	 // these are the component-wise outputs from the shading process
	 Color ambIllumOut, diffIllumOut, transIllumOut, selfIllumOut; // the diffuse clrs
	 Color specIllumOut, reflIllumOut;   // the specular colors

	 // User Illumination outputs for render elements, name matched
	 int   nUserIllumOut;    // one set of names for all illum params instances
	 const MCHAR** userIllumNames;  // we just keep ptr to shared name array, not destroyed
	 Color* userIllumOut; // the user illum color array, new'd & deleted w/ the class

	 float diffIllumIntens; // used only by reflection dimming, intensity of diffIllum prior to color multiply
	 float finalAttenuation; // for combining components

	 // these are the outputs of the combiner
	 Color finalC;  // final clr: combiner composites into this value.
	 Color finalT;  // shader transp clr out

public:
	 // Illum params are allocated by materials during shading process to
	 // communicate with shaders & render elements
	 // So materials need to know how many userIllum slots they will use
	 // most materials know this, but std2 will have to get it from the shader
	 CoreExport IllumParams( int nUserOut = 0, const MCHAR** pUserNames = NULL );        

// IllumParams(){ 
//    nUserIllumOut = 0;
//    userIllumOut = NULL;
//    userIllumNames = NULL;
// }


	 CoreExport ~IllumParams();

	 // returns number of user illum channels for this material
	 int nUserIllumChannels(){ return nUserIllumOut; }

	 // returns null if no name array specified
	 const MCHAR* GetUserIllumName( int n ) { 
			DbgAssert( n < nUserIllumOut );
			if( userIllumNames )
				 return userIllumNames[n];
			return NULL;
	 }

	 // render elements, mtls & shaders can use this to find the index associated with a name
	 // returns -1 if it can't find the name
	 CoreExport int FindUserIllumName( const MCHAR* name );

	 // knowing the index, these set/get the user illum output color
	 void SetUserIllumOutput( int n, Color& out ){
			DbgAssert( n < nUserIllumOut );
			userIllumOut[n] = out;
	 }

	 CoreExport void SetUserIllumOutput( const MCHAR* name, Color& out );

	 Color GetUserIllumOutput( int n ){
			DbgAssert( n < nUserIllumOut );
			return userIllumOut[n];
	 }

	 CoreExport Color GetUserIllumOutput( const MCHAR* name, int n );

	 /*! \remarks This method is called by the Standard material prior to
	 calling the <b>Illum()</b> method of the Shader. It sets to black all the
	 output colors:\n\n
	 <b>ambIllumOut=diffIllumOut=transIllumOut=selfIllumOut=</b>\n\n
	 <b> specIllumOut=reflIllumOut=Color(0.0f, 0.0f, 0.0f);</b> */
	 void ClearOutputs() { 
			finalC = finalT = ambIllumOut=diffIllumOut=transIllumOut=selfIllumOut=
			specIllumOut=reflIllumOut= Color( 0.0f, 0.0f, 0.0f ); 
			finalAttenuation = diffIllumIntens = 0.0f;
			for( int i=0; i < nUserIllumOut; ++i )
				 userIllumOut[i] = finalC;
	 }

	 void ClearInputs() { 
			/*mtlFlags = */ stdParams = hasComponents = 0;
			pShader = NULL; pMtl = NULL;
			stdIDToChannel = NULL;
			refractAmt = reflectAmt = 0.0f; 
			for( int i=0; i < STD2_NMAX_TEXMAPS; ++i )
				 channels[ i ] = Color( 0, 0, 0 );
	 }
};
