
// Copyright (c) 2016 Autodesk, Inc.
// All rights reserved.
// 
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.

#pragma once

/** \brief 3ds Max QSpinBox classes. 
 * These classes can be used as a simple drop-in replacement of Qts QSpinBox or
 * QDoubleSpinBox classes - to provide some additional features to the end-user
 * as dragging the spinner buttons to continuously change the value, cancelling
 * that and also reset by right mouse-button click on the buttons of the actual
 * SpinBox control. 
 * Most of the additional properties exposed by these classes get automatically
 * wired up on a PB2 param, when getting set up by the P_AUTO_UI_QT paramblock
 * definition. 
 *
 *  - min gets wired up to minimum().
 *  - max gets wired up to maximum().
 *  - scale gets wired up to singleStep() - so the scale is actually equals to 
 *    one click on a plus/minus button.
 */

#include "CoreExport.h"
#include <memory>

#pragma warning( push ) 
#pragma warning( disable: 4127 4251 4275 4800 ) 
#include <QtCore/QtGlobal>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
	#include <QtWidgets/QDoubleSpinBox>
	#include <QtWidgets/QSpinBox>
#else
	#include <QtGui/QDoubleSpinBox>
	#include <QtGui/QSpinBox>
#endif
#pragma warning( pop )

#ifndef MAX_COMPONENTS
#include "../ILightingUnits.h"
#endif

class QMenu;

//-----------------------------------------------------------------------------
namespace MaxSDK {
//-----------------------------------------------------------------------------

template<typename T> class QmaxSpinBoxPrivate;

class CoreExport QmaxDoubleSpinBox: public QDoubleSpinBox
{	
	Q_OBJECT

	/** \brief The reset value of this Component.
	 * \see resetValue(), setResetValue( double ) */
	Q_PROPERTY( double resetValue READ resetValue WRITE setResetValue )

	/** \brief Show the animation key brackets around the component.
	 * \see hasAnimationKeyBrackets(), setAnimationKeyBrackets() */
	Q_PROPERTY( bool animationKeyBrackets READ hasAnimationKeyBrackets WRITE setAnimationKeyBrackets )

	// working, but don't change the UI for now - just here for extensibility
	Q_PROPERTY( bool indeterminate READ isIndeterminate WRITE setIndeterminate )

	/** \brief Determines whether this component is in interactive mode.
	 * \see isInteractive(), interactiveChanged() */
	Q_PROPERTY( bool interactive READ isInteractive NOTIFY interactiveChanged )

public:

	explicit QmaxDoubleSpinBox( QWidget* parent = nullptr );
	virtual ~QmaxDoubleSpinBox();

	/** \brief The reset value of this component.
	 * If a user right-clicks the button-area of a QmaxSpinBox, it will be 
	 * reset to this value.
	 * \return The reset - aka default - value of this component.
	 * \see reset(), resetValue(), setResetValue( double ) */
	double resetValue() const;

	/** \brief Determines if the animation key brackets around the component 
	 *         should be shown. 
	 * \note The whole animationKeyBrackets property normally gets driven 
	 *       internally by a param-block mapper, so if this component is bound 
	 *       to a param-block, it may change automatically while scrubbing 
	 *       through the time-line, etc. 
	 * \return True, if the animation key brackets should be shown.
	 * \see setAnimationKeyBrackets() */
	bool hasAnimationKeyBrackets() const;

	// working, but don't change the UI for now - just here for extensibility
	bool isIndeterminate() const;
	
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
	 * \see interactiveChanged(), interactiveChanged */
	bool isInteractive() const;

	virtual QValidator::State validate( QString& input, int& pos ) const override;
    virtual void stepBy(int steps) override;

Q_SIGNALS:

	/** \brief Gets emitted as soon the components interactive mode changes.
	 * \param interactive	The new interactive mode of the component.
	 * \param userCancelled	True if the user actively cancelled an interactive
	 *		session, for example by pressing the right mouse-button during a 
	 *		interactive dragging-operation. As a matter of fact, this param is
	 *		only meaningful if the interactive param was false.
	 * \see interactiveChanged(), interactiveChanged */
	void interactiveChanged( bool interactive, bool userCancelled = false );

	/** \brief Gets emitted when the context menu is shown.
	* \param contextMenu	A menu in which custom actions can be added. */
	void contextMenuCustomization( QMenu& contextMenu );

public Q_SLOTS:

	/** \brief Changes the reset value of this component.
	 * \param[in] resetValue The new resetValue
	 * \see reset(), resetValue() */
	void setResetValue( double resetValue );

