#ifndef _ZGAMECLIENT_H
#define _ZGAMECLIENT_H

#include "ZPrerequisites.h"
#include "MMatchClient.h"
#include "MSharedCommandTable.h"
#include "MMatchStage.h"
#include "ZChannelRule.h"
#include "ZGame.h"
#include "ZNetAgreementBuilder.h"
#include "MEmblemMgr.h"
#include "ZCommandUDPHackShield.h"

typedef bool(ZONCOMMANDCALLBACK)(MCommand* pCommand);
class MListBox;
class ZCharacterViewList;
class UPnP;


class ZGameClient : public MMatchClient
{
protected:
	char				m_szChannel[256];
	char				m_szChannelRule[128];
	char				m_szStageName[256];
	char				m_szChatRoomInvited[64];
	unsigned int		m_nRoomNo;
	int					m_nStageCursor;
	bool				m_bLadderGame;
	bool				m_bPlayerWars;
	MCHANNEL_TYPE		m_CurrentChannelType;
	bool				m_bEnableInterface;
	char				m_szVoteText[256];

private:
	unsigned long int		m_nPrevClockRequestAttribute;
	
	int						m_nBridgePeerCount;
	unsigned long int		m_tmLastBridgePeer;

	int						m_nCountdown;
	unsigned long int		m_tmLastCountdown;

	int						m_nRequestID;			///< Ŭ�� ������� ���̴� RequestID
	MUID					m_uidRequestPlayer;		///< Ŭ�� �������� ��û�� 
	ZNetAgreementBuilder	m_AgreementBuilder;		///< ���Ǻ��
	MMatchProposalMode		m_nProposalMode;

	// ��ǥ�������̽� ����
	bool					m_bVoteInProgress;		///< ��ǥ�� �������ΰ�
	bool					m_bCanVote;				///< ��ǥ���� �ִ°�

	// Ŭ�� �ں� ����
	MEmblemMgr				m_EmblemMgr;

	// ��Ÿ�ɼ� ����
	bool					m_bPriorityBoost;		///< �����ִ�ȭ
	bool					m_bRejectNormalChat;	///< �Ϲ�ê ���
	bool					m_bRejectTeamChat;		///< ��ê ���
	bool					m_bRejectClanChat;		///< Ŭ��ê ���
	bool					m_bRejectWhisper;		///< �Ӹ����
	bool					m_bRejectInvite;		///< �ʴ����
	bool					m_bShowExtra;
	int						m_nPingLimitStrikes;

#ifdef _SRVRPNG
	DWORD					m_ServerPing;
#endif
	
#ifdef _AFKSYSTEM
	DWORD					m_nLastKeyTime;
#endif

public:
	bool IsRejoin;
#ifdef _CW_VOTE
	int LastVoteID;
	bool bMatching;
#endif // _CW_VOTE
	// �����ʸ�Ʈ �� �������� (�α����Ҷ��� 1ȸ ����, ����� ���⿡ ���� ������ ui����)
	bool ShowLocalDamage;
	vector<string> Ignores;
	struct DTCHARINFO {
		int tournamentPoint, wins, losses, ranking, winners, lastWeekGrade;
	};
	struct PWCHARINFO
	{
		int Ranking, Wins, Losses, Draws, Score;
	};
	const PWCHARINFO* GetMyPlayerWarsCharInfo() { return &m_PWCharInfo; };
	const DTCHARINFO* GetMyDuelTournamentCharInfo()		{ return &m_dtCharInfo; }
	const DTCHARINFO* GetMyDuelTournamentCharInfoPrev() { return &m_dtCharInfoPrev; }
private:
	DTCHARINFO				m_dtCharInfo, m_dtCharInfoPrev;
	PWCHARINFO m_PWCharInfo;

protected:
	void SetChannelRuleName(const char* pszName)	{ strcpy(m_szChannelRule, pszName); }
	int GetBridgePeerCount()			{ return m_nBridgePeerCount; }
	void SetBridgePeerCount(int nCount)	{ m_nBridgePeerCount = nCount; }
	void UpdateBridgePeerTime(unsigned long int nClock)	{ m_tmLastBridgePeer = nClock; }
	void StartBridgePeer();

