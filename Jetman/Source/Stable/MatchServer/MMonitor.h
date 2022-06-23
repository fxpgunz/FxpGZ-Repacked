#ifndef MMONITOR_H
#define MMONITOR_H

#include "MClient.h"
#include "MMap.h"

class MMonitorCommandProcessor;
class MCommand;
struct MOBJECTBASICINFO;
struct MOBJECTCACHEINFO;
struct MBLOB_BASICINFO_UPDATE;


/// �Ϲ� Ŀ�´������Ϳ� �ǻ� ������ ������ ���� Monitor
class MMonitor : public MClient{
protected:
	MMapManager	m_MapManager;	///< ����Ϳ��� ������ ��

protected:
	// socket event
	virtual bool OnSockConnect(SOCKET sock);
	virtual bool OnSockDisconnect(SOCKET sock);
	virtual bool OnSockRecv(SOCKET sock, char* pPacket, DWORD dwSize);
	virtual void OnSockError(SOCKET sock, SOCKET_ERROR_EVENT ErrorEvent, int &ErrorCode);



	virtual void OnRegisterCommand(MCommandManager* pCommandManager);
	virtual bool OnCommand(MCommand* pCommand);
	virtual void BrowseMap(const char* szMapName) = 0;
	virtual void OnResponseObjectList(int nMapID, int nCellID, MOBJECTBASICINFO* pObjBIArray, int nObjBIArrayCount);
	virtual void OnResponseObjectList(int nMapID, MOBJECTBASICINFO* pObjBIArray, int nObjBIArrayCount);
	virtual void OnResponseObjectCacheUpdate(MOBJECTCACHEINFO* pObjBIArray, int nObjBIArrayCount);
	virtual void OnResponseObjectBasicInfoUpdate(MBLOB_BASICINFO_UPDATE* pObjBlArray, int nObjBlArrayCount);

	/// ���� ������ ����Ѵ�.
	void OutputLocalInfo(void);
public:
	MMonitor(void);

	
	bool Create(int nPort);
	void Destroy(void);
	

	/// Ŀ�´������� ������ �α� �޼����� ����� ������� �����̷���
	virtual void Log(const char* szLog){ OutputMessage(szLog, MZMOM_LOCALREPLY); }

	virtual int OnConnected(MUID* pTargetUID, MUID* pAllocUID, MCommObject* pCommObj);

	/// ����Ǿ� �ִ� ����
	MUID GetConnectedServer(void){ return m_Server; }
};



// The monitor and the server because it is not separated in a temporary function defined
class MCommandCommunicator;
void MTempSetZoneServer(MCommandCommunicator* pServer);
MCommandCommunicator* MTempGetZoneServer(void);
void MTempSetMaster(MCommandCommunicator* pMaster);
MCommandCommunicator* MTempSetMaster(void);


#endif
