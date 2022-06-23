#include "stdafx.h"
#include "../../sdk/UnitTest++/src/UnitTest++.h"
#include "MDuelTournamentMatchMaker.h"
#include "MMatchDuelTournamentMgr.h"

#ifdef _DEBUG

class MMockMatchObjectContainer : public MMatchObjectContainer 
{
public:
	std::set<MUID> m_setDisabledUid;

	// �׽�Ʈ���� uid ��ȿ�˻縦 ó���ϱ� ���� �������̵�
	virtual bool IsEnabledUid(const MUID& uid) {
		return m_setDisabledUid.find(uid) == m_setDisabledUid.end();
	}

} g_MockMatchObjectContainer;


TEST(DuelTournamentMatchMaker_AddRemovePlayer)
{
	MUID uidInvalid;
	uidInvalid.SetInvalid();

	MDuelTournamentMatchMaker matchMaker(&g_MockMatchObjectContainer);

	// ���� 3���� ��ʸ�Ʈ ��û�� �ߴ�
	CHECK_EQUAL(true, matchMaker.AddPlayer(MUID(0,1), 1000, 0));
	CHECK_EQUAL(true, matchMaker.AddPlayer(MUID(0,2), 1000, 0));
	CHECK_EQUAL(true, matchMaker.AddPlayer(MUID(0,3),  998, 0));
	// 3���� ����־�� �Ѵ�
	CHECK_EQUAL(3, matchMaker.GetNumPlayer());

	// �̹� �߰��� ������ �ߺ� �߰��Ǵ� ��� -> �����ؾ��Ѵ�
	CHECK_EQUAL(false, matchMaker.AddPlayer(MUID(0,1), 1000, 0));
	CHECK_EQUAL(false, matchMaker.AddPlayer(MUID(0,1), 1111, 0));

	// ��ȿ���� ���� uid�� �߰����� �� -> �����ؾ��Ѵ�
	CHECK_EQUAL(false, matchMaker.AddPlayer(uidInvalid, 1000, 0));

	// ������ �����Ѵ�
	CHECK_EQUAL(true, matchMaker.RemovePlayer(MUID(0,1)));
	// 2���� ����־�� �Ѵ�
	CHECK_EQUAL(2, matchMaker.GetNumPlayer());

	// ���� ������ �����Ϸ� �Ѵ� -> �����ؾ��Ѵ�
	CHECK_EQUAL(false, matchMaker.RemovePlayer(MUID(0,1)));
	// ��ȿ���� ���� uid�� �ְ� ���Ž�Ų�� -> �����ؾ��Ѵ�
	CHECK_EQUAL(false, matchMaker.RemovePlayer(uidInvalid));

	// ���� ����
	matchMaker.RemoveAll();
	CHECK_EQUAL(0, matchMaker.GetNumPlayer());
}

