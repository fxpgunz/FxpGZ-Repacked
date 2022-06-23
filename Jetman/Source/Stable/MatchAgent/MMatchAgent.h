#ifndef MMATCHAGENT_H
#define MMATCHAGENT_H

#include "winsock2.h"
#include "MServer.h"
#include "MSafeUDP.h"
#include "MAgentClient.h"
#include "MStageAgent.h"
#include "MPacketCrypter.h"


class MMatchAgent : public MServer {
private:
	static MMatchAgent*		m_pInstance;		///< ���� �ν��Ͻ�

protected:
	MUID				m_uidMatchServer;

	MUID				m_uidNextAlloc;
	
	MAgentClients		m_Clients;
	MStageAgents		m_Stages;

	MSafeUDP			m_SafeUDP;
	MPacketCrypter		m_MatchServerPacketCrypter;					///< ��ȣȭ Ŭ����
protected:
	char	m_szIP[64];
	int		m_nTCPPort;
	int		m_nUDPPort;

	bool	m_bMatchServerConnectTrying;
	bool	m_bMatchServerConnected;
	UINT	m_nConnectedTime;
	char	m_szMatchServerIP[64];
	int		m_nMatchServerTCPPort;
	
	UINT	m_nLastLiveCheckSendTime;
	UINT	m_nLastLiveCheckRecvTime;

public:
	void SetIP(char* pszIP)					{ strcpy(m_szIP, pszIP); }
	char* GetIP()							{ return m_szIP; }
	void SetTCPPort(int nPort)				{ m_nTCPPort = nPort; }
	int GetTCPPort()						{ return m_nTCPPort; }
	void SetUDPPort(int nPort)				{ m_nUDPPort = nPort; }
	int GetUDPPort()						{ return m_nUDPPort; }

	void SetMatchServerTrying(bool bVal)	{ m_bMatchServerConnectTrying = bVal; }
	bool GetMatchServerTrying()				{ return m_bMatchServerConnectTrying; }
	void SetMatchServerConnected(bool bVal)	{ m_bMatchServerConnected = bVal; }
	bool GetMatchServerConnected()			{ return m_bMatchServerConnected; }
	void SetConnectedTime(UINT nTime)		{ m_nConnectedTime = nTime; }
	UINT GetConnectedTime()					{ return m_nConnectedTime; }
	void SetMatchServerIP(char* pszIP)		{ strcpy(m_szMatchServerIP, pszIP); }
	char* GetMatchServerIP()				{ return m_szMatchServerIP; }
	void SetMatchServerTCPPort(int nPort)	{ m_nMatchServerTCPPort = nPort; }
	int GetMatchServerTCPPort()				{ return m_nMatchServerTCPPort; }

	void SetLastLiveCheckSendTime(unsigned long nTime)	{ m_nLastLiveCheckSendTime = nTime; }
	unsigned long GetLastLiveCheckSendTime()			{ return m_nLastLiveCheckSendTime; }
	void SetLastLiveCheckRecvTime(unsigned long nTime)	{ m_nLastLiveCheckRecvTime = nTime; }
	unsigned long GetLastLiveCheckRecvTime()			{ return m_nLastLiveCheckRecvTime; }

	unsigned long GetStageCount()			{ return (unsigned long)m_Stages.size(); }
	unsigned long GetClientCount()			{ return (unsigned long)m_Clients.size(); }

public:
	MMatchAgent(void);
	virtual ~MMatchAgent(void);

	/// ���� �ν��Ͻ� ���
	static MMatchAgent* GetInstance(void)	{ return m_pInstance; }

	/// �ʱ�ȭ
	bool Create(int nPort);
	/// ����
	void Destroy(void);

	/// MatchServer�� �����Ѵ�.
	void ConnectToMatchServer(char* pszAddr, int nPort);
	/// MatchServer�� ������ �����Ѵ�.
	void DisconnectFromMatchServer();

protected:
	/// Create()ȣ��ÿ� �Ҹ��� �Լ�
	virtual bool OnCreate(void);
	/// Destroy()ȣ��ÿ� �Ҹ��� �Լ�
	virtual void OnDestroy(void);
	/// ����� Ŀ�ǵ� ���
	virtual void OnRegisterCommand(MCommandManager* pCommandManager);
	/// ����� Ŀ�ǵ� ó��
	virtual bool OnCommand(MCommand* pCommand);
	/// ����� ����
	virtual void OnRun(void);

