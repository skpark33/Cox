#pragma once



#include <list>
#include <mutex>
#include <memory>



// \brief
// 모듈의 기능 및 설명 사용방법 기술.
//
// \code
// - 샘플 코드를 넣어주세요.
// \encode
// \warning
// \sa
// \author	이현석
// \date		2021-07-26 17:32:52		최초작성
typedef struct _bonjr_camera_info_
{
	CString			m_strModel;
	CString			m_strName;
	uint32_t		m_nOEMCode;
	uint32_t		m_nTCamApp;
	uint32_t		m_nROMTempMask;
	uint32_t		m_nENGTempMask;
	uint32_t		m_nTempMode;
	int32_t			m_nUseDHCP;
	CString			m_strIPAddress;
	uint32_t		m_nPort;
	CString			m_strNetMask;
	CString			m_strGateWay;
	CString			m_strMacAddress;
	CString			m_strSWVer;
	CString			m_strBuildDate;
	CString			m_strBuildTime;
	CString			m_strEngVer;
	CString			m_strSerial;
	CString			m_strAlias;
	CString			m_strCameraID;
	CString			m_strGroupID;
	BOOL			m_bManaualAdd;

	~_bonjr_camera_info_()
	{
	}

	_bonjr_camera_info_()
		: m_strModel(_T(""))
		, m_strName(_T(""))
		, m_nOEMCode(0)
		, m_nTCamApp(0)
		, m_nROMTempMask(0)
		, m_nENGTempMask(0)
		, m_nTempMode(0)
		, m_nUseDHCP(0)
		, m_strIPAddress(_T(""))
		, m_nPort(0)
		, m_strNetMask(_T(""))
		, m_strGateWay(_T(""))
		, m_strMacAddress(_T(""))
		, m_strSWVer(_T(""))
		, m_strBuildDate(_T(""))
		, m_strBuildTime(_T(""))
		, m_strEngVer(_T(""))
		, m_strSerial(_T(""))
		, m_strAlias(_T(""))
		, m_strCameraID(_T(""))
		, m_strGroupID(_T(""))
		, m_bManaualAdd(FALSE)
	{}

	_bonjr_camera_info_(_bonjr_camera_info_* rhs)
	{
		SetCam(rhs);
	}

	BOOL		IsSameCam(_bonjr_camera_info_* a_pOtherCamera)
	{
		return (this->m_strIPAddress == a_pOtherCamera->m_strIPAddress)
				&& (this->m_nPort == a_pOtherCamera->m_nPort);
	}

	BOOL		IsValidID()
	{
		return !this->m_strIPAddress.IsEmpty() && !this->m_strGroupID.IsEmpty();
	}

	void		SetCam(_bonjr_camera_info_* rhs)
	{
		this->m_strModel		= rhs->m_strModel;
		this->m_strName			= rhs->m_strName;
		this->m_strAlias		= rhs->m_strAlias;
		this->m_nOEMCode		= rhs->m_nOEMCode;
		this->m_nTCamApp		= rhs->m_nTCamApp;
		this->m_nROMTempMask	= rhs->m_nROMTempMask;
		this->m_nENGTempMask	= rhs->m_nENGTempMask;
		this->m_nTempMode		= rhs->m_nTempMode;
		this->m_nUseDHCP		= rhs->m_nUseDHCP;
		this->m_strIPAddress	= rhs->m_strIPAddress;
		this->m_nPort			= rhs->m_nPort;
		this->m_strNetMask		= rhs->m_strNetMask;
		this->m_strGateWay		= rhs->m_strGateWay;
		this->m_strMacAddress	= rhs->m_strMacAddress;
		this->m_strSWVer		= rhs->m_strSWVer;
		this->m_strBuildDate	= rhs->m_strBuildDate;
		this->m_strBuildTime	= rhs->m_strBuildTime;
		this->m_strEngVer		= rhs->m_strEngVer;
		this->m_strSerial		= rhs->m_strSerial;
		this->m_strCameraID		= rhs->m_strCameraID;
		this->m_strGroupID		= rhs->m_strGroupID;
		this->m_bManaualAdd		= rhs->m_bManaualAdd;
	}

	void		Reset()
	{
		this->m_strModel		= _T("");
		this->m_strName			= _T("");
		this->m_strAlias		= _T("");
		this->m_nOEMCode		= 0;
		this->m_nTCamApp		= 0;
		this->m_nROMTempMask	= 0;
		this->m_nENGTempMask	= 0;
		this->m_nTempMode		= 0;
		this->m_nUseDHCP		= 0;
		this->m_strIPAddress	= _T("");
		this->m_strMacAddress	= _T("");
		this->m_nPort			= 0;
		this->m_strNetMask		= _T("");
		this->m_strGateWay		= _T("");
		this->m_strSWVer		= _T("");
		this->m_strBuildDate	= _T("");
		this->m_strBuildTime	= _T("");
		this->m_strEngVer		= _T("");
		this->m_strSerial		= _T("");
		this->m_strCameraID		= _T("");
		this->m_strGroupID		= _T("");
		this->m_bManaualAdd		= FALSE;
	}

	CString		GetItemName(IN BOOL a_bNewLine = TRUE)
	{
		CString strText;
		if( this->m_strAlias.IsEmpty() ) {
			strText = this->m_strIPAddress;
		}
		else {
			if( a_bNewLine ) {
				strText.Format(_T("%s\r\n(%s)"),
							   this->m_strAlias,
							   this->m_strIPAddress);
			}
			else {
				strText.Format(_T("%s (%s)"),
							   this->m_strAlias,
							   this->m_strIPAddress);
			}
		}
		return strText;
	}
} BONJR_CAM_INFO;



