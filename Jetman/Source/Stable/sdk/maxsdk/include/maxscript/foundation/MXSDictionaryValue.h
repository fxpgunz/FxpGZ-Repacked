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

applyable_class_debug_ok(MXSDictionaryValue)

class MXSDictionaryValue : public Value
{
public:
	enum key_type { key_name, key_string, key_int};

	ScripterExport MXSDictionaryValue(key_type keyType = key_name);
	ScripterExport ~MXSDictionaryValue() {}

	classof_methods(MXSDictionaryValue, Value);
	BOOL		_is_collection() { DbgAssert(!is_sourcepositionwrapper(this)); return 1; }
#	define	is_dictionary(v) ((DbgVerify(!is_sourcepositionwrapper(v)), (v))->tag == class_tag(MXSDictionaryValue))

	static Value* make(Value** arg_list, int count);

	void collect() { delete this; }
	void gc_trace();
	void sprin1(CharStream* s);

	// scene I/O 
	IOResult Save(ISave* isave);
	static Value* Load(ILoad* iload, USHORT chunkID, ValueLoader* vload);

	// hashtable hash and compare fns for string keys
	static DWORD_PTR string_hash_fn(const void* key);
	static int dict_eq_fn_string(const void* key1, const void* key2);

	// hashtable hash and compare fns for integer keys
	static DWORD_PTR int_hash_fn(const void* key);
	static int dict_eq_fn_int(const void* key1, const void* key2);

	ScripterExport void put(Value* key, Value* val, bool putOnlyIfNew = false);
	ScripterExport void set(Value* key, Value* val);
	ScripterExport Value* get(Value* key);
	ScripterExport void remove(Value* key);
	ScripterExport bool has(Value* key);
	ScripterExport Array* getKeys(); // The returned Array* is not held by the dictionary, and therefore must be protected from gc
	ScripterExport void free();

	ScripterExport key_type getType() { return mKeyType; }
	ScripterExport int getCount();
	ScripterExport const HashTable* getHashTable();

	ScripterExport bool keyValueProperType(Value* key);
	ScripterExport static bool keyValueProperType(Value* key, key_type keyType);

	// built-in property accessors 
	def_property(count);
	def_property(type);
	def_property(keys);

	// helper function for mxs exposure - throws error if key not of proper type
	void type_check_key(Value* key, const TCHAR* where);

	/* ---------------------- MXSDictionaryValue primitives  --------------------- */
#include "../macros/define_implementations.h"
#include "../protocols/MXSDictionaryValue.inl"
	use_generic(copy, "copy");
	use_generic(coerce, "coerce");
	use_generic(free, "free");

	ScripterExport Value* map(node_map& m);

	
protected:
	HashTable* mHashTable = nullptr;
	key_type mKeyType = key_name;
};
