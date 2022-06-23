//
// Copyright 2016 Autodesk, Inc.  All rights reserved. 
//
// This computer source code and related instructions and comments are the
// unpublished confidential and proprietary information of Autodesk, Inc. and
// are protected under applicable copyright and trade secret law.  They may
// not be disclosed to, copied or used by any third party without the prior
// written consent of Autodesk, Inc.
//
//

#pragma once
#include "../kernel/value.h"

/* ------------------- DataPair class instance ----------------------- */

applyable_class_debug_ok (DataPair)

class DataPair : public Value
{
public:
	Value*		v1;
	Value*		v2;
	Value*		v1_name;
	Value*		v2_name;

	DataPair();
	DataPair(Value* v1, Value* v2, Value* v1_name = NULL, Value* v2_name = NULL);

	classof_methods(DataPair, Value);
#	define	is_datapair(v) ((DbgVerify(!is_sourcepositionwrapper(v)), (v))->tag == class_tag(DataPair))
	static Value* make(Value** arg_list, int count);

	void		collect() { delete this; }
	void		sprin1(CharStream* s);
	void		gc_trace();

	// scene I/O 
	IOResult Save(ISave* isave);
	static Value* Load(ILoad* iload, USHORT chunkID, ValueLoader* vload);

	use_generic ( get_props,	"getPropNames");
	use_generic ( copy,			"copy");
	Value*	get_property(Value** arg_list, int count);
	Value*	set_property(Value** arg_list, int count);
};

