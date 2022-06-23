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

/** \file NotificationAPI_Events.h

		This file contains The notification API events we receive when something is updated in the scene
*/

// Standard includes
#include <stdio.h>
// Max SDK includes
#include <strclass.h>
#include <containers/Array.h>
#include <ref.h>

class Mtl;
class Texmap;
class INode;
class ReferenceTarget;
class ViewExp;

namespace MaxSDK
{;
namespace NotificationAPI
{;
	//! The classes instances we receive when we are notified of a change:

    //! The update types flags for nodes, you will receive a INodeEvent
    enum NodeEventType
    {
        /** @name NodeEventType
		*/
		//@{
        EventType_Node_ParamBlock          =1<<0,   //!< Parameter from Parameter block updated, event sent is INodeParamBlockEvent
        EventType_Node_Uncategorized       =1<<1,   //!< A generic event for changes which cannot be categorized, for example as a result of NotifyRefChanged(REFMSG_CHANGE, PART_ALL). In some cases, this event may be sent alongside other, categorized events (e.g. ParamBlock changes), as some plugins may send multiple events for a single change.
		EventType_Node_WireColor           =1<<2,   //!< INode wirecolor updated
		EventType_Node_Transform           =1<<3,   //!< INode transform matrix updated
	
		EventType_Node_Deleted             =1<<4,   //!< INode deleted from the scene
		EventType_Node_Hide                =1<<5,	//!< INode hidden or unhidden, query the node to know
        EventType_Node_Selection           =1<<6,	//!< INode selected or unselected
		EventType_Node_Material_Replaced   =1<<7,	//!< INode material has been replaced by another material, in that case you will receive a INodeMaterialEvent
        EventType_Node_Material_Updated    =1<<8,	//!< INode material has been updated, in that case you will receive a INodeEvent
        EventType_Node_Reference           =1<<9,	//!< INode subanim structure changed, for example a shadow generator instance referenced by a light node has changed, when you receive this message, you need to retranslate the whole node
        
        EventType_Node_RenderProperty      =1<<10,  //!< INodePropertyEvent
        EventType_Node_GIProperty          =1<<11,  //!< INodePropertyEvent
        EventType_Node_DisplayProperty     =1<<12,  //!< INodePropertyEvent

        EventType_Mesh_Vertices            =1<<13,	//!< Mesh vertices updated
		EventType_Mesh_Faces               =1<<14,	//!< Mesh faces updated
		EventType_Mesh_UVs                 =1<<15,	//!< Mesh UVs updated
		//@}
    };
    
    //! The update types for materials, you will receive a IMaterialEvent
    enum MaterialEventType{
        /** @name MaterialEventType
		*/
		//@{
		EventType_Material_ParamBlock      =1<<0,//!< Parameter from Parameter block updated, in that case use the GetParamBlockData function from the Event to retrieve which param has changed
        EventType_Material_Deleted         =1<<1,//!< material deleted
        EventType_Material_Reference       =1<<2,//!< material reference has changed, use this message only if EventType_Material_ParamBlock does not work for the event you are trying to catch as if you monitor both kinds of events you are likely to receive 2 notifications for a change in a paramblock such as add a texmap in a slot.
        EventType_Material_Uncategorized   =1<<3,//!< A generic event for changes which cannot be categorized, for example as a result of NotifyRefChanged(REFMSG_CHANGE, PART_ALL). In some cases, this event may be sent alongside other, categorized events (e.g. ParamBlock changes), as some plugins may send multiple events for a single change.
		//@}
    };

    //! The update types for materials, you will receive a ITexmapEvent
    enum TexmapEventType{
        /** @name TexmapEventType
		*/
		//@{
		EventType_Texmap_ParamBlock      =1<<0,//!< Parameter from Parameter block updated, in that case use the GetParamBlockData function from the Event to retrieve which param has changed
        EventType_Texmap_Deleted         =1<<1,//!< texmap deleted
        EventType_Texmap_Uncategorized   =1<<2,//!< A generic event for changes which cannot be categorized, for example as a result of NotifyRefChanged(REFMSG_CHANGE, PART_ALL). In some cases, this event may be sent alongside other, categorized events (e.g. ParamBlock changes), as some plugins may send multiple events for a single change.
		//@}
    };

