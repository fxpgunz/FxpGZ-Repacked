//-----------------------------------------------------------------------------
// Copyright 2016 Autodesk, Inc.  All rights reserved.
//
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.
//-----------------------------------------------------------------------------

#pragma once

#include <CoreExport.h>
#include <Qt/QtMax.h>
#include <QtWidgets/QWidget>


namespace MaxSDK
{

//-----------------------------------------------------------------------------
// class QmaxMultiSpinner
//-----------------------------------------------------------------------------
/* \brief This class can be used as a base class for the creation of a multi-spinner
* UI control that works on 3ds Max array type data structures, such as Point2, Point3, etc. .
*
* The Qt meta-object compiler does not work with template classes, so the 
* QmaxMultiSpinner class works on a QVariant for its spinner value. This spinner
* value contains the 3ds Max data type. 
* When sub-classing from QmaxMultiSpinner, you'll need to implement two mapping functions
* that map the multiple spinner values to the 3ds Max data type structure and vice versa.
* \see toMaxTypeVariant(), fromMaxTypeVariant()
*/
class QmaxMultiSpinnerPrivate;
class CoreExport QmaxMultiSpinner : public QWidget
{
	Q_OBJECT

	/** \brief The 3ds Max type value wrapped in a variant associated with this spinner control.
	* \see value(), setValue(), valueChanged() */
	Q_PROPERTY( QVariant value READ value WRITE setValue NOTIFY valueChanged );

	/** \brief The reset value of this Component.
	* \see resetValue(), setResetValue() */
	Q_PROPERTY( QVariant resetValue READ resetValue WRITE setResetValue )

	/** \brief Indicates whether the animation key brackets are shown around the component.
	* \see hasAnimationKeyBrackets(), setAnimationKeyBrackets() */
	Q_PROPERTY( bool animationKeyBrackets READ hasAnimationKeyBrackets WRITE setAnimationKeyBrackets )
	
	/** \brief The minimum value of the spinners.
	* The default is 0.
	* \see minimum(), setMinimum(), maximum(), setMaximum() */
	Q_PROPERTY( double minimum READ minimum WRITE setMinimum )

	/** \brief The maximum value of the spinners.
	* The default is 99.99.
	* \see maximum(), setMaximum(), minimum(), setMinimum() */
	Q_PROPERTY( double maximum READ maximum WRITE setMaximum )

	/** \brief The step increment/decrement value of the spinners.
	* The singleStep value is applied to the current spinner value when
	* the user clicks on the up/down arrow buttons of the spinner. 
	* The default is 1.0.
	* \see singleStep(), setSingleStep() */
	Q_PROPERTY( double singleStep READ singleStep WRITE setSingleStep )

	/** \brief The prefix of the spinners.
	* The prefix is prepended to the displayed value of the spinner.
	* A common usage for a suffix would be the display of a unit of
	* measurement or a currency symbol.
	* For displaying no prefix just apply an empty string.
	* By default no prefix is shown.
	* \see prefix(), setPrefix(), suffix(), setSuffix() */
	Q_PROPERTY( QString prefix READ prefix WRITE setPrefix )

	/** \brief The suffix of the spinners.
	* The suffix is appended to the displayed value of the spinner.
	* A common usage for a suffix would be the display of a unit of 
	* measurement or a currency symbol.
	* For displaying no suffix just apply an empty string.
	* By default no suffix is shown.
	* \see suffix(), setSuffix(), prefix(), setPrefix() */
	Q_PROPERTY( QString suffix READ suffix WRITE setSuffix )

	/** \brief The decimals of the spinners.
	* Sets how many decimals the spinners will use for displaying and interpreting doubles.
	* The default is 3.
	* \see decimals(), setDecimals() */
	Q_PROPERTY(int decimals READ decimals WRITE setDecimals)

public:

	/** \brief Constructs a multi-spinner control with a predefined number of spinners.
	* \param numSpinners The number of spinners the control should have.
	* \param numCols The number of layout columns in which the multiple spinners should be arranged.
	* \param parent The control's widget parent.
	*/
	explicit QmaxMultiSpinner( int numSpinners, int numCols, QWidget* parent = nullptr );

