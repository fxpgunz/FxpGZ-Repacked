#ifndef _ZSOUNDENGINE_H
#define _ZSOUNDENGINE_H

class ZLoadingProgress;

#ifdef _BIRDSOUND

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
#else

#include "MZFileSystem.h"
#include "ZActorSound.h"
#include "MQuestNPC.h"

#include <map>
#include <string>
using namespace std;

#pragma comment(lib, "dxguid.lib")

struct MMatchItemDesc;
struct FSOUND_SAMPLE;

struct SoundSource
{
	FSOUND_SAMPLE* pFS;
	float fMaxDistance; // to Culling
	unsigned long int	nLastPlayedTime;
	SoundSource()
	{
		pFS = 0;
		fMaxDistance = 1000000000.0f; // 2d
		nLastPlayedTime = 0;
	}
};

struct DelaySound
{
	SoundSource* pSS;
	DWORD dwDelay;
	rvector pos;
	int priority;
	bool bPlayer;
	DelaySound()
	{
		pSS = 0;
		dwDelay = 0;
		priority = 0;
		bPlayer = false;
	}
};

struct AmbSound
{
	int type;
	SoundSource* pSS;
	rvector pos[2];		//�簢��(min, max)
	float radius;
	rvector center;
	int iChannel;
	float dx, dy, dz;
	char szSoundName[64];	// ���� �̸�.. reset�Ҷ� �ʿ���
	AmbSound()
	{
		type = 0;
		pSS = NULL;
		iChannel = -1;
		dx = dy = dz = 0;
		szSoundName[0] = 0;
	}
};

typedef map<string, SoundSource*> SESMAP;
typedef list<DelaySound> DSLIST;
typedef list<AmbSound> ASLIST;

class ZSoundEngine
{
private:
	char					m_SoundFileName[256];	// for Reload
	ZActorSoundManager		m_ASManager;			// NPC�� ���� ���� Ŭ����
protected:
	char*			m_pMusicBuffer;
	char			m_szOpenedMusicName[256];

	SESMAP			m_SoundEffectSource;
	SESMAP			m_SoundEffectSource2D;
	DSLIST			m_DelaySoundList; // ���� ���� ����Ʈ
	ASLIST			m_AmbientSoundList; // ȯ�� ���� ����Ʈ

	float			m_fEffectVolume;
	float			m_fMusicVolume;

	bool			m_bEffectMute;
	bool			m_bMusicMute;
	bool			m_bSoundEnable;
public:
	bool			m_b3DSound;
	bool			m_b3DSoundUpdate;
	rvector			m_ListenerPos;
	bool			m_bInverse;
	void			Set3DSound( bool b3DSound ) { m_b3DSound = b3DSound; }
	bool			m_bHWMixing;

protected:
	bool			m_b8Bits;
	bool			m_bBackGroundAudio;

	DWORD	m_Time;
	DWORD	m_DelayTime;

	bool			m_bEffectVolControl;	
	float			m_fEffectVolFactor;
	float			m_fEffectVolEnd;
	
	bool			m_bBGMVolControl;
	float			m_fBGMVolFactor;
	float			m_fBGMVolEnd;

	MZFileSystem*	m_pfs;
	bool			m_bBattleMusic;
	const char*		GetBGMFileName(int nBgmIndex);
	bool OpenMusic(const char* szFileName, MZFileSystem* pfs);
	bool CheckCulling(const char* szName, SoundSource* pSS, const rvector& vSoundPos, bool bHero, int* pnoutPriority=NULL);
	static void MusicEndCallback(void* pCallbackContext);
public:
	ZSoundEngine();
	virtual ~ZSoundEngine();
	bool Create(HWND hwnd, bool bHWMixing = false, ZLoadingProgress *pLoading = NULL, bool backGroundAudio = false);
	bool Reset(HWND hwnd, bool bHWMixing, bool backgroundAudio );
	void Destroy();
	bool LoadResource( char* pFileName_ ,ZLoadingProgress *pLoading = NULL );
	bool Reload();

	// BGM
	bool OpenMusic(int nBgmIndex, MZFileSystem* pfs);
	void CloseMusic();
	void PlayMusic(bool bLoop = true);
	void StopMusic();
	void SetMusicVolume(float fVolume);
	float GetMusicVolume( void);
	void SetMusicMute(bool b);

	/// Sound Effect Source
	void SetEffectVolume(float fVolume);
	void SetEffectVolume( int iChannel, float fVolume );
	void StopLoopSound();
	void StopSound( int iChannel );
	void SetEffectMute(bool b);
	bool SetSamplingBits( bool b8bit );
	void SetInverseSound( bool bInverse )	{ m_bInverse = bInverse; }