	void UpdateStageSetting(MSTAGE_SETTING_NODE* pSetting, STAGE_STATE nStageState, const MUID& uidMaster);
	void SetCountdown(int nCountdown)	{ m_nCountdown = nCountdown; m_tmLastCountdown = 0; }
	int GetCountdown()					{ return m_nCountdown; }
	bool Countdown(int nClock) {
		if (nClock - m_tmLastCountdown > 1000) {
			m_nCountdown--;
			m_tmLastCountdown = nClock;
			return true;
		}
		return false;
	}

	static int FindListItem(MListBox* pListBox, const MUID& uid);

protected:
	ZONCOMMANDCALLBACK*		m_fnOnCommandCallback;
	
	bool					m_bIsBigGlobalClock;	// �۷ι� Ŭ���� ���ú��� �� ũ�� true
	unsigned long int		m_nClockDistance;	///< �۷ι� Ŭ������ �ð���

	MMatchStageSetting		m_MatchStageSetting;
	bool					m_bForcedEntry;		///< �����ؼ� ������ ����
protected:
	virtual bool OnCommand(MCommand* pCommand);
	virtual bool OnSockDisconnect(SOCKET sock);
	virtual bool OnSockConnect(SOCKET sock);
	virtual void OnSockError(SOCKET sock, SOCKET_ERROR_EVENT ErrorEvent, int &ErrorCode);
	virtual int OnConnected(SOCKET sock, MUID* pTargetUID, MUID* pAllocUID, unsigned int nTimeStamp);
	virtual void OnRegisterCommand(MCommandManager* pCommandManager);
	virtual void OnPrepareCommand(MCommand* pCommand);	///< Ŀ�ǵ带 ó���ϱ� ����
	virtual int OnResponseMatchLogin(const MUID& uidServer, int nResult, const char* szServerName, const MMatchServerMode nServerMode,
									 const char* szAccountID, const MMatchUserGradeID nUGradeID, const MMatchPremiumGradeID nPGradeID,
									 int nCountryFlag, int nCoins, int nECoins,
									 const MUID& uidPlayer, bool bEnabledSurvivalMode, bool bEnabledDuelTournament, unsigned char* szEncryptMsg);	// update sgk 0409
	virtual void OnBridgePeerACK(const MUID& uidChar, int nCode);
	virtual void OnObjectCache(unsigned int nType, void* pBlob, int nCount);
	virtual void OnAgentError(int nError);

	virtual void OnPrepareRun(void);

protected:
	void OnMatchNotify(unsigned int nMsgID);
//	void OnPeerList(const MUID& uidStage, void* pBlob, int nCount);
//	void OnAddPeer(const MUID& uidChar, const char* szIP, const int nPort =				
//		MATCHCLIENT_DEFAULT_UDP_PORT, MTD_CharInfo* pCharInfo = NULL, int nTeam = 0);
	void OnStageSettingUpdate(DWORD ping);
	void OnAnnounce(unsigned int nType, char* szMsg);
	void OnResponseResult(const int nResult);

	void OnChannelResponseJoin(const MUID& uidChannel, MCHANNEL_TYPE nChannelType, const char* szChannelName, bool bEnableInterface);
	void OnChannelChat(const MUID& uidChannel, char* szName, char* szChat,int nGrade, int nR, int nG, int nB);
	void OnChannelList(void* pBlob, int nCount);
	void OnChannelResponseRule(const MUID& uidchannel, const char* pszRuleName);

	void OnLadderPrepare(const MUID& uidStage, const int nTeam);
	void OnLadderLaunch(const MUID& uidStage, const char* pszMapName, bool PlayerWars);
	void OnLadderResponseChallenge(const int nResult);

