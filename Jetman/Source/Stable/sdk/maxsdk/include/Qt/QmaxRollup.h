// Copyright (c) 2016 Autodesk, Inc.
// All rights reserved.
// 
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.

#pragma once

 
#include "CoreExport.h"
#include "RollupConstants.h"

#pragma warning(push)
#pragma warning(disable: 4127 4251 4275 4512 4800 )
#include <QtWidgets/QWidget>
#pragma warning(pop)

namespace MaxSDK
{
	class QmaxRollupContainer;
	class QmaxRollupPrivate;
	
	/** \brief 3ds Max Qt Rollup class. 
	 * This class provides access to the Qt implementation of the 3ds Max Rollup widget.  
	 * It exposes properties such as title and category, so they can be easily set
	 * in code or in the Qt Designer. 
	 * 
	 * Rollups are the base containers for other widgets for most plug-ins.  They are displayed
	 * on the 3ds Max command panel.   	 *
	 */
	class CoreExport QmaxRollup : public QWidget
	{
		Q_OBJECT
		/** \brief The rollup title.  
		 * \see title(), setTitle() */
		Q_PROPERTY( QString title READ title WRITE setTitle )
		/** \brief The rollup open status. 
		 * \see setOpen(), open(), close() */
		Q_PROPERTY( bool open READ isOpen WRITE setOpen NOTIFY toggled )
		/** \brief The rollup category.  
		 * \see category(), setCategory(), resetCategory() */
		Q_PROPERTY( int category READ category WRITE setCategory )
		Q_FLAGS( Options )

	public:

        // Note: These enum values match the legacy rollup flags (APPENDROLL_CLOSED, etc.)
		/** \brief Create options for QmaxRollup. */
		enum Option
		{
			NoOptions =		0x0, ///< Create rollup in a closed state.
			DontAutoClose = 0x2, ///< Don't close this rollup when doing a "close all".
			SaveCat =		0x4, ///< Save the category provided in the RollupOrder.cfg
			BorderLess =	0x10, ///< Don't display a title or border, don't support drag-and-drop, right-click menu, or collapse/expand.
			NoPadding =		0x20, ///< Create the rollup with minimal padding.  This is primarily for MaxScript, which performs its own padding.
			FixedCat =		0x40 ///< Fixed rollups have no drag pattern in the header.
		};

		/** \brief Creates a QmaxRollup with no title, with an optionally specified category and parent.  
		 * \param category The category.  For third-party plug-ins, this is usually ROLLUP_CAT_STANDARD.
		 * \param parent The widget parent.  You typically do not need to supply a parent, unless you are placing
		 * the rollup somewhere other than the command panel (such as in a dialog).
		 */
		explicit QmaxRollup( int category = ROLLUP_CAT_STANDARD, QWidget* parent = nullptr ); 
		/** \brief Creates a QmaxRollup with the specified title, and optional category and parent.  
		 * \param title The title for the rollup.
		 * \param category The category.  For third-party plug-ins, this is usually ROLLUP_CAT_STANDARD.
		 * \param parent The widget parent.  You typically do not need to supply a parent, unless you are placing
		 * the rollup somewhere other than the command panel (such as in a dialog).
		 */
		explicit QmaxRollup( const QString& title, int category = ROLLUP_CAT_STANDARD, QWidget* parent = nullptr ); 
		/** \brief Destructor */
		virtual ~QmaxRollup();

		Q_DECLARE_FLAGS( Options, Option );

		/** \brief Sets the options for the rollup.
		 * \see Option, options()
		 */
		void setOptions( Options options ); 
		/** \brief Returns the options for the rollup. */
		Options options() const; 


		/** \brief Returns the rollup's widget, or a nullptr if there is none. 
		 * \see setWidget(), takeWidget()
		 */
		QWidget *widget() const;

		/** \brief Puts a widget (such as a panel containing other widgets) into the rollup.
		 * The widget becomes a child of the rollup, and will be destroyed when
		 * the rollup is deleted or when a new widget is set.  If the rollup already contains
		 * a widget, it is destroyed.
		 * \see widget(), takeWidget()
		 */
		virtual void setWidget( QWidget *widget );

		/** \brief Removes the rollup's widget, and passes ownership of the widget to 
		 * the caller.  The widget is not destroyed.
		 * \return The containing widget
		 * \see widget(), setWidget */
		virtual QWidget *takeWidget();
		
		/** \brief Returns whether the rollup is open or closed ("rolled up"). */
		bool isOpen() const; 
		
		/** \brief Returns the rollup's title. */
		QString title() const; 
		/** \brief Sets the rollup's title. 
		 * \param title The title string. */
		void setTitle( const QString& title ); 

		int category() const; 
		/** \brief Sets the rollup's category. */
		void setCategory( int category ); 
		/** \brief Resets the rollup's category to its original category. */
		void resetCategory();

		/** \brief Returns the container for the rollup. */
		QmaxRollupContainer* container() const; 

	public slots: 

		/** \brief Sets the open/closed state of the rollup.
		 * \param isOpen If true, sets the rollup state to open. */
		virtual void setOpen( bool isOpen = true ); 
		/** \brief Sets the state of the rollup to open. */
		inline void open(){ setOpen( true ); }
		/** \brief Sets the state of the rollup to closed. */
		inline void close(){ setOpen( false ); }
		
	signals:

		/** \brief Emitted when the open/close state of the rollup is changed. 
		 * \param isOpen True when the rollup state is now open. */
		void toggled( bool isOpen = false );
		/** \brief Emitted when the rollup is opened. */
		void opened();
		/** \brief Emitted when the rollup is closed */
		void closed(); 
			
	protected:

		explicit QmaxRollup( QmaxRollupPrivate* _d, const QString& title, QWidget* parent = nullptr ); 

		virtual void paintEvent( QPaintEvent* event ) override; 

		virtual void mousePressEvent( QMouseEvent* event ) override;
		virtual void mouseReleaseEvent( QMouseEvent* event ) override;
		virtual void mouseMoveEvent( QMouseEvent* event ) override;

		virtual bool event( QEvent* event ) override;

		Q_DECLARE_PRIVATE( QmaxRollup );

	private:

		QmaxRollupPrivate* d_ptr; 
		Q_DISABLE_COPY( QmaxRollup );

		friend class QmaxRollupContainer; 
	};

}
