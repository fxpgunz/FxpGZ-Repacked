
#pragma once

#include "../kernel/MAXScript_TLS.h"
#include "../../assert1.h"
#include "../../Noncopyable.h"
// forward declarations
class Value;

/*! Class that uses the RAII idiom to push/pop the thread local current_locals_frame. 
	This ensures that the thread local current_locals_frame is always properly restored, even in the event of 
	an exception. 
	Instances of this class are automatically created via the various XXX_value_local(s), xxx_typed_value_local(s)
	and value_local_array_tls macros below.
	If validateFrame is false, no validation of thread_local(current_locals_frame) is done. This is normally true,
	only false in Name::intern. Lots of places intern names w/o setting up a local scope.
*/
class MXS_Exit_ValueLocal_Scope : public MaxSDK::Util::Noncopyable 
{
	Value** frame;
	MAXScript_TLS* _tls;

public:
	ScripterExport MXS_Exit_ValueLocal_Scope(bool validateFrame = true, MAXScript_TLS* tls = nullptr);
	ScripterExport MXS_Exit_ValueLocal_Scope(MAXScript_TLS* tls);
	ScripterExport ~MXS_Exit_ValueLocal_Scope();
};

/* value local macros - for managing C local variable references to Value*'s for the collector - see Collectable.cpp */

#define one_value_local(n1)										\
	struct { int count; Value** link; Value* n1; } vl =	\
		{ 1, NULL, NULL };											\
	vl.link = thread_local(current_locals_frame);			\
	thread_local(current_locals_frame) = (Value**)&vl;		\
	MXS_Exit_ValueLocal_Scope mxs_Exit_ValueLocal_Scope;
	
#define one_value_local_noFrameValidate(n1)					\
	struct { int count; Value** link; Value* n1; } vl =	\
		{ 1, NULL, NULL };											\
	vl.link = thread_local(current_locals_frame);			\
	thread_local(current_locals_frame) = (Value**)&vl;		\
	MXS_Exit_ValueLocal_Scope mxs_Exit_ValueLocal_Scope(false);

#define one_typed_value_local(n1)								\
	struct { int count; Value** link; n1; } vl =				\
		{ 1, NULL, NULL };											\
	vl.link = thread_local(current_locals_frame);			\
	thread_local(current_locals_frame) = (Value**)&vl;		\
	MXS_Exit_ValueLocal_Scope mxs_Exit_ValueLocal_Scope;
	
#define one_typed_value_local_noFrameValidate(n1)				\
	struct { int count; Value** link; n1; } vl =				\
		{ 1, NULL, NULL };											\
	vl.link = thread_local(current_locals_frame);			\
	thread_local(current_locals_frame) = (Value**)&vl;		\
	MXS_Exit_ValueLocal_Scope mxs_Exit_ValueLocal_Scope(false);
	
#define two_value_locals(n1, n2)									\
	struct { int count; Value** link; Value *n1, *n2; } vl =		\
		{ 2, NULL, NULL, NULL };									\
	vl.link = thread_local(current_locals_frame);			\
	thread_local(current_locals_frame) = (Value**)&vl;		\
	MXS_Exit_ValueLocal_Scope mxs_Exit_ValueLocal_Scope;
	
#define two_typed_value_locals(n1, n2)							\
	struct { int count; Value** link; n1; n2; } vl =		\
		{ 2, NULL, NULL, NULL };									\
	vl.link = thread_local(current_locals_frame);			\
	thread_local(current_locals_frame) = (Value**)&vl;		\
	MXS_Exit_ValueLocal_Scope mxs_Exit_ValueLocal_Scope;
	
#define three_value_locals(n1, n2, n3)							\
	struct { int count; Value** link; Value *n1, *n2, *n3; } vl =	\
		{ 3, NULL, NULL, NULL, NULL };							\
	vl.link = thread_local(current_locals_frame);			\
	thread_local(current_locals_frame) = (Value**)&vl;		\
	MXS_Exit_ValueLocal_Scope mxs_Exit_ValueLocal_Scope;
	
#define three_typed_value_locals(n1, n2, n3)					\
	struct { int count; Value** link; n1; n2; n3; } vl =	\
		{ 3, NULL, NULL, NULL, NULL };							\
	vl.link = thread_local(current_locals_frame);			\
	thread_local(current_locals_frame) = (Value**)&vl;		\
	MXS_Exit_ValueLocal_Scope mxs_Exit_ValueLocal_Scope;
	