	void OnStageJoin(const MUID& uidChar, const MUID& uidStage, unsigned int nRoomNo, char* szStageName);
	void OnStageLeave(const MUID& uidChar, const MUID& uidStage);
//	void OnStageEnterBattle(const MUID& uidChar, const MUID& uidStage, MCmdEnterBattleParam nParam, MTD_PeerListNode* pPeerNode);
//	void OnStageLeaveBattle(const MUID& uidChar, const MUID& uidStage);
	void OnStageStart(const MUID& uidChar, const MUID& uidStage, int nCountdown);
	void OnStageRelayStart();
	void OnStageLaunch(const MUID& uidStage, const char* pszMapName);
	void OnStageFinishGame(const MUID& uidStage, const bool bIsRelayMapUnFinish);
	void OnStageMap(const MUID& uidStage, char* szMapName, bool bIsRelayMap = false);
	void OnStageTeam(const MUID& uidChar, const MUID& uidStage, unsigned int nTeam);
	void OnStagePlayerState(const MUID& uidChar, const MUID& uidStage, MMatchObjectStageState nStageState);
	void OnStageMaster(const MUID& uidStage, const MUID& uidChar);
	void OnStageChat(const MUID& uidChar, const MUID& uidStage, char* szChat, int nR, int nG, int nB);
	void OnStageList(int nPrevStageCount, int nNextStageCount, void* pBlob, int nCount);
	void OnStageRelayMapListUpdate(int nRelayMapType, int nRelayMapRepeatCount, void* pStageRelayMapListBlob);
	void OnStageRelayMapElementUpdate(int nRelayMapType, int nRelayMapRepeatCount);
	void OnResponseFriendList(void* pBlob, int nCount);
	void OnChannelPlayerList(int nTotalPlayerCount, int nPage, void* pBlob, int nCount);
	void OnChannelAllPlayerList(const MUID& uidChannel, void* pBlob, int nBlobCount);
	void OnResponseStageSetting(const MUID& uidStage, void* pStageBlob, int nStageCount, void* pCharBlob, 
		                        int nCharCount, STAGE_STATE nStageState, const MUID& uidMaster);
	void OnResponseRecommandedChannel(const MUID& uidChannel);
	void OnResponsePeerRelay(const MUID& uidPeer);
	void OnResponseGameInfo(const MUID& uidStage, void* pGameInfoBlob, void* pRuleInfoBlob, void* pPlayerInfoBlob);
	void OnResponseCharInfoDetail(void* pBlob);

	void OnLoadingComplete(const MUID& uidChar, int nPercent);
	void OnForcedEntryToGame();

	void OnUserWhisper(char* pszSenderName, char* pszTargetName, char* pszMessage);
	void OnChatRoomJoin(char* pszPlayerName, char* pszChatRoomName);
	void OnChatRoomLeave(char* pszPlayerName, char* pszChatRoomName);
	void OnChatRoomSelectWrite(char* pszChatRoomName);
	void OnChatRoomInvite(char* pszSenderName, char* pszRoomName);
	void OnChatRoomChat(char* pszChatRoomName, char* pszPlayerName, char* pszChat);

	void OnResponseUpdateStageEquipLook( const MUID& uidPlayer, const int nParts, const int nItemID );

	// ���󰡱� ����.
	void OnFollowResponse( const int nMsgID );
	void OnExpiredRentItem(void* pBlob);	///< �Ⱓ�� ������ ��� ���� ��������

	void OnBirdTest();
protected:
	// Ŭ������
	void OnResponseCreateClan(const int nResult, const int nRequestID);
	void OnResponseAgreedCreateClan(const int nResult);
	void OnClanAskSponsorAgreement(const int nRequestID, const char* szClanName, MUID& uidMasterObject, const char* szMasterName);
	void OnClanAnswerSponsorAgreement(const int nRequestID, const MUID& uidClanMaster, char* szSponsorCharName, const bool bAnswer);
	void OnClanResponseCloseClan(const int nResult);
	void OnClanResponseJoinClan(const int nResult);
	void OnClanAskJoinAgreement(const char* szClanName, MUID& uidClanAdmin, const char* szClanAdmin);
	void OnClanAnswerJoinAgreement(const MUID& uidClanAdmin, const char* szJoiner, const bool bAnswer);
	void OnClanResponseAgreedJoinClan(const int nResult);
	void OnClanUpdateCharClanInfo(void* pBlob);
	void OnClanResponseLeaveClan(const int nResult);
	void OnClanResponseChangeGrade(const int nResult);
	void OnClanResponseExpelMember(const int nResult);
	void OnClanMsg(const char* szSenderName, const char* szMsg);
	void OnClanMemberList(void* pBlob);
	void OnClanResponseClanInfo(void* pBlob);
	void OnClanStandbyClanList(int nPrevStageCount, int nNextStageCount, void* pBlob);
	void OnClanMemberConnected(const char* szMember);