	virtual ~QmaxMultiSpinner();

	/** \brief The 3ds Max type value wrapped in a variant associated with this spinner control.
	* \see setValue(), valueChanged() */
	QVariant value() const;

	/** \brief The reset value of this component.
	* If a user right-clicks the button-area of a spinner, it will be
	* reset to this value.
	* \return The reset - aka default - value of this component.
	* \see reset(), setResetValue() */
	QVariant resetValue() const;

	/** \brief Determines if the animation key brackets around the spinners
	*         should be shown.
	* \note The whole animationKeyBrackets property normally gets driven
	*       internally by a param-block mapper, so if this component is bound
	*       to a param-block, it may change automatically while scrubbing
	*       through the time-line, etc.
	*       The brackets are shown for all spinners of the control.
	* \return True, if the animation key brackets should be shown.
	* \see setAnimationKeyBrackets() */
	bool hasAnimationKeyBrackets() const;

	/** \brief Determines whether this component is in interactive mode.
	* A Component is in interactive mode if a user is changing its value in a
	* continuous manner, that may be cancelled by the user at the end of the
	* interaction.
	* For the spinner this is while dragging the buttons to change the value
	* or by holding down the left mouse-button over a button to increment
	* or decrement the value over time.
	*
	* The basic idea of having an interactive mode is to enable the undo/redo
	* system to track those interactive continuous changes as ONE single change
	* (or even just ignores the change on cancel).
	*
	* Therefore the interactiveChanged signal that gets emitted as soon as the
	* interactive mode changes has an additional param that is only meant to
	* be meaningful on ending an interactive session: the userCancelled param.
	* \see interactiveChanged() */
	bool isInteractive() const;

	/** \brief Returns the step increment/decrement value of the spinners.
	* \see setSingleStep() */
	double singleStep() const;

	/** \brief Sets the step increment/decrement value of the spinners.
	* \see singleStep() */
	void setSingleStep( double val );

	/** \brief Returns the minimum value of the spinners.
	* \see setMinimum(), maximum(), setMaximum() */
	double minimum() const;

	/** \brief Sets the minimum value of the spinners.
	* \see minimum(), maximum(), setMaximum() */
	void setMinimum( double min );

	/** \brief Returns the maximum value of the spinners.
	* \see setMaximum(), minimum(), setMinimum() */
	double maximum() const;

	/** \brief Sets the maximum value of the spinners.
	* \see maximum(), minimum(), setMinimum() */
	void setMaximum( double max );

	/** \brief Returns the prefix of the spinners.
	* \see setPrefix(), suffix(), setSuffix() */
	QString prefix() const;

	/** \brief Sets the prefix of the spinners.
	* \see prefix(), suffix(), setSuffix() */
	void setPrefix( const QString& prefix );

	/** \brief Returns the suffix of the spinners.
	* \see setSuffix(), prefix(), setPrefix() */
	QString suffix() const;

	/** \brief Sets the suffix of the spinners.
	* \see suffix(), prefix(), setPrefix() */
	void setSuffix( const QString& suffix );

	/** \brief Returns the number of decimals of the spinners.
	* \see setDecimals() */
	int decimals() const;

	/** \brief Sets the step number of decimals of the spinners.
	* \see decimals() */
	void setDecimals( int decimals );

	/** \brief Returns the number of layout columns in which the multiple
	* spinners are arranged.
	* \see setNumColumns() */
	int numColumns() const;

	/** \brief Sets the number of layout columns in which the multiple
	* spinners should be arranged.
	* \param numCols The number of layout columns.
	* \see numColumns() */
	void setNumColumns( int numCols );
	
