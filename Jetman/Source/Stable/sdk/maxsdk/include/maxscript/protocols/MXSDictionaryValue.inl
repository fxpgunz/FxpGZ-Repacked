/*	
 *		see def_abstract_generics.h for more info.
 */
#pragma warning(push)
#pragma warning(disable:4100)

	use_generic(get, "get");
	use_generic(put, "put");

	def_visible_primitive (GetDictValue, "GetDictValue");
	def_visible_primitive (PutDictValue, "PutDictValue");
	def_visible_primitive (SetDictValue, "SetDictValue");
	def_visible_primitive (RemoveDictValue, "RemoveDictValue");
	def_visible_primitive (HasDictValue, "HasDictValue");
	def_visible_primitive (GetDictKeys, "GetDictKeys");
	def_visible_primitive (GetDictKeyType, "GetDictKeyType");
#pragma warning(pop)
