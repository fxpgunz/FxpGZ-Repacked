//**************************************************************************/ 
// Copyright (c) 1998-2006 Autodesk, Inc. 
// All rights reserved. 
//  
//  Use of this software is subject to the terms of the Autodesk license  
//  agreement provided at the time of installation or download, or which  
//  otherwise accompanies this software in either electronic or hard copy form. 
//**************************************************************************/ 
// FILE:        IMNMeshDataChannel.h 
// DESCRIPTION: Data Channel Modifier Interfaces and enums
// AUTHOR:      Peter Watje
// HISTORY:     created 3/2016
//**************************************************************************/ 

#pragma once
#include <MaxHeap.h>
#include <maxapi.h>
#include <MaxTypes.h>
#include <inode.h>
#include <strclass.h>
#include <time.h>
#include <baseinterface.h>
#include <containers/Array.h>


/*! 
These are classes and enums to support the Data Channel Modifier.  The Data Channel Modifier can be extened by adding 
additional Engines.   This is done by having a plugin derive from IDataChannelEngine and using the SuperClassID DATACHANNELENGINE_SUPER_CLASS_ID.
Any plugins that do that will appear in the Data Channel modifiers drop down list.

An IDataChannelEngine is just a black box solution that takes an MNMesh and then outputs an array that describes the mesh.  For instance 
a Curvature engine would compute the curvature at each vertex and then return an array of floats one for each geometric vertex that describe the 
curvature.

The Data Channel modifier allows the vertex, edge and face channels to be modified.  NOTE channels here just represent an array 
of data that describes part of a mesh and is not related to channels that flow up the stack.  Vertex data is a bit tricky since 
some channels need face information and some do not.  For instance the Vertex Selection channel implicitly is attached to the 
geometry face index so you cannot change that while something like a map channel has an explicit face channel attached to it.  
The Map Channel modifier allows you to copy different face index channels around but not edit them directly.
*/

namespace MaxSDK
{
	namespace ChannelData
	{

		/*!
		A list of the input data types to be passed into the engine.  This will be extended as add more exposure to different types in
		different spaces.
		*/
		enum InputDataType {
			kType_INode = 0,		//! this entry will be pointer to the inode that owns the geometry in the stack
			kType_MNMesh,           //! this entry will be pointer to the MNMesh in the stack
			kType_Stack_Input,		//! this will be a pointer to a IOutputData that is the input from the previous engine

		};

		/*! TBD A custom blind data type for any data that is not covered in OutputDataTypes
		*/
		/*
		class CustomDataType
		{
		public:
			//! Number of elements in this type
			virtual unsigned int Count() = 0;
			//! Size of each elements 
			virtual unsigned int SizePerElement() = 0;
			//! Pointer to the data
			virtual void* Data() = 0;
			//! Deletes the data
			virtual void DeleteData() = 0;
		};
		*/

		/*!
		A list of the output data to be passed fomr the engine.  This will be extended as add more exposure to different types in
		different spaces.
		*/
		enum OutputDataType {

			kType_Int = 0,         //! int passed as pointer an int
			kType_Float,			//! float passed as pointer an float
			kType_Point3,			//! Point3 passed as pointer an point3
			kType_Matrix3,			//! Matrix3 passed as pointer an Matrix3

			kType_IntArray,         //! array of ints passed as pointer to MaxSDK::Array
			kType_FloatArray,			//! array of floats passed as pointer to MaxSDK::Array
			kType_Point3Array,			//! array of Point3's passed as pointer to MaxSDK::Array
			kType_Matrix3Array,			//! array of Matrix3's passed as pointer to MaxSDK::Array

			//TBD kType_Custom,			//! custom blind data passed as pointer something derived from CustomDataType
		};

		/*!
		A list of all all the MNMesh channel types you can write to 
		*/
		enum ChannelType
		{
			kChannel_Vertex = 0,	//!  Vertex channel which can include the position, normals, selection etc
			kChannel_Edge,			//!  Edge channel which can include the  selection, crease etc
			kChannel_Face,			//!  Face chanel which can include matids, smoothing groups etx
			kChannel_NoChannelAssigned,		//!  Means that this data does not use a channel and should be returned if there is no output
			kChannel_DoesNotCare,		//!  Means that this data is not associated with a channel and will work on any channel type

		};
		/*!
		List of vertex channels you can write to
		*/
		enum VertexOutputChannel
		{
			kOutputChannel_XYZ = 0,				//! The position channel this also has an implied face channel attached to it
			kOutputChannel_VertexColor,			//! The verte color channel this also has an implied face channel attached to it (really map channel 1)
			kOutputChannel_MapChannel,			//! The map channel this also has an implied face channel attached to it (really map channels -2 to 99)
			kOutputChannel_Normal,				//! The explicit normals channel this also has an implied face channel attached to it (really map channels -2 to 99)
			kOutputChannel_VertexSelection,		//! This is the soft selection channel implicitly tied to the position channel
			kOutputChannel_VertexCreaseWeight,	//! This is the crease weight channel implicitly tied to the position channel
			kOutputChannel_VData,				//! TBD this is the VData channel implicitly tied to the position channel 

		};

