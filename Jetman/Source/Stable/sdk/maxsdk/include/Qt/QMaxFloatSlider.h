//////////////////////////////////////////////////////////////////////////////
//
//  Copyright 2016 Autodesk, Inc.  All rights reserved.
//
//  Use of this software is subject to the terms of the Autodesk license
//  agreement provided at the time of installation or download, or which
//  otherwise accompanies this software in either electronic or hard copy form.
//
//////////////////////////////////////////////////////////////////////////////
#pragma once

#include "QtMax.h"
#include <QtWidgets/QWidget.h>

/*! \file
 * \warning This API is unsupported and provided for experiment only.  It should not be used for production.
 * This API will change or disappear in a future release.
 */

namespace MaxSDK
{;

// Builds a fancy horizontal slider control, as follows:
// The slider has a set of uniformly spaced tick marks, each associated with a label that is aligned to the tick mark.
// The labels and tick marks may be positioned above or below the slider.
// Each tick mark is associated with a floating-point values. The values are interpolated between the tick marks.
// A spin box control will optionally be created, positioned above or below the slider (on the opposite side of the labels).
/*!
 * \brief Qt Max slider for float values.
*/
class CoreExport QmaxFloatSlider :
    public QWidget
{
    Q_OBJECT

    Q_PROPERTY(float value READ value WRITE setValue NOTIFY valueChanged USER true)

    Q_PROPERTY(QString spinboxPrefix READ spinboxPrefix WRITE setSpinboxPrefix)
    Q_PROPERTY(QString spinboxSuffix READ spinboxSuffix WRITE setSpinboxSuffix)
    Q_PROPERTY(QString spinboxLabel READ spinboxLabel WRITE setSpinboxLabel)
    Q_PROPERTY(bool spinboxVisible READ spinboxVisible WRITE setSpinboxVisible)

    // These properties apply to the spinbox only, and use the generic "minimum/maximum/etc." property names for compatibility with generic functionality
    Q_PROPERTY(int decimals READ spinboxDecimals WRITE setSpinboxDecimals)
    Q_PROPERTY(float singleStep READ spinboxSingleStep WRITE setSpinboxSingleStep)
    Q_PROPERTY(float minimum READ spinboxMinimum WRITE setSpinboxMinimum)        
    Q_PROPERTY(float maximum READ spinboxMaximum WRITE setSpinboxMaximum)

public:

    enum class TickMarkPosition
    {
        Above,
        Below
    };

    QmaxFloatSlider(
        const TickMarkPosition tickmark_position,
        // The list of tick marks, with associated labels and values
        const std::vector<std::pair<QString, float>>& tick_marks,
        QWidget* parent = nullptr);

    ~QmaxFloatSlider();

    // Properties
    float value() const;
    QString spinboxPrefix() const;
    QString spinboxSuffix() const;
    QString spinboxLabel() const;
    bool spinboxVisible() const;
    int spinboxDecimals() const;
    float spinboxSingleStep() const;
    float spinboxMinimum() const;
    float spinboxMaximum() const;

public slots:

    void setValue(const float value);
    void setValue(const double value);
    void setSpinboxPrefix(const QString prefix);
    void setSpinboxSuffix(const QString suffix);
    void setSpinboxLabel(const QString label);
    void setSpinboxVisible(const bool visible);
    void setSpinboxDecimals(const int decimals);
    void setSpinboxSingleStep(const float single_step);
    void setSpinboxMinimum(const float minimum);
    void setSpinboxMaximum(const float maximum);

private slots:

    void slider_value_changed();

signals:

    void valueChanged(float value);

private:

    void BuildLayout();

private:

    // PImpl
    struct Implementation;
    Implementation* const m_implementation;
};


};