#define four_value_locals(n1, n2, n3, n4)						\
	struct { int count; Value** link; Value *n1, *n2, *n3, *n4; } vl =	\
		{ 4, NULL, NULL, NULL, NULL, NULL };					\
	vl.link = thread_local(current_locals_frame);			\
	thread_local(current_locals_frame) = (Value**)&vl;		\
	MXS_Exit_ValueLocal_Scope mxs_Exit_ValueLocal_Scope;
	
#define four_typed_value_locals(n1, n2, n3, n4)				\
	struct { int count; Value** link; n1; n2; n3; n4; } vl =		\
		{ 4, NULL, NULL, NULL, NULL, NULL };					\
	vl.link = thread_local(current_locals_frame);			\
	thread_local(current_locals_frame) = (Value**)&vl;		\
	MXS_Exit_ValueLocal_Scope mxs_Exit_ValueLocal_Scope;

#define five_value_locals(n1, n2, n3, n4, n5)				\
	struct { int count; Value** link; Value *n1, *n2, *n3, *n4, *n5; } vl =	\
		{ 5, NULL, NULL, NULL, NULL, NULL, NULL };			\
	vl.link = thread_local(current_locals_frame);			\
	thread_local(current_locals_frame) = (Value**)&vl;		\
	MXS_Exit_ValueLocal_Scope mxs_Exit_ValueLocal_Scope;

#define five_typed_value_locals(n1, n2, n3, n4, n5)		\
	struct { int count; Value** link; n1; n2; n3; n4; n5; } vl =	\
		{ 5, NULL, NULL, NULL, NULL, NULL, NULL };			\
	vl.link = thread_local(current_locals_frame);			\
	thread_local(current_locals_frame) = (Value**)&vl;		\
	MXS_Exit_ValueLocal_Scope mxs_Exit_ValueLocal_Scope;
	
#define six_value_locals(n1, n2, n3, n4, n5, n6)			\
	struct { int count; Value** link; Value *n1, *n2, *n3, *n4, *n5, *n6; } vl = \
		{ 6, NULL, NULL, NULL, NULL, NULL, NULL, NULL };	\
	vl.link = thread_local(current_locals_frame);			\
	thread_local(current_locals_frame) = (Value**)&vl;		\
	MXS_Exit_ValueLocal_Scope mxs_Exit_ValueLocal_Scope;

#define six_typed_value_locals(n1, n2, n3, n4, n5, n6)	\
	struct { int count; Value** link; n1; n2; n3; n4; n5; n6; } vl =		\
		{ 6, NULL, NULL, NULL, NULL, NULL, NULL, NULL };	\
	vl.link = thread_local(current_locals_frame);			\
	thread_local(current_locals_frame) = (Value**)&vl;		\
	MXS_Exit_ValueLocal_Scope mxs_Exit_ValueLocal_Scope;
	
#define seven_value_locals(n1, n2, n3, n4, n5, n6, n7)				\
	struct { int count; Value** link; Value *n1, *n2, *n3, *n4, *n5, *n6, *n7; } vl = \
		{ 7, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };		\
	vl.link = thread_local(current_locals_frame);						\
	thread_local(current_locals_frame) = (Value**)&vl;					\
	MXS_Exit_ValueLocal_Scope mxs_Exit_ValueLocal_Scope;
	
#define seven_typed_value_locals(n1, n2, n3, n4, n5, n6, n7)		\
	struct { int count; Value** link; n1; n2; n3; n4; n5; n6; n7; } vl =	\
		{ 7, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };		\
	vl.link = thread_local(current_locals_frame);						\
	thread_local(current_locals_frame) = (Value**)&vl;					\
	MXS_Exit_ValueLocal_Scope mxs_Exit_ValueLocal_Scope;
	
#define eight_value_locals(n1, n2, n3, n4, n5, n6, n7, n8)				\
	struct { int count; Value** link; Value *n1, *n2, *n3, *n4, *n5, *n6, *n7, *n8; } vl = \
		{ 8, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };	\
	vl.link = thread_local(current_locals_frame);							\
	thread_local(current_locals_frame) = (Value**)&vl;						\
	MXS_Exit_ValueLocal_Scope mxs_Exit_ValueLocal_Scope;
	
#define eight_typed_value_locals(n1, n2, n3, n4, n5, n6, n7, n8)		\
	struct { int count; Value** link; n1; n2; n3; n4; n5; n6; n7; n8; } vl =	\
		{ 8, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };	\
	vl.link = thread_local(current_locals_frame);							\
	thread_local(current_locals_frame) = (Value**)&vl;						\
	MXS_Exit_ValueLocal_Scope mxs_Exit_ValueLocal_Scope;