// TP�� ����� �������� �׷��� ���� �׽�Ʈ
TEST(DuelTournamentMatchMaker_PickMatchableGroups)
{
	MMockDTMatchMaker matchMaker(&g_MockMatchObjectContainer);
	vector<MMockDTMatchMaker::PairDTMatch> vecMatchGroup;

	// 1�� �ְ� 2���� �䱸�غ���
	CHECK_EQUAL(true, matchMaker.AddPlayer(MUID(0,1), 1000, 0));
	matchMaker.PickMatchableGroups(vecMatchGroup, 2, 10);
	CHECK_EQUAL(true, vecMatchGroup.empty());

	// TP�� ���� 2���� �ְ� 2�� ��ġ
	matchMaker.RemoveAll();
	CHECK_EQUAL(true, matchMaker.AddPlayer(MUID(0,1), 1000, 0));
	CHECK_EQUAL(true, matchMaker.AddPlayer(MUID(0,2), 1000, 0));
	matchMaker.PickMatchableGroups(vecMatchGroup, 2, 10);
	CHECK_EQUAL(1, (int)vecMatchGroup.size());
	CHECK(MUID(0,1) == vecMatchGroup[0].first.uid);
	CHECK(MUID(0,2) == vecMatchGroup[0].second.uid);

	// tpGap�� 0�϶� (�Ѵ� TP�� �����Ƿ� ��ġ�� �����ؾ� �Ѵ�)
	matchMaker.PickMatchableGroups(vecMatchGroup, 2, 0);
	CHECK_EQUAL(1, (int)vecMatchGroup.size());

	// tpGap�� �����϶�
	matchMaker.PickMatchableGroups(vecMatchGroup, 2, -1);
	CHECK_EQUAL(true, vecMatchGroup.empty());

	// ��ġ�ο��� 1�����϶�
	matchMaker.PickMatchableGroups(vecMatchGroup, 1, 10);
	CHECK_EQUAL(true, vecMatchGroup.empty());
	matchMaker.PickMatchableGroups(vecMatchGroup, 0, 10);
	CHECK_EQUAL(true, vecMatchGroup.empty());
	matchMaker.PickMatchableGroups(vecMatchGroup, -1, 10);
	CHECK_EQUAL(true, vecMatchGroup.empty());

	// 5���� �ְ� 2�� ��ġ
	matchMaker.RemoveAll();
	CHECK_EQUAL(true, matchMaker.AddPlayer(MUID(0,1), 940, 0));
	CHECK_EQUAL(true, matchMaker.AddPlayer(MUID(0,2), 1000, 0));
	CHECK_EQUAL(true, matchMaker.AddPlayer(MUID(0,3), 1001, 0));
	CHECK_EQUAL(true, matchMaker.AddPlayer(MUID(0,4), 1002, 0));
	CHECK_EQUAL(true, matchMaker.AddPlayer(MUID(0,5), 1003, 0));
	matchMaker.PickMatchableGroups(vecMatchGroup, 2, 10);
	CHECK_EQUAL(2, (int)vecMatchGroup.size());
	// MUID(0,1)�� ���Եȴ�
	CHECK(MUID(0,2) == vecMatchGroup[0].first.uid);
	CHECK(MUID(0,3) == vecMatchGroup[0].second.uid);
	CHECK(MUID(0,4) == vecMatchGroup[1].first.uid);
	CHECK(MUID(0,5) == vecMatchGroup[1].second.uid);
	// 3�� ��ġ
	matchMaker.PickMatchableGroups(vecMatchGroup, 3, 10);
	CHECK_EQUAL(1, (int)vecMatchGroup.size());
	// MUID(0,1), MUID(0,5)�� ���´�
	CHECK(MUID(0,2) == vecMatchGroup[0].first.uid);
	CHECK(MUID(0,4) == vecMatchGroup[0].second.uid);
}