    //! The update types for materials, you will receive a ITexmapEvent
    enum ReferenceTargetEventType{
		EventType_ReferenceTarget_ParamBlock      =1<<0,//!< Parameter from Parameter block updated, in that case use the GetParamBlockData function from the Event to retrieve which param has changed
        EventType_ReferenceTarget_Deleted         =1<<1,//!< reference target deleted
        EventType_ReferenceTarget_Uncategorized   =1<<2,//!< A generic event for changes which cannot be categorized, for example as a result of NotifyRefChanged(REFMSG_CHANGE, PART_ALL). In some cases, this event may be sent alongside other, categorized events (e.g. ParamBlock changes), as some plugins may send multiple events for a single change.
    };

    enum ViewEventType{
        /** @name ViewEventType
		*/
		//@{
		EventType_View_Properties = 1<<0,		//!< View properties updated -transform, FOV, etc. - (view can be a camera, light or viewport) , you will receive a IViewEvent
        EventType_View_Active    = 1<<1,	    //!< Active view switched to another view (new view can be a camera, light or viewport) , you will receive a IViewEvent
        EventType_View_Deleted   = 1<<2,	    //!< Active view was deleted (camera node was deleted for example), new view type is monitored automatically, you will receive a IViewEvent
		//@}
    };

    enum SceneEventType{
        /** @name SceneEventType
		*/
		//@{
        EventType_Scene_Node_Added               =1<<0,	//!< INode added to the scene, you will receive a ISceneNodeEvent
        EventType_Scene_Node_Removed               =1<<1,	//!< INode removed from the scene, you will receive a ISceneNodeEvent
        //@}
    };

     enum RenderSettingsEventType{
        /** @name RenderSettingsEventType
		*/
		//@{
        EventType_RenderSettings_LockView       =1<<0,	//!< Lock/Unlock view check button triggered from the render settings dialog, you will receive a IGenericEvent
        //@}
    };

	enum RenderEnvironmentEventType{
        /** @name RenderEnvironmentEventType
		*/
		//@{
        EventType_RenderEnvironment_BackgroundColor     =1<<0,	//!< Rendering environment background color changed
		EventType_RenderEnvironment_EnvironmentMap      =1<<1,	//!< Rendering environment background environment map changed 
		EventType_RenderEnvironment_EnvironmentMapState =2<<2	//!< Rendering environment background environment map state changed. Invoked by the "Use Map" checkbox toggling
        //@}
    };
    
	//! The types of notifiers which may be registered, each corresponding to its own event class
	enum NotifierType
    {
		NotifierType_Node_Camera,		//!< INodeEvent, for a node that evaluates to a CameraObject
		NotifierType_Node_Light,        //!< INodeEvent, for a node that evaluates to a LightObject
		NotifierType_Node_Helper,		//!< INodeEvent, for a node that evaluates to a HelperObject
		NotifierType_Node_Geom,		    //!< INodeEvent, for a node that evaluates to a GeomObject
		NotifierType_Material,		    //!< IMaterialEvent
		NotifierType_Texmap,		    //!< ITexmapEvent
        NotifierType_ReferenceTarget,	//!< IReferenceTargetEvent
		NotifierType_View,	    	    //!< IViewEvent
		NotifierType_SceneNode,         //!< ISceneNodeEvent
        NotifierType_RenderSettings, 	//!< GenericEvent
        NotifierType_RenderEnvironment, //!< GenericEvent
	};
		
	//! IGenericEvent is a common class to all notifications events received:
	class IGenericEvent 
	{	
	public:
		//! Get the type of notifier:
		virtual NotifierType	GetNotifierType	(void)const=0;
		//! Get the update type
		virtual size_t		    GetEventType	(void)const=0;
        //! For debugging purpose you may use this to print the event to an existing file stream
        virtual void		    DebugPrintToFile(FILE* f)const=0;
    protected:
        // All events are owned by the notification API and may not be released through this interface.
        virtual ~IGenericEvent(){};
	};

    //! Is a data structure for parameter blocks updates, used with events such as : EventType_Material_ParamBlock, EventType_Node_ParamBlock, ... All related to paramblocks updates
    struct ParamBlockData
    {
        enum ParameterBlockContainerType{
            /** @name ParameterBlockContainerType
            Is the container type of the paramblock
		    */
		    //@{
            UNKNOWN_CONTAINER_TYPE = 0, //!< Unknown is an error
            REFERENCES,                 //!< means the paramblock data comes from the References of the ReferenceTarget
            CUSTOM_ATTRIBUTES,          //!< means the paramblock data comes from the Custom attributes of the ReferenceTarget
            SHADER,                     //!< means the paramblock data comes from a Shader* being part of the ReferenceTarget
            //@}
       };

