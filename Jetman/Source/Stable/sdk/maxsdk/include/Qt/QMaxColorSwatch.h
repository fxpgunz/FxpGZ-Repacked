//-----------------------------------------------------------------------------
// Copyright 2015 Autodesk, Inc.  All rights reserved.
//
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.
//-----------------------------------------------------------------------------

#pragma once

#include "../CoreExport.h"
#include "../acolor.h"
#include "QtMax.h"

#pragma warning(push)
#pragma warning(disable: 4127 4251 4275 4512 4800 )
#include <QtWidgets/QWidget>
#include <QtCore/QObject>
#include <QtCore/QMetaProperty>
#pragma warning(pop)

class CSCallback;

namespace MaxSDK
{;

class QMaxColorSwatchPrivate; 

//-----------------------------------------------------------------------------
// QMaxColorSwatch
//-----------------------------------------------------------------------------
/** \brief Simple %UI component to display and modify a color-value. 
 * 
 * The color aka 'value' is stored as a AColor, what means that the RGB color
 * and alpha channels are stored in floats in a range between 0.0f and 1.0f.
 * 
 * If the alpha channel should not be used, please set the useAlpha property
 * to false, in that case the ColorPicker shown on left mouse click will
 * display in non-alpha mode.
 * 
 */
class CoreExport QMaxColorSwatch: public QWidget
{
	Q_OBJECT

	/** \brief The color value associated with this color swatch.
	 * \see value(), setValue(), valueChanged() */
	Q_PROPERTY( AColor value READ value WRITE setValue NOTIFY valueChanged );

	/** \brief Determines whether the color swatch should expose the alpha component.
	 * \see useAlpha(), setUseAlpha(), value() */
	Q_PROPERTY( bool useAlpha READ useAlpha WRITE setUseAlpha );
	
	/** \brief The title of the color picker when shown. 
	 * \see title(), setTitle() */
	Q_PROPERTY( QString title READ title WRITE setTitle );

	/** \brief Determines whether the modal version of the color picker dialog
	 * should be used.
	 * \see useModalPicker(), setUseModalPicker() */
	Q_PROPERTY( bool useModalPicker READ useModalPicker WRITE setUseModalPicker );

	/** \brief Determines whether the color swatch should ignore the
	* application-wide color correction and display the raw RGB(A) value.
	* \see ignoreColorCorrection(), setIgnoreColorCorrection() */
	Q_PROPERTY( bool ignoreColorCorrection READ ignoreColorCorrection WRITE setIgnoreColorCorrection );

	/** \brief Show the animation key brackets around the component.
	 * \see hasAnimationKeyBrackets(), setAnimationKeyBrackets() */
	Q_PROPERTY( bool animationKeyBrackets READ hasAnimationKeyBrackets WRITE setAnimationKeyBrackets );

	/** \brief Determines whether the color swatch is in interactive mode.
	 * \see isInteractive(), interactiveChanged() */
	Q_PROPERTY( bool interactive READ isInteractive NOTIFY interactiveChanged );


public:

	/** \brief Default constructor.
	 * Constructs a new QMaxColorSwatch with the following settings: 
	 * - no alpha used
	 * - non-modal color-picker
	 * - empty title
	 * - the color value is just black (0,0,0)
	 */
	explicit QMaxColorSwatch( QWidget* parent = nullptr );

	virtual ~QMaxColorSwatch();

	/** \brief The color value associated with this color swatch.
	 * \see setValue(), valueChanged(), value */
	AColor value() const; 

	/** \brief Determines whether the color swatch should expose the alpha component
	 * \see setUseAlpha(), useAlpha */
	bool useAlpha() const;
	
	/** \brief The title used in the color-picker, when opened.
	 * \see setTitle(), title */
	QString title() const;
	
	/** \brief Determines whether the color swatch is in interactive mode.
	 * A color watch is in interactive mode, while the color picker dialog is
	 * opened and associated to this color swatch - so the user may perform
	 * several sequential 'interactive' color changes, that are meant to be
	 * captured as one common value change in terms of the undo/redo history.
	 *
	 * Since the user has the option to cancel the interactive changes - by
	 * pressing the "Cancel" button on the color picker dialog, the signal
	 * emitted by that change also includes that information.
	 *
	 * \see interactive, interactiveChanged() */
	bool isInteractive() const; 

