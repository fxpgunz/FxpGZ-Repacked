/*		Exception.h - exception class for MAXScript
 *
 *		Copyright (c) John Wainwright, 1996
 *		
 *
 */

#pragma once

#include "../ScripterExport.h"
#include "../../strclass.h"
#include "../../WindowsDefines.h"
#include "../foundation/ValueHolderMember.h"

class Value;
class Thunk;
class ValueMetaClass;
class CharStream;
class MAXScriptException;
struct MAXScript_TLS;

extern MCHAR* null_string;

/**
This method centralizes the handling of maxscript exceptions in c++ code. It's typical usage looks like:

ScopedMaxScriptEvaluationContext scopedMaxScriptEvaluationContext;
MAXScript_TLS* _tls = scopedMaxScriptEvaluationContext.Get_TLS();
two_value_locals_tls( thingValue, propNameValue );
try 
{
  ScopedSaveCurrentFrames scopedSaveCurrentFrames(_tls);
  vl.thingValue = InterfaceFunction::FPValue_to_val( thing );
  vl.propNameValue = Name::intern( propName );
  vl.thingValue = vl.thingValue->get_property( &vl.propNameValue, 1 );
  vl.thingValue->to_fpvalue( retVal );
}
catch (MAXScriptException& e)
{
  ProcessMAXScriptException(e, _T("GetScriptedProperty"), false, false, true);
}
catch (...)
{
  ProcessMAXScriptException(UnknownSystemException(), _T("GetScriptedProperty"), false, false, true);
}

* \param e	The maxscript exception to process
* \param caption	The string to be used in the title bar of the error message box, and is used as part of
						the verbose description of the exception. Can be null.
* \param displayErrorMessageBox	If true, an error message box is displayed (unless max is in quiet mode).
* \param showSourcePosition	If true, source position of error is shown in maxscript editor.
* \param isTerminalHandling	If true, terminal handling of the exception is occurring. If false, the catch will be rethrowing the exception.
* \param quietErrors	If true, error information is not written to Listener, a brief description of the exception and its source location is written to the log file.
*/

ScripterExport void ProcessMAXScriptException(MAXScriptException& e, const MCHAR* caption, bool displayErrorMessageBox, bool showSourcePosition, bool isTerminalHandling, bool quietErrors = false);

class ScripterExport MAXScriptException : public MaxHeapOperators
{
	MSTR mCapturedMXSCallstack; // the captured mxs callstack
	MSTR mCapturedCplusplusCallstack; // the captured C++ callstack
	static bool sMXSCallstackCaptureEnabled; // whether MXS Callstack Capture is enabled
	static bool sCplusplusCallstackCaptureEnabled; // whether C++ Callstack Capture is enabled
public:
	MAXScriptException() {}
	virtual ~MAXScriptException() {}
	virtual void sprin1(CharStream* s);
	void CaptureMXSCallstack(); // capture the mxs callstack if MXS Callstack Capture is enabled
	// skipFirstNStackLevels - How many stack levels to not capture. The default value of 4 causes the capture to start at the call to this method  
	void CaptureCplusplusCallstack(int skipFirstNStackLevels = 4); // capture the C++ callstack if C++ Callstack Capture is enabled
	const MSTR& MXSCallstack() { return mCapturedMXSCallstack; }
	const MSTR& CplusplusCallstack() { return mCapturedCplusplusCallstack; }
	static void MXSCallstackCaptureEnabled( bool enabled);
	static bool isMXSCallstackCaptureEnabled( );
	static void CplusplusCallstackCaptureEnabled( bool enabled);
	static bool isCplusplusCallstackCaptureEnabled( );

	/**
	helper class for disabling mxs callstack capture if an exception is thrown. 
	useful in code where exceptions are not unexpected, and the exception is 'eaten'.
	for example:
	<code>
	try
	{
		MAXScriptException::ScopedMXSCallstackCaptureDisable scopedMXSCallstackCaptureDisable;
		UUID = MaxSDK::Util::MaxString::FromMCHAR(res->to_string());
	}
	catch (...)
	{
		// Return value wasn't a string (e.g. "undefined").
	}
	</code>
	**/
	class ScripterExport ScopedMXSCallstackCaptureDisable : public MaxHeapOperators
	{
	private:
		bool m_disabled;
		bool m_clearErrorSourceData;
		bool m_oldCaptureState;
		MAXScript_TLS* m_tls;
	public:
		/**
		ScopedMXSCallstackCaptureDisable ctor:
		* \param disable	If true, disable mxs callstack capture in ctor
		* \param clearErrorSourceData	If true, call clear_error_source_data() in Enable() if an 
		* \param _tls The current MAXScript_TLS*
		exception is thrown.
		**/
		ScopedMXSCallstackCaptureDisable(bool disable = true, bool clearErrorSourceData = true);
		ScopedMXSCallstackCaptureDisable(MAXScript_TLS* _tls, bool disable = true, bool clearErrorSourceData = true);
		~ScopedMXSCallstackCaptureDisable() { Enable(); }
		void Enable();
	};