enum
{
	NOTIFY_ADD_CAMERA = 0,
	NOTIFY_BONJR_FINISH,
};


// \brief
// 모듈의 기능 및 설명 사용방법 기술.
//
// \code
// - 샘플 코드를 넣어주세요.
// \encode
// \warning
// \sa
// \author	이현석
// \date		2021-07-14 15:45:18		최초작성
class CBonjourMan
{
public:
	friend CBonjourMan& theBonjrMan();

	~CBonjourMan();

	BOOL				IsInit();

	void				InitBonjour(
							_In_ CWnd*					a_pParentWnd);

	void				WaitforInitBonjrFinish();

	void				closeBonjourService();

	void				RefreshBonjour(
							_In_ CWnd*					a_pParentWnd);

	UINT				GetCameraListSize();

	BONJR_CAM_INFO*		GetCameraInfo(
							_In_ UINT					a_nIdx);

	CString				Split(
							_In_ const CString&			a_strSrc,
							_In_ const CString&			a_strToken,
							_In_ int					a_nListCnt,
							_In_ int					a_nGetIdx);

	int					Split(
							_In_ const CString&			a_strSrc,
							_In_ const CString&			a_strToken,
							_Out_ CStringList*			a_pList);

	void				RunThreadProc();

	void				RunTimeoutThread();

private:
	CBonjourMan();

	BOOL				startBonjourService();

	static THREADFUNC	ThreadProc(
							_In_ LPVOID					a_pVoid);

	static THREADFUNC	TimeoutThread(
							_In_ LPVOID					a_pVoid);

	void				AddBonjrCamera(
							_In_ BONJR_CAM_INFO*		a_pCamera);

	void				BrowseResolve(
							_In_ uint32_t				a_nIdx,
							_In_ const char *			a_szReplyName,
							_In_ const char *			a_szReplyType,
							_In_ const char *			a_szReplyDomain);

	static int			CopyLables(
							_In_ char*					a_pszDst,
							_In_ const char*			a_pLim,
							_In_ const char **			a_ppSrcp,
							_In_ int					a_nLables);

	static void			DNSSD_API BrowseReply(
							_In_ DNSServiceRef			a_ServiceRef,
							_In_ const DNSServiceFlags	a_Flags,
							_In_ uint32_t				a_nIdx,
							_In_ DNSServiceErrorType	a_nErrCode,
							_In_ const char *			a_szReplyName,
							_In_ const char *			a_szReplyType,
							_In_ const char *			a_szReplyDomain,
							_In_ void *					a_pContext);

	static void			DNSSD_API ResolveReply(
							_In_ DNSServiceRef			a_ServiceRef,
							_In_ DNSServiceFlags		a_Flags,
							_In_ uint32_t				a_nIdx,
							_In_ DNSServiceErrorType	a_nErrCode,
							_In_ const char *			a_szFullName,
							_In_ const char *			a_szHostTaget,
							_In_ uint16_t				a_nPort,
							_In_ uint16_t				a_nTxtLen,
							_In_ const unsigned char *	a_szTxtRecord,
							_In_ void *					a_pConText);



private:
	BOOL										m_bInit;
	CWnd*										m_pParentWnd;
	HANDLE										m_hThread;
	UINT										m_nThreadID;
	HANDLE										m_hTimeoutThread;
	UINT										m_nTimeoutThreadID;
	HANDLE										m_hEvtSock[2];
	HANDLE										m_hEvtInit;
	HANDLE										m_hEvtTimeoutClose;

	DNSServiceRef								m_pServiceRef;
	DNSServiceRef								m_pResolveRef;
	SOCKET										m_hSock;
	std::list<std::shared_ptr<BONJR_CAM_INFO> >	m_vList;
	//std::list<BONJR_CAM_INFO*>					m_vList;

	std::mutex									m_hMutex;

	UINT										m_nRelyCount;
	CTickCheck									m_hTimeoutTick;
	DWORD										m_dwTimeout;


	BOOL										m_bNotify;
	CTickCheck									m_hMainTimeoutTick;
	HANDLE										m_hEvtInitFinish;
};