// Ư�� �÷��̾ �����ϸ鼭 TP�� ���� ������ ���������� �׷� ���� �׽�Ʈ
TEST(DuelTournamentMatchMaker_PickGroupForPlayer)
{
	MMockDTMatchMaker::PairDTMatch matchGroup;

	MMockDTMatchMaker matchMaker(&g_MockMatchObjectContainer);
	CHECK_EQUAL(true, matchMaker.AddPlayer(MUID(0,1), 1000, 0));
	CHECK_EQUAL(true, matchMaker.AddPlayer(MUID(0,2), 1100, 0));
	CHECK_EQUAL(true, matchMaker.AddPlayer(MUID(0,3), 1260, 0));
	CHECK_EQUAL(true, matchMaker.AddPlayer(MUID(0,4), 1300, 0));
	// (0,1)�� �׷����� �� �ִ� ������ (0,2) �ۿ� ����
	CHECK_EQUAL(true, matchMaker.PickGroupForPlayer(matchGroup, 2, MMockDTMatchMaker::DTUser(MUID(0,1), 1000) ));
	CHECK(MUID(0,1) == matchGroup.first.uid);
	CHECK(MUID(0,2) == matchGroup.second.uid);
	// (0,2)�� (0,3)���� (0,1)�� �� ������.
	CHECK_EQUAL(true, matchMaker.PickGroupForPlayer(matchGroup, 2, MMockDTMatchMaker::DTUser(MUID(0,2), 1100) ));
	CHECK(MUID(0,1) == matchGroup.first.uid);
	CHECK(MUID(0,2) == matchGroup.second.uid);
	CHECK_EQUAL(true, matchMaker.PickGroupForPlayer(matchGroup, 2, MMockDTMatchMaker::DTUser(MUID(0,3), 1260) ));
	CHECK(MUID(0,3) == matchGroup.first.uid);
	CHECK(MUID(0,4) == matchGroup.second.uid);
	CHECK_EQUAL(true, matchMaker.PickGroupForPlayer(matchGroup, 2, MMockDTMatchMaker::DTUser(MUID(0,4), 1300) ));
	CHECK(MUID(0,3) == matchGroup.first.uid);
	CHECK(MUID(0,4) == matchGroup.second.uid);

	// 3�� ��ġ
	CHECK_EQUAL(true, matchMaker.PickGroupForPlayer(matchGroup, 3, MMockDTMatchMaker::DTUser(MUID(0,2), 1100) ));
	CHECK(MUID(0,2) == matchGroup.first.uid);		// (0,1)~(0,3) ���� �� �׷��� TP���̰� ����
	CHECK(MUID(0,4) == matchGroup.second.uid);

	// 4�� ��ġ
	CHECK_EQUAL(true, matchMaker.PickGroupForPlayer(matchGroup, 4, MMockDTMatchMaker::DTUser(MUID(0,1), 1000) ));
	CHECK(MUID(0,1) == matchGroup.first.uid);
	CHECK(MUID(0,4) == matchGroup.second.uid);
	CHECK_EQUAL(true, matchMaker.PickGroupForPlayer(matchGroup, 4, MMockDTMatchMaker::DTUser(MUID(0,4), 1300) ));
	CHECK(MUID(0,1) == matchGroup.first.uid);
	CHECK(MUID(0,4) == matchGroup.second.uid);

	// ���� �ִ� �������� ���� �ο��� ��ġ�õ�
	CHECK_EQUAL(false, matchMaker.PickGroupForPlayer(matchGroup, 5, MMockDTMatchMaker::DTUser(MUID(0,1), 1000) ));

	// ��ġ �ο��� 1���Ϸ� �־�����
	CHECK_EQUAL(false, matchMaker.PickGroupForPlayer(matchGroup, 1, MMockDTMatchMaker::DTUser(MUID(0,1), 1000) ));
	CHECK_EQUAL(false, matchMaker.PickGroupForPlayer(matchGroup, 0, MMockDTMatchMaker::DTUser(MUID(0,1), 1000) ));
	CHECK_EQUAL(false, matchMaker.PickGroupForPlayer(matchGroup, -1, MMockDTMatchMaker::DTUser(MUID(0,1), 1000) ));

	// ���� ����� �������� �־�����
	CHECK_EQUAL(false, matchMaker.PickGroupForPlayer(matchGroup, 2, MMockDTMatchMaker::DTUser(MUID(0,1), 4444) ));
	CHECK_EQUAL(false, matchMaker.PickGroupForPlayer(matchGroup, 2, MMockDTMatchMaker::DTUser(MUID(1,1), 1000) ));
}