#define nine_value_locals(n1, n2, n3, n4, n5, n6, n7, n8, n9)					\
	struct { int count; Value** link; Value *n1, *n2, *n3, *n4, *n5, *n6, *n7, *n8, *n9; } vl = \
		{ 9, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };	\
	vl.link = thread_local(current_locals_frame);									\
	thread_local(current_locals_frame) = (Value**)&vl;								\
	MXS_Exit_ValueLocal_Scope mxs_Exit_ValueLocal_Scope;
	
#define nine_typed_value_locals(n1, n2, n3, n4, n5, n6, n7, n8, n9)			\
	struct { int count; Value** link; n1; n2; n3; n4; n5; n6; n7; n8; n9; } vl =	\
		{ 9, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };	\
	vl.link = thread_local(current_locals_frame);									\
	thread_local(current_locals_frame) = (Value**)&vl;								\
	MXS_Exit_ValueLocal_Scope mxs_Exit_ValueLocal_Scope;
	
#define ten_value_locals(n1, n2, n3, n4, n5, n6, n7, n8, n9, n10)						\
	struct { int count; Value** link; Value *n1, *n2, *n3, *n4, *n5, *n6, *n7, *n8, *n9, *n10; } vl =	\
		{ 10, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };	\
	vl.link = thread_local(current_locals_frame);											\
	thread_local(current_locals_frame) = (Value**)&vl;										\
	MXS_Exit_ValueLocal_Scope mxs_Exit_ValueLocal_Scope;

#define ten_typed_value_locals(n1, n2, n3, n4, n5, n6, n7, n8, n9, n10)				\
	struct { int count; Value** link; n1; n2; n3; n4; n5; n6; n7; n8; n9; n10; } vl =	\
		{ 10, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };	\
	vl.link = thread_local(current_locals_frame);											\
	thread_local(current_locals_frame) = (Value**)&vl;										\
	MXS_Exit_ValueLocal_Scope mxs_Exit_ValueLocal_Scope;

#define eleven_value_locals(n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11)					\
	struct { int count; Value** link; Value *n1, *n2, *n3, *n4, *n5, *n6, *n7, *n8, *n9, *n10, *n11; } vl =	\
		{ 11, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };	\
	vl.link = thread_local(current_locals_frame);													\
	thread_local(current_locals_frame) = (Value**)&vl;												\
	MXS_Exit_ValueLocal_Scope mxs_Exit_ValueLocal_Scope;

#define eleven_typed_value_locals(n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11)			\
	struct { int count; Value** link; n1; n2; n3; n4; n5; n6; n7; n8; n9; n10; n11; } vl =	\
		{ 11, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };	\
	vl.link = thread_local(current_locals_frame);													\
	thread_local(current_locals_frame) = (Value**)&vl;												\
	MXS_Exit_ValueLocal_Scope mxs_Exit_ValueLocal_Scope;

#define value_local_array(var, count) {									\
	var = &((Value**)_alloca(((count) + 2) * sizeof(Value*)))[2];	\
	memset(var, 0, (count) * sizeof(Value*));								\
	var[-2] = (Value*)(static_cast<INT_PTR>(count));					\
	var[-1] = (Value*)thread_local(current_locals_frame);				\
	thread_local(current_locals_frame) = &var[-2]; }					\
	MXS_Exit_ValueLocal_Scope mxs_Exit_ValueLocal_Scope_##var;

// same as above but assumes that the following has been run and _tls is available in current scope:
// MAXScript_TLS* _tls = (MAXScript_TLS*)TlsGetValue(thread_locals_index);

#define one_value_local_tls(n1)										\
	struct { int count; Value** link; Value* n1; } vl =	\
		{ 1, NULL, NULL };											\
	vl.link = _tls->current_locals_frame;			\
	_tls->current_locals_frame = (Value**)&vl;		\
	MXS_Exit_ValueLocal_Scope mxs_Exit_ValueLocal_Scope(_tls);

#define one_value_local_noFrameValidate_tls(n1)					\
	struct { int count; Value** link; Value* n1; } vl =	\
		{ 1, NULL, NULL };											\
	vl.link = _tls->current_locals_frame;			\
	_tls->current_locals_frame = (Value**)&vl;		\
	MXS_Exit_ValueLocal_Scope mxs_Exit_ValueLocal_Scope(false, _tls);

#define one_typed_value_local_tls(n1)								\
	struct { int count; Value** link; n1; } vl =				\
		{ 1, NULL, NULL };											\
	vl.link = _tls->current_locals_frame;			\
	_tls->current_locals_frame = (Value**)&vl;		\
	MXS_Exit_ValueLocal_Scope mxs_Exit_ValueLocal_Scope(_tls);

