/**********************************************************************
 *<
	DESCRIPTION:
		
	This tutorial is a simple example of function publishing.

	OVERVIEW:		
		
	Function publishing (FP) exposes multiple functions and properties, 
	and member functions to 3sd Max, other plug-ins, and MAXScript. Through the FP 
	model we can expose multiple functions by implementing a class the implements 
	the FPInterface interface, or one of it's descendant classes.
	
	When exposing a function to MAXScript via FP you can call 
	a member function using the dot operator. For example:

		foo.BarMethod()

	Whereas in standard MAXScript syntax, to call a function on an object
	would require the scripter to parse two different words, i.e.:

		BarMethod foo

	This plugin demonstrates how to code a simple function published interface called 
	'fpbasics'. 
	that contains the following functions and properties:

		'products' - This is a member function that takes 
		two numbers and returns their product.

		'message' - This is a void member function class that takes 
		a single string and displays it in a standard Win32 message box.

		'number' - A floating point read/write property

		'position' - A point2 read/write property.
	
	This plugin is implemented as a Global Utility Plugin (.gup).
	It is important to realize that this is a mechanism that can be used to 
	extend MAXScript outside of standard maxscript plugins (.dlx). 
	The MAXScript usage is listed below with the member function implementations 
	of class FP_Basic.

	CREATED BY:		Chris Johnson

	HISTORY:		Started June 7 2005

 *>	Copyright (c) 2003, All Rights Reserved.
 **********************************************************************/

#ifndef __fp_basics__H
#define __fp_basics__H
#pragma warning(push, 3)
#include "Max.h"
#include "resource.h"
#include "istdplug.h"
#include "iparamb2.h"
#include "iparamm2.h"
#include "iFnPub.h"

#include <guplib.h>
#pragma warning(pop)

extern TCHAR *GetString(int id);

extern HINSTANCE hInstance;

extern const Class_ID fp_basics_CLASS_ID;

//======================================================
//Function Publishing Examples
//======================================================

extern const Interface_ID FP_BASIC_INTERFACE;

inline class IFP_Basic* GetFP_basic() 
{ 
	return (IFP_Basic*) GetCOREInterface(FP_BASIC_INTERFACE); 
}

//! \brief Basic example of a interface. 
/*!	This is the class that clients will use. The publishing system 
	will expose pointers to member functions of this class. 
	When the client invokes these functions, with an instance of this 
	class, it will be resolved at runtime to a concrete instance of FP_Basic,
	shown below. */
class IFP_Basic : public FPStaticInterface
{
	public:
		virtual float GetNum() = 0;
		virtual void  SetNum(float x) = 0;
		virtual float products(float x, float y) = 0;
		virtual void  message() = 0;
		virtual Point2 GetPosition() = 0;
		virtual void   SetPosition( Point2 pos ) = 0;
		virtual Point2& GetLocation() = 0;
		virtual void   SetLocation( Point2& pos ) = 0;
		// Note that the enum is local to this class, and not in the global scope. 
		enum
		{ 
			em_getNum, 
			em_setNum, 
			em_products, 
			em_message,
			em_getPosition,
			em_setPosition,
			em_getLocation,
			em_setLocation
		};
};

//! \brief Implementation class for IFP_Basic
/*! This class must override the pure virtual functions in class IFP_Basic (declared above).
	It also includes function maps which expose it to the function publishing system. */
class FP_Basic : public IFP_Basic
{
	private:
		float  m_Num;
		Point2 m_Point;
		Point2 m_Location;

	public:
		float GetNum();
		void  SetNum(float x);

		float products(float x, float y);
		void  message();

		Point2 GetPosition();
		void   SetPosition( Point2 pos );

		Point2& GetLocation();
		void   SetLocation( Point2& pos );

