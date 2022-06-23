/*		local_class.h - macros for defining local classes and generics in a MAXScript extension .dlx
 *
 *
 *		Copyright (c) Autodesk, Inc.  1988.  John Wainwright.
 *
 */

#pragma once

#include "ClassCfg.h"
class Value;

#define local_visible_class(_cls)										\
	class _cls##Class : public ValueMetaClass							\
	{															\
	public:														\
		_cls##Class(const MCHAR* name) : ValueMetaClass (name) { }				\
		void	collect() { delete this; }								\
	};															\
	extern _cls##Class _cls##_class;

#define local_visible_class_s(_cls, _super)									\
	class _cls##Class;													\
	extern _cls##Class _cls##_class;						\
	class _cls##Class : public ValueMetaClass							\
	{																	\
	public:																\
					_cls##Class(const MCHAR* name) : ValueMetaClass (name) { }	\
		void		collect() { delete this; }							\
		Value*		classOf_vf(Value** arg_list, int count)				\
		{																\
			UNUSED_PARAM(arg_list);										\
			CHECK_ARG_COUNT(classOf, 1, count + 1);						\
			return &_super##_class;										\
		}																\
		Value*		superClassOf_vf(Value** arg_list, int count)		\
		{																\
			UNUSED_PARAM(arg_list);										\
			UNUSED_PARAM(count);										\
			return _super##_class.classOf_vf(NULL, 0);					\
		}																\
		Value* isKindOf_vf(Value** arg_list, int count)					\
		{																\
			CHECK_ARG_COUNT(isKindOf, 2, count + 1);					\
			Value* arg0 = arg_list[0];									\
			return (arg0 == &_cls##_class || arg0 == &_super##_class) ?	\
				&true_value :											\
				_super##_class.isKindOf_vf(arg_list, count);			\
		}																\
		BOOL is_kind_of(ValueMetaClass* c)								\
		{																\
			return (c == &_cls##_class) ? 1								\
						: _super##_class.is_kind_of(c);					\
		}																\
	};

#define local_applyable_class(_cls)									\
	class _cls##Class : public ValueMetaClass							\
	{															\
	public:														\
		_cls##Class(const MCHAR* name) : ValueMetaClass (name) { }				\
		void	collect() { delete this; }								\
		Value* apply(Value** arglist, int count, CallContext* cc = NULL);	\
	};															\
	extern _cls##Class _cls##_class;

#define local_visible_class_instance(_cls, _name)							\
	_cls##Class _cls##_class (_M(_name));

class MS_LOCAL_ROOT_CLASS;
typedef Value* (MS_LOCAL_ROOT_CLASS::*local_value_vf)(Value**, int);
// confusing macros. Do not use them.
#define cat0(_a) _a
#define cat1(_a) cat0(_a)
#define cat2(_a, _b) cat0(_a)##cat0(_b)
#define str0(_c) #_c
#define str1(_c) str0(_c)

/*! \name Concatenation Macros
	For details on the token pasting operator and joining two strings together
	in the preprocessor see:
	\li ISO/IEC 14882:2003(E) 16.3.3 The ## operator
	\li ISO/IEC 9899:1999 (E) 6.10.3.3 The ## operator */
//@{
//! Helper macro used in concatenating two strings.
#define CONCATENATE_HELPER(x,y) x##y
//! Joins two strings together in the preprocessor before the code is compiled.
#define CONCATENATE(x,y) CONCATENATE_HELPER(x,y)
//@}

#define MS_LOCAL_ROOT_CLASS_TAG     &CONCATENATE(MS_LOCAL_ROOT_CLASS, _class)
#define MS_LOCAL_GENERIC_CLASS_TAG  &CONCATENATE(MS_LOCAL_GENERIC_CLASS, _class)
#define MS_LOCAL_GENERIC_CLASS_CLASS CONCATENATE(MS_LOCAL_GENERIC_CLASS, Class)
#define MS_LOCAL_GENERIC_CLASS_class CONCATENATE(MS_LOCAL_GENERIC_CLASS, _class)