// Ư�� �ð����� ���� ��ٸ� ������ ã�Ƴ��� �׽�Ʈ
TEST(DuelTournamentMatchMaker_FindLongWaitPlayer)
{
	MMockDTMatchMaker matchMaker(&g_MockMatchObjectContainer);
	CHECK_EQUAL(true, matchMaker.AddPlayer(MUID(0,1), 1000, 1500));
	CHECK_EQUAL(true, matchMaker.AddPlayer(MUID(0,2), 1100, 1700));
	CHECK_EQUAL(true, matchMaker.AddPlayer(MUID(0,3), 1260, 1300));
	CHECK_EQUAL(true, matchMaker.AddPlayer(MUID(0,4), 1300, 1900));
	const MMockDTMatchMaker::DTUser* pDTUser;
	pDTUser = matchMaker.FindLongWaitPlayer(690, 2000);
	CHECK(MUID(0,3) == pDTUser->uid);
}

// �������� ���� �����ϴ� ���(��Ī�׷��� ����� ����� ��Ͽ��� ���� ���� ���) �׽�Ʈ
TEST(DuelTournamentMatchMaker_RemovePlayers)
{
	MMockDTMatchMaker matchMaker(&g_MockMatchObjectContainer);
	vector<MMockDTMatchMaker::PairDTMatch> vecMatchGroup;

	CHECK_EQUAL(true, matchMaker.AddPlayer(MUID(0,1), 1400, 0));
	CHECK_EQUAL(true, matchMaker.AddPlayer(MUID(0,2), 1001, 0));	//1
	CHECK_EQUAL(true, matchMaker.AddPlayer(MUID(0,3), 1002, 0));	//1
	CHECK_EQUAL(true, matchMaker.AddPlayer(MUID(0,4), 1552, 0));	//	3
	CHECK_EQUAL(true, matchMaker.AddPlayer(MUID(0,5), 1551, 0));	//  3
	CHECK_EQUAL(true, matchMaker.AddPlayer(MUID(0,6), 1551, 0));	//  3
	CHECK_EQUAL(true, matchMaker.AddPlayer(MUID(0,7), 1553, 0));	
	CHECK_EQUAL(true, matchMaker.AddPlayer(MUID(0,8), 1333, 0));	// 2
	CHECK_EQUAL(true, matchMaker.AddPlayer(MUID(0,9), 1003, 0));	//1
	CHECK_EQUAL(true, matchMaker.AddPlayer(MUID(0,10), 1334, 0));	// 2
	CHECK_EQUAL(true, matchMaker.AddPlayer(MUID(0,11), 800, 0));
	CHECK_EQUAL(true, matchMaker.AddPlayer(MUID(0,12), 1324, 0));	// 2
	matchMaker.PickMatchableGroups(vecMatchGroup, 3, 10);
	CHECK_EQUAL(3, (int)vecMatchGroup.size());
	for (int i=0; i<(int)vecMatchGroup.size(); ++i)
		matchMaker.RemovePlayers(vecMatchGroup[i].first, vecMatchGroup[i].second);
	CHECK_EQUAL(3, matchMaker.GetNumPlayer());
	CHECK(matchMaker.m_mapUser.end() != matchMaker.m_mapUser.find(MDuelTournamentMatchMaker::DTUser(MUID(0,1), 1400)));
	CHECK(matchMaker.m_mapUser.end() != matchMaker.m_mapUser.find(MDuelTournamentMatchMaker::DTUser(MUID(0,7), 1553)));
	CHECK(matchMaker.m_mapUser.end() != matchMaker.m_mapUser.find(MDuelTournamentMatchMaker::DTUser(MUID(0,11), 800)));
}