#define one_typed_value_local_noFrameValidate_tls(n1)				\
	struct { int count; Value** link; n1; } vl =				\
		{ 1, NULL, NULL };											\
	vl.link = _tls->current_locals_frame;			\
	_tls->current_locals_frame = (Value**)&vl;		\
	MXS_Exit_ValueLocal_Scope mxs_Exit_ValueLocal_Scope(false, _tls);

#define two_value_locals_tls(n1, n2)									\
	struct { int count; Value** link; Value *n1, *n2; } vl =		\
		{ 2, NULL, NULL, NULL };									\
	vl.link = _tls->current_locals_frame;			\
	_tls->current_locals_frame = (Value**)&vl;		\
	MXS_Exit_ValueLocal_Scope mxs_Exit_ValueLocal_Scope(_tls);

#define two_typed_value_locals_tls(n1, n2)							\
	struct { int count; Value** link; n1; n2; } vl =		\
		{ 2, NULL, NULL, NULL };									\
	vl.link = _tls->current_locals_frame;			\
	_tls->current_locals_frame = (Value**)&vl;		\
	MXS_Exit_ValueLocal_Scope mxs_Exit_ValueLocal_Scope(_tls);

#define three_value_locals_tls(n1, n2, n3)							\
	struct { int count; Value** link; Value *n1, *n2, *n3; } vl =	\
		{ 3, NULL, NULL, NULL, NULL };							\
	vl.link = _tls->current_locals_frame;			\
	_tls->current_locals_frame = (Value**)&vl;		\
	MXS_Exit_ValueLocal_Scope mxs_Exit_ValueLocal_Scope(_tls);

#define three_typed_value_locals_tls(n1, n2, n3)					\
	struct { int count; Value** link; n1; n2; n3; } vl =	\
		{ 3, NULL, NULL, NULL, NULL };							\
	vl.link = _tls->current_locals_frame;			\
	_tls->current_locals_frame = (Value**)&vl;		\
	MXS_Exit_ValueLocal_Scope mxs_Exit_ValueLocal_Scope(_tls);

#define four_value_locals_tls(n1, n2, n3, n4)						\
	struct { int count; Value** link; Value *n1, *n2, *n3, *n4; } vl =	\
		{ 4, NULL, NULL, NULL, NULL, NULL };					\
	vl.link = _tls->current_locals_frame;			\
	_tls->current_locals_frame = (Value**)&vl;		\
	MXS_Exit_ValueLocal_Scope mxs_Exit_ValueLocal_Scope(_tls);

#define four_typed_value_locals_tls(n1, n2, n3, n4)				\
	struct { int count; Value** link; n1; n2; n3; n4; } vl =		\
		{ 4, NULL, NULL, NULL, NULL, NULL };					\
	vl.link = _tls->current_locals_frame;			\
	_tls->current_locals_frame = (Value**)&vl;		\
	MXS_Exit_ValueLocal_Scope mxs_Exit_ValueLocal_Scope(_tls);

#define five_value_locals_tls(n1, n2, n3, n4, n5)				\
	struct { int count; Value** link; Value *n1, *n2, *n3, *n4, *n5; } vl =	\
		{ 5, NULL, NULL, NULL, NULL, NULL, NULL };			\
	vl.link = _tls->current_locals_frame;			\
	_tls->current_locals_frame = (Value**)&vl;		\
	MXS_Exit_ValueLocal_Scope mxs_Exit_ValueLocal_Scope(_tls);

#define five_typed_value_locals_tls(n1, n2, n3, n4, n5)		\
	struct { int count; Value** link; n1; n2; n3; n4; n5; } vl =	\
		{ 5, NULL, NULL, NULL, NULL, NULL, NULL };			\
	vl.link = _tls->current_locals_frame;			\
	_tls->current_locals_frame = (Value**)&vl;		\
	MXS_Exit_ValueLocal_Scope mxs_Exit_ValueLocal_Scope(_tls);

#define six_value_locals_tls(n1, n2, n3, n4, n5, n6)			\
	struct { int count; Value** link; Value *n1, *n2, *n3, *n4, *n5, *n6; } vl = \
		{ 6, NULL, NULL, NULL, NULL, NULL, NULL, NULL };	\
	vl.link = _tls->current_locals_frame;			\
	_tls->current_locals_frame = (Value**)&vl;		\
	MXS_Exit_ValueLocal_Scope mxs_Exit_ValueLocal_Scope(_tls);

#define six_typed_value_locals_tls(n1, n2, n3, n4, n5, n6)	\
	struct { int count; Value** link; n1; n2; n3; n4; n5; n6; } vl =		\
		{ 6, NULL, NULL, NULL, NULL, NULL, NULL, NULL };	\
	vl.link = _tls->current_locals_frame;			\
	_tls->current_locals_frame = (Value**)&vl;		\
	MXS_Exit_ValueLocal_Scope mxs_Exit_ValueLocal_Scope(_tls);