	// Delegate max data type conversion to the actual subclass.
	/** \brief Creates a double array out of a variant with the subclass
	* specific data structure.
	*
	* The implementation of this method should use the given variant, containing its
	* subclass specific data structure, to create a double array which can be used
	* to fill the spinner values of the control.
	* For instance a QVariant with a Point2 value would construct and return
	* a two value double array.
	* The array order should match the order of the spinners in the UI.
	* \param value A variant with the subclass specific data structure.
	* \return A double array containing the values for the spinners of the control.
	* \see toMaxTypeVariant() */
	virtual QVector<double> fromMaxTypeVariant( const QVariant& value ) const = 0;

	/** \brief Creates a variant with the subclass specific data structure out 
	* of the multiple spinners values.
	*
	* The implementation of this method should use the given double array values, 
	* which represent the spinner values as shown in UI, 
	* to construct a QVariant containing the subclass specific data structure.
	* For instance a two value double array would construct and return a QVariant 
	* with a Point2 value.
	* The spinner values in the array match the order as shown in the UI.
	* \param spinnerValues The multiple spinner values for the creation of the variant.
	* \return A variant with the subclass specific data structure.
	* \see fromMaxTypeVariant() */
	virtual QVariant toMaxTypeVariant( const QVector<double>& spinnerValues ) const = 0;


Q_SIGNALS:

	/** \brief Emitted if the actual variant value has changed.
	* \param value The new value associated with this spinner control.
	* \see value(), setValue(). */
	void valueChanged( const QVariant& value );

	/** \brief Emitted as soon as the components interactive mode changes.
	* \param interactive	The new interactive mode of the component.
	* \param userCancelled	True if the user actively cancelled an interactive
	*		session, for example by pressing the right mouse-button during a
	*		interactive dragging-operation. As a matter of fact, this param is
	*		only meaningful if the interactive param was false.
	* \see interactiveChanged(), interactiveChanged */
	void interactiveChanged( bool interactive, bool userCancelled = false );


public Q_SLOTS:

	/** \brief Sets the value associated with this spinner control.
	* \param value The new value associated with this spinner control. If
	*    the new value is different from the old one, a valueChanged() is emitted.
	* \see value(), valueChanged(). */
	void setValue( const QVariant& value );

	/** \brief Changes the reset value of this component.
	* \param resetValue The new resetValue
	* \see reset(), resetValue() */
	void setResetValue( const QVariant& resetValue );

	/** \brief Sets whether the animation key brackets around the spinners
	* should be shown.
	* \note The whole animationKeyBrackets property normally gets driven
	*		internally by a param-block mapper, so if this component is bound
	*		to a param-block, it may change automatically while scrubbing
	*		through the time-line, etc.
	* \see hasAnimationKeyBrackets() */
	void setAnimationKeyBrackets( bool animationKeyBrackets = true );

	/** \brief Resets this components value to its resetValue.
	* \see setResetValue(), resetValue() */
	void reset();

private:

	Q_DECLARE_PRIVATE( QmaxMultiSpinner );
	Q_DISABLE_COPY( QmaxMultiSpinner );

	QmaxMultiSpinnerPrivate* d_ptr;
};

//-----------------------------------------------------------------------------
// class QmaxPoint2Spinner
//-----------------------------------------------------------------------------
/* \brief This class represents a multi-spinner UI control for the 3ds Max 
* data type Point2.
*/
class CoreExport QmaxPoint2Spinner : public QmaxMultiSpinner
{
public:

	QmaxPoint2Spinner( QWidget* parent = nullptr )
		: QmaxMultiSpinner( 2, 2, parent ) {}

	virtual QVector<double> fromMaxTypeVariant( const QVariant& value ) const override
	{
		QVector<double> spinnerValues;
		spinnerValues.reserve( 2 );
		if ( value.canConvert<Point2>() )
		{
			Point2 p = qvariant_cast<Point2>(value);
			spinnerValues << p.x << p.y;
		}
		return spinnerValues;
	}