	/**
	helper class for disabling c++ callstack capture if a system exception is thrown. 
	useful in code where exceptions are not unexpected, and the exception is 'eaten'.
	for example:
	<code>
	try
	{
		MAXScriptException::ScopedCplusplusCallstackCaptureDisable scopedCplusplusCallstackCaptureDisable;
		UUID = MaxSDK::Util::MaxString::FromMCHAR(res->to_string());
	}
	catch (...)
	{
		// Return value wasn't a string (e.g. "undefined").
	}
	</code>
	**/
	class ScripterExport ScopedCplusplusCallstackCaptureDisable : public MaxHeapOperators
	{
	private:
		bool m_disabled;
		bool m_oldCaptureState;
	public:
		/**
		ScopedCplusplusCallstackCaptureDisable ctor:
		* \param disable	If true, disable c++ callstack capture in ctor
		exception is thrown.
		**/
		ScopedCplusplusCallstackCaptureDisable(bool disable = true);
		~ScopedCplusplusCallstackCaptureDisable() { Enable(); }
		void Enable();
	};
};

// Classes that derived from MAXScriptRuntimeErrorException automatically capture the mxs callstack. 
// Compile errors, FunctionReturn, etc. don't need to capture the mxs callstack.
// NOTE: If a class derives from MAXScriptRuntimeErrorException and takes const MCHAR* args in the ctor, the
// class ctor should call MAXScriptRuntimeErrorException(false) and then manually call CaptureMXSCallstack after acquiring the argument strings.
// This is to prevent potential problems where the string passed comes from a typical GetString implementation
// that returns a pointer to a static buffer, and that GetString function is called again while doing the stack dump.
// This would result in the wrong description being used.
class ScripterExport MAXScriptRuntimeErrorException : public MAXScriptException
{
public:
	MAXScriptRuntimeErrorException() { CaptureMXSCallstack(); }
	MAXScriptRuntimeErrorException(bool doStackCapture) { if (doStackCapture) CaptureMXSCallstack(); }
};

// this class is thrown if a non-maxscript exception (such as a c++ access violation) occurs
class ScripterExport UnknownSystemException : public MAXScriptRuntimeErrorException
{
	MSTR mAdditionalExceptionInformation;
public:
	UnknownSystemException() 
	{
		// if occurs, system exception occurred outside the scope of an 
		// ScopedMaxScriptSEHandler instance
		DbgAssert(false); 
	} 
	UnknownSystemException(int skipFirstNStackLevels, const MSTR& info) 
	{
		CaptureCplusplusCallstack(skipFirstNStackLevels);
		SetAdditionalExceptionInformation(info);
	}
	void	sprin1(CharStream* s);
	void	SetAdditionalExceptionInformation(const MSTR& info)
	{
		mAdditionalExceptionInformation = info;
	}
	const MSTR& GetAdditionalExceptionInformation()
	{
		return mAdditionalExceptionInformation;
	}
};

class ScripterExport SignalException : public MAXScriptException
{
public:
	void	sprin1(CharStream* s);
};

class ScripterExport CompileError : public MAXScriptException
{
public:
	MCHAR*	description;
	MCHAR*	info;
	MCHAR*  line;
	MCHAR*	file;
			CompileError (const MCHAR* d, const MCHAR* i, const MCHAR* l, const MCHAR* f = null_string);
			CompileError () { description = NULL; info = null_string; line = null_string; file = null_string; }
		   ~CompileError ();

	void	sprin1(CharStream* s);
	void	set_file(const MCHAR* f);
};

class ScripterExport SyntaxError : public CompileError
{
	MCHAR*	wanted;
	MCHAR*	got;
public:
			SyntaxError (const MCHAR* w, const MCHAR* g, const MCHAR* l = null_string, const MCHAR* f = null_string);
		   ~SyntaxError ();

