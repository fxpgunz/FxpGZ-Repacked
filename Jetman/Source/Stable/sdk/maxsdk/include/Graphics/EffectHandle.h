//
// Copyright 2013 Autodesk, Inc.  All rights reserved.
//
// Use of this software is subject to the terms of the Autodesk license
// agreement provided at the time of installation or download, or which 
// otherwise accompanies this software in either electronic or hard copy form.   
//
//

#pragma once

#include "SmartHandle.h"
#include "../strclass.h"
#include "../containers/Array.h"

namespace MaxSDK { namespace Graphics {

struct ParameterDesc;
class DrawContext;
class EffectInstanceHandle;
typedef MaxSDK::Array<MSTR> TechniqueNames;

/** EffectHandle is a class provides features similar as Effect in Direct3D
This class is used for viewport display and Quicksilver.
It can also be used for realistic material display. See IHLSLMaterialTranslator for more details.
Example of use:
\code
//Initialize the effect
EffectHandle hEffect;
EffectInstanceHandle hEffectInstance;
MSTR errorInfo;
hEffect.InitializeWithFile(shaderFileName, &errorInfo);
hEffectInstance = hEffect.CreateEffectInstance();
hEffectInstance.SetFloatParameter(_M("myFloatShaderParam"), floatValue);
...

//Before draw calls:
//Render with the effect
hEffect.Activate(dc); //Activate the effect first
hEffectInstance0.Apply(dc);  //Apply values to the effect
for (int i = 0; i < hEffect.GetPassCount(dc); ++i)
{
	hEffect.ActivatePass(dc, i); //Apply each pass
	pVirtualDevice.Draw(...);
}
hEffect.PassesFinished(dc); Finish this effect instance

hEffectInstance1.Apply(dc); //Apply next instance, which parent effect is same as above.
for (int i = 0; i < hEffect.GetPassCount(dc); ++i)
{
	hEffect.ActivatePass(dc, i); //Apply and draw this instance
	pVirtualDevice.Draw(...);
}
hEffect.PassesFinished(dc);

hEffect.Terminate(); //End of the effect usage.
\endcode
*/
class EffectHandle : public SmartHandle
{
public:

	GraphicsDriverAPI EffectHandle();
	GraphicsDriverAPI EffectHandle(const EffectHandle& from);
	GraphicsDriverAPI EffectHandle& operator = (const EffectHandle& from);
	GraphicsDriverAPI virtual ~EffectHandle();
	
	/** Activates the material handle with the specified pipeline context. 
	\param[in] dc The pipeline context.
	*/
	GraphicsDriverAPI void Activate(DrawContext& dc);

	/** Terminates the material handle, performing any cleanup required by the material handle. 
	*/
	GraphicsDriverAPI void Terminate();

	/** Retrieves the number of passes required to render the material handle.  
	Each geometry to be drawn with the effect must be drawn that number of times.
	This pass count can only be retrieved while the effect is active.
	\param[in] dc The pipeline context.
	\return Returns pass count.
	*/
	GraphicsDriverAPI unsigned int GetPassCount(DrawContext& dc) const;

	/** Activates the specified pass of the material handle, at which point it may render geometry as necessary for the pass.
	\param[in] dc The pipeline context.
	\param[in] pass The index of pass.
	*/
	GraphicsDriverAPI void ActivatePass(DrawContext& dc, unsigned int pass);

	/** Called after all of the passes for the material handle have been drawn. 
	\param[in] dc The pipeline context.
	*/
	GraphicsDriverAPI void PassesFinished(DrawContext& dc);

	/** Initialize an instance of FXEffectHandle.
	A FXEffectHandle should be initialized before it's used.
	\param[in] fileName HLSL shader file name.
	\return true if successfully initialized, false otherwise.
	*/
	GraphicsDriverAPI bool InitializeWithFile(const MSTR& fileName);

	/** Initialize an instance of FXEffectHandle with shader string.
	A FXEffectHandle should be initialized before it's used.
	\param[in] data HLSL shader content. 
	\param[in] length The length of data.
	\param[out] errorInfo A MSTR pointer, return error strings if effect is failed to load. Can be nullptr.
	\return true if successfully initialized, false otherwise.
	*/
	GraphicsDriverAPI bool InitializeWithData(void* data, size_t length, MSTR* errorInfo = nullptr);

	/** Initialize an instance of FXEffectHandle from resource.
	A FXEffectHandle should be initialized before it's used.
	\param[in] iShaderID resource ID of the shader
	\param[in] hInst module instance handle
	\param[in] strTypeName resource block name
	\param[out] errorInfo A MSTR pointer, return error strings if effect is failed to load. Can be nullptr.
	\return true if successfully initialized, false otherwise.
	*/
	GraphicsDriverAPI bool InitializeWithResource(int iShaderID, HINSTANCE hInst, const MSTR& strTypeName, MSTR* errorInfo = nullptr);
	
	/** Returns the parameter count of material.
	\return the parameter count.
	*/
	GraphicsDriverAPI int GetParameterCount() const;

	/** Returns the index of parameter.
	\param[in] parameterName The unique parameter name of the parameter.
	\return valid index if successful, otherwise -1
	*/
	GraphicsDriverAPI int GetParameterIndex(const MSTR& parameterName) const;

	/** Sets active technique by name
	    Only available for ".fx" based effect. 
	\param[in] techniqueName Name of the technique to activate
	*/
	GraphicsDriverAPI void SetActiveTechniqueByName(const MSTR& techniqueName);

	/** Sets active technique by zero-based index
		Only available for ".fx" based effect. 
	\param[in] techIndex Index of the technique to activate
	*/
	GraphicsDriverAPI void SetActiveTechnique(int techIndex);

	/** Gets current active technique index
	\return the active technique index
	*/
	GraphicsDriverAPI int GetActiveTechnique();

	/** Get a string list of technique names in the effect
	\return TechniqueNames a list of string contains technique names in the effect
	*/
	GraphicsDriverAPI TechniqueNames GetTechniqueNames();

	/** Create an new effect instance
	\return new instance of the effect parameter packet. It contains values of all parameters.
	*/
	GraphicsDriverAPI EffectInstanceHandle CreateEffectInstance();
};


} } // end namespace