		/*!
		List of edge channels you can write to
		*/
		enum EdgeOutputChannel
		{
			kOutputChannel_EdgeSelection = 0,	//! Edge selection data
			kOutputChannel_EdgeCreaseWeight,	//! Edge crease weight data
			kOutputChannel_EData,				//! TBD edge data
		};

		/*!
		List of face channels you can write to
		*/
		enum FaceOutputChannel
		{
			kOutputChannel_FaceSelection = 0,	//! Face selection channel
			kOutputChannel_MatID,				//! Material ID channel
			kOutputChannel_SmoothGroup,			//! Smoothing Group ID channel
			kOutputChannel_FData,				//! TBD face data
		};

		/*!
		Vertex channels are bit tricky since they are tied to a face index channel this enumerates the
		different channels
		*/
		enum IndexChannel
		{
			kIndexChannel_None = 0,				//! The data is not associated with any face indices ( things like edge or face data )
			kIndexChannel_Position,				//! Position face indices
			kIndexChannel_Normal,				//! Normal face indices
			kIndexChannel_MapChannel,			//! Map face indices all others in implied to be attached to the position channel.  We start at -2 which is the illum channel, not cannot have any enums after this one 
		};


		/*!
		Descripes an input entry which consists of an type description and pointer to the data
		*/
		class IInputData : public MaxHeapOperators
		{
		public:
			/*!
			Constructor 
			\param type describes the data to be used for input
			\param data pointer to the actual data
			*/
			IInputData(InputDataType type, void* data)
			{
				mType = type;
				mData = data;
			}
			/*!
			Constructor
			*/
			IInputData()
			{
				mType = InputDataType::kType_INode;
				mData = nullptr;
			}
			/*! Returns the data description */
			InputDataType GetType() const { return mType; }
			/*! Returns a pointer to teh actual data*/
			void* Get() const { return mData; }
		protected:

		private:
			InputDataType mType;
			void* mData;
		};

		/*!
		Descripes an output entry which consists of an type description and pointer to the data
		Also lets the engine tell the modifier what channel it supports
		Other data that the engine needs to store per instance can also be stored here since the output
		data is stored on the modifier local data for modifiers
		*/
		class IOutputData : public MaxHeapOperators
		{

		public:
			//! Constructor
			IOutputData()
			{
				mType = kType_Int;
				mChannelType = kChannel_NoChannelAssigned;
				mIndexChannel = kIndexChannel_Position;
				mData = nullptr;
			}
			/*!
			Constructor
			\param type describes the data to be used for output
			\param data pointer to the actual data
			*/
			IOutputData(OutputDataType type, void* data)
			{
				mType = type;
				mData = data;
			}
			
			/*!
			Just a quick way to set the data and type
			\param type describes the data to be used for output
			\param data pointer to the actual data
			*/
			void SetData(OutputDataType type, void* data)
			{
				mType = type;
				mData = data;
			}
			/*! Returns the channel that this data is appropriate for*/
			ChannelType GetChannels() const { return mChannelType; }
			/*! Sets the channel that this data is appropriate for 
			\param chan the type of channel that we are setting			*/
			void SetChannels(ChannelType chan) { mChannelType = chan; }
			/*! Returns the index channel that this data is appropriate for*/
			IndexChannel GetIndexChannel() const { return mIndexChannel; }
			/*! Sets the index channel that this data is appropriate for 
			\param chan the type of index channel that we are setting			*/
			void SetIndexChannel(IndexChannel chan) { mIndexChannel = chan; }
			/*! Returns the data description */
			OutputDataType GetType()  const  { return mType; }
			/*! Sets the data type of the channel */
			void SetType(OutputDataType type) { mType = type; }
			/*! Returns the pointer to the actual data*/
			void* Get()  const { return mData; }
		protected:

		private:
			ChannelType mChannelType;
			IndexChannel mIndexChannel;
			OutputDataType mType;
			void* mData;
		};