	/** \brief Sets whether the animation key brackets around the component 
	 * should be shown.
	 * \note The whole animationKeyBrackets property normally gets driven
	 *		internally by a param-block mapper, so if this component it bound
	 *		to a param-block, it may change automatically while scrubbing
	 *		through the time-line, etc.
	 * \see hasAnimationKeyBrackets() */
	void setAnimationKeyBrackets( bool animationKeyBrackets = true );
	
	void setIndeterminate( bool indeterminate );

	/** \brief Resets this components value to its resetValue.
	 * \see setResetValue(), resetValue() */
	void reset();

protected:

	virtual void mousePressEvent( QMouseEvent* event ) override;
	virtual void mouseMoveEvent( QMouseEvent* event ) override;
	virtual void mouseReleaseEvent( QMouseEvent* event ) override;
	virtual void timerEvent( QTimerEvent* event ) override;
	virtual void paintEvent( QPaintEvent* event ) override;
	virtual void contextMenuEvent( QContextMenuEvent *event ) override; 
	virtual void keyPressEvent( QKeyEvent* event ) override;

	void setInteractive( bool interactive, bool userCanceled = false );

	virtual QString textFromValue( double val ) const override; 
	virtual double valueFromText( const QString& text ) const override;

protected:

	explicit QmaxDoubleSpinBox( QmaxSpinBoxPrivate<double>* d, QWidget* parent = nullptr );

	inline QmaxSpinBoxPrivate<double>* d_func() { return d_ptr; }
	inline const QmaxSpinBoxPrivate<double>* d_func() const { return d_ptr; }
	
	QmaxSpinBoxPrivate<double>* d_ptr;
	
	friend class QmaxSpinBoxPrivate<double>; 
};

//-----------------------------------------------------------------------------
// QmaxUniverseSpinBox - a special QmaxDoubleSpinBox that displays current units
//-----------------------------------------------------------------------------
class CoreExport QmaxWorldSpinBox : public QmaxDoubleSpinBox {
	Q_OBJECT

public:

	explicit QmaxWorldSpinBox( QWidget* parent = nullptr );

protected:
	virtual QString textFromValue( double val ) const override;
	virtual double valueFromText( const QString& text ) const override;
	virtual void fixup( QString &str ) const override;
};



//-----------------------------------------------------------------------------
// class QmaxLightingUnitSpinBox
//-----------------------------------------------------------------------------

class QmaxLightingSpinBoxPrivate;
/** \brief a special QmaxSpinBox for displaying lighting units.
* \see QmaxDoubleSpinBox
*/
class CoreExport QmaxLightingSpinBox : public QmaxDoubleSpinBox {
	Q_OBJECT

	// The type of quantity being represented by this spin box
	Q_PROPERTY( QuantityType quantityType READ quantityType WRITE setQuantityType )

	// The unit system in which the internal value is represented (i.e. what's stored in the 'value' property).
	// The spin box will convert from this system to the display system when displaying the value.
	Q_PROPERTY( UnitSystem internalUnitSystem READ internalUnitSystem WRITE setInternalUnitSystem )

	Q_ENUMS( QuantityType UnitSystem )

public:

	// The type of quantity being represented by this spin box
	enum QuantityType {
		Luminance,
		Illuminance
	};
	#ifndef MAX_COMPONENTS
		typedef ILightingUnits::LightingSystems UnitSystem;
	#else
		enum UnitSystem {
			DISPLAY_INTERNATIONAL,  //! The SI system used cd?m� and lux
			DISPLAY_AMERICAN		//! The AS system uses cd?f� and footcandles
		};
	#endif

	explicit QmaxLightingSpinBox( QWidget* parent = nullptr );

	QuantityType quantityType() const;
	void setQuantityType( const QuantityType val );

	UnitSystem internalUnitSystem() const;
	void setInternalUnitSystem( const UnitSystem val );

	virtual double valueFromText( const QString &text ) const override;
	virtual QString textFromValue( double val ) const override;
	virtual void fixup( QString &str ) const override;

private:

	Q_DECLARE_PRIVATE_D( QmaxDoubleSpinBox::d_func(), QmaxLightingSpinBox );
};


//-----------------------------------------------------------------------------
// QmaxSpinBox - an integer based version of max special behaviour QSpinBox 
//-----------------------------------------------------------------------------
class CoreExport QmaxSpinBox : public QSpinBox {
	Q_OBJECT