	// ���ǰ���
	void OnResponseProposal(const int nResult, const MMatchProposalMode nProposalMode, const int nRequestID);
	void OnAskAgreement(const MUID& uidProposer, 
		                void* pMemberNamesBlob, 
						const MMatchProposalMode nProposalMode, 
						const int nRequestID);
	void OnReplyAgreement(const MUID& uidProposer, 
		                  const MUID& uidChar, 
						  const char* szReplierName, 
						  const MMatchProposalMode nProposalMode,
					      const int nRequestID, 
						  const bool bAgreement);
	void ReplyAgreement(const MUID& uidProposer, const MMatchProposalMode nProposalMode, bool bAgreement);
protected:
	void OnGameLevelUp(const MUID& uidChar);
	void OnGameLevelDown(const MUID& uidChar);

public:				// ���� -_-; ���÷��̿��� �� �̺�Ʈ�� ó���ؾ� ��;
	void OnSpawnWorldItem(void* pBlob);
	void OnObtainWorldItem(const MUID& uidChar, const int nItemUID);
	void OnRemoveWorldItem(const int nItemUID);

	void OnNotifyActivatedTrapItemList(void* pBlob);

protected:
	// Local
	void OnLocalReport119();
protected:
	void OnAdminAnnounce(const char* szMsg, const ZAdminAnnounceType nType);
	void OnStaffHelp(const char* szMsg, const char* szUsr, const int UgradeID);
	void OnAdminWall(const char* szMsg, const char* szCharName);
public:
	ZGameClient();
	virtual ~ZGameClient();
	int	GetPingLimitStrikes()			{ return m_nPingLimitStrikes; }
	void SetPingLimitStrikes(int nlimit) { m_nPingLimitStrikes = nlimit; }
	void PriorityBoost(bool bBoost);
	bool GetPriorityBoost()				{ return m_bPriorityBoost; }
	bool GetRejectNormalChat()			{ return m_bRejectNormalChat; }
	void SetRejectNormalChat(bool bVal)	{ m_bRejectNormalChat = bVal; }
	bool GetRejectTeamChat()			{ return m_bRejectTeamChat; }
	void SetRejectTeamChat(bool bVal)	{ m_bRejectTeamChat = bVal; }
	bool GetRejectClanChat()			{ return m_bRejectClanChat; }
	void SetRejectClanChat(bool bVal)	{ m_bRejectClanChat = bVal; }
	bool GetRejectWhisper()				{ return m_bRejectWhisper; }
	void SetRejectWhisper(bool bVal)	{ m_bRejectWhisper = bVal; }
	bool GetRejectInvite()				{ return m_bRejectInvite; }
	void SetRejectInvite(bool bVal)		{ m_bRejectInvite = bVal; }
	bool GetExtra()						{ return m_bShowExtra; }
	void SetExtra(bool bVal)			{ m_bShowExtra = bVal; }
#ifdef _SRVRPNG
	DWORD GetServerPing()				{ return m_ServerPing; }
#endif
#ifdef _SRVRPNG
	void OnServerPing(DWORD ping, bool ServerType = true);
#endif
	unsigned long int GetClockCount(void) { return timeGetTime(); }		// ���� Ŭ�� ��ȯ
	unsigned long int GetGlobalClockCount(void);		///< ������ ����� Ŭ���� ��ȯ�Ѵ�.


#ifdef _AFKSYSTEM
	void SetLastKeyTime(DWORD nLastKeyTime) { m_nLastKeyTime = nLastKeyTime; }
	DWORD GetLastKeyTime() { return m_nLastKeyTime; }
#endif

	virtual void OutputMessage(const char* szMessage, MZMOMType nType=MZMDM_GENERAL);
	
	void SetOnCommandCallback(ZONCOMMANDCALLBACK pCallback) { m_fnOnCommandCallback = pCallback;}

//	void InitializeGame();
//	void StartGame();
//	void FinalizeGame();

	void Tick(void);
	void Disconnect()							{ MMatchClient::Disconnect(m_Server); }
	
	MMatchStageSetting* GetMatchStageSetting() { return &m_MatchStageSetting; }
	bool IsForcedEntry() { return m_bForcedEntry; }
	bool IsLadderGame() { return m_bLadderGame; }
	bool IsPlayerWars() { return m_bPlayerWars; }
	bool IsDuelTournamentGame() { return m_MatchStageSetting.GetGameType() == MMATCH_GAMETYPE_DUELTOURNAMENT; }
	bool IsSpyMode() { return m_MatchStageSetting.GetGameType() == MMATCH_GAMETYPE_SPY; }

	void ReleaseForcedEntry();
	void ClearStageSetting();
public:
	void RequestPrevStageList();
	void RequestNextStageList();
	void RequestStageList(int nPage);
	void StartStageList();
	void StopStageList();
	void StartChannelList(MCHANNEL_TYPE nChannelType);
	void StopChannelList();
	//void SetChannelType(MCHANNEL_TYPE type);