// (�������� ������)��ȿ�� �� uid�� ����� ��Ͽ��� �����ϴ� ��� �׽�Ʈ
TEST(DuelTournamentMatchMaker_CleanDisabledUid)
{
	MMockDTMatchMaker matchMaker(&g_MockMatchObjectContainer);
	CHECK_EQUAL(true, matchMaker.AddPlayer(MUID(0,1), 1000, 0));
	CHECK_EQUAL(true, matchMaker.AddPlayer(MUID(0,2), 1000, 0));
	CHECK_EQUAL(true, matchMaker.AddPlayer(MUID(0,3), 1000, 0));
	CHECK_EQUAL(true, matchMaker.AddPlayer(MUID(0,4), 1000, 0));
	// ���� ��ȿ�� �� uid�� (0,2) (0,4) ��� ����
	g_MockMatchObjectContainer.m_setDisabledUid.insert(MUID(0,2));
	g_MockMatchObjectContainer.m_setDisabledUid.insert(MUID(0,4));
	
	matchMaker.CleanDisabledUid();
	// ������ 2�� ���̰�, ��ȿ�� uid�� ���� ������
	CHECK_EQUAL(2, matchMaker.GetNumPlayer());
	CHECK(matchMaker.m_mapUser.end() != matchMaker.m_mapUser.find(MDuelTournamentMatchMaker::DTUser(MUID(0,1), 1000)));
	CHECK(matchMaker.m_mapUser.end() != matchMaker.m_mapUser.find(MDuelTournamentMatchMaker::DTUser(MUID(0,3), 1000)));
}


///////////////////////////////////////////////////////////////////////////////////////////////
//			MDuelTournamentMatchLauncher ��ġ��ó�� ���� �׽�Ʈ

struct MOCKUSERDATA 
{
	int tp;
	bool bMatchWait;
};

typedef map<MUID, MOCKUSERDATA> MapMockUser;
typedef MapMockUser::iterator	ItorMockUser;
static ItorMockUser PickRandomly(MapMockUser& m)
{
	int size = (int)m.size();
	int r = rand() % size;
	ItorMockUser it = m.begin();
	for (int i=0; i<r; ++i) ++it;
	return it;
}

struct _functorIntLess {
	bool operator() (int i,int j) { return (i<j);}
} functorIntLess;


// �׽�Ʈ�� ���� ������ ��⸦ ��Ī���� ���� ��� �α׸� ���⵵�� ����� Ŭ����
class MMockDuelTournamentMatchLauncher : public MDuelTournamentMatchLauncher
{
public:
	MapMockUser* pMapMockUser;
	MMockDuelTournamentMatchLauncher(MapMockUser* m) : pMapMockUser(m) {}

	struct MatchResult
	{
		std::vector<int> vecGroupTP;
		bool bOverWaitTimeLimitGroup;		// ���ð� �ʰ��� ������� �׷��ΰ�
		void Sort() { std::sort(vecGroupTP.begin(), vecGroupTP.end(), functorIntLess); }
		int GetSmallestTP() { _ASSERT(!vecGroupTP.empty()); return vecGroupTP[0]; }
		int GetBiggestTP()  { _ASSERT(!vecGroupTP.empty()); return *(--vecGroupTP.end()); }
		int GetTpGap() { return GetBiggestTP() - GetSmallestTP(); }
	};
	std::vector<MatchResult> m_vecResult;

	MatchResult m_biggestTpGapResult;

	virtual void LaunchMatch(MDUELTOURNAMENTTYPE nType, MDuelTournamentPickedGroup* pPickedGroup, MDUELTOURNAMENTMATCHMAKINGFACTOR matchFactor) {
		int size = (int)pPickedGroup->size();
		m_vecResult.clear();
		MatchResult result;
		result.bOverWaitTimeLimitGroup = (matchFactor==MDUELTOURNAMENTMATCHMAKINGFACTOR_OVERWAIT ? true : false);
		for (int i=0; i<size; ++i) {
			ItorMockUser it = pMapMockUser->find((*pPickedGroup)[i]);
			result.vecGroupTP.push_back( it->second.tp);
		}
		result.Sort();
		m_vecResult.push_back(result);

		if (m_biggestTpGapResult.vecGroupTP.empty())
			m_biggestTpGapResult = result;
		else if (m_biggestTpGapResult.GetTpGap() < result.GetTpGap())
			m_biggestTpGapResult = result;

		LogMatch(result);
	}

