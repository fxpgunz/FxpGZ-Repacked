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

#include "QtMax.h"
#include <QtWidgets/QWidget>

#include "../iparamb2Typedefs.h"

class IParamBlock2;
class ReferenceMaker;

namespace MaxSDK
{;

//==================================================================================================
// class QMaxParamBlockWidget
//
//! \brief Base class for QWidgets which are to be controlled by a parameter block/map.
/*! The Parameter Block/Map system may be used to control Qt widgets. ClassDesc2::CreateQtWidget() is
    used to create the Qt widget to be associated with a particular parameter map. But the parameter map
    system has internal mechanisms to point an existing parameter to a different object, enabling a UI
    to be re-directed without being re-created. This class wraps a QWidget, exposing functionality to
    re-direct the Qt widget to a new parameter block.
    \sa \ref parammap_qt. */
class QMaxParamBlockWidget : public QWidget
{
public:

    //! \brief Replaces the parameter block connected to the widget. 
    /*! The new parameter block replaces the one initially passed to ClassDesc2::CreateQtWidget(). The implementation
        is free to modify the widget as it wishes as a result of the contents of the new parameter block.
        \param owner The owner of the dialog. This will normally be the owner of the parameter block, but in the case of P_CLASS_PARAMS, where the parameter block doesn't
            have an owner, this will be the object for which the dialog is being displayed.
        \param param_block The new parameter block. This value may be null, typically as a temporary state when switching from editing
            one object to another. */
    virtual void SetParamBlock(ReferenceMaker* owner, IParamBlock2* const param_block) = 0;
    //! \brief Called whenever the entire UI needs to be updated, e.g. as a result of the time slider changing.
    /*! This method is expected to update the UI for any controls that are managed by this widget. Controls
        that are managed automatically by the parameter block system need not be updated.
        \param t The time at which the parameter values must be updated. */
    virtual void UpdateUI(const TimeValue t) = 0;
    //! \brief Called whenever a parameter value changes.
    /*! This method is expected to update any UI controls related to the parameter which has changed. Controls
        that are managed automatically by the parameter block system need not be updated.
        \param t The time at which the parameter values must be updated.
        \param param_id The ID of the parameter that has changed.
        \param tab_index The tab index of the parameter value that has changed. Applicable if and only if the parameter is a tab type. */
    virtual void UpdateParameterUI(const TimeValue t, const ParamID param_id, const int tab_index) = 0;
};

}       // namespace