#define seven_value_locals_tls(n1, n2, n3, n4, n5, n6, n7)				\
	struct { int count; Value** link; Value *n1, *n2, *n3, *n4, *n5, *n6, *n7; } vl = \
		{ 7, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };		\
	vl.link = _tls->current_locals_frame;						\
	_tls->current_locals_frame = (Value**)&vl;					\
	MXS_Exit_ValueLocal_Scope mxs_Exit_ValueLocal_Scope(_tls);

#define seven_typed_value_locals_tls(n1, n2, n3, n4, n5, n6, n7)		\
	struct { int count; Value** link; n1; n2; n3; n4; n5; n6; n7; } vl =	\
		{ 7, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };		\
	vl.link = _tls->current_locals_frame;						\
	_tls->current_locals_frame = (Value**)&vl;					\
	MXS_Exit_ValueLocal_Scope mxs_Exit_ValueLocal_Scope(_tls);

#define eight_value_locals_tls(n1, n2, n3, n4, n5, n6, n7, n8)				\
	struct { int count; Value** link; Value *n1, *n2, *n3, *n4, *n5, *n6, *n7, *n8; } vl = \
		{ 8, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };	\
	vl.link = _tls->current_locals_frame;							\
	_tls->current_locals_frame = (Value**)&vl;						\
	MXS_Exit_ValueLocal_Scope mxs_Exit_ValueLocal_Scope(_tls);

#define eight_typed_value_locals_tls(n1, n2, n3, n4, n5, n6, n7, n8)		\
	struct { int count; Value** link; n1; n2; n3; n4; n5; n6; n7; n8; } vl =	\
		{ 8, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };	\
	vl.link = _tls->current_locals_frame;							\
	_tls->current_locals_frame = (Value**)&vl;						\
	MXS_Exit_ValueLocal_Scope mxs_Exit_ValueLocal_Scope(_tls);

#define nine_value_locals_tls(n1, n2, n3, n4, n5, n6, n7, n8, n9)					\
	struct { int count; Value** link; Value *n1, *n2, *n3, *n4, *n5, *n6, *n7, *n8, *n9; } vl = \
		{ 9, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };	\
	vl.link = _tls->current_locals_frame;									\
	_tls->current_locals_frame = (Value**)&vl;								\
	MXS_Exit_ValueLocal_Scope mxs_Exit_ValueLocal_Scope(_tls);

#define nine_typed_value_locals_tls(n1, n2, n3, n4, n5, n6, n7, n8, n9)			\
	struct { int count; Value** link; n1; n2; n3; n4; n5; n6; n7; n8; n9; } vl =	\
		{ 9, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };	\
	vl.link = _tls->current_locals_frame;									\
	_tls->current_locals_frame = (Value**)&vl;								\
	MXS_Exit_ValueLocal_Scope mxs_Exit_ValueLocal_Scope(_tls);

#define ten_value_locals_tls(n1, n2, n3, n4, n5, n6, n7, n8, n9, n10)						\
	struct { int count; Value** link; Value *n1, *n2, *n3, *n4, *n5, *n6, *n7, *n8, *n9, *n10; } vl =	\
		{ 10, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };	\
	vl.link = _tls->current_locals_frame;											\
	_tls->current_locals_frame = (Value**)&vl;										\
	MXS_Exit_ValueLocal_Scope mxs_Exit_ValueLocal_Scope(_tls);

#define ten_typed_value_locals_tls(n1, n2, n3, n4, n5, n6, n7, n8, n9, n10)				\
	struct { int count; Value** link; n1; n2; n3; n4; n5; n6; n7; n8; n9; n10; } vl =	\
		{ 10, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };	\
	vl.link = _tls->current_locals_frame;											\
	_tls->current_locals_frame = (Value**)&vl;										\
	MXS_Exit_ValueLocal_Scope mxs_Exit_ValueLocal_Scope(_tls);

#define eleven_value_locals_tls(n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11)					\
	struct { int count; Value** link; Value *n1, *n2, *n3, *n4, *n5, *n6, *n7, *n8, *n9, *n10, *n11; } vl =	\
		{ 11, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };	\
	vl.link = _tls->current_locals_frame;													\
	_tls->current_locals_frame = (Value**)&vl;												\
	MXS_Exit_ValueLocal_Scope mxs_Exit_ValueLocal_Scope(_tls);

