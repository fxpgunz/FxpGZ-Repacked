/*	dlxClass.h - Sample MAXScript SDK code for adding a new Value class
 *
 *  John Wainwright, 1998
 */
 
#include <maxscript/foundation/functions.h>
#include <maxscript/macros/generic_class.h>

void dlxclass_init();

DECLARE_LOCAL_GENERIC_CLASS(ListValue, ListGeneric);

local_applyable_class (ListValue)

class ListValue : public Value
{
public:
	Value*		head;	// head item
	ListValue*	tail;	// rest of list

				ListValue();
				ListValue(Value* head, ListValue* tail);

	ValueMetaClass* local_base_class() { return class_tag(ListValue); } // local base class in this class's plug-in
				classof_methods(ListValue, Value);
	void		collect() { delete this; }
	void		sprin1(CharStream* s);
	void		gc_trace();
#	define		is_list(p) ((p)->tag == class_tag(ListValue))
	BOOL		_is_collection() { return 1; }

	// collection mapping
	Value*		map(node_map& m);

	// internal methods
	virtual void append(Value* v);

	// operations 
#include <maxscript\macros\local_abstract_generic_functions.h>
#	include "setpro.h"

#include <maxscript\macros\local_implementations.h>
#	include "listpro.h"

	//properties
	def_property ( count );

	Value*	get_property(Value** arg_list, int count);
	Value*	set_property(Value** arg_list, int count);
};


local_applyable_class (SetValue)

class SetValue : public ListValue
{
public:
				SetValue();

				classof_methods(SetValue, ListValue);
	void		collect() { delete this; }
#	define		is_set(p) ((p)->tag == class_tag(SetValue))

	// internal methods
	void		append(Value* v);
	BOOL		is_member(Value* v);

	// operations 
#include <maxscript\macros\define_implementations.h>
#	include "setpro.h"
};


