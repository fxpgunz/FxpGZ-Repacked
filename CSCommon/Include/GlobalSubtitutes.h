#pragma once

#ifdef _TDD		// 모든 단위 테스트 관련 코드는 _TDD로 감싸도록 합니다

#include <map>

// 테스트 실행 전에 싱글턴과 전역변수를 테스트용 객체로 대체하기 쉽도록 여기에 등록하고,
// 테스트 후에는 대체된 객체들을 일괄적으로 제거할 수 있도록 작성합니다.

enum E_GLOBAL_INSTANCE
{
	GI_NpcMeshMgr,
	GI_EffectManager,
	GI_MatchItemDescMgr,
};

void ClearGlobalSub();

void* ZTddGetGlobalSub(E_GLOBAL_INSTANCE id);
void ZTddSetGlobalSub(E_GLOBAL_INSTANCE id, void* pSubtitute);
void ZTddRemoveGlobalSub(E_GLOBAL_INSTANCE id);


#endif