	// Game-Specified Sound Effect
	// �� �Լ� �迭�� ����� ��� max distance culling, Priority ������ �ȵ�
	//int PlaySE( FSOUND_SAMPLE* pFS, rvector& pos, bool bPlayer = false, bool bLoop = false );
	//int PlaySE( FSOUND_SAMPLE* pFS, bool bLoop = false );
	int PlaySE( FSOUND_SAMPLE* pFS, const rvector& pos, int Priority , bool bPlayer = false, bool bLoop = false );

	/**
		WEAPON SOUND
	**/
	// melee ���� �Ҹ�
	void PlaySoundBladeConcrete(MMatchItemDesc *pDesc, rvector pos);				// �� �ܴ� �Ҹ� 
	void PlaySoundBladeDamage(MMatchItemDesc *pDesc, rvector& pos);					// �ǰ� �Ҹ�
	void PlaySoundHangOnWall(MMatchItemDesc *pDesc, rvector& pos);					// ���� Į �Ŵ¼Ҹ�
	void PlaySoundChargeComplete(MMatchItemDesc *pDesc, const rvector& pos);				// �������(����) �Ϸ�Ǵ� �Ҹ�
	void PlaySoundSmash(MMatchItemDesc *pDesc, rvector& pos, bool bObserverTarget);	// ������(������) ���ݼҸ�
	void PlaySoundSheath(MMatchItemDesc *pDesc, const rvector& pos, bool bObserverTarget);// ���� ������ �Ҹ�

	void PlaySEFire(MMatchItemDesc *pDesc, float x, float y, float z, bool bPlayer=false);
	void PlaySEDryFire(MMatchItemDesc *pDesc, float x, float y, float z, bool bPlayer=false);
	void PlaySEReload(MMatchItemDesc *pDesc, float x, float y, float z, bool bPlayer=false);

	// �Ѿ� ƨ��� �Ҹ�
	void PlaySERicochet(float x, float y, float z);
	void PlaySEHitObject( float x, float y, float z, RBSPPICKINFO& info_ );

	// �Ѿ��� ���� �´� �Ҹ�
	void PlaySEHitBody(float x, float y, float z);

	int PlaySound(const char* Name,const rvector& pos,bool bHero=false, bool bLoop = false, DWORD dwDelay = 0 );
	void PlaySoundElseDefault(char* Name,char* NameDefault,rvector& pos,bool bHero=false, bool bLoop = false, DWORD dwDelay = 0 );
	int PlaySound(const char* Name, bool bLoop=false, DWORD dwDelay = 0 ); // only 2D sound
	
	bool isPlayAble(char* name);
	bool isPlayAbleMtrl(char* name);//Ư��������~

	void Run(void);
	void UpdateAmbSound(rvector& Pos, rvector& Ori);
	float GetArea( rvector& Pos, AmbSound& a );

	int GetEnumDeviceCount();
	const char* GetDeviceDescription( int index );

	FSOUND_SAMPLE* GetFS( const char* szName, bool bHero = false );
	SoundSource* GetSoundSource( const char* szName, bool bHero );

	/*
	 *	for Sound Debugging
	 */
	void SetFramePerSecond( int n ) { m_DelayTime = 1000 / n; }
	void Set3DSoundUpdate(bool b);
	bool Get3DSoundUpdate() const { return m_b3DSoundUpdate; }

	// AmbientSound
	void SetAmbientSoundBox( char* Name, rvector& pos1, rvector& pos2, bool b2d = true );
	void SetAmbientSoundSphere( char* Name, rvector& pos, float radius, bool b2d = true );
	void ClearAmbientSound();

	// Special Functions
	void PlayVoiceSound(char* szName);
    
	// �����ð��� ���� ���� ������������ �� �������� ������ �ִ� �Լ�
	void SetVolumeControlwithDuration( float fStartPercent, float fEndPercent, DWORD dwDuration, bool bEffect, bool bBGM );

	// NPC Sound ���� �Լ�
	bool LoadNPCResource(MQUEST_NPC nNPC, ZLoadingProgress* pLoading = NULL);
	void ReleaseNPCResources();
	void PlayNPCSound(MQUEST_NPC nNPC, MQUEST_NPC_SOUND nSound, rvector& pos, bool bMyKill=true);

	///Custom: Mp3 functions
	void playNextSong();
	void playPreviousSong();
	void pause();
	void LoadCustomMusic();
	void ClearCustomMusic();
	///TODO: repeat/shuffle functions
};

#endif	// _BIRDSOUND

constexpr int BGMID_INTRO = 0;
constexpr int BGMID_LOBBY = 1;
constexpr int BGMID_BATTLE = 2;
constexpr int BGMID_FIN = 12;

constexpr int MAX_BGM = 13;




