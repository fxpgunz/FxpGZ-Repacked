#ifndef ZINTERFACEBACKGROUND_H
#define ZINTERFACEBACKGROUND_H




#include "RBspObject.h"
using namespace RealSpace2;

#ifdef _CHURCH

#define LOGIN_SCENE_FIXEDSKY	0			// �ϴÿ� ī�޶� ����
#define LOGIN_SCENE_FALLDOWN	1			// ī�޶� �������鼭 �ΰ� ����

class ZInterfaceBackground {
private:
	RBspObject* m_pChurch;
	RBspObject* m_pChurchEnd;
	ZMapDesc*   m_pMapDesc;
	RMesh*		m_pPigeonMesh;
	RVisualMeshMgr* m_VMeshMgr;
	RVisualMesh* m_pVMesh;
	bool		m_bReverseCam;
	bool		m_bForcedChange;
	int			m_nSceneNumber;
	int			m_nPrevSceneNumber;
	DWORD		m_nSceneChangedTime;
	DWORD		m_nReverseCamTime;
	rvector		m_vCamPos;
	rvector		m_vCharPos;
	rvector		m_vCharDir;
	rvector 	m_EndCamPos;
	rvector		m_EndCamDir;
	rmatrix		m_matWorld;
protected:
	void SetFogState(float fStart, float fEnd, unsigned long int nColor);
public:
	ZInterfaceBackground();
	virtual ~ZInterfaceBackground();
	void SetScene(int nSceneNumber);
	void LoadMesh();
	void Free();
	void SetRenderState();
	void OnInvalidate();
	void OnRestore();
	void ReverseCam();
	void SetForcedChange(bool bForcedChange);
	void Draw();
	void OnUpdate(float fElapsed);
	bool  CreatePigeon(int nNumPigeon);
	void UpdatePigeon();
	int  SetRandomPigeon(RVisualMesh* pVMesh, float fAdd);
	RBspObject* const	GetChurchEnd() { return m_pChurchEnd; }
	rvector& GetCharPos() { return m_vCharPos; }
	rvector& GetCharDir() { return m_vCharDir; }

};
#else

#define LOGIN_SCENE_FIXEDSKY	0			// �ϴÿ� ī�޶� ����
#define LOGIN_SCENE_FALLDOWN	1			// ī�޶� �������鼭 �ΰ� ����
#define LOGIN_SCENE_FIXEDCHAR	2			// ĳ�������� ī�޶� ����
#define LOGIN_SCENE_SELECTCHAR	3			// ĳ���� ���� ��
#define LOGIN_SCENE_MAIN		4

class ZInterfaceBackground{
private:
	RBspObject*	m_pLogin;					// ���
	ZMapDesc* m_pMapDesc;

	rmatrix		m_matWorld;					// ���� ��

	int			m_nSceneNumber;				// ��� ��ȣ
	
	rvector		m_vCamPosSt;				// ī�޶� ���� ��ġ
	rvector		m_vCamPosEd;				// ī�޶� ���� ��ġ
	rvector		m_vCamDirSt;				// ī�޶� ���� ����
	rvector		m_vCamDirEd;				// ī�޶� ���� ��ġ
	rvector		m_vCharPos;					// ĳ���� ��ġ
	rvector		m_vCharDir;					// ĳ���� ����

	DWORD		m_dwClock;

//	bool		m_bShowMaietLogo;			// Maiet logo ����


protected:
	void SetFogState( float fStart, float fEnd, unsigned long int color);


public:
	ZInterfaceBackground( void);
	virtual ~ZInterfaceBackground( void);

	RBspObject*	const GetChurchEnd() { return m_pLogin; }
	int GetScene() { return m_nSceneNumber; }
	void SetScene(int nSceneNumber);

	void LoadMesh( void);
	void Free(void);
	void Draw(void);

	void OnUpdate(float fElapsed);
	void OnInvalidate();
	void OnRestore();

	rvector& GetCharPos() { return m_vCharPos; }
	rvector& GetCharDir() { return m_vCharDir; }
};

#endif

#endif