		/*!  These are enums that describe the engine type.There are 3 an Input, Operand and Output.
			 An Input Engine just creates data that is sent to the stack.
			 An Operand takes data flowing thru the stack and modifies it and then puts it back on the stack
			An Output takes the data and pushes it back to the mesh
		*/
		enum EngineTypes
		{
			kEngine_Error = 0,      //! Error condition 
			kEngine_Input,          //! Input Engine just creates data that is sent to the stack. 
			kEngine_Operand,            //! Operand takes data flowing thru the stack and modifies it and then puts it back on the stack 
			kEngine_Output,             //! Output takes the data and pushes it back to the mesh 
		};



		/*! Interface for IDataChannelEngine */
		#define DATACHANNELENGINE_INTERFACE Interface_ID(0xDE21BB1f, 0x82444a1)

		/*! This is a black box engine that takes some input and outputs some data.  For the DataChannel modifier the input is
			an MNMesh and Node and the output can be vertex, edge or face channel data.   Engines come in 3 flavors an Input Engine
			which does not use kType_Stack_Input and just generates a channel, a Operand Engine which takes an input channel and generates an
			output channel, and finally an Output Engine which does not generate an output channel and usually pushes that stack data (input)
			to the MNMesh */
		class IDataChannelEngine : public ReferenceTarget, public BaseInterface
		{

		public:

			/*! All Data Channel engines are in this superclass */
			virtual SClass_ID SuperClassID() { return DATACHANNELENGINE_SUPER_CLASS_ID; }
			/*! Returns the name to be used in the UI for the Data Channel modifier 
			\param name the name to be used in the Data Channel Modifier
			*/
			virtual void GetUIName(TSTR& name) = 0;
			/*! Returns the Operand Info to be used by the ui (uses Tooltip string by default)
			*/
			virtual void GetOperandInfo(TSTR& info) { GetTooltip(info); }
			/*! Returns the tooltip to be used by the ui
			*/
			virtual void GetTooltip(TSTR& name) = 0;
			/*! Returns what this engine needs for data */
			virtual MaxSDK::Array<InputDataType> InputRequirements() = 0;
			/*! Returns what this engine outputs for data. Per instance data can be stored here also though if you change the requirements existing data may be deleted */
			virtual MaxSDK::Array<OutputDataType> OutputRequirements() = 0;
			/*! Returns what channel the output data can written to NOTE only one output channel is supported and the first one is the only one applied to the mnmesh */
			virtual MaxSDK::Array<ChannelType> OutputChannelTypes() = 0;
			/*! Vertex channels have an face index channel assigned to them.  This returns what face index channel the output data is associated with.  Only applies if the output channel is a vertex channel*/
			virtual IndexChannel IndexChannelToBeUsed() = 0;

			/*! This is called before the output requirements are called so the engine can respond to 
				a change in the input type and adjust the output appropriately.  Only an engine that requires
				an input channel from the stack ie has an input channel set to kType_Stack_Input needs to listen to this 
				\param data is the channel containing the stack array coming in from the stack in response from the kType_Stack_Input flag set as an input requirement */
			virtual void InputType(IOutputData data) {UNUSED_PARAM(data);};
			/*! This lets the modifier know that the engine is in valid state with the input data.  For instance
				if the input type changed to a data that was not supported it can send an error message to the user
				that this is not a valid case 
				\param errorString is a string describing why the engine failed */
			virtual bool ValidConditions(TSTR& errorString) { UNUSED_PARAM(errorString);return true;  };

			/*! This does all the work.  It takes the input data and then fills out the output data 
				\param t the time that the data is to be processed
				\param inData the data
				\param outData where the output data is stored
				\param tempPerInstanceData is a flag that can be used to signal UI commands for the process.  So the UI can set all the flags 
					in mPerTempInstanceData array then in the Process can check the tempPerInstanceData and test and clear it.
				\param channelsChanged are the parts of the MNMesh that have changed flowing up the stack to the modifier to the engine used to rebuild/update any cached data the engine may have created.
				Returns the interval that the outdata is good for
				*/
			virtual Interval Process(TimeValue t, MaxSDK::Array<IInputData>& inData, MaxSDK::Array<IOutputData>& outData, int& tempPerInstanceData, PartID& channelsChanged) = 0;

			/*! This allows engines that creates floats to define the domain of there values.  This gives the display some hints on how to gradiate the color			*/
			virtual bool HasDomain() { return false; }
			/*! Returns the min value for the domain			*/
			virtual float DomainMin() { return -1.0E38; }
			/*! Returns the max value for the domain			*/
			virtual float DomainMax() { return 1.0E38; }

