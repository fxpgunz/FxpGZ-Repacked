#include "stdafx.h"
#include "ZBandiCapturer.h"
#include "FileInfo.h"
#include <sys/stat.h>

///////////////////////////////////////////////////////////////////////

static BCAP_CONFIG g_nConfig;

BCAP_PRESET a_Resolution[3] = { BCAP_PRESET_320x240, BCAP_PRESET_640x480, BCAP_PRESET_DEFAULT };
enum BCAP_FILESIZE		// 파일크기(용량제한)
{
	BCAP_FILESIZE_100MB,
	BCAP_FILESIZE_300MB,
	BCAP_FILESIZE_500MB,
	BCAP_FILESIZE_UNLIMITED,
};
BCAP_FILESIZE m_eFileSize;

void SetBandiCaptureConfig(int iResolution)
{// 게임중 "옵션"에서 접근하기 위한 함수
	if (iResolution > 2) // 원하지 않는 값이 나올시...임시임
		iResolution = 0;
	BCapConfigPreset(&g_nConfig, a_Resolution[iResolution]);	// 프리셋 적용
}
void SetBandiCaptureFileSize(int iFileSize)
{// 게임중 "옵션"에서 접근하기 위한 함수
	if (iFileSize > 3) // 원하지 않는 값이 나올시...임시임
		iFileSize = 0;
	m_eFileSize = (BCAP_FILESIZE)iFileSize;
}

bool ZBandiCapturer::Init(HWND nHwnd, LPDIRECT3DDEVICE9 pDevice)
{
	m_hHandle = nHwnd;
	m_pDevice = pDevice;

	if (FAILED(m_bandiCaptureLibrary.Create(BANDICAP_DLL_FILE_NAME)))
	{
		_ASSERT(0);
		return false;
	}

	if (FAILED(m_bandiCaptureLibrary.Verify("MAIET-GUNZ-20080916", "d25f910a")))
	{
		ASSERT(0);
		return false;
	}

	m_bInited = true;
	m_fToggleFileSize = 0;
	m_lFileSize = 0;

	return true;
}

bool ZBandiCapturer::Start()
{
#ifdef _DEBUG
	return false;
#endif
	if (m_bandiCaptureLibrary.IsCapturing() == FALSE)
	{
		BCAP_CONFIG cfg = GetConfig();

		//// 화질/음질
		//BCapConfigPreset(&cfg, BCAP_PRESET_MJPEG_HIGH_QUALITY);	// 프리셋 적용
		m_bandiCaptureLibrary.CheckConfig(&cfg);	// 잘못된 설정 바로 잡기
		m_bandiCaptureLibrary.SetConfig(&cfg);		// 설정 적용
		m_bandiCaptureLibrary.SetMinMaxFPS(15, 999);	// 최소, 최대 프레임 설정

		// 현재시간을 이용해서 파일명을 만듭니다. 경로명은 가급적 사용자의 "내 문서" 폴더로 지정해 주세요.
		TCHAR	folderName[MAX_PATH];
		//TCHAR	pathName[MAX_PATH];

		GetFolder(folderName);
		m_bandiCaptureLibrary.MakePathnameByDate(folderName, _T("Gunz"), m_pathName, MAX_PATH);

		// 캡처를 시작합니다.
		HRESULT hr = m_bandiCaptureLibrary.Start(m_pathName, m_hHandle, BCAP_MODE_D3D9_SCALE, (LONG_PTR)m_pDevice);
		mlog("path: %s f name: %s", m_pathName, folderName);
		if (FAILED(hr))
		{
			_ASSERT(0);
			return false;
		}
	}
	else
	{
		m_bandiCaptureLibrary.Stop();
	}

	return true;
}


void ZBandiCapturer::Stop()
{
	if (m_bandiCaptureLibrary.IsCapturing())
	{
		m_bandiCaptureLibrary.Stop();
		m_fToggleFileSize = 0;
		m_lFileSize = 0;
	}
}

bool ZBandiCapturer::IsCapturing()
{
#ifdef _DEBUG
	return false;
#endif
	return (m_bandiCaptureLibrary.IsCapturing() == TRUE);
}

void ZBandiCapturer::DrawCapture(MDrawContext* pDC)
{
	if (m_bandiCaptureLibrary.IsCapturing())
	{
		m_bandiCaptureLibrary.Work((LONG_PTR)m_pDevice);

		// 캡처중임을 알리는 메시지 출력
		OnDrawCapture(pDC);
	}

}

void ZBandiCapturer::OnDrawCapture(MDrawContext* pDC)
{
	MFont *pFont = MFontManager::Get("FONTa10_O2Wht");

	pDC->SetFont(pFont);
	pDC->SetColor(MCOLOR(0xFFFF0000));

	char szTemp[64];
	// 녹화시간표시
	sprintf(szTemp, "Video REC %.3f(sec)", (float)m_bandiCaptureLibrary.GetCaptureTime() / 1000);
	pDC->Text(RGetScreenWidth() - 180, 15, szTemp);

	switch (m_eFileSize)
	{
	case BCAP_FILESIZE_100MB:		{	if (m_lFileSize > 100000000)	Stop();		sprintf(szTemp, "[100MB]");		}break;
	case BCAP_FILESIZE_300MB:		{	if (m_lFileSize > 300000000)	Stop();		sprintf(szTemp, "[300MB]");		}break;
	case BCAP_FILESIZE_500MB:		{	if (m_lFileSize > 500000000)	Stop();		sprintf(szTemp, "[500MB]");		}break;
	case BCAP_FILESIZE_UNLIMITED:	{	sprintf(szTemp, "[Unlimited]");		}break;
	}
	if (m_bandiCaptureLibrary.GetCaptureTime() - m_fToggleFileSize > 1000)	// 녹화 파일크기
	{ // 1초에 한번씩만 갱신
		m_lFileSize = get_file_length(m_pathName);
		m_fToggleFileSize = m_bandiCaptureLibrary.GetCaptureTime();
	}
	// 녹화 파일크기 표시
	sprintf(szTemp, "%s %.3fMB", szTemp, (float)m_lFileSize / 1000000);
	pDC->Text(RGetScreenWidth() - 180, 30, szTemp);
}

void ZBandiCapturer::ToggleStart()
{
	if (IsCapturing() == false)
	{
		Start();
	}
	else
	{
		Stop();
	}
}

BCAP_CONFIG ZBandiCapturer::GetConfig()
{
	//BCAP_CONFIG cfg;

	g_nConfig.VideoCodec = FOURCC_MJPG;
	g_nConfig.VideoFPS = 24;
	return g_nConfig;
}

void ZBandiCapturer::GetFolder(TCHAR* pPath)
{
	char foldername[_MAX_PATH] = "c:\\";

	if (GetMyDocumentsPath(pPath))
	{
		strcpy(foldername, pPath);
		strcat(foldername, GUNZ_FOLDER);
		CreatePath(foldername);
		strcat(foldername, "/Video");
		CreatePath(foldername);
		strcat(foldername, "/");
	}
	strcpy(pPath, foldername);
}
void ZBandiCapturer::SetResolution(int iResolution)
{
	m_iResolution = iResolution;
}
int ZBandiCapturer::GetResolution()
{
	return m_iResolution;
}

int ZBandiCapturer::get_file_length(char *fn)
{
	struct stat sbuf;
	int   len;

	if (stat(fn, &sbuf))
	{
		printf("File %s stat error\n", fn);
		len = 0;
	}
	else
	{
		printf("File size = %d\n", sbuf.st_size);

		len = sbuf.st_size;
	}
	return len;
}