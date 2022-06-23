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

#include <utilexp.h>

#pragma warning( push )
#pragma warning( disable: 4127 4512 )
#include <QtCore/QtCore>
#pragma warning( pop )

namespace MaxSDK
{;

//-----------------------------------------------------------------------------
/* \brief Convenience helper class to allow a dll to automatically load and
 * unload Qt's translations files from internal resources.
 * 
 * This class is just a thin helper on top of Qt's translation system, it's 
 * used with the internal localization progress of 3dsmax itself but can also 
 * be handy for third party developers.
 * 
 * To use this class, make sure to add a .qrc resource file into your project  
 * keeping all the compiled translations (use lrelease for that) within this
 * kind of structure: 
 *		:/__qmax_translations/[moduleName]_de-DE.qm
 *		:/__qmax_translations/[moduleName]_fr-FR.qm
 *		...
 * The native language version inside your code / ui files can be omitted.
 * To enable your translation automatically, just add a static instance of 
 * QmaxTranslationLoader somewhere in your dll and pass the correct moduleName 
 * in the constructor.
 */
class UtilExport QmaxTranslationLoader : public QObject
{
public:
	//-------------------------------------------------------------------------
	/* \brief Creates a QmaxTranslationLoader using the given module name. 
	 * Depending on the timing of this constructor call - this may also 
	 * internally call loadTranslation(...) using the current 3dsmax language 
	 * settings.
	 * 
	 * \param[in] moduleName	The module name to be loaded. 
	 * \param[in] parent		The QObject parent to own this object. 
	 */
	QmaxTranslationLoader( const QString& moduleName, QObject* parent = nullptr );

	//-------------------------------------------------------------------------
	/* \brief Destroys the QmaxTranslationLoader.
	 * Depending on the timing of this destructor call - this may also
	 * internally call unloadTranslation(), if loaded. 
	 */
	virtual ~QmaxTranslationLoader();

	//-------------------------------------------------------------------------
	/* \brief Loads the translation based on the moduleName of this object and
	 * the given localeName. It will unload prior loaded translation of this
	 * module.
	 * In most cases, this is done internally by the constructor and the 
	 * application itself, but it's also fine to call that as runtime. 
	 * 
	 * \param[in] localeName	The locale name defining the translation to be 
	 *							loaded. If that locale is loaded already, the 
	 *							function will return true immediately.
	 * \return	true, if the translations for the given locale could be loaded 
	 *			or is already loaded - false, otherwise
	 */
	virtual bool loadTranslation( const QString& localeName );
	
	//-------------------------------------------------------------------------
	/* \brief Unloads the loaded translation based on the moduleName of this 
	 * object. Will be called on Application shutdown.
	 */
	virtual void unloadTranslation();

private: 

	class QmaxTranslationLoaderPrivate* d_ptr; 

	Q_DISABLE_COPY( QmaxTranslationLoader );
	Q_DECLARE_PRIVATE( QmaxTranslationLoader ); 
};

}; // end of namespace MAXSDK -------------------------------------------------