			/*! This allows the engine to override the modifier display.
				Return true and implement display if you want a custom display
			*/
			virtual bool CustomDisplay() { return false; }
			/*! Same display from base object except outdata which is the data that the engine created in the process method			*/
			virtual int Display(TimeValue t, INode* inode, ViewExp* vpt, int flagst, MaxSDK::Array<IOutputData>& outData) { UNUSED_PARAM(t); UNUSED_PARAM(inode); UNUSED_PARAM(vpt); UNUSED_PARAM(flagst); UNUSED_PARAM(outData); return 1; }


			/*! Lets the engine bring up any UI that is needed */
			virtual void BeginEditParams(IObjParam  *ip, ULONG flags, Animatable *prev) = 0;
			/*! Lets the engine tear down any UI that is created */
			virtual void EndEditParams(IObjParam *ip, ULONG flags, Animatable *next) = 0;

			/* Sets the per instance data array which is an array of ints one per instance which allows the engines to set
			temporary flags at a per instance level 
			\param data the array of per instance data to be stored */
			void SetPerInstanceData(MaxSDK::Array<int>* data) { mPerTempInstanceData = data; }

			/* Override this method to allow an engine to force the system to treat it as an Input engine.  This allows engines to optionally use
			the kType_Stack_Input channel, if it's there, and not be restricted to use as an Operand engine.  Note that any engine returning "true"
			here must be aware that the kType_Stack_Input channel may not actually be available at Process time. */
			virtual bool ForceAsInputEngine() { return false; }


			/*! **************  WIP  FUTURE THREADING SUPPORT ****************************  */

			/*! This returns whether the engine is thread safe.  If the engine is not thread safe only the Process will be called.  If all the engines in the stack are thread safe
			   first the engine Threaded_Setup is called on all the engines sequentially to allow for non thread safe operatiuons to be called and cached like getvalue, getobjecttm etc.
			   This lets the engines do any non thread safe operations.  Then the engines Threaded_Process will be called on a block of data that the engine is to proces.
			   NOTE operands that write directly to the mnmesh are not thread safe and typically output operands are not thread safe
			   */
			virtual bool Threadable() { return false;  }

			/*! This is where all the non thread safe calls should be processed and cached before the Threaded_Process is called.  All the channel allocations should be done here.
			\param t the time that the data is to be processed
			\param inData the data
			\param outData where the output data is stored
			\param tempPerInstanceData is a flag that can be used to signal UI commands for the process.  So the UI can set all the flags
			in mPerTempInstanceData array then in the Process can check the tempPerInstanceData and test and clear it.
			\param channelsChanged are the parts of the MNMesh that have changed flowing up the stack to the modifier to the engine used to rebuild/update any cached data the engine may have created.
			Returns the interval that the outdata is good for
			*/
			virtual Interval Threaded_Setup(TimeValue t, MaxSDK::Array<IInputData>& inData, MaxSDK::Array<IOutputData>& outData, int& tempPerInstanceData, PartID& channelsChanged) { UNUSED_PARAM(t); UNUSED_PARAM(inData); UNUSED_PARAM(outData); UNUSED_PARAM(tempPerInstanceData); UNUSED_PARAM(channelsChanged); return FOREVER; };

			/*! This does all the work.  It takes the input data and then fills out the output data.  This allows for more efficient threeding insteading each engine doing it own threading.  The threading is vertical
			instead of horizontal allowing for more scalability has the stack grows.
			\param startIndex is the output channel element that you should begin working on
			\param endIndex is the output channel element that you should stop working on
			\param t the time that the data is to be processed
			\param inData the data
			\param outData where the output data is stored
			\param tempPerInstanceData is a flag that can be used to signal UI commands for the process.  So the UI can set all the flags
			in mPerTempInstanceData array then in the Process can check the tempPerInstanceData and test and clear it.
			\param channelsChanged are the parts of the MNMesh that have changed flowing up the stack to the modifier to the engine used to rebuild/update any cached data the engine may have created.
			Returns the interval that the outdata is good for
			*/
			virtual Interval Threaded_Process(const int startIndex, const int endIndex, TimeValue t, MaxSDK::Array<IInputData>& inData, MaxSDK::Array<IOutputData>& outData, int& tempPerInstanceData, PartID& channelsChanged) { UNUSED_PARAM(startIndex); UNUSED_PARAM(endIndex); UNUSED_PARAM(t); UNUSED_PARAM(inData); UNUSED_PARAM(outData); UNUSED_PARAM(tempPerInstanceData); UNUSED_PARAM(channelsChanged); return FOREVER; };



		protected:

			/*! A list of all the temporary per instance data used to handle UI.  Since the UI is tied to the single instance but there
			can be multiple local data which needs to be processed this can let you mark which local data has been processed.  The life time of 
			the data the pointer's points to is managed by the Data Channel Modifier's local data */
			MaxSDK::Array<int>* mPerTempInstanceData;


		};
	}
}