       enum ParameterBlockType{
            /** @name ParameterBlockType
		    */
		    //@{
            UNKNOWN_PB_TYPE = 0,//!< Unknown is an error
            PB_ONE,     //!< means we are dealing with a paramblock 1
            PB_TWO,     //!< means we are dealing with a paramblock 2
            //@}
       };

       //! Is a data structure for parameter blocks container information
       struct ContainerTypeAndIndex{
            ParameterBlockContainerType m_Type;  //!< is the parameter block container type
            int                         m_Index; //!< is the index in the array of container type (as an example say m_Type = REFERENCES, then m_Index should be used with Object* pObj; IParamBlock* pbock = pObj->GetReference(m_Index);
            bool operator==(const ContainerTypeAndIndex& other){return (other.m_Index == m_Index && other.m_Type == m_Type);}
            bool operator!=(const ContainerTypeAndIndex& other){return (other.m_Index != m_Index || other.m_Type != m_Type);}
       };

       //!<Retrieve the parameter block that was updated in the references or custom attributes or whatever of the Object* from the INode* or Mtl* or Texmap* etc...
       //!< Example is a parameter from the shadow generator of a light node has been updated :
       //!< m_ParamBlockType is the type of the param block such as PB1 or PB2
       //!< m_ParamBlockIndexPath contains the path to retrieve which parameter block was udpated.
       //!<   Example : if m_ParamBlockIndexPath contains 2 elements like :
       //!<     m_ParamBlockIndexPath[0].m_Type  = CUSTOM_ATTRIBUTES;
       //!<     m_ParamBlockIndexPath[0].m_Index = 3;
       //!<     m_ParamBlockIndexPath[1].m_Type  = REFERENCES;
       //!<     m_ParamBlockIndexPath[1].m_Index = 4;
       //!<   So to retrieve which paramblock was updated you have to use :
       //!<   Say, it's a node : INode* pNode = initialized somewhere;
       //!<     //Using minimal code checking for clarity
       //!<      ICustAttribContainer* custAttribCont = pNode->GetCustAttribContainer();
       //!<      const int indexInCustAttr = m_ParamBlockIndexPath[0].m_Index;
       //!<      CustAttrib* custAttrib = custAttribCont->GetCustAttrib(indexInCustAttr);
       //!<      //Now get i-th reference from that custom attrib. using the index from m_ParamBlockIndexPath[1].m_Index
       //!<      const int indexInReferences = m_ParamBlockIndexPath[1].m_Index;
       //!<      RefTargetHandle pSubRefTarg = custAttrib->GetReference(indexInReferences);
       //!<      //And what we get now should be a paramblock of type PB_ONe or PB_TWO
       //!<      switch (m_Type){
       //!<        case PB_ONE:{
       //!<           IParamBlock* pblock1        = dynamic_cast<IParamBlock*>(pSubRefTarg);
       //!<        }break;
       //!<        case PB_TWO:{
       //!<           IParamBlock2* pblock2        = dynamic_cast<IParamBlock2*>(pSubRefTarg);
       //!<        }break;
       //!<      }
       //!< m_ParametersIDsUpdatedInThatParamBlock contains the ParamIDs(in case of type PB_TWO or the Param indices in case of type PB_ONE) which were updated
       //!< m_ParametersNames contains all the parameters names that were updated wherever they come from so you can use only the names if you want.
       
       ParameterBlockType                       m_ParamBlockType;                       //!<Retrieve the parameter block type (can be IParamBlock or IParamBlock2 or..)
       MaxSDK::Array<ContainerTypeAndIndex>     m_ParamBlockIndexPath;                  //!<Retrieve the parameter block containers path
       MaxSDK::Array<int>                       m_ParametersIDsUpdatedInThatParamBlock; //!<Retrieve all the parameters updated in the parameter block
       MaxSDK::Array<MSTR>                      m_ParametersNames;                      //!<Retrieve all the parameters names that were updated in the parameter block (whereever they come from references/custom attributes etc.)
    };

	//! You receive a IMaterialEvent instance when something is changed in a Material:
    //! Events monitored : all related to materials
	class IMaterialEvent : public IGenericEvent
	{
	public:
		virtual Mtl* GetMtl(void)const=0; //!<Retrieve the Mtl*
        virtual const MaxSDK::Array<ParamBlockData>& GetParamBlockData (void)const=0; //!<Retrieve all the parameters updated in the parameter blocks in case of EventType_Material_ParamBlock, they are sorted by indices of the parameterblocks in the References of the object
    protected:
        // All events are owned by the notification API and may not be released through this interface.
        virtual ~IMaterialEvent(){};
	};