#define eleven_typed_value_locals_tls(n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11)			\
	struct { int count; Value** link; n1; n2; n3; n4; n5; n6; n7; n8; n9; n10; n11; } vl =	\
		{ 11, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };	\
	vl.link = _tls->current_locals_frame;													\
	_tls->current_locals_frame = (Value**)&vl;												\
	MXS_Exit_ValueLocal_Scope mxs_Exit_ValueLocal_Scope(_tls);

#define value_local_array_tls(var, count) {									\
	var = &((Value**)_alloca(((count) + 2) * sizeof(Value*)))[2];	\
	memset(var, 0, (count) * sizeof(Value*));								\
	var[-2] = (Value*)(static_cast<INT_PTR>(count));					\
	var[-1] = (Value*)_tls->current_locals_frame;				\
	_tls->current_locals_frame = &var[-2]; }					\
	MXS_Exit_ValueLocal_Scope mxs_Exit_ValueLocal_Scope_##var(_tls);

//! \deprecated Deprecated as of 3ds Max 2017. value local storage handled by MXS_Exit_ValueLocal_Scope instances
#pragma deprecated("pop_value_local_array_tls")
#pragma deprecated("pop_value_locals_tls")
	
// this function is used by return_value_tls macro to place the value in the 'current_results' thread local
// and pop the current local frame. return_value_tls is implemented this way to ensure that code that
// looks like return_value_tls(new Point3()) works correctly. Up through 3ds Max 2012, 2 new values ended
// up being created
template<typename Type> Type templated_return_value_fn(Type r)
{
	thread_local(current_result) = r;
	return r;
}

// use this macro to return a value that is protected from garbage collection
// all functions that return a Value* should return via return_value_tls
#define return_value(r)	return templated_return_value_fn(r)

// this function is used by return_value_tls macro to place the value in the 'current_results' thread local
// and pop the current local frame. return_value_tls is implemented this way to ensure that code that
// looks like return_value_tls(new Point3()) works correctly. Up through 3ds Max 2012, 2 new values ended
// up being created
template<typename Type> Type templated_return_value_fn_tls(Type r,  MAXScript_TLS* _tls)
{
	_tls->current_result = r;
	return r;
}

// use this macro to return a value that is protected from garbage collection
// all functions that return a Value* should return via return_value_tls
#define return_value_tls(r)	return templated_return_value_fn_tls(r, _tls)

//! \deprecated Deprecated as of 3ds Max 2017. Use return_value_tls instead. value local storage handled by MXS_Exit_ValueLocal_Scope instances
#pragma deprecated("return_value_no_pop")
#pragma deprecated("return_protected")

/*! Class that uses the RAII idiom to allocate and deallocate value temp arrays. 
	This ensures that the current frame data is always properly deallocated, even in the event of 
	an exception. 
*/
class ScopedValueTempArray : public MaxSDK::Util::Noncopyable 
{
	Value** m_frame;
	Value** m_localArray;
	MAXScript_TLS* m_tls;
	int m_count;
public:
	// allocates temp array with specified size
	ScripterExport ScopedValueTempArray(unsigned int count = 0, MAXScript_TLS* tls = nullptr);
	ScripterExport ScopedValueTempArray(MAXScript_TLS* tls);
	// deallocates temp array 
	ScripterExport ~ScopedValueTempArray();

	// resize temp array to specified size
	ScripterExport void Resize(int newCount);
	operator Value** () { return m_localArray; }
	Value** data() { return m_localArray; }
};

//! \deprecated Deprecated as of 3ds Max 2017. Use ScopedValueTempArray instead. 
#pragma deprecated("value_temp_array")
#pragma deprecated("value_temp_array0")
#pragma deprecated("realloc_value_temp_array")

/*! Class that uses the RAII idiom to save and restore the current frame data. 
	This ensures that the current frame data is properly restored in the event of
	an exception. An instance should be created at the top of a try expression,
	and as the expresion is exited, either normally or via an exception, the current
	frame data is automatically restored.
*/
class ScopedSaveCurrentFrames : public MaxSDK::Util::Noncopyable
{
public:
	ScripterExport ScopedSaveCurrentFrames(MAXScript_TLS* tls = nullptr);
	ScripterExport ~ScopedSaveCurrentFrames();
	ScripterExport void RestoreCurrentFrames();
private:
	Value** m_sclf;
	Value** m_scsf;
	Value** m_scf;
	MAXScript_TLS* m_tls;
};

//! \deprecated Deprecated as of 3ds Max 2017. Use ScopedSaveCurrentFrames instead. 
#pragma deprecated("save_current_frames")
#pragma deprecated("restore_current_frames")