		/** \brief The reset value of this Component.
		* \see resetValue(), setResetValue( double ) */
		Q_PROPERTY( int resetValue READ resetValue WRITE setResetValue )

		/** \brief Show the animation key brackets around the component.
		* \see hasAnimationKeyBrackets(), setAnimationKeyBrackets() */
		Q_PROPERTY( bool animationKeyBrackets READ hasAnimationKeyBrackets WRITE setAnimationKeyBrackets )

		// working, but don't change the UI for now - just here for extensibility
		Q_PROPERTY( bool indeterminate READ isIndeterminate WRITE setIndeterminate )

		/** \brief Determines whether this component is in interactive mode.
		* \see isInteractive(), interactiveChanged() */
		Q_PROPERTY( bool interactive READ isInteractive NOTIFY interactiveChanged )

public:

	explicit QmaxSpinBox( QWidget* parent = nullptr );
	virtual ~QmaxSpinBox();

	/** \brief The reset value of this component.
	 * If a user right-clicks the button-area of a QmaxSpinBox, it will be
	 * reset to this value.
	 * \return The reset - aka default - value of this component.
	 * \see reset(), resetValue(), setResetValue( int ) */
	int resetValue() const;

	/** \brief Determines if the animation key brackets around the component
	 *         should be shown.
	 * \note The whole animationKeyBrackets property normally gets driven
	 *       internally by a param-block mapper, so if this component it bound
	 *       to a param-block, it may change automatically while scrubbing
	 *       through the time-line, etc.
	 * \return True, if the animation key brackets should be shown.
	 * \see setAnimationKeyBrackets() */
	bool hasAnimationKeyBrackets() const;

	// working, but don't change the UI for now - just here for extensibility
	bool isIndeterminate() const;

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
	 * \see interactiveChanged(), interactiveChanged */
	bool isInteractive() const;

	virtual QValidator::State validate( QString& input, int& pos ) const override;
    virtual void stepBy(int steps) override;

Q_SIGNALS:

	/** \brief Gets emitted as soon the components interactive mode changes.
	* \param interactive	The new interactive mode of the component.
	* \param userCancelled	True if the user actively cancelled an interactive
	*		session, for example by pressing the right mouse-button during a
	*		interactive dragging-operation. As a matter of fact, this param is
	*		only meaningful if the interactive param was false.
	* \see interactiveChanged(), interactiveChanged */
	void interactiveChanged( bool interactive, bool userCancelled = false );

	/** \brief Gets emitted when the context menu is shown.
	* \param contextMenu	A menu in which custom actions can be added. */
	void contextMenuCustomization(QMenu& contextMenu);

public Q_SLOTS:

	/** \brief Changes the reset value of this component.
	* \param[in] resetValue the new resetValue
	* \see reset(), resetValue() */
	void setResetValue( int resetValue );

	/** \brief Sets whether the animation key brackets around the component
	 * should be shown.
	 * \note The whole animationKeyBrackets property normally gets driven
	 *		internally by a param-block mapper, so if this component it bound
	 *		to a param-block, it may change automatically while scrubbing
	 *		through the time-line, etc.
	 * \see hasAnimationKeyBrackets() */
	void setAnimationKeyBrackets( bool animationKeyBrackets = true );

	void setIndeterminate( bool indeterminate );

	/** \brief Resets this components value to its resetValue.
	* \see setResetValue(), resetValue() */
	void reset();

protected:

	virtual void mousePressEvent( QMouseEvent* event ) override;

	virtual void mouseMoveEvent( QMouseEvent* event ) override;
	virtual void mouseReleaseEvent( QMouseEvent* event ) override;
	virtual void timerEvent( QTimerEvent* event ) override;
	virtual void paintEvent( QPaintEvent* event ) override;
	virtual void contextMenuEvent( QContextMenuEvent *event ) override;
	virtual void keyPressEvent( QKeyEvent* event ) override;

	void setInteractive( bool interactive, bool userCanceled = false );

	virtual int valueFromText( const QString &text ) const override;

protected:

	explicit QmaxSpinBox( QmaxSpinBoxPrivate<int>* d, QWidget* parent = nullptr );

	inline QmaxSpinBoxPrivate<int>* d_func() { return d_ptr; }
	inline const QmaxSpinBoxPrivate<int>* d_func() const { return d_ptr; }

	QmaxSpinBoxPrivate<int>* d_ptr;

	friend class QmaxSpinBoxPrivate<int>;
};

//-----------------------------------------------------------------------------
} // end namespace MaxSDK
//-----------------------------------------------------------------------------
