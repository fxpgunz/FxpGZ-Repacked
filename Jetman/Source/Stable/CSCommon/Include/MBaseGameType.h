#ifndef _MBASEGAMETYPE_H
#define _MBASEGAMETYPE_H

#include <crtdbg.h>
#include <map>
#include <set>
using namespace std;


/// ���� Ÿ��
enum MMATCH_GAMETYPE {
	MMATCH_GAMETYPE_DEATHMATCH_SOLO		=0,			///< ���� ������ġ
	MMATCH_GAMETYPE_DEATHMATCH_TEAM		=1,			///< �� ������ġ
	MMATCH_GAMETYPE_GLADIATOR_SOLO		=2,			///< ���� �۷�������
	MMATCH_GAMETYPE_GLADIATOR_TEAM		=3,			///< �� �۷�������
	MMATCH_GAMETYPE_ASSASSINATE			=4,			///< ������(�ϻ���)
	MMATCH_GAMETYPE_TRAINING			=5,			///< ����

	MMATCH_GAMETYPE_SURVIVAL			=6,			///< �����̹�
	MMATCH_GAMETYPE_QUEST				=7,			///< ����Ʈ

	MMATCH_GAMETYPE_BERSERKER			=8,			///< ������ġ ����Ŀ
	MMATCH_GAMETYPE_DEATHMATCH_TEAM2	=9,			///< ��������ġ �ͽ�Ʈ��
	MMATCH_GAMETYPE_DUEL				=10,		///< ��� ��ġ
	MMATCH_GAMETYPE_DUELTOURNAMENT		=11,		///< ��� ��ʸ�Ʈ
	MMATCH_GAMETYPE_CTF					=12,		///< ��� ��ʸ�Ʈ
	/*MMATCH_GAMETYPE_CLASSIC_SOLO,
	MMATCH_GAMETYPE_CLASSIC_TEAM,*/
	MMATCH_GAMETYPE_GUNGAME,
	MMATCH_GAMETYPE_SPYMODE,
	MMATCH_GAMETYPE_QUEST_CHALLENGE,
	MMATCH_GAMETYPE_INFECTION,
	MMATCH_GAMETYPE_BLITZKRIEG,
	MMATCH_GAMETYPE_SKILLMAP,
	MMATCH_GAMETYPE_DEMOLITION,

	MMATCH_GAMETYPE_MAX,
};
// ��尡 �߰��Ǹ� ���÷��� ����� ��� �� �߰��ؾ� �Ѵ� (cpp)
extern const char* MMatchGameTypeAcronym[MMATCH_GAMETYPE_MAX];

#define		MAX_RELAYMAP_LIST_COUNT			20
struct RelayMap
{
	int				nMapID;
};

enum RELAY_MAP_TYPE
{
	RELAY_MAP_TURN			= 0,
	RELAY_MAP_RANDOM,

	RELAY_MAP_MAX_TYPE_COUNT
};

enum RELAY_MAP_REPEAT_COUNT
{
	RELAY_MAP_1REPEAT			= 0,
	RELAY_MAP_2REPEAT,
	RELAY_MAP_3REPEAT,
	RELAY_MAP_4REPEAT,
	RELAY_MAP_5REPEAT,

	RELAYMAP_MAX_REPEAT_COUNT,
};

// ����Ʈ ���� Ÿ��
const MMATCH_GAMETYPE MMATCH_GAMETYPE_DEFAULT = MMATCH_GAMETYPE_DEATHMATCH_SOLO;
//const MMATCH_GAMETYPE MMATCH_GAMETYPE_DEFAULT = MMATCH_GAMETYPE_DEATHMATCH_TEAM2;


struct MMatchGameTypeInfo
{
	MMATCH_GAMETYPE		nGameTypeID;			// ID
	char				szGameTypeStr[32];		// ���� Ÿ�� �̸�
	float				fGameExpRatio;			// ����ġ ��� ���� - ���� Gladiator�� 50%�̴�
	float				fTeamMyExpRatio;		// �������� ���ο��� ����Ǵ� ����ġ
	float				fTeamBonusExpRatio;		// �������� ������ �����Ǵ� ����ġ
	set<int>			MapSet;					// �� ����Ÿ�Կ��� �÷��� ������ ��
	void Set(const MMATCH_GAMETYPE a_nGameTypeID, const char* a_szGameTypeStr, const float a_fGameExpRatio,
		     const float a_fTeamMyExpRatio, const float a_fTeamBonusExpRatio);
	void AddMap(int nMapID);
	void AddAllMap();
};