constexpr const char* VOICE_COOL = "nar/NAR01";	// 0.7  �߰� �Ϸ�
constexpr const char* VOICE_NICE = "nar/NAR02";	// 1.0  �߰� �Ϸ�
constexpr const char* VOICE_GREAT = "nar/NAR03";		// 0.8  �߰� �Ϸ�
constexpr const char* VOICE_WONDERFUL = "nar/NAR04";		// 1.2  �߰� �Ϸ�
constexpr const char* VOICE_KILLEDALL = "nar/NAR05";	// 2.0  �߰� �Ϸ�
constexpr const char* VOICE_HEADSHOT = "nar/NAR06";	// 0.7  �߰� �Ϸ�
constexpr const char* VOICE_FANTASTIC = "nar/NAR07";	// 1.5  �߰� �Ϸ�
constexpr const char* VOICE_EXCELLENT = "nar/NAR08";	// 1.0  �߰� �Ϸ�
constexpr const char* VOICE_UNBELIEVABLE = "nar/NAR09";	// 1.3  �߰� �Ϸ�
constexpr const char* VOICE_GET_READY = "nar/NAR10";	// 1.3  �߰� �Ϸ�
constexpr const char* VOICE_LETS_ROCK = "nar/NAR11";	// 1.1  �߰� �Ϸ�
constexpr const char* VOICE_FINAL_ROUND = "nar/NAR27";	// 1.2  �߰� �Ϸ�
constexpr const char* VOICE_YOU_WON = "nar/NAR12";		// 1.0  �߰� �Ϸ�
constexpr const char* VOICE_YOU_LOSE = "nar/NAR13";	// 1.3  �߰� �Ϸ�
constexpr const char* VOICE_RED_TEAM_WON = "nar/NAR14";	// 1.4  �߰� �Ϸ�
constexpr const char* VOICE_BLUE_TEAM_WON = "nar/NAR15";	// 1.4  �߰� �Ϸ�
constexpr const char* VOICE_DRAW_GAME = "nar/NAR16";	// 1.2  �߰� �Ϸ�
constexpr const char* VOICE_REDTEAM_BOSS_DOWN = "nar/NAR19";	// 2.0  �߰� �Ϸ�
constexpr const char* VOICE_BLUETEAM_BOSS_DOWN = "nar/NAR20";		// 2.1  �߰� �Ϸ�
constexpr const char* VOICE_PLAYER_NOT_READY = "nar/NAR26";	// 1.5  �߰� �Ϸ�
constexpr const char* VOICE_BERSERKER_DOWN = "nar/NAR28";		// 1.3  �߰� �Ϸ�
constexpr const char* VOICE_GOT_BERSERKER = "nar/NAR29";	// 1.6  �߰� �Ϸ�
constexpr const char* VOICE_QUEST_START_FAIL = "nar/NAR22";	// 2.8  �߰� �Ϸ�
constexpr const char* VOICE_CTF = "nar/NAR30";
constexpr const char* VOICE_NEW_INTRUDER = "nar/NAR17";		// 1.3
constexpr const char* VOICE_NEW_CHALLENGER = "nar/NAR18";	// 1.6
constexpr const char* VOICE_RED_HAS_FLAG = "nar/NAR32";
constexpr const char* VOICE_BLUE_HAS_FLAG = "nar/NAR31";
constexpr const char* VOICE_RED_FLAG_RETURN = "nar/NAR34";
constexpr const char* VOICE_BLUE_FLAG_RETURN = "nar/NAR33";
constexpr const char* VOICE_RED_TEAM_SCORE = "nar/NAR36";
constexpr const char* VOICE_BLUE_TEAM_SCORE = "nar/NAR35";
constexpr const char* VOICE_GO_BACK = "";
constexpr const char* VOICE_FOLLOW_ME = "";
constexpr const char* VOICE_BACK_ME_UP = "";
constexpr const char* VOICE_COVER_ME = "";
constexpr const char* VOICE_ENEMY_IN_SIGHT = "";
constexpr const char* VOICE_THANK_YOU = "";
constexpr const char* VOICE_SORRY = "";
constexpr const char* VOICE_HAHAHA = "";
constexpr const char* VOICE_OOPS = "";
constexpr const char* VOICE_UH_OH = "";
constexpr const char* VOICE_YES = "";
constexpr const char* VOICE_NO = "";
constexpr const char* VOICE_HEAL_ME_PLEASE = "";
constexpr const char* VOICE_MEDIC = "";
constexpr const char* VOICE_HOLD = "";
constexpr const char* VOICE_DEFEND = "";
constexpr const char* VOICE_NEGATIVE = "";
constexpr const char* VOICE_AFFIRMATIVE = "";

constexpr int VOICE_MAX = 45;



















namespace CustomMusic
{
	extern std::vector<vector<string>> m_customMusic;
	extern int g_NextMusicIndex;
	extern std::vector<string> folderName;
}


#endif