	//! You receive a ITexmapEvent instance when something is changed in a Texmap:
    //! Events monitored : all related to texmaps
	class ITexmapEvent : public IGenericEvent
	{
	public:
		virtual Texmap* GetTexmap(void)const=0; //!<Retrieve the Texmap*
        virtual const MaxSDK::Array<ParamBlockData>& GetParamBlockData(void)const=0; //!<Retrieve all the parameters updated in the parameter blocks in case of EventType_Texmap_ParamBlock, they are sorted by indices of the parameterblocks in the References of the object
    protected:
        // All events are owned by the notification API and may not be released through this interface.
        virtual ~ITexmapEvent(){};
	};

    //! You receive a IReferenceTargetEvent instance when something is changed in a ReferenceTarget:
    //! Events monitored : all related to reference targetss
    class IReferenceTargetEvent : public IGenericEvent
    {
    public:
        virtual ReferenceTarget* GetReferenceTarget(void)const=0; //!<Retrieve the ReferenceTarget*
        virtual const MaxSDK::Array<ParamBlockData>& GetParamBlockData(void)const=0; //!<Retrieve all the parameters updated in the parameter blocks in case of EventType_ReferenceTarget_ParamBlock, they are sorted by indices of the parameterblocks in the References of the object
    protected:
        // All events are owned by the notification API and may not be released through this interface.
        virtual ~IReferenceTargetEvent(){};
    };

    //! Base class for all events associated with notifier types: NotifierType_Node_*
	class INodeEvent : public IGenericEvent
	{
	public:
		virtual INode* GetNode(void) const = 0;//!<Retrieve the INode*
    protected:
        // All events are owned by the notification API and may not be released through this interface.
        virtual ~INodeEvent(){};
	};

    //! Event related to a parameter block change in a node's object.
    class INodeParamBlockEvent : public virtual INodeEvent
    {
    public:
        virtual const MaxSDK::Array<ParamBlockData>& GetParamBlockData(void)const=0; //!<Retrieve all the parameters updated in the parameter blocks in case of EventType_Node_ParamBlock, they are sorted by indices of the parameterblocks in the References of the object
    protected:
        // All events are owned by the notification API and may not be released through this interface.
        virtual ~INodeParamBlockEvent(){};
    };

    //! Event related to a change in a node's object GI properties
    class INodePropertyEvent : public virtual INodeEvent
    {
    public:
        /*! The PartID refers to the node property which has changed. 
            Value values are depend on the even type: 
            \li EventType_Node_RenderProperty: \ref partid_node_render_property. 
            \li EventType_Node_GIProperty: \ref partid_node_global_illumination_property. 
            \li EventType_Node_DisplayProperty: \ref partid_disp_props.  */
        virtual PartID GetPropertyID() const = 0;
    protected:
        // All events are owned by the notification API and may not be released through this interface.
        virtual ~INodePropertyEvent(){};
    };

    //! You receive a IViewEvent instance when something dealing with the view has changed:
    //! Events received with that class : EventType_View_Transform or EventType_View_Active
    class IViewEvent : public IGenericEvent
    {
    public:
        //! Returns true if the active view is a camera or a light node, false if it's a viewport
        virtual bool ViewIsACameraOrLightNode() const = 0;   
        //! Returns the INode* of the new active view if it is a camera or a light node, NULL if it's a viewport
        virtual INode* GetViewCameraOrLightNode() const = 0;    
        //! Returns the newly active viewport. The pointer will not normally be null, but could be so in the case of an internal error.
        virtual ViewExp* GetView() const = 0; 
        //! Returns the 'view undo ID' associated with the active viewport, for internal use.
        virtual int GetViewID() const = 0; 
    protected:
        // All events are owned by the notification API and may not be released through this interface.
        virtual ~IViewEvent(){};
    };

    //! You receive a ISceneNodeEvent instance when some nodes have been added/removed to/from the scene
    //! Event monitored: EventType_Scene_Node_Added/EventType_Scene_Node_Removed
    class ISceneNodeEvent : public IGenericEvent
	{
	public:
        virtual INode* GetNode() const = 0;
    protected:
        // All events are owned by the notification API and may not be released through this interface.
        virtual ~ISceneNodeEvent(){};
	};

    };//end of namespace NotificationAPI
};//end of namespace Max