	const char*		GetChannelName()		{ return m_szChannel; }
	MCHANNEL_TYPE	GetChannelType()		{ return m_CurrentChannelType; }
	bool			GetEnableInterface()	{ return m_bEnableInterface; }
	const char*		GetChannelRuleName()	{ return m_szChannelRule; }
	const char*		GetStageName()			{ return m_szStageName; }
	int				GetStageNumber()		{ return m_nRoomNo; }
	
	
	
	const char* GetChatRoomInvited(){ return m_szChatRoomInvited; }
	void SetChatRoomInvited(const char* pszRoomName)	{ strcpy(m_szChatRoomInvited, pszRoomName); }

	bool AmIStageMaster() { return (m_MatchStageSetting.GetMasterUID() == GetPlayerUID()); }

	const char* GetVoteMessage() { return m_szVoteText; }
public:
	// Ŭ�� ����
	void AnswerSponsorAgreement(bool bAnswer);
	void AnswerJoinerAgreement(bool bAnswer);
	void RequestCreateClan(char* szClanName, char** ppMemberCharNames);

	// ���� ����
	void RequestProposal(const MMatchProposalMode nProposalMode, char** ppReplierCharNames, const int nReplierCount);
	void ReplyAgreement(bool bAgreement);
public:	// ��ǥ����
	bool IsVoteInProgress()				{ return m_bVoteInProgress;	}
	void SetVoteInProgress(bool bVal)	{ m_bVoteInProgress = bVal; }
	bool CanVote()						{ return m_bCanVote; }
	void SetCanVote(bool bVal)			{ m_bCanVote = bVal; }

public:
	void RequestGameSuicide();
	// game ���� �ҷ��ش�
	//void OnGameRoundState(const MUID& uidStage, int nRoundState, int nRound);
	void OnStageEnterBattle(const MUID& uidChar, MCmdEnterBattleParam nParam);
public:
	// Validate ������
	int ValidateRequestDeleteChar();
public:
	// Request ������
	void RequestChannelJoin(const MUID& uidChannel);
	void RequestChannelJoin(const MCHANNEL_TYPE nChannelType, char* szChannelName);
	void RequestOnLobbyCreated();		// �κ� ���ö� ��û�ϴ� �޽�����
	void RequestOnGameDestroyed();		// ������ �������� ��û�ϴ� �޽�����
protected:
	void OnNotifyCallVote(const char* pszDiscuss, const char* pszArg);
	void OnNotifyVoteResult(const char* pszDiscuss, int nResult);
	void OnVoteAbort( const int nMsgCode );
protected:
	void OnBroadcastClanRenewVictories(const char* pszWinnerClanName, const char* pszLoserClanName, int nVictories);
	void OnBroadcastClanInterruptVictories(const char* pszWinnerClanName, const char* pszLoserClanName, int nVictories);
	void OnBroadcastDuelRenewVictories(const char* pszChampionName, const char* pszChannelName, int nRoomno, int nVictories);
	void OnBroadcastDuelInterruptVictories(const char* pszChampionName, const char* pszInterrupterName, int nVictories);
protected:
	// Emblem ����
	void ProcessEmblem(unsigned int nCLID, unsigned int nChecksum);
	void RequestEmblemURL(unsigned int nCLID);
	void OnClanResponseEmblemURL(unsigned int nCLID, unsigned int nEmblemChecksum, const char* szEmblemURL);
	void OnClanEmblemReady(unsigned int nCLID, const char* szURL);

public:
	MEmblemMgr *GetEmblemManager() { return &m_EmblemMgr; }	

	// UPnP
protected:
	UPnP *m_pUPnP;

public:
	bool CreateUPnP(unsigned short nUDPPort);
	bool DestroyUPnP();
	UPnP* GetUDPInfo()		{ return m_pUPnP; }

	void ChangeQuestStage();

	void OnRecieveGambleItem( unsigned int nRecvItem, unsigned int nCnt, unsigned int nTime);

