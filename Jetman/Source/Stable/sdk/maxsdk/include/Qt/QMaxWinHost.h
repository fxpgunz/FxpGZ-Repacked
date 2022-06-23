//-----------------------------------------------------------------------------
// Copyright 2016 Autodesk, Inc.  All rights reserved.
//
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.
//-----------------------------------------------------------------------------


#pragma once

#include "CoreExport.h"

#pragma warning(push)
#pragma warning(disable: 4127 4251 4275 4512 4800 )
#include <QtWidgets/QWidget>
#pragma warning(pop)

namespace MaxSDK
{;

//-----------------------------------------------------------------------------
// QMaxWinHost
//-----------------------------------------------------------------------------
/** \brief Simple widget that can host a native win32 window.
*
* This class is used to embed non-qt windows into the Qt window framework.
* It will take care of basic things like moving/resizing the hosted window
* when the winHost container changes its geometry.
*/
class CoreExport QMaxWinHost : public QWidget
{
	Q_OBJECT
	
public:
	explicit QMaxWinHost( HWND hwnd, QWidget* parent = nullptr );
	explicit QMaxWinHost( QWidget* parent = nullptr, Qt::WindowFlags f = 0 );
	~QMaxWinHost() override;

	/** \brief Sets the native window that is hosted by this widget.
	* The native window will be re-parented as child of this widget.
	* If the window has been changed, a hostedWindowChanged signal will be 
	* emitted containing the new HWND as well as the old one. 
	* \see hostedWindow(), hostedWindowChanged()
	*/
	void setHostedWindow( HWND hwnd );

	/** \brief Returns the native window that is hosted by this widget.
	* \see setHostedWindow(), hostedWindowChanged()
	*/
	HWND hostedWindow() const;

Q_SIGNALS:
		
	/** \brief Gets emitted, if the native window that is hosted by this 
	* widget has been changed. 
	* \param newHWND the new HWND inside this object
	* \param oldHWND the previous HWND used by this object before
	* \see setHostedWindow(), hostedWindow()
	*/
	void hostedWindowChanged( HWND newHWND, HWND oldHWND );

protected:

	void resizeEvent( QResizeEvent* event ) override;
	void moveEvent( QMoveEvent* event ) override;
	void focusInEvent( QFocusEvent* event ) override;
	void showEvent( QShowEvent* event ) override;
	void hideEvent( QHideEvent* event ) override;
	bool event( QEvent* event ) override;
	bool nativeEvent( const QByteArray &eventType, void* msg, long* result ) override;

private:

    class QMaxWinHostPrivate* const d_ptr;
    Q_DECLARE_PRIVATE( QMaxWinHost ); 
};


};
