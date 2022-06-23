//-----------------------------------------------------------------------------
// Copyright 2016 Autodesk, Inc.  All rights reserved.
//
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.
//-----------------------------------------------------------------------------

#pragma once

#include "CoreExport.h"
#include "BuildWarnings.h"
#include "strbasic.h"
#include "QMaxWinHost.h"

#pragma warning(push)
#pragma warning(disable: 4127 4251 4275 4512 4800)
#include <QtCore/QPointer>
#include <QtGui/QtEvents>
#pragma warning(pop)


// forward declarations
class QToolBar;
class QDockWidget;


namespace MaxSDK
{
	// forward declarations
	class QmaxDockingWinHostDelegate;
	class QmaxDockingWinHostPrivate;

	//-----------------------------------------------------------------------------
	// class QmaxDockingWinHost
	//-----------------------------------------------------------------------------
	/** \brief This class acts as docking host for native win32 ui controls.
	 * 
	 * The helper class can be used, to embed a native ui control in a QToolBar / QDockWidget.
	 * As complement you can implement ui control specific docking logic in a 
	 * QmaxDockingWinHostDelegate which will be called by the dock host.
	 * \see QmaxDockingWinHostDelegate
	 */
	class CoreExport QmaxDockingWinHost : public QMaxWinHost
	{
		Q_OBJECT

	public:
		QmaxDockingWinHost( QWidget* parent = nullptr, Qt::WindowFlags f = 0 );
		~QmaxDockingWinHost() override;

		/** \brief Sets the docking delegate that will receive the dock host events. 
		 * \note The dock host will <b>not take ownership</b> of the delegate.
		 */
		void setDockingDelegate( QmaxDockingWinHostDelegate* dockingHandler );

		/** \brief Returns the docking delegate. */
		QmaxDockingWinHostDelegate* dockingDelegate() const;


		/** \brief Returns the parent dock widget where this widget is embedded.
		 * If the dock widget is null the winHost might be embedded in a toolbar.
		 * \see toolbar()
		 */
		QDockWidget* dockWidget() const;

		/** \brief Returns the parent toolbar where this widget is embedded.
		 * If the toolbar is null the winHost might be embedded in a dock widget.
		 * \see dockwidget()
		 */
		QToolBar* toolbar() const;


		virtual QSize minimumSizeHint() const override;
		virtual QSize sizeHint() const override;

	protected:

		virtual bool event ( QEvent* evt ) override;
		virtual bool nativeEvent( const QByteArray& eventType, void* msg, long* result ) override;
		virtual void resizeEvent( QResizeEvent* evt ) override;

	private:

		Q_DECLARE_PRIVATE( QmaxDockingWinHost );
		QmaxDockingWinHostPrivate* d_ptr;
	};
	
};