/*! Class that uses the RAII idiom to push/pop setting a structured error handler. 
	This ensures that the structured error handler is properly restored in the event of
	an exception. An instance is created as a member of ScopedMaxScriptEvaluationContext instances.
	The structured error handler is called when a structured error is thrown (such as an access
	violation). It captures information on the error, captures the c++ call stack, stores this
	information in an UnknownSystemException, and throws the UnknownSystemException.
*/
class ScopedMaxScriptSEHandler : public MaxSDK::Util::Noncopyable
{
	_se_translator_function m_oldTranslatorFunction;
	ScripterExport static void trans_func( unsigned int u, EXCEPTION_POINTERS* pExp );
public:
	ScopedMaxScriptSEHandler()
	{
		m_oldTranslatorFunction = _set_se_translator( trans_func );
	}
	~ScopedMaxScriptSEHandler()
	{
		_se_translator_function currentTranslatorFunction = _set_se_translator( m_oldTranslatorFunction );
		currentTranslatorFunction;
		DbgAssert(m_oldTranslatorFunction == nullptr || currentTranslatorFunction == m_oldTranslatorFunction);
	}
	ScripterExport static MSTR ExceptionInformation(EXCEPTION_POINTERS* lpExceptionInfo);
};

/*! Class that uses the RAII idiom to create a maxscript visible stack frame. 
	This ensures a stack frame is allocated in the constructor
	and properly deallocated in the destructor. This ensures resources are 
	properly cleaned up in the event of an exception. 
	Also note, that this will optionally allocate thread local storage for the calling thread. 
	This is done because using any of the core maxscript classes
	requires access to maxscript's thread local storage. If this is not done, the
	application could crash.
*/
class ScopedMaxScriptEvaluationContext : public MaxSDK::Util::Noncopyable
{
	bool m_ContextExitted;
	ScopedMaxScriptSEHandler* m_ScopedMaxScriptSEHandler;
	MAXScript_TLS* m_tls;
public:
	// conditionally calls init_thread_locals() and pushs new stack frame
	ScripterExport ScopedMaxScriptEvaluationContext(bool initThreadLocals = true, MAXScript_TLS* _tls = nullptr);
	// pops stack frame if not previously popped
	ScripterExport ~ScopedMaxScriptEvaluationContext();
	// pops stack frame
	ScripterExport void ExitContext();
	// pops stack frame and moves result into callers frame if only on stack
	ScripterExport void ExitContext(Value*& result);
	// Gets the MAXScript_TLS*
	MAXScript_TLS* Get_TLS() { return m_tls; }

};

#define save_current_source()											\
	Value* _save_source = thread_local(source_file);			\
	MSZipPackage* _save_pkg = thread_local(current_pkg);		\
	unsigned int _save_pos = thread_local(source_pos);			\
	unsigned int _save_line = thread_local(source_line);		\
	DWORD _save_flags = thread_local(source_flags);

#define restore_current_source() {								\
	thread_local(source_file) = _save_source;					\
	thread_local(current_pkg) = _save_pkg;						\
	thread_local(source_pos) = _save_pos;						\
	thread_local(source_line) = _save_line;					\
	thread_local(source_flags) = _save_flags;}

#define save_current_source_no_pos()							\
	Value* _save_source = thread_local(source_file);		\
	MSZipPackage* _save_pkg = thread_local(current_pkg);	\
	DWORD _save_flags = thread_local(source_flags);

#define restore_current_source_no_pos() {						\
	thread_local(source_file) = _save_source;					\
	thread_local(current_pkg) = _save_pkg;						\
	thread_local(source_flags) = _save_flags;}

#define save_current_source_pos()								\
	unsigned int  _save_pos = thread_local(source_pos);	\
	unsigned int _save_line = thread_local(source_line);	\
	DWORD _save_flags = thread_local(source_flags);

#define restore_current_source_pos() {							\
	thread_local(source_pos) = _save_pos;						\
	thread_local(source_line) = _save_line;					\
	thread_local(source_flags) = _save_flags;}

#define save_current_source_to_frame() {								\
	Value** _frame = thread_local(current_scan_frame);				\
	if (_frame) _frame[3] = thread_local(source_file);				\
	if (_frame) _frame[4] = (Value*)thread_local(source_pos);	\
	if (_frame) _frame[8] = (Value*)thread_local(source_line);	\
	if (_frame) _frame[7] = (Value*)static_cast<DWORD_PTR>(thread_local(source_flags));}

#define save_current_source_to_frame_no_pos() {					\
	Value** _frame = thread_local(current_scan_frame);			\
	if (_frame) _frame[3] = thread_local(source_file);			\
	if (_frame) _frame[7] = (Value*)static_cast<DWORD_PTR>(thread_local(source_flags));}