	void	sprin1(CharStream* s);
};

class ScripterExport TypeError : public MAXScriptRuntimeErrorException
{
	ValueHolderMember	target;
	ValueMetaClass* wanted_class;
	MCHAR*	description;
public:
			TypeError (const MCHAR* d, Value* t, ValueMetaClass* c = NULL);
		   ~TypeError ();

	void	sprin1(CharStream* s);
};

class ScripterExport NoMethodError : public MAXScriptRuntimeErrorException
{
	ValueHolderMember	target;
	MCHAR*	fn_name;
public:
			NoMethodError (const MCHAR* fn, Value* t);
		   ~NoMethodError ();

	void	sprin1(CharStream* s);
};

#define unimplemented(m, t) throw NoMethodError (m, t)

class ScripterExport AccessorError : public MAXScriptRuntimeErrorException
{
	ValueHolderMember	target;
	ValueHolderMember	prop;
public:
			AccessorError (Value* t, Value* p);

	void	sprin1(CharStream* s);
};

class ScripterExport AssignToConstError : public MAXScriptRuntimeErrorException
{
	ValueHolderMember	thunk;
public:
			AssignToConstError (Thunk* t);

	void	sprin1(CharStream* s);
};

class ScripterExport ArgCountError : public MAXScriptRuntimeErrorException
{
	int		wanted;
	int		got;
	MCHAR*	fn_name;
public:
			ArgCountError (const MCHAR* fn, int w, int g);
		   ~ArgCountError ();

	void	sprin1(CharStream* s);
};

class ScripterExport RuntimeError : public MAXScriptRuntimeErrorException
{
public:
			MCHAR*	desc1;
			MCHAR*  desc2;
			ValueHolderMember  info;
			RuntimeError (const MCHAR* d1);
			RuntimeError (const MCHAR* d1, const MCHAR* d2);
			RuntimeError (const MCHAR* d1, Value* ii);
			RuntimeError (const MCHAR* d1, const MCHAR* d2, Value* ii);
			RuntimeError (Value* ii);
		   ~RuntimeError ();

	void	init(const MCHAR* d1, const MCHAR* d2, Value* ii);
	void	sprin1(CharStream* s);
};

class ScripterExport UserThrownError : public MAXScriptRuntimeErrorException
{
public:
			MCHAR*	desc;
			ValueHolderMember  info;
			BOOL	debugBreak;
			UserThrownError (const MCHAR* d1, Value* ii, BOOL dbgBreak);
			UserThrownError (const MCHAR* di, BOOL dbgBreak);
		   ~UserThrownError ();

	void	init(const MCHAR* d1, Value* ii, BOOL dbgBreak);
	void	sprin1(CharStream* s);
};

class ScripterExport DebuggerRuntimeError : public MAXScriptException
{
public:
	MCHAR*	desc1;
	MCHAR*  desc2;
	ValueHolderMember  info;
	DebuggerRuntimeError (const MCHAR* d1);
	DebuggerRuntimeError (const MCHAR* d1, const MCHAR* d2);
	DebuggerRuntimeError (const MCHAR* d1, Value* ii);
	DebuggerRuntimeError (const MCHAR* d1, const MCHAR* d2, Value* ii);
	DebuggerRuntimeError (Value* ii);
	~DebuggerRuntimeError ();

	void	init(const MCHAR* d1, const MCHAR* d2, Value* ii);
	void	sprin1(CharStream* s);
};

class ScripterExport IncompatibleTypes : public MAXScriptRuntimeErrorException
{
	ValueHolderMember  val1;
	ValueHolderMember  val2;
public:
			IncompatibleTypes (Value* v1, Value* v2);

	void	sprin1(CharStream* s);
};

class ScripterExport ConversionError : public MAXScriptRuntimeErrorException
{
	ValueHolderMember	val;
	MCHAR*   type;
public:
			ConversionError (Value* v, const MCHAR* t);
		   ~ConversionError ();

	void	sprin1(CharStream* s);
};

class FunctionReturn : public MAXScriptException
{
public:
	ValueHolderMember	return_result;
			FunctionReturn (Value* v);

	void	sprin1(CharStream* s);
};

class LoopExit : public MAXScriptException
{
public:
	ValueHolderMember	loop_result;
			LoopExit (Value* v);

	void	sprin1(CharStream* s);
};

class LoopContinue : public MAXScriptException
{
public:
			LoopContinue () { }

	void	sprin1(CharStream* s);
};