	virtual QVariant toMaxTypeVariant( const QVector<double>& spinnerValues ) const override
	{
		if ( spinnerValues.count() == 2 )
			return QVariant::fromValue( Point2( spinnerValues[0], spinnerValues[1] ) );
		else
			return QVariant();
	}

};

//-----------------------------------------------------------------------------
// class QmaxPoint3Spinner
//-----------------------------------------------------------------------------
/* \brief This class represents a multi-spinner UI control for the 3ds Max
* data type Point3.
*/
class CoreExport QmaxPoint3Spinner : public QmaxMultiSpinner
{
public:

	QmaxPoint3Spinner( QWidget* parent = nullptr )
		: QmaxMultiSpinner( 3, 3, parent ) {}

	virtual QVector<double> fromMaxTypeVariant( const QVariant& value ) const override
	{
		QVector<double> spinnerValues;
		spinnerValues.reserve( 3 );
		if ( value.canConvert<Point3>() )
		{
			Point3 p = qvariant_cast<Point3>(value);
			spinnerValues << p.x << p.y << p.z;
		}
		return spinnerValues;
	}

	virtual QVariant toMaxTypeVariant( const QVector<double>& spinnerValues ) const override
	{
		if ( spinnerValues.count() == 3 )
			return QVariant::fromValue( Point3( spinnerValues[0], spinnerValues[1], spinnerValues[2] ) );
		else
			return QVariant();
	}

};

//-----------------------------------------------------------------------------
// class QmaxPoint4Spinner
//-----------------------------------------------------------------------------
/* \brief This class represents a multi-spinner UI control for the 3ds Max
* data type Point4.
*/
class CoreExport QmaxPoint4Spinner : public QmaxMultiSpinner
{
public:

	QmaxPoint4Spinner( QWidget* parent = nullptr )
		: QmaxMultiSpinner( 4, 1, parent ) {}

	virtual QVector<double> fromMaxTypeVariant( const QVariant& value ) const override
	{
		QVector<double> spinnerValues;
		spinnerValues.reserve( 4 );
		if ( value.canConvert<Point4>() )
		{
			Point4 p = qvariant_cast<Point4>(value);
			spinnerValues << p.x << p.y << p.z << p.w;
		}
		return spinnerValues;
	}

	virtual QVariant toMaxTypeVariant( const QVector<double>& spinnerValues ) const override
	{
		if ( spinnerValues.count() == 4 )
			return QVariant::fromValue( Point4( spinnerValues[0], spinnerValues[1], spinnerValues[2], spinnerValues[3] ) );
		else
			return QVariant();
	}

};

//-----------------------------------------------------------------------------
// class QmaxMatrix3Spinner
//-----------------------------------------------------------------------------
/* \brief This class represents a multi-spinner UI control for the 3ds Max
* data type Martix3.
*/
class CoreExport QmaxMatrix3Spinner : public QmaxMultiSpinner
{
public:

	QmaxMatrix3Spinner( QWidget* parent = nullptr )
		: QmaxMultiSpinner( 12, 3, parent ) {}

	virtual QVector<double> fromMaxTypeVariant( const QVariant& value ) const override
	{
		QVector<double> spinnerValues;
		spinnerValues.reserve( 12 );
		if ( value.canConvert<Matrix3>() )
		{
			Matrix3 m = qvariant_cast<Matrix3>(value);
			float *v = (float *)m.GetAddr();
			spinnerValues << v[0] << v[1] << v[2] << v[3] << v[4] << v[5] << v[6] << v[7] << v[8] << v[9] << v[10] << v[11];
		}
		return spinnerValues;
	}

	virtual QVariant toMaxTypeVariant( const QVector<double>& spinnerValues ) const override
	{
		if ( spinnerValues.count() == 12 )
			return QVariant::fromValue( Matrix3( 
				Point3( spinnerValues[0], spinnerValues[1], spinnerValues[2] ), 
				Point3( spinnerValues[3], spinnerValues[4], spinnerValues[5] ),
				Point3( spinnerValues[6], spinnerValues[7], spinnerValues[8] ),
				Point3( spinnerValues[9], spinnerValues[10], spinnerValues[11] ) ) );
		else
			return QVariant();
	}

};


}; // end of MaxSDK namespace -------------------------------------------------