class MBaseGameTypeCatalogue
{
private:
	MMatchGameTypeInfo			m_GameTypeInfo[MMATCH_GAMETYPE_MAX];
public:
	MBaseGameTypeCatalogue();
	virtual ~MBaseGameTypeCatalogue();

	inline MMatchGameTypeInfo* GetInfo(MMATCH_GAMETYPE nGameType);
	inline const char* GetGameTypeStr(MMATCH_GAMETYPE nGameType);
	inline void SetGameTypeStr(MMATCH_GAMETYPE nGameType, const char* szName);
	inline bool IsCorrectGameType(const int nGameTypeID);	///< �ùٸ� ���� Ÿ�� ID���� ����
	inline bool IsTeamGame(MMATCH_GAMETYPE nGameType);		///< �ش� ����Ÿ���� �������� ����
	inline bool IsTeamExtremeGame(MMATCH_GAMETYPE nGameType);		///< �ش� ����Ÿ���� �������� ����
	inline bool IsTeamLimitTime(MMATCH_GAMETYPE nGameType);
	inline bool IsWaitForRoundEnd(MMATCH_GAMETYPE nGameType);		///< ���� ���������� ����� �ؾ��ϴ��� ����
	inline bool IsQuestOnly(MMATCH_GAMETYPE nGameType);	///< �ش� ����Ÿ���� ����Ʈ���� ����
	inline bool IsSurvivalOnly(MMATCH_GAMETYPE nGameType); ///< �ش� ����Ÿ���� �����̹����� ����
	inline bool IsQuestDerived(MMATCH_GAMETYPE nGameType);	///< �ش� ����Ÿ���� ����Ʈ ����(NPC�� ��������)���� ����
	inline bool IsNewQuestDerived(MMATCH_GAMETYPE nGameType);
	inline bool IsWorldItemSpawnEnable(MMATCH_GAMETYPE nGameType);	///< �ش� ����Ÿ�Կ��� ����������� �����Ǵ��� ����
};


//////////////////////////////////////////////////////////////////////////////////
inline bool MBaseGameTypeCatalogue::IsTeamGame(MMATCH_GAMETYPE nGameType)
{
	// ���� ���������� Description�� teamgame���� ���ΰ��� �ֵ��� ����.

	if ((nGameType == MMATCH_GAMETYPE_DEATHMATCH_TEAM) ||
		(nGameType == MMATCH_GAMETYPE_DEATHMATCH_TEAM2)||
		(nGameType == MMATCH_GAMETYPE_GLADIATOR_TEAM) ||
		(nGameType == MMATCH_GAMETYPE_ASSASSINATE) ||
		(nGameType == MMATCH_GAMETYPE_CTF) ||
		(nGameType == MMATCH_GAMETYPE_SPYMODE)||
		(nGameType == MMATCH_GAMETYPE_INFECTION)||
		(nGameType == MMATCH_GAMETYPE_BLITZKRIEG)||
		(nGameType == MMATCH_GAMETYPE_DEMOLITION))
	{
		return true;
	}
	return false;
}

inline bool MBaseGameTypeCatalogue::IsTeamLimitTime(MMATCH_GAMETYPE nGameType)
{
	// ���� ���������� Description�� teamgame���� ���ΰ��� �ֵ��� ����.

	if ((nGameType == MMATCH_GAMETYPE_DEATHMATCH_TEAM) ||
		(nGameType == MMATCH_GAMETYPE_GLADIATOR_TEAM) ||
		(nGameType == MMATCH_GAMETYPE_DUEL) ||
		(nGameType == MMATCH_GAMETYPE_ASSASSINATE) ||
		(nGameType == MMATCH_GAMETYPE_CTF)||
		(nGameType == MMATCH_GAMETYPE_SPYMODE)||
		(nGameType == MMATCH_GAMETYPE_INFECTION)||
		(nGameType == MMATCH_GAMETYPE_DEMOLITION))

	{
		return true;
	}
	return false;
}

