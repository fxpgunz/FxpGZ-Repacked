//-----------------------------------------------------------------------------
// Copyright 2015 Autodesk, Inc.  All rights reserved.
//
// This computer source code and related instructions and comments are the
// unpublished confidential and proprietary information of Autodesk, Inc. and
// are protected under applicable copyright and trade secret law.  They may not
// be disclosed to, copied or used by any third party without the prior written
// consent of Autodesk, Inc.
//-----------------------------------------------------------------------------

#pragma once

#include "CoreExport.h"
#include "BuildWarnings.h"
#include "strbasic.h"

#pragma warning(push)
#pragma warning(disable: 4127 4251 4275 4512 4800)
#include <QtWidgets/QWidget>
#include <QtCore/QPointer>
#include <QtGui/QtEvents>
#pragma warning(pop)


#pragma warning(push)
#pragma warning(disable: 4251 4275)

// forward declarations
class QToolBar;
class QDockWidget;

namespace MaxSDK
{
	// forward declarations
	class QmaxDockingWinHost;
	class QmaxDockingWinHostDelegatePrivate;
	class QmaxProxyDockingWinHostDelegatePrivate;

	//-----------------------------------------------------------------------------
	// class QmaxDockingWinHostDelegate
	//-----------------------------------------------------------------------------
	/* \brief This class acts as delegate for the QmaxDockingWinHost.
	 * 
	 * Use this class to implement specific docking logic for the native ui control
	 * that is embedded in a QmaxDockingWinHost.
	 * \see QmaxDockingWinHost
	 */
	class CoreExport QmaxDockingWinHostDelegate : public QObject
	{
	public:

		/* \brief Constructs a docking delegate. 
		 */
		QmaxDockingWinHostDelegate( QObject* parent = nullptr );
		virtual ~QmaxDockingWinHostDelegate();

		/* \brief Returns the widget that acts as host for this docking delegate. 
		The widget can be embedded in a QToolBar or QDockWidget. */
		QmaxDockingWinHost* dockHost() const;

		/* \brief Returns the dockwidget where the dock host is embedded.
		 * If the dockwidget is null the dockHost might be embedded in a toolbar.
		 * \see toolbar()
		 */
		QDockWidget* dockWidget() const;

		/* \brief Returns the parent toolbar where the dock host is embedded.
		 * If the toolbar is null the winHost might be embedded in a dockwidget.
		 * \see dockwidget()
		 */
		QToolBar* toolbar() const;

		// Size constraints methods.

		/* \brief This function is called by the dock host to determine the minimum size
		 * to what the dock host and its content can shrink. */
		virtual QSize minimumSize( Qt::Orientation orientation ) const { UNUSED_PARAM(orientation); return QSize(); }
		
		/* \brief This function is called by the dock host to determine the preferred size
		 * that the dock host and its content should have. */
		virtual QSize preferredSize( Qt::Orientation orientation ) const { UNUSED_PARAM(orientation); return QSize(); }

		/* \brief This function is called by the dock host to determine the maximum size
		 * to what the dock host and its content can grow. */
		virtual QSize maximumSize( Qt::Orientation orientation ) const { UNUSED_PARAM(orientation); return QSize( QWIDGETSIZE_MAX, QWIDGETSIZE_MAX ); }

		/* \brief Call this function when the minimumSize / maximumSize of the hosted ui component has been changed
		and the dock host need to update its geometry. */
		void sizeConstraintsChanged();

		/* \brief Called when the orientation of the QToolBar where the dock host is embedded changes. */
		virtual void orientationChanged( Qt::Orientation orientation ) { UNUSED_PARAM(orientation); }

		/* \brief Called when the floating state of the QToolBar/QDockWidget where the dock host is embedded changes. */
		virtual void topLevelChanged( bool topLevel ) { UNUSED_PARAM(topLevel); }

		/* \brief Called when the dock area of the QDockWidget where the dock host is embedded changes. */
		virtual void dockLocationChanged( Qt::DockWidgetArea area ) { UNUSED_PARAM(area); }