	/** \brief If the animation key brackets should be shown. 
	 * \see animationKeyBrackets, setAnimationKeyBrackets() */
	bool hasAnimationKeyBrackets() const; 

	/** \brief Determines whether the modal version of the color picker dialog
	* should be used.
	* When using the non-modal version of the color picker dialog, what's the
	* default behavior, a color picker dialog can be used to manipulate the
	* color of different swatches by just clicking on them. The active swatch
	* get painted with white outline, so the user can distinguish what color
	* swatch is to be modified.
	* Switching color swatches in that way will emit an interactiveChanged
	* signal to the previous active color swatch, as if the user had clicked
	* on the Ok button to close the dialog.
	 * \see useModalPicker, setUseModalPicker() */
	bool useModalPicker() const; 
	
	/** \brief Sets, if a modal color-picker should be used.
	 * \see useModalPicker, useModalPicker() */
	void setUseModalPicker( bool use_modal_picker );

	/** \brief Determines whether the color swatch should ignore the 
	* application-wide color correction and display the raw RGB(A) value.
	* \see setIgnoreColorCorrection(), ignoreColorCorrection */
	bool ignoreColorCorrection() const; 

	virtual QSize minimumSizeHint() const override; 

signals:
	
	/** \brief Will be emitted, if the actual color value has changed. 
	 * \param value The new color value associated with this color swatch. 
	 *    Please note, that Qt internally takes care if this signal gets handled 
	 *    asynchronously and creates a copy of this parameter, so it's always 
	 *    safe to access its data.  
	 * \note If alpha is not used, setting a color with a different alpha value
	 *    will NOT emit a valueChanged, of course.
	 * \see value, value(), setValue(). */
	void valueChanged( const AColor& value ); 

	/** \brief Will be emitted, if swatch changed its interactive state.
	 * \param interactive The new interactive state of this color swatch. 
	 * \param canceled If the interactive param is false, meaning the color 
	 *    picker dialog has been closed, the user has canceled the previous 
	 *    interactive changes. In that case, no redo/undo record should be filed.
	 * \see interactive, isInteractive(). */
	void interactiveChanged( bool interactive, bool canceled = false ); 
	
public slots:

	/** \brief Sets the color value associated with this color swatch.
	 * \param value The new color to be  associated with this color swatch. If 
	 *    the new color differs from the recent one, a valueChange() is emitted. 
	 * \note If alpha is not used, setting a color with a different alpha value
	 *    will NOT emit a valueChanged, of course.
	 * \see value, value(), valueChanged(). */
	void setValue( const AColor& value );

	/** \brief Sets whether the color swatch should expose the alpha component.
	 * \see useAlpha(), useAlpha */
	void setUseAlpha( bool useAlpha );

	/** \brief Sets the title used in the color-picker, when opened.
	 * \see title(), title */
	void setTitle( const QString& title );
	
	/** \brief Sets whether the animation key brackets should be shown. 
	 * \see hasAnimationKeyBrackets(), animationBrackets */
	void setAnimationKeyBrackets( bool show_brackets );

	/** \brief Sets whether the color swatch should ignore the application-wide
	*  color correction and display the raw RGB(A) value.
	* \see ignoreColorCorrection(), ignoreColorCorrection */
	void setIgnoreColorCorrection( bool ignore );

protected:

	explicit QMaxColorSwatch( QMaxColorSwatchPrivate* _d, QWidget* parent = nullptr );
	virtual void paintEvent( QPaintEvent *event ) override;

	virtual void mousePressEvent( QMouseEvent* event ) override; 
	virtual void mouseMoveEvent( QMouseEvent* event ) override; 
	virtual void mouseReleaseEvent( QMouseEvent* event ) override; 

	virtual void dragEnterEvent( QDragEnterEvent* event ) override; 
	virtual void dragMoveEvent( QDragMoveEvent* event ) override; 
	virtual void dropEvent( QDropEvent* event ) override; 
	
	void startInteractive();
	void stopInteractive(bool canceled);

	Q_DECLARE_PRIVATE( QMaxColorSwatch );

private:

	QMaxColorSwatchPrivate* d_ptr;
	Q_DISABLE_COPY( QMaxColorSwatch );

	friend class ColorSwatch; // temporary legacy support
	friend class CSCallback; //for startInteractive()/stopInteractive()
};


}; // end of MaxSDK namespace -------------------------------------------------
