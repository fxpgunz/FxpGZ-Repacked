#ifndef MBMATCHAGENT_H
#define MBMATCHAGENT_H

#include "MMatchAgent.h"

class COutputView;
class CCommandLogView;

class MBMatchAgent : public MMatchAgent {
public:
	COutputView*		m_pView;
	CCommandLogView*	m_pCmdLogView;

protected:
	/// Create()ȣ��ÿ� �Ҹ��� �Լ�
	virtual bool OnCreate(void);
	/// Destroy()ȣ��ÿ� �Ҹ��� �Լ�
	virtual void OnDestroy(void);
	/// Ŀ�ǵ带 ó���ϱ� ����
	virtual void OnPrepareCommand(MCommand* pCommand);
	/// ����� Ŀ�ǵ� ó��
	virtual bool OnCommand(MCommand* pCommand);
public:
	MBMatchAgent(COutputView* pView=NULL);
	virtual void Log(unsigned int nLogLevel, const char* szLog);
};


#endif