	// When using the function map system, your class constructor will be written 
	// automatically via the DECLARE_DESCRIPTOR_INIT macro. Therefore you may
	// use a function name 'init' to do custom initialization during construction. 
	// It is preferable to use DECLARE_DESCRIPTOR_INIT, rather than DECLARE_DESCRIPTOR, since
	// this macro gives you the option of calling the init method on construction.
	// Note: You do not need to put semicolons (;) at the end of each macro. 

	#pragma warning(push)
	#pragma warning(disable:4238) // warning C4238: nonstandard extension used : class rvalue used as lvalue
	DECLARE_DESCRIPTOR_INIT(FP_Basic)
	BEGIN_FUNCTION_MAP

		// PROP_FNS is an abbreviation for PROPerty FunctioNS. It requires getter and setter
		// functions, a getter and setter enumeration, and a type (Not in that order). 
		// They must be entered in the order shown below.
		PROP_FNS(IFP_Basic::em_getNum, GetNum, IFP_Basic::em_setNum, SetNum, TYPE_FLOAT)

		// FN_2 is an abbreviation for a function that takes two arguements.
		// Here the function is registered as returning a float value, the function name
		// is passed in as well as the types of the two arguments.
		FN_2(IFP_Basic::em_products, TYPE_FLOAT, products, TYPE_FLOAT, TYPE_FLOAT)

		// VFN_0 is an abbreviation for a Void Function with 0 arguments. Hence a function
		// that closes with a (), i.e. foo.BarMethod()
		VFN_0(IFP_Basic::em_message, message)

		// This is another property definition for out point2 property called 'position'.
		// This macro when used with TYPE_POINT2_BV will generate a C4238 warning.
		PROP_FNS( IFP_Basic::em_getPosition, GetPosition, IFP_Basic::em_setPosition, SetPosition, TYPE_POINT2_BV)
		// This is another property definition for out point2 property called 'location'.
		PROP_FNS( IFP_Basic::em_getLocation, GetLocation, IFP_Basic::em_setLocation, SetLocation, TYPE_POINT2_BR)
	END_FUNCTION_MAP
	#pragma warning (pop)

	// The macro's above expand in the preprocessor to the following:
	// Note: Many parenthesis have been removed for clarity
	/*
	//! \begin preprocessor expansion
	public:
		FP_Basic()
		{
			init();
		}
		void init();
		FP_Basic(Interface_ID id, char* name, StringResID descr, ClassDesc* cd, USHORT flags, ...)
		{
			init();
			va_list ap;
			( __va_start(&ap, flags) ); 
			load_descriptor(id, name, descr, cd, flags, ap); 
			( ap = (va_list)0 );
		};
	public:
		FPStatus _dispatch_fn(FunctionID fid, TimeValue t, FPValue& result, FPParams* p)
		{
			FPStatus status = 1;
			switch (fid)
			{
				// For GetNum / SetNum --------------------
				case IFP_Basic::em_getNum:
					result.LoadPtr(TYPE_FLOAT, GetNum() );
					break; 
				case IFP_Basic::em_setNum:
					SetNum((p->params[0]).f);
					break;
				// For products ---------------------------
				case IFP_Basic::em_products:
					result.LoadPtr(TYPE_FLOAT, products( (p->params[0]).f , (p->params[1]).f )	);
					break;
				// For message ----------------------------
				case IFP_Basic::em_message:
					message();
					break;
				// For GetPosition / SetPosition ----------
				case IFP_Basic::em_getPosition:
					result.LoadPtr(TYPE_POINT2_BV,  
						&( GetPosition())); // This generates warning 4238, which is purposefully disabled
					break;
				case IFP_Basic::em_setPosition:
					SetPosition( *((p->params[0]).p2) );
					break;
				// For GetLocation / SetLocation ----------
				case IFP_Basic::em_getLocation: 
					result.LoadPtr(TYPE_POINT2_BR, 
						&( GetLocation())); 
					break;
				case IFP_Basic::em_setLocation: 
					SetLocation( *((p->params[0]).p2) );
					break;
				default:
					status = -1;
			}
			return status;
		}
	//! \end preprocessor expansion
	*/
};

#endif