#define save_current_source_to_frame_pos() {							\
	Value** _frame = thread_local(current_scan_frame);				\
	if (_frame) _frame[4] = (Value*)thread_local(source_pos);	\
	if (_frame) _frame[8] = (Value*)thread_local(source_line);}

#define save_current_source_to_frame_no_file() {					\
	Value** _frame = thread_local(current_scan_frame);				\
	if (_frame) _frame[4] = (Value*)thread_local(source_pos);	\
	if (_frame) _frame[8] = (Value*)thread_local(source_line);}

// same as above but assumes that the following has been run and _tls is available in current scope:
// MAXScript_TLS* _tls = (MAXScript_TLS*)TlsGetValue(thread_locals_index);

#define save_current_source_tls()											\
	Value* _save_source = _tls->source_file;			\
	MSZipPackage* _save_pkg = _tls->current_pkg;		\
	unsigned int _save_pos = _tls->source_pos;			\
	unsigned int _save_line = _tls->source_line;		\
	DWORD _save_flags = _tls->source_flags;

#define restore_current_source_tls() {								\
	_tls->source_file = _save_source;					\
	_tls->current_pkg = _save_pkg;						\
	_tls->source_pos = _save_pos;						\
	_tls->source_line = _save_line;					\
	_tls->source_flags = _save_flags;}

#define save_current_source_no_pos_tls()							\
	Value* _save_source = _tls->source_file;		\
	MSZipPackage* _save_pkg = _tls->current_pkg;	\
	DWORD _save_flags = _tls->source_flags;

#define restore_current_source_no_pos_tls() {						\
	_tls->source_file = _save_source;					\
	_tls->current_pkg = _save_pkg;						\
	_tls->source_flags = _save_flags;}

#define save_current_source_pos_tls()								\
	unsigned int  _save_pos = _tls->source_pos;	\
	unsigned int _save_line = _tls->source_line;	\
	DWORD _save_flags = _tls->source_flags;

#define restore_current_source_pos_tls() {							\
	_tls->source_pos = _save_pos;						\
	_tls->source_line = _save_line;					\
	_tls->source_flags = _save_flags;}

#define save_current_source_to_frame_tls() {								\
	Value** _frame = _tls->current_scan_frame;				\
	if (_frame) _frame[3] = _tls->source_file;				\
	if (_frame) _frame[4] = (Value*)_tls->source_pos;	\
	if (_frame) _frame[8] = (Value*)_tls->source_line;	\
	if (_frame) _frame[7] = (Value*)static_cast<DWORD_PTR>(_tls->source_flags);}

#define save_current_source_to_frame_no_pos_tls() {					\
	Value** _frame = _tls->current_scan_frame;			\
	if (_frame) _frame[3] = _tls->source_file;			\
	if (_frame) _frame[7] = (Value*)static_cast<DWORD_PTR>(_tls->source_flags);}

#define save_current_source_to_frame_pos_tls() {							\
	Value** _frame = _tls->current_scan_frame;				\
	if (_frame) _frame[4] = (Value*)_tls->source_pos;	\
	if (_frame) _frame[8] = (Value*)_tls->source_line;}

#define save_current_source_to_frame_no_file_tls() {					\
	Value** _frame = _tls->current_scan_frame;				\
	if (_frame) _frame[4] = (Value*)_tls->source_pos;	\
	if (_frame) _frame[8] = (Value*)_tls->source_line;}


/*! Class that uses the RAII idiom to push/pop the thread local current_controller. 
	This ensures that the thread local current_controller is always properly restored, even in the event of 
	an exception. 
*/
class ScopedPushControl : public MaxSDK::Util::Noncopyable 
{
	Control* m_save_cc;
	MAXScript_TLS* m_tls;
	bool m_ControlPopped;
public:
	ScripterExport ScopedPushControl(Control* c, MAXScript_TLS* tls = nullptr);
	ScripterExport ~ScopedPushControl();
	ScripterExport void PopControl();
};

//! \deprecated Deprecated as of 3ds Max 2017. Use ScopedPushControl instead. 
#pragma deprecated("push_control")
#pragma deprecated("pop_control")

// When an exception occurs, mxs stores data regarding source
// code location corresponding to the code generating the error.
// This data is used by the debugger and to display the source code
// in the editor
// Following function stores this data in the thread local variables
ScripterExport void save_error_source_data(MAXScript_TLS* tls = nullptr);

// The following function clears the data associated with errors.
// This function must be used if a catch "eats" an mxs runtime exception.
// A catch statement "eats" an exception if it does not either
// call show_source_pos() nor rethrow the exception.
ScripterExport void clear_error_source_data(MAXScript_TLS* tls = nullptr);