	// �ڱ��ڽ��� ���������� MD5 ���� ���ؼ� �װ��� ��ȣȭ�Ѵ�.
    void GetEncryptMD5HashValue(char* szEncryptMD5Value);							// update sgk 0724

protected:
	void OnDuelTournamentPrepare(MDUELTOURNAMENTTYPE nType, MUID uidStage, void* pBlobPlayerInfo);
	void OnDuelTournamentLaunch(const MUID& uidStage, const char* pszMapName);

protected:
	ZCommandUDPHackShield m_UPDCommadHackShield;
public:
	bool IsUDPCommandValidationCheck(int nCommandID);

protected:
	void OnAdminResponseKickPlayer(int nResult);
	void OnAdminResponseBlockPlayer(int nResult);
	void OnAdminResponseMutePlayer(int nResult);
};





bool ZPostCommand(MCommand* pCmd);

// �� �÷��̾��� uid
#define ZGetMyUID() (ZGetGameClient() ? ZGetGameClient()->GetPlayerUID() : MUID(0,0))


MCommand* ZNewCmd(int nID);

unsigned long int ZGetClockDistance(unsigned long int nGlobalClock, unsigned long int nLocalClock);

// Post Command Macro For Convenience
#define ZPOSTCMD0(_ID)									{ MCommand* pC=ZNewCmd(_ID); ZPostCommand(pC); }
#define ZPOSTCMD1(_ID, _P0)								{ MCommand* pC=ZNewCmd(_ID); pC->AP(_P0); ZPostCommand(pC); }
#define ZPOSTCMD2(_ID, _P0, _P1)						{ MCommand* pC=ZNewCmd(_ID); pC->AP(_P0); pC->AP(_P1); ZPostCommand(pC); }
#define ZPOSTCMD3(_ID, _P0, _P1, _P2)					{ MCommand* pC=ZNewCmd(_ID); pC->AP(_P0); pC->AP(_P1); pC->AP(_P2); ZPostCommand(pC); }
#define ZPOSTCMD4(_ID, _P0, _P1, _P2, _P3)				{ MCommand* pC=ZNewCmd(_ID); pC->AP(_P0); pC->AP(_P1); pC->AP(_P2); pC->AP(_P3); ZPostCommand(pC); }
#define ZPOSTCMD5(_ID, _P0, _P1, _P2, _P3, _P4)			{ MCommand* pC=ZNewCmd(_ID); pC->AP(_P0); pC->AP(_P1); pC->AP(_P2); pC->AP(_P3); pC->AP(_P4); ZPostCommand(pC); }
#define ZPOSTCMD6(_ID, _P0, _P1, _P2, _P3, _P4, _P5)	{ MCommand* pC=ZNewCmd(_ID); pC->AP(_P0); pC->AP(_P1); pC->AP(_P2); pC->AP(_P3); pC->AP(_P4); pC->AP(_P5); ZPostCommand(pC); }
#define ZPOSTCMD7(_ID, _P0, _P1, _P2, _P3, _P4, _P5, _P6)	{ MCommand* pC=ZNewCmd(_ID); pC->AP(_P0); pC->AP(_P1); pC->AP(_P2); pC->AP(_P3); pC->AP(_P4); pC->AP(_P5); pC->AP(_P6); ZPostCommand(pC); }
#define ZPOSTCMDLEAD(_ID, _UID, _P0)								{ MCommand* pC=ZNewCmd(_ID); pC->AP(_P0); pC->m_Receiver = _UID; ZPostCommand(pC); }
#define ZPOSTANTILEAD(_P0, _P1, _UID)	{ MCommand* pC=ZNewCmd(MC_GUNZ_ANTILEAD); pC->AP(_P0); pC->AP(_P1); pC->SetReceiverUID(_UID); ZPostCommand(pC); }
#define ZPOSTDMGTAKEN(_P0, _P1, _UID)	{ MCommand* pC=ZNewCmd(MC_GUNZ_DMGGIVEN); pC->AP(_P0); pC->AP(_P1); pC->SetReceiverUID(_UID); ZPostCommand(pC); }
#define ZPOSTHITRESPONSE(_P0, _UID)	{ MCommand* pC=ZNewCmd(MC_MATCH_HIT_REPLY); pC->AP(_P0); pC->SetReceiverUID(_UID); ZPostCommand(pC); }
#define ZPOSTLASTDMG(_P0, _P1)	{ MCommand* pC=ZNewCmd(MC_GUNZ_LASTDMG); pC->AP(_P0); pC->AP(_P1); ZPostCommand(pC); }

#define HANDLE_COMMAND(message, fn)    \
	case (message): return fn(pCommand);

bool GetUserInfoUID(MUID uid,MCOLOR& _color,char* sp_name,MMatchUserGradeID& gid);


#endif