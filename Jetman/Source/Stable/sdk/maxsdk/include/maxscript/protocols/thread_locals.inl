/*	
 *		thread_locals.h - thread locals for each interpreter thread in MAXScript
 *
 *			Copyright © John Wainwright 1996
 *
 */

/* thread locals and initial values */
#pragma warning(push)
#pragma warning(disable:4100)

// This file describes the thread local variables. This file is included in various contexts where the definition of 'def_thread_local'
// is context specific. For example, in one place type, 'def_thread_local' is used to declare the thread local variable, in another it
// is used to initialize the thread local variable, in another it is used during garbage collection to mark Value* derived values
// as being in use.

// Column 1 is the type of the thread local variable
// Column 2 is the thread local variable's name
// Column 3 is whether the type derives from Value*, and the contents of the thread local variable should be protected against garbage collection
// Column 4 is whether to re-initialize the thread local variable on a reset_thread_locals
// Column 5 is the value used to initialize the thread local variable

	def_thread_local( BOOL,											seh_set,						FALSE,	FALSE,	FALSE);			// whether the structured exception handler has been set for the thread
	def_thread_local( NewCollectableLinkedListItem*,		last_new_collectable,	FALSE,	FALSE,	new NewCollectableLinkedListItem);	// linked list of Collectables created via new before initialized as a Value
	def_thread_local( CharStream*,								current_stdout,			TRUE,		TRUE,		new (GC_IN_HEAP) WindowStream(_M("Script Output")));
	def_thread_local( String*,										undo_label,					TRUE,		TRUE,		new (GC_IN_HEAP) String(_M("MAXScript"))); // current undo label
	def_thread_local( BOOL,											force_listener_open,		FALSE,	TRUE,		TRUE);			// whether to force listener open on output to it

	def_thread_local( Value**,										current_frame,				FALSE,	TRUE,		NULL);			// current interpreter frame (for thunk evals)
	def_thread_local( Value**,										current_scan_frame,		FALSE,	TRUE,		NULL);			// current interpreter frame (for gc scanner) 
	def_thread_local( Value**,										current_locals_frame,	FALSE,	TRUE,		NULL);			// C++ local frame
	def_thread_local( Value*,										current_result,			TRUE,		TRUE,		NULL);			// C++ current Value* function result
	def_thread_local( Value*,										last_new_heap_value,		TRUE,		TRUE,		NULL);			// last non-GC_PERMANENT heap value created
	def_thread_local( long,											stack_limit,				FALSE,	TRUE,		ALLOCATOR_STACK_SIZE);	// max stack size to catch recurse loops, 1Mb to start, 
																																						// minus buffer to handle calls made during error handling
	def_thread_local( LONG_PTR,									stack_base,					FALSE,	TRUE,		(LONG_PTR)_alloca(sizeof(int)));	// current stack base
	def_thread_local( MSPlugin*,									current_plugin,			TRUE,		TRUE,		NULL);			// current scripted plugin (for plugin thunk evals)
	def_thread_local( Struct*,										current_struct,			TRUE,		TRUE,		NULL);			// current struct (for struct member thunk evals)
	def_thread_local( Value*,										current_container,		TRUE,		TRUE,		NULL);			// current container for nested property access
	def_thread_local( int,											container_index,			FALSE,	TRUE,		0);				// current container index (if any)
	def_thread_local( Value*,										container_prop,			TRUE,		TRUE,		NULL);			// current container prop (if any)
	def_thread_local( Value*,										current_prop,				TRUE,		TRUE,		NULL);			// most recent prop access (if any)

	def_thread_local( Value*,										source_file,				TRUE,		TRUE,		NULL);			// current source file
	def_thread_local( UINT_PTR,									source_pos,					FALSE,	TRUE,		0);				// current pos in source file
	def_thread_local( UINT_PTR,									source_line,				FALSE,	TRUE,		0);				// current pos in source file, instead of INT_PTR, will have an int to simplify things

	def_thread_local( int,											needs_redraw,				FALSE,	TRUE,		0);				// 0- no redraw needed, 1 - redraw needed, 2 - complete redraw needed
	def_thread_local( BOOL,											redraw_mode,				FALSE,	TRUE,		1);				// redraw on
	def_thread_local( BOOL,											pivot_mode,					FALSE,	TRUE,		0);				// pivot off
	def_thread_local( BOOL,											undo_mode,					FALSE,	TRUE,		1);				// undo on
	def_thread_local( Value*,										current_level,				TRUE,		TRUE,		&all_objects);	// $objects
	def_thread_local( BOOL,											use_time_context,			FALSE,	TRUE,		0);				// use MAX time slider
	def_thread_local( TimeValue,									current_time,				FALSE,	TRUE,		0);
	def_thread_local( Value*,										current_coordsys,			TRUE,		TRUE,		n_default);
	def_thread_local( Value*,										center_mode,				TRUE,		TRUE,		n_default);

	def_thread_local( int,											rand_accum,					FALSE,	FALSE,	0);				// for our own rand()
	def_thread_local( HANDLE,										message_event,				FALSE,	TRUE,		NULL);			// listener_message synch event
	def_thread_local( int,											stream_rand_accum,		FALSE,	TRUE,		0);				// for stream_rand()

	def_thread_local( MSZipPackage*,								current_pkg,				TRUE,		TRUE,		NULL);			// currently open zip package, if any

	def_thread_local( void*,										alloc_frame,				FALSE,	FALSE,	NULL);			// top frame of allocator stack
	def_thread_local( void*,										alloc_tos,					FALSE,	FALSE,	NULL);			// top of allocator stack
	def_thread_local( void*,										alloc_stack_lim,			FALSE,	FALSE,	NULL);			// limit of allocator stack

	def_thread_local( Control*,									current_controller,		FALSE,	TRUE,		NULL);			// currently evaluating scripted controller

	def_thread_local( BOOL,											try_mode,					FALSE,	TRUE,		0);				// try(...)
	def_thread_local( MAXScriptException*,						current_exception,		FALSE,	TRUE,		NULL);			// current exception that was thrown, if any. Non-null only in catch expression
	def_thread_local( UnknownSystemException*,				current_UnknownSystemException,	TRUE,		FALSE,	NULL);		// current UnknownSystemException that was thrown, if any. 

	def_thread_local( BOOL,											thread_not_active,		FALSE,	TRUE,		FALSE);			// set to TRUE when thread found not to be active any more

	def_thread_local( BOOL,											err_occurred,				FALSE,	TRUE,		FALSE);			// set to TRUE when error thrown
	def_thread_local( Value*,										err_source_file,			TRUE,		TRUE,		NULL);			// source file where error thrown
	def_thread_local( UINT_PTR,									err_source_pos,			FALSE,	TRUE,		0);				// pos in source file where error thrown
	def_thread_local( UINT_PTR,									err_source_line,			FALSE,	TRUE,		0);				// line in source file where error thrown

	def_thread_local( Value*,										current_frame_owner,		TRUE,		TRUE,		NULL);			// owner of current stack frame while compiling
	
	def_thread_local( BOOL,											is_dubugger_thread,		FALSE,	TRUE,		FALSE);			// owner of current stack frame while compiling

	def_thread_local( DWORD,										source_flags,				FALSE,	TRUE,		0);				// current source file flags
	def_thread_local( DWORD,										err_source_flags,			FALSE,	TRUE,		0);				// source file flags for source file where error thrown

	def_thread_local( BOOL,											disable_trace_back,		FALSE,	TRUE,		FALSE);			// true if no stack traceback is to occur if an error occurs

	def_thread_local( StringStream*,								resource_value_reader,	TRUE,		TRUE,		NULL);			// Stringstream used to prevalidate resource Values read from resource files

#pragma warning(pop)
