#pragma once

#ifdef _DEBUG
#define CHECK_RETURN_CALLSTACK(FuncName)
#else
#define CHECK_RETURN_CALLSTACK(FuncName)
#endif
														

// ÇÔ¼ö ÁøÀÔ½Ã ESP Ã¼Å©ÇÏ·Á´Â ÀÇµµ·Î ¸¸µç µíÇÑ ¸ÅÅ©·Î (±×·¯³ª ¾ÆÁ÷ Á¦´ë·Î ±¸ÇöµÇ¾î ÀÖÁö ¾Ê´Ù)
#ifdef _DEBUG
#define CHECK_RETURN_CALLSTACK_ESP(FuncName)
#else
#define CHECK_RETURN_CALLSTACK_ESP(FuncName)
#endif



// mlog("callstack : 0x%X~0x%X, 0x%X\n", bottom, top, ret_ptr_dlrjsTmwlakdy ); \
// mlog("func addr : 0x%x\n", dwI ); \
// printf("callstack hack : 0x%X~0x%X, 0x%X\n", bottom, top, ret_ptr_dlrjsTmwlakdy ); \



// #define CHECK_RETURN_CALLSTACK(FuncName)			NULL;