		/* \brief Called when the toolbar area of the QToolBar where the dock host is embedded changes. */
		virtual void toolBarAreaChanged( Qt::ToolBarArea area ) { UNUSED_PARAM(area); }

		 /* \brief Called when the dock host is shown. */
		virtual void showEvent( QShowEvent* evt ) { UNUSED_PARAM(evt); }

		/* \brief Called after the dock host is hidden. */
		virtual void hideEvent( QHideEvent* evt ) { UNUSED_PARAM(evt); }

		/* \brief Called when the dock host changed its size. */
		virtual void resizeEvent( QResizeEvent* evt ) { UNUSED_PARAM(evt); }

		/* \brief Called when the dock host receives a close request. */
		virtual void closeEvent( QCloseEvent* evt ) { UNUSED_PARAM(evt); }

		/* \brief Called when the dock host is in deletion. 
		 * \note You should not call any functions of the dock host's parent QToolBar / QDockWidget,
		 * since the widget hierarchy is already in deletion.
		 */
		virtual void deleteEvent() {;}

		/* \brief Can be reimplemented to receive native Windows events send to the dock host. */
		virtual bool winEvent( MSG* msg, long* result ) { UNUSED_PARAM(msg); UNUSED_PARAM(result); return false; }


	protected:
		
		explicit QmaxDockingWinHostDelegate( QmaxDockingWinHostDelegatePrivate* d,  QObject* parent = nullptr );

		bool event( QEvent* evt ) override;

		QmaxDockingWinHostDelegatePrivate* d_ptr;
		Q_DECLARE_PRIVATE( QmaxDockingWinHostDelegate );
		
	private:

		friend class QmaxDockingWinHost;
		friend class QmaxProxyDockingWinHostDelegatePrivate;
	};


	//-----------------------------------------------------------------------------
	// class QmaxProxyDockingWinHostDelegate
	//-----------------------------------------------------------------------------
	/* \brief This class acts as proxy for other QmaxDockingWinHost delegates.
	 * 
	 * Use this class to override partial functionality of an already existing QmaxDockingWinHostDelegate.
	 */
	class CoreExport QmaxProxyDockingWinHostDelegate : public QmaxDockingWinHostDelegate
	{
	public:

		/* \brief Constructs a proxy delegate which works on the given baseDelegate. 
		 * \param baseDelegate The base delegate that should be proxied. 
		 */
		QmaxProxyDockingWinHostDelegate( QmaxDockingWinHostDelegate* baseDelegate, QObject* parent = nullptr );
		virtual ~QmaxProxyDockingWinHostDelegate() {;}

		/* \brief Returns the base delegate. */
		QmaxDockingWinHostDelegate*	baseDelegate() const;

		/* \brief Sets the base delegate that should be proxied. */
		void setBaseDelegate( QmaxDockingWinHostDelegate* baseDelegate );

		virtual QSize minimumSize( Qt::Orientation orientation ) const override;
		virtual QSize preferredSize( Qt::Orientation orientation ) const override;
		virtual QSize maximumSize( Qt::Orientation orientation ) const override;

		virtual void showEvent ( QShowEvent* evt ) override;
		virtual void hideEvent ( QHideEvent* evt ) override;
		virtual void resizeEvent ( QResizeEvent* evt ) override;
		virtual void deleteEvent() override;
		virtual bool winEvent( MSG* msg, long* result ) override;

		virtual void orientationChanged( Qt::Orientation orientation ) override;
		virtual void topLevelChanged( bool topLevel ) override;
		virtual void dockLocationChanged( Qt::DockWidgetArea area ) override;
		virtual void toolBarAreaChanged( Qt::ToolBarArea area ) override;

	protected:
		
		Q_DECLARE_PRIVATE_D( QmaxDockingWinHostDelegate::d_ptr, QmaxProxyDockingWinHostDelegate );

	};

};


#pragma warning(pop)