inline bool MBaseGameTypeCatalogue::IsWaitForRoundEnd(MMATCH_GAMETYPE nGameType)
{
	// ���� ���������� ����� �ϴ°�?

	if ((nGameType == MMATCH_GAMETYPE_DEATHMATCH_TEAM) ||
		(nGameType == MMATCH_GAMETYPE_DUEL) ||
		(nGameType == MMATCH_GAMETYPE_GLADIATOR_TEAM) ||
		(nGameType == MMATCH_GAMETYPE_ASSASSINATE) ||
		(nGameType == MMATCH_GAMETYPE_DUELTOURNAMENT)||
		(nGameType == MMATCH_GAMETYPE_INFECTION)||
		(nGameType == MMATCH_GAMETYPE_SPYMODE)||
		(nGameType == MMATCH_GAMETYPE_DEMOLITION))
	{
		return true;
	}
	return false;
}


inline bool MBaseGameTypeCatalogue::IsQuestDerived(MMATCH_GAMETYPE nGameType)
{
	if ( (nGameType == MMATCH_GAMETYPE_SURVIVAL) ||(nGameType == MMATCH_GAMETYPE_QUEST) )
	{
		return true;
	}

	return false;
}

inline bool MBaseGameTypeCatalogue::IsNewQuestDerived(MMATCH_GAMETYPE nGameType)
{
	if ((nGameType == MMATCH_GAMETYPE_QUEST_CHALLENGE) || (nGameType == MMATCH_GAMETYPE_BLITZKRIEG))
		return true;

	return false;
}

inline bool MBaseGameTypeCatalogue::IsQuestOnly(MMATCH_GAMETYPE nGameType)
{
	return nGameType == MMATCH_GAMETYPE_QUEST;
}

inline bool MBaseGameTypeCatalogue::IsSurvivalOnly(MMATCH_GAMETYPE nGameType)
{
	return nGameType == MMATCH_GAMETYPE_SURVIVAL;
}

inline const char* MBaseGameTypeCatalogue::GetGameTypeStr(MMATCH_GAMETYPE nGameType)
{
	return m_GameTypeInfo[nGameType].szGameTypeStr;
}

inline void MBaseGameTypeCatalogue::SetGameTypeStr(MMATCH_GAMETYPE nGameType, const char* szName)
{
	strcpy_s( m_GameTypeInfo[nGameType].szGameTypeStr,sizeof(m_GameTypeInfo[nGameType].szGameTypeStr), szName) ;
}

bool MBaseGameTypeCatalogue::IsCorrectGameType(const int nGameTypeID)
{
	if ((nGameTypeID < 0) || (nGameTypeID >= MMATCH_GAMETYPE_MAX)) return false;
	return true;
}

inline MMatchGameTypeInfo* MBaseGameTypeCatalogue::GetInfo(MMATCH_GAMETYPE nGameType)
{
	_ASSERT((nGameType >= 0) && (nGameType < MMATCH_GAMETYPE_MAX));
	return &m_GameTypeInfo[nGameType];
}

inline bool MBaseGameTypeCatalogue::IsWorldItemSpawnEnable(MMATCH_GAMETYPE nGameType)
{
	if ( (nGameType == MMATCH_GAMETYPE_SURVIVAL) || (nGameType == MMATCH_GAMETYPE_QUEST) || (nGameType == MMATCH_GAMETYPE_QUEST_CHALLENGE))
	{
		return false;
	}

	return true;

}

//////////////////////////////////////////////////////////////////////////////////
inline bool MBaseGameTypeCatalogue::IsTeamExtremeGame(MMATCH_GAMETYPE nGameType)
{
	// ���� ���������� Description�� teamgame���� ���ΰ��� �ֵ��� ����.
	//blitz is extreme, players can always respawn
	if ((nGameType == MMATCH_GAMETYPE_DEATHMATCH_TEAM2) ||
		(nGameType == MMATCH_GAMETYPE_CTF)||
		(nGameType == MMATCH_GAMETYPE_BLITZKRIEG))
	{
		return true;
	}
	return false;
}

#endif