	void LogMatch(MatchResult& result)
	{
		return; // �α������ ���������� �ʹ� ������
		if (result.bOverWaitTimeLimitGroup)
			mlog("TIMEOVER: ");
		else 
			mlog("TP      : ");
		int size = (int)result.vecGroupTP.size();
		for (int i=0; i<size; ++i) {
			mlog("%d ", result.vecGroupTP[i]);
		}
		mlog("\n");
	}
};

// ��ü����� �׽�Ʈ�غ���
TEST(DuelTournamentMatchLauncher_FullFunctionalityTest)
{
	srand(timeGetTime());
	// �����ϰ� ����Ǯ�� �����Ѵ�
	std::map<MUID, MOCKUSERDATA> mapUser;
	for (int i=0; i<500; ++i) {
		MUID uid(0,i+1);
		MOCKUSERDATA userdata;
		userdata.tp = rand()%500 + 1000;
		userdata.bMatchWait = false;
		mapUser.insert(pair<MUID, MOCKUSERDATA>(uid, userdata));
	}

	const ACCEPTABLE_TPGAP = 15;

	MMockDTMatchMaker matchMaker(&g_MockMatchObjectContainer);
	MMockDuelTournamentMatchLauncher launcher(&mapUser);
	launcher.SetLimitUserWaitTime(10000);
	launcher.SetAcceptableTpGap(ACCEPTABLE_TPGAP);

	// [��ġ��û=>�׷���=>��ġ����˻�]�� ����� Ƚ���� �ݺ��غ���
	DWORD curTime = 0;
	for (int i=0; i<100; ++i)
	{
		curTime += 500;

		// �Ϻθ� ������Ų��
		int numJoin = rand() % 50;
		for (int k=0; k<numJoin; ++k) {
			ItorMockUser it = PickRandomly(mapUser);
			if (it->second.bMatchWait) continue;
			matchMaker.AddPlayer(it->first, it->second.tp, curTime++);
		}
        
		// ��Ī��Ų��
		if (i%5 == 0) {	// ���� 5���� �ѹ� ��Ī����
			launcher.LaunchAvailableMatch(MDUELTOURNAMENTTYPE_QUATERFINAL, matchMaker, curTime);

			int size = (int)launcher.m_vecResult.size();
			for (int k=0; k<size; ++k)
			{
				CHECK_EQUAL(GetDTPlayerCount(MDUELTOURNAMENTTYPE_QUATERFINAL), (int)launcher.m_vecResult[k].vecGroupTP.size());
				
				if (false == launcher.m_vecResult[k].bOverWaitTimeLimitGroup)
				{
					int smallestTP = launcher.m_vecResult[k].GetSmallestTP();
					int biggestTP  = launcher.m_vecResult[k].GetBiggestTP();
					int tpGap = biggestTP - smallestTP;
					CHECK(0 < tpGap && tpGap <= ACCEPTABLE_TPGAP);
				}

				// ���ð� ������ �׷������� ���� �׷��� �������� Ȯ���� ����� ����.. -_-
			}
		}
	}

	mlog("\n<the biggest tp gap match>\n");
	launcher.LogMatch(launcher.m_biggestTpGapResult);
}

// CleanDisabledUid�� �ϴ���, ��Ī�Ǵ� ������ ���������ų� �ϴ� ������ .. ��� ó���� ���ΰ� ? �������� �Ǵ� ���ΰ�. Ȯ���غ���
// ���� �ϳ� ������ �������� ������ ���°�? �ڵ带 �ٽ� �ѹ� ����
// �Ǿհ� �ǵڰ� �ҿܴ��ؼ� �������� ū ������ �ٰ� ���� ������ Ȯ���غ���
// ���ݱ��� ����ӵ��� ������� �ʰ� �׽�Ʈ�� ����ϴµ��� �����ߴ�. �̰��� ������ �Ϻζ�� ���ǿ� �ɸ´� �������� �����ؾ��Ѵ�.

#endif //_DEBUG