	/// �ʱ�ȭ
	void OnInitialize();

	MUID GetMatchServerUID()	{ return m_uidMatchServer; }

	/// ���ο� UID ����
	virtual MUID UseUID(void);

	/// UDP
	MSafeUDP* GetSafeUDP() { return &m_SafeUDP; }
	void SendCommandByUDP(MCommand* pCommand, char* szIP, int nPort);
	static bool UDPSocketRecvEvent(DWORD dwIP, WORD wRawPort, char* pPacket, DWORD dwSize);
	void ParseUDPPacket(char* pData, MPacketHeader* pPacketHeader, DWORD dwIP, WORD wRawPort);
	void ParsePacket(char* pData, MPacketHeader* pPacketHeader, DWORD dwIP, WORD wRawPort);

	/// Ŭ���̾�Ʈ ����
	int ClientAdd(const MUID& uid);
	/// Ŭ���̾�Ʈ ����
	int ClientRemove(const MUID& uid, MAgentClients::iterator* pNextItor);

	/// UID�� ������Ʈ ����
	MAgentClient* GetClient(const MUID& uid);
	/// CommUID�� ������Ʈ ����
	MAgentClient* GetPlayerByCommUID(const MUID& uid);

	MStageAgent* FindStage(const MUID& uidStage);
	bool StageAdd(const MUID& uidStage);
	bool StageRemove(const MUID& uidStage, MStageAgents::iterator* pNextItor);
	bool StageJoin(const MUID& uidPlayer, const MUID& uidStage);
	bool StageLeave(const MUID& uidPlayer, const MUID& uidStage);

protected:
	/// Ŀ�ؼ��� �޾Ƶ��� ���
	virtual int OnAccept(MCommObject* pCommOBj);
	/// Ŀ�ؼ��� �̷���� ���
	virtual int OnConnected(MUID* pTargetUID, MUID* pAllocUID, unsigned int nTimeStamp, MCommObject* pCommObj);
	/// Ŀ�ؼ��� ����� ���
	virtual int OnDisconnect(MCommObject* pCommObj);
	virtual void OnAgentLocalLogin(const MUID& uidComm, const MUID& uidPlayer);

	/// MatchServer�� ������ Ȯ���Ѵ�.
	void OnResponseLiveCheck(unsigned long nTimeStamp);

	void DebugTest();
	void OnDebugTest(const MUID& uidComm, const char* pszMsg);

	void OnStageReserve(const MUID& uidStage);
	void OnRelayPeer(const MUID& uidChar, const MUID& uidPeer, const MUID& uidStage);
	void OnPeerBindTCP(const MUID& uidComm, const MUID& uidChar);
	void OnPeerBindUDP(const MUID& uidChar, char* szLocalIP, int nLocalPort, char* szIP, int nPort);
	void OnPeerUnbind(const MUID& uidComm, const MUID& uidChar);
	void OnTunnelingTCP(const MUID& uidSender, const MUID& uidReceiver, void* pBlob, int nCount);
	void OnTunnelingUDP(const MUID& uidSender, const MUID& uidReceiver, void* pBlob, int nCount);
	void SendPeerTunnel(MAgentClient* pClient, MAgentClient* pTarget, void* pBlob,int nCount);
	void RoutePeerTunnel(MAgentClient* pClient,const MUID& uidReceiver,void* pBlob,int nCount);

public:
	void RouteToListener(MObject* pObject, MCommand* pCommand);
	/// Command�� ���� Stage �����ڿ��� ����
	//void RouteToStage(const MUID& uidStage, MCommand* pCommand);

	/// MatchServer�� ������ Ȯ���Ѵ�.
	void RequestLiveCheck();

	/// ���� Ŭ�� ����
	static unsigned long int GetGlobalClockCount(void);
	/// Local Clock�� Global Clock���� ��ȯ
	static unsigned long int ConvertLocalClockToGlobalClock(unsigned long int nLocalClock, unsigned long int nLocalClockDistance);
	/// Global Clock�� Local Clock���� ��ȯ
	static unsigned long int ConvertGlobalClockToLocalClock(unsigned long int nGlobalClock, unsigned long int nLocalClockDistance);

public: // For Debug
	friend void MDebugUtil_PrintStagePeerList();
};


#endif
