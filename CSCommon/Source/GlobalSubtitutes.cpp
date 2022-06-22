#include "stdafx.h"

#ifdef _TDD		// 모든 단위 테스트 관련 코드는 _TDD로 감싸도록 합니다
#include "GlobalSubtitutes.h"

typedef std::map<E_GLOBAL_INSTANCE, void*> MapGlobalSub;
static MapGlobalSub mapGlobalSub;


void ClearGlobalSub()
{
	// 대체용 객체들의 메모리 해제는 여기서 하지 않습니다. 생성한 곳에서 직접 해야합니다.

	mapGlobalSub.clear();
}


void* ZTddGetGlobalSub(E_GLOBAL_INSTANCE id)
{
	MapGlobalSub::iterator it = mapGlobalSub.find(id);
	if (it!=mapGlobalSub.end())
		return it->second;
	return NULL;
}

void ZTddSetGlobalSub(E_GLOBAL_INSTANCE id, void* pSubtitute)
{
	MapGlobalSub::iterator it = mapGlobalSub.find(id);
	if (it!=mapGlobalSub.end())
	{
		_ASSERT(0); // 이미 추가된 대체용 객체가 있습니다. 메모리 누수가 일어날 것입니다.
	}
	mapGlobalSub[id] = pSubtitute;
}

void ZTddRemoveGlobalSub(E_GLOBAL_INSTANCE id)
{
	mapGlobalSub[id] = NULL;
}

#endif
