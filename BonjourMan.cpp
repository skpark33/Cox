#include "stdafx.h"
#include "BonjourMan.h"
#include "MyHelperMan.h"

// 
// \brief <pre>
// 내용을 넣으세요.
// </pre>
// 
CBonjourMan& theBonjrMan()
{
	static CBonjourMan man;
	return man;
}


// 
// \brief <pre>
// 내용을 넣으세요.
// </pre>
// 
CBonjourMan::CBonjourMan()
	: m_hSock(INVALID_SOCKET)
	, m_pServiceRef(nullptr)
	, m_pResolveRef(nullptr)
	, m_hEvtInit(nullptr)
	, m_hEvtInitFinish(nullptr)
	, m_hEvtTimeoutClose(nullptr)
	, m_nRelyCount(0)
	, m_bInit(FALSE)
	, m_dwTimeout(2000)
	, m_hThread(nullptr)
	, m_nThreadID(0)
	, m_hTimeoutThread(nullptr)
	, m_nTimeoutThreadID(0)
	, m_bNotify(FALSE)
	, m_pParentWnd(nullptr)
{
	m_hEvtInit			= CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hEvtInitFinish	= CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hEvtTimeoutClose	= CreateEvent(NULL, TRUE, FALSE, NULL);
}


// 
// \brief <pre>
// 내용을 넣으세요.
// </pre>
// 
CBonjourMan::~CBonjourMan()
{
	WSACleanup();

	if( m_hEvtInit != nullptr ) {
		CloseHandle(m_hEvtInit);
		m_hEvtInit = nullptr;
	}

	if( m_hEvtTimeoutClose != nullptr ) {
		CloseHandle(m_hEvtTimeoutClose);
		m_hEvtTimeoutClose = nullptr;
	}

	if( m_hEvtInitFinish != nullptr ) {
		CloseHandle(m_hEvtInitFinish);
		m_hEvtInitFinish = nullptr;
	}
}


// 
// \brief <pre>
// 내용을 넣으세요.
// </pre>
// \return  BOOL 
// 
BOOL CBonjourMan::startBonjourService()
{
	//m_pParentWnd	= a_pParentWnd;
	DNSServiceErrorType nErr;
	int nResult = 0;
	if( m_pServiceRef != nullptr ) {
		return FALSE;
	}

	nErr = DNSServiceCreateConnection(&m_pServiceRef);
	m_hEvtSock[0] = CreateEvent(NULL, TRUE, FALSE, NULL);
	if( kDNSServiceErr_NoError != nErr ) {
		DebugString(_T("[ %s ] [E] DNSServiceCreateConnection Fail. (%d)"),
			__FUNCTIONW__, nErr);
		return FALSE;
	}
	else {
		DebugString(_T("[ %s ] DNSServiceCreateConnection Succes. (%d)"),
			__FUNCTIONW__, nErr);
	}

	m_pResolveRef = m_pServiceRef;
	nErr = DNSServiceBrowse(
						&m_pResolveRef,
						kDNSServiceFlagsShareConnection,
						0,
						"_cox-cam._tcp",
						NULL,
						CBonjourMan::BrowseReply,
						nullptr);

	if( kDNSServiceErr_NoError != nErr ) {
		DebugString(_T("[ %s ] [E] DNSServiceBrowse (%d)\r\n"),
			__FUNCTIONW__, nErr);
		return FALSE;
	}

	m_hSock = DNSServiceRefSockFD(m_pServiceRef);
	if( m_hSock == -1 ) {
		DebugString(_T("[ %s ] [E] DNSServiceRefSockFD\r\n"), __FUNCTIONW__);
		return FALSE;
	}

	m_hEvtSock[1] = WSACreateEvent();
	if( WSAEventSelect(m_hSock, m_hEvtSock[1], FD_READ | FD_CLOSE)
														== SOCKET_ERROR )
	{
		DebugString(_T("[ %s ] [E] WSACreateEvent Fail\r\n"), __FUNCTIONW__);
		closeBonjourService();
		return FALSE;
	}
	return TRUE;
}


// 
// \brief <pre>
// 내용을 넣으세요.
// </pre>
// 
void CBonjourMan::InitBonjour(
	_In_ CWnd*				a_pParentWnd)
{
	m_bInit			= FALSE;
	m_pParentWnd	= a_pParentWnd;
	m_bNotify		= TRUE;
	m_hTimeoutTick.ResetTick();

	ResetEvent(m_hEvtInit);
	ResetEvent(m_hEvtInitFinish);
	m_hThread = (HANDLE)_beginthreadex(NULL,
									   0,
									   &CBonjourMan::ThreadProc,
									   this,
									   0,
									   &m_nThreadID);

	ResetEvent(m_hEvtTimeoutClose);
	m_hTimeoutThread = (HANDLE)_beginthreadex(NULL,
											  0,
											  &CBonjourMan::TimeoutThread,
											  this,
											  0,
											  &m_nTimeoutThreadID);

	WaitForSingleObject(m_hEvtInit, INFINITE);
	WaitforInitBonjrFinish();
}


// 
// \brief <pre>
// 내용을 넣으세요.
// </pre>
// 
void CBonjourMan::WaitforInitBonjrFinish()
{
	if( IsInit() ) {
		BOOL bLoopFlag = TRUE;
		while( bLoopFlag ) {
			MSG msg;
			if( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) ) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			switch( MsgWaitForMultipleObjects(1,
											  &m_hEvtInitFinish,
											  FALSE,
											  10,
											  QS_ALLINPUT) )
			{
				case WAIT_OBJECT_0:
					bLoopFlag = FALSE;
					break;

				case WAIT_TIMEOUT:
				case WAIT_OBJECT_0 + 1:
				default:
					if( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) ) {
						TranslateMessage(&msg);
						DispatchMessage(&msg);
					}
					if( m_hMainTimeoutTick.IsPass(5000) ) {
						bLoopFlag = FALSE;
						m_bNotify = FALSE;
					}
			}
		}
	}
}


// 
// \brief <pre>
// 내용을 넣으세요.
// </pre>
// \return  BOOL 
// 
BOOL CBonjourMan::IsInit()
{
	return m_bInit;
}


// 
// \brief <pre>
// 내용을 넣으세요.
// </pre>
// 
void CBonjourMan::closeBonjourService()
{
	if( m_pServiceRef == nullptr ) {
		return;
	}

	if( m_hThread != nullptr ) {
		SetEvent(m_hEvtSock[0]);
		WaitForSingleObject(m_hThread, INFINITE);
	}

	if( m_hTimeoutThread != nullptr ) {
		SetEvent(m_hEvtTimeoutClose);
		WaitForSingleObject(m_hTimeoutThread, INFINITE);
	}

	CloseHandle(m_hEvtSock[0]);
	if( m_hEvtSock[1] ) {
		WSACloseEvent(m_hEvtSock[1]);
		m_hEvtSock[1] = nullptr;
	}

	closesocket(m_hSock);

	DNSServiceRefDeallocate(m_pServiceRef);
	m_pServiceRef	= nullptr;
	m_nRelyCount	= 0;
}


// 
// \brief <pre>
// 내용을 넣으세요.
// </pre>
// \param   LPVOID					a_pVoid
// \return  THREADFUNC 
// 
THREADFUNC CBonjourMan::TimeoutThread(
	_In_ LPVOID					a_pVoid)
{
	CBonjourMan* pThis = (CBonjourMan*)a_pVoid;
	if( pThis != nullptr ) {
		pThis->RunTimeoutThread();
	}
	return 0;
}


// 
// \brief <pre>
// 내용을 넣으세요.
// </pre>
// 
void CBonjourMan::RunTimeoutThread()
{
	DebugString(_T("[ BONJRMAN ] Timeout check thread Start."));

	while( TRUE ) {
		DWORD dwRet = WaitForSingleObject(m_hEvtTimeoutClose, 500);

		if( dwRet == WAIT_OBJECT_0 ) {
			DebugString(_T("[ BONJRMAN ] Timeout check thread receive stop signal. break"));
			break;
		}

		if( m_hTimeoutTick.IsPass(m_dwTimeout, TRUE) ) {
			std::unique_lock<std::mutex> lock(m_hMutex);
			if( m_bNotify && m_pParentWnd != nullptr ) {
				m_bNotify = FALSE;
				SetEvent(m_hEvtInitFinish);
				::PostMessage(m_pParentWnd->GetSafeHwnd(),
							  UMSG_NOTIFY,
							  NOTI_MSG_BONJR_FINISH,
							  0);
			}
			break;
		}
	}
	DebugString(_T("[ BONJRMAN ] Timeout check thread End."));
}


// 
// \brief <pre>
// 내용을 넣으세요.
// </pre>
// \param   LPVOID					a_pVoid
// \return  THREADFUNC 
// 
THREADFUNC CBonjourMan::ThreadProc(
	_In_ LPVOID					a_pVoid)
{
	CBonjourMan* pThis = (CBonjourMan*)a_pVoid;
	if( pThis != nullptr ) {
		pThis->RunThreadProc();
	}
	return 0;
}


// 
// \brief <pre>
// 내용을 넣으세요.
// </pre>
// \param   LPVOID			a_pVoid
// \return  BOOL 
// 
void CBonjourMan::RunThreadProc()
{
	DebugString(_T("[ BONJRMAN ] Service thread Start."));

	m_bInit = startBonjourService();
	SetEvent(m_hEvtInit);
	if( !m_bInit ) {
		return;
	}

	WSANETWORKEVENTS events;

	while( TRUE ) {
		DWORD dwRet = WSAWaitForMultipleEvents(2, m_hEvtSock, FALSE, 100, FALSE);
		if( dwRet == WSA_WAIT_TIMEOUT ) {
			continue;
		}

		int nIdx = dwRet - WSA_WAIT_EVENT_0;
		if( nIdx == 0 ) {
			break;
		}

		int nRet = WSAEnumNetworkEvents(m_hSock, m_hEvtSock[1], &events);
		if( events.lNetworkEvents & FD_READ
			|| events.lNetworkEvents & FD_CLOSE )
		{
			DNSServiceErrorType nErr = 0;
			if( m_hSock != (SOCKET)DNSServiceRefSockFD(m_pServiceRef) ) {
				break;
			}
			
			nErr = DNSServiceProcessResult(m_pServiceRef);
			if( kDNSServiceErr_NoError != nErr ) {
				break;
			}
		}
	}

	DebugString(_T("[ BONJRMAN ] Service thread End."));
}


// 
// \brief <pre>
// 내용을 넣으세요.
// </pre>
// \param	DNSServiceRef		a_ServiceRef
// \param	DNSServiceFlags		a_Flags
// \param	uint32_t			a_nIdx
// \param	DNSServiceErrorType	a_nErrCode
// \param	char*				a_szRelyName
// \param	char*				a_szRelyType
// \param	char*				a_szRelyDomain
// \param	void*				a_pContext
// 
void DNSSD_API CBonjourMan::BrowseReply(
	_In_ DNSServiceRef			a_ServiceRef,
	_In_ const DNSServiceFlags	a_Flags,
	_In_ uint32_t				a_nIdx,
	_In_ DNSServiceErrorType	a_nErrCode,
	_In_ const char *			a_szReplyName,
	_In_ const char *			a_szReplyType,
	_In_ const char *			a_szReplyDomain,
	_In_ void *					a_pContext)
{
	theBonjrMan().BrowseResolve(a_nIdx, a_szReplyName, a_szReplyType, a_szReplyDomain);
}


// 
// \brief <pre>
// 내용을 넣으세요.
// </pre>
// \param	char*			a_nIdx
// \param	char*			a_szReplyName
// \param	char*			a_szReplyType
// \param	char*			a_szReplyDomain
// 
void CBonjourMan::BrowseResolve(
	_In_ uint32_t				a_nIdx,
	_In_ const char *			a_szReplyName,
	_In_ const char *			a_szReplyType,
	_In_ const char *			a_szReplyDomain)
{
	m_hTimeoutTick.ResetTick();
	m_nRelyCount++;

	DNSServiceErrorType nErr;
	m_pResolveRef = m_pServiceRef;
	nErr = DNSServiceResolve(
		&m_pResolveRef,
		kDNSServiceFlagsShareConnection,
		a_nIdx,
		a_szReplyName,
		a_szReplyType,
		a_szReplyDomain,
		CBonjourMan::ResolveReply,
		nullptr);

	if( kDNSServiceErr_NoError != nErr ) {
		DebugString(_T(">>> [ E ] DNSServiceResolve Fail (%d)"), nErr);
	}
}


// 
// \brief <pre>
// 내용을 넣으세요.
// </pre>
// \param	a_ServiceRef
// \param	a_Flags
// \param	a_nIdx
// \param	a_nErrCode
// \param	a_szFullName
// \param	a_szHostTarget
// \param	a_nPort
// \param	a_nTxtLen
// \param	a_szTxtRecord
// \param	a_pConText
// 
void DNSSD_API CBonjourMan::ResolveReply(
	_In_ DNSServiceRef			a_ServiceRef,
	_In_ DNSServiceFlags		a_Flags,
	_In_ uint32_t				a_nIdx,
	_In_ DNSServiceErrorType	a_nErrCode,
	_In_ const char *			a_szFullName,
	_In_ const char *			a_szHostTaget,
	_In_ uint16_t				a_nPort,
	_In_ uint16_t				a_nTxtLen,
	_In_ const unsigned char *	a_szTxtRecord,
	_In_ void *					a_pConText)
{
	const unsigned char *	max_txt = a_szTxtRecord + a_nTxtLen;
	const char *			pszTxt = a_szFullName;
	char					szName[kDNSServiceMaxDomainName];
	char					szType[kDNSServiceMaxDomainName];
	char *					pszCopy;
	char					szInfo[64];
	char					szModel[20] = { NULL, };
	char					szIP[20] = { NULL, };
	char					szMac[20] = { NULL, };
	uint16_t				nPortNum;

	nPortNum = ((a_nPort >> 8) & 0x00FF) | ((a_nPort << 8) & 0xFF00);
	if( 0 != CopyLables(szName, szName + kDNSServiceMaxDomainName, &pszTxt, 3) ) { return; }
	pszTxt = a_szFullName;
	if( 0 != CopyLables(szType, szType + kDNSServiceMaxDomainName, &pszTxt, 1) ) { return; }
	if( 0 != CopyLables(szType, szType + kDNSServiceMaxDomainName, &pszTxt, 2) ) { return; }

	BONJR_CAM_INFO stCamera;
	while( a_szTxtRecord < max_txt ) {
		const unsigned char *const end = a_szTxtRecord + 1 + a_szTxtRecord[0];
		::memset(&szInfo[0], 0x00, sizeof(szInfo));
		pszCopy = &szInfo[0];
		a_szTxtRecord++;
		while( a_szTxtRecord < end ) {
			if( *a_szTxtRecord == '\\' || *a_szTxtRecord == '\"' ) {
				a_szTxtRecord++;
				continue;
			}
			*pszCopy++ = *a_szTxtRecord++;
		}
		if( 0 == ::strncmp(szInfo, "model=", 6) ) {
			stCamera.m_strModel = szInfo;
			stCamera.m_strModel = theHelperMan().Split(stCamera.m_strModel, _T("="), 2, 1);
		}
		else if( 0 == ::strncmp(szInfo, "name=", 5) ) {
			stCamera.m_strName = szInfo;
			stCamera.m_strName = theHelperMan().Split(stCamera.m_strName, _T("="), 2, 1);
		}
		else if( 0 == ::strncmp(szInfo, "oem=", 4) ) {
			CString strTemp;
			strTemp = szInfo;
			stCamera.m_nOEMCode = _ttoi(theHelperMan().Split(strTemp, _T("="), 2, 1));
		}
		else if( 0 == ::strncmp(szInfo, "type=", 5) ) {
			CString strTemp;
			strTemp = szInfo;
			stCamera.m_nTCamApp = _ttoi(theHelperMan().Split(strTemp, _T("="), 2, 1));
		}
		else if( 0 == ::strncmp(szInfo, "rtemp=", 6) ) {
			CString strTemp;
			strTemp = szInfo;
			stCamera.m_nROMTempMask = _ttoi(theHelperMan().Split(strTemp, _T("="), 2, 1));
		}
		else if( 0 == ::strncmp(szInfo, "stemp=", 6) ) {
			CString strTemp;
			strTemp = szInfo;
			stCamera.m_nENGTempMask = _ttoi(theHelperMan().Split(strTemp, _T("="), 2, 1));
		}
		else if( 0 == ::strncmp(szInfo, "ctemp=", 6) ) {
			CString strTemp;
			strTemp = szInfo;
			stCamera.m_nTempMode = _ttoi(theHelperMan().Split(strTemp, _T("="), 2, 1));
		}
		else if( 0 == ::strncmp(szInfo, "dhcp=", 5) ) {
			CString strTemp;
			strTemp = szInfo;
			stCamera.m_nUseDHCP = _ttoi(theHelperMan().Split(strTemp, _T("="), 2, 1));
		}
		else if( 0 == ::strncmp(szInfo, "ip=", 3) ) {
			stCamera.m_strIPAddress = szInfo;
			stCamera.m_strIPAddress = theHelperMan().Split(stCamera.m_strIPAddress, _T("="), 2, 1);
		}
		else if( 0 == ::strncmp(szInfo, "port=", 5) ) {
			CString strTemp;
			strTemp = szInfo;
			stCamera.m_nPort = _ttoi(theHelperMan().Split(strTemp, _T("="), 2, 1));
		}
		else if( 0 == ::strncmp(szInfo, "netmask=", 8) ) {
			stCamera.m_strNetMask = szInfo;
			stCamera.m_strNetMask = theHelperMan().Split(stCamera.m_strNetMask, _T("="), 2, 1);
		}
		else if( 0 == ::strncmp(szInfo, "gw=", 3) ) {
			stCamera.m_strGateWay = szInfo;
			stCamera.m_strGateWay = theHelperMan().Split(stCamera.m_strGateWay, _T("="), 2, 1);
		}
		else if( 0 == ::strncmp(szInfo, "mac=", 4) ) {
			stCamera.m_strMacAddress = szInfo;
			stCamera.m_strMacAddress = theHelperMan().Split(stCamera.m_strMacAddress, _T("="), 2, 1);
		}
		else if( 0 == ::strncmp(szInfo, "fwver=", 6) ) {
			stCamera.m_strSWVer = szInfo;
			stCamera.m_strSWVer = theHelperMan().Split(stCamera.m_strSWVer, _T("="), 2, 1);
		}
		else if( 0 == ::strncmp(szInfo, "build_date=", 11) ) {
			stCamera.m_strBuildDate = szInfo;
			stCamera.m_strBuildDate = theHelperMan().Split(stCamera.m_strBuildDate, _T("="), 2, 1);
		}
		else if( 0 == ::strncmp(szInfo, "build_time=", 11) ) {
			stCamera.m_strBuildTime = szInfo;
			stCamera.m_strBuildTime = theHelperMan().Split(stCamera.m_strBuildTime, _T("="), 2, 1);
		}
		else if( 0 == ::strncmp(szInfo, "fpgaver=", 8) ) {
			stCamera.m_strEngVer = szInfo;
			stCamera.m_strEngVer = theHelperMan().Split(stCamera.m_strEngVer, _T("="), 2, 1);
		}
		else if( 0 == ::strncmp(szInfo, "sn=", 3) ) {
			stCamera.m_strSerial = szInfo;
			stCamera.m_strSerial = theHelperMan().Split(stCamera.m_strSerial, _T("="), 2, 1);
		}
	}

	theBonjrMan().AddBonjrCamera(&stCamera);
	DNSServiceRefDeallocate(a_ServiceRef);
}


// 
// \brief <pre>
// 내용을 넣으세요.
// </pre>
// \param   a_pszDst
// \param   a_pLim
// \param   a_ppSrcp
// \param   a_nLables
// \return  int 
// 
int CBonjourMan::CopyLables(
	_In_ char*					a_pszDst,
	_In_ const char*			a_pLim,
	_In_ const char **			a_ppSrcp,
	_In_ int					a_nLables)
{
	const char* pszSrc = *a_ppSrcp;
	while( '.' != *pszSrc || --a_nLables > 0 ) {
		if( '\\' == *pszSrc ) {
			*a_pszDst++ = *pszSrc++;
		}
		if( !*pszSrc || a_pszDst >= a_pLim ) {
			return -1;
		}
		*a_pszDst++ = *pszSrc++;
		if( !*pszSrc || a_pszDst >= a_pLim ) {
			return -1;
		}
	}
	*a_pszDst++ = 0;
	*a_ppSrcp = pszSrc + 1;
	return 0;
}


// 
// \brief <pre>
// 내용을 넣으세요.
// </pre>
// \param   BONJR_CAM_INFO*		a_pCamera
// 
void CBonjourMan::AddBonjrCamera(
	_In_ BONJR_CAM_INFO*		a_pCamera)
{
	class findIP
	{
	public:
		findIP(const CString& a_strIP) : m_strIP(a_strIP) {}
		bool operator() (std::shared_ptr<BONJR_CAM_INFO>& a_rhs)
		//bool operator() (BONJR_CAM_INFO* &a_rhs)
		{
			return a_rhs->m_strIPAddress.Compare(m_strIP) == 0 
						? true : false;
		}
	private:
		CString		m_strIP;
	};

	std::unique_lock<std::mutex> lock(m_hMutex);
	auto it = std::find_if(m_vList.begin(),
						   m_vList.end(),
						   findIP(a_pCamera->m_strIPAddress));

	if( it != m_vList.end() ) {
		DebugString(_T("[ %s ] \"%s\" IP already registered."),
			__FUNCTIONW__, a_pCamera->m_strIPAddress);
		return;
	}
	
	//std::shared_ptr<BONJR_CAM_INFO> ptr	= std::make_shared<BONJR_CAM_INFO>(BONJR_CAM_INFO::Create(a_pCamera));
	/*std::shared_ptr<BONJR_CAM_INFO> ptr = 
		std::make_shared<BONJR_CAM_INFO>(new BONJR_CAM_INFO());*/
	//DebugString(_T("use %d"), ptr.use_count());
	//BONJR_CAM_INFO* pCamera = BONJR_CAM_INFO::Create(a_pCamera);
	//m_vList.push_back(std::make_shared<BONJR_CAM_INFO>(pCamera));

	std::shared_ptr<BONJR_CAM_INFO> pCamera = std::make_shared<BONJR_CAM_INFO>(BONJR_CAM_INFO(a_pCamera));
	m_vList.push_back(pCamera);

	DebugString(GetMakeString(_T("[%d/%d] %s\t%s\t%s\t%d\t%s\t%s\t%s"),
		(UINT)m_vList.size(),
		m_nRelyCount,
		a_pCamera->m_strModel,
		a_pCamera->m_strName,
		a_pCamera->m_strIPAddress,
		a_pCamera->m_nTempMode,
		a_pCamera->m_strSWVer,
		a_pCamera->m_strEngVer,
		a_pCamera->m_strSerial));
	
	DebugString(_T("Recive Count[%d]"), m_nRelyCount);
	if( m_bNotify && m_pParentWnd != nullptr ) {
		::PostMessage(m_pParentWnd->GetSafeHwnd(),
					  UMSG_NOTIFY,
					  NOTI_MSG_BONJR_ADD_CAM,
					  (LPARAM)pCamera.get());
		m_hMainTimeoutTick.ResetTick();
	}
}


// 
// \brief <pre>
// 내용을 넣으세요.
// </pre>
// 
void CBonjourMan::RefreshBonjour(
	_In_ CWnd*					a_pParentWnd)
{
	std::unique_lock<std::mutex> lock(m_hMutex);
	m_vList.clear();
	closeBonjourService();

	InitBonjour(a_pParentWnd);
}


// \brief <pre>
// 내용을 넣으세요.
// </pre>
// \return  UINT 
// 
UINT CBonjourMan::GetCameraListSize()
{
	std::unique_lock<std::mutex> lock(m_hMutex);
	return (UINT)m_vList.size();
}


// 
// \brief <pre>
// 내용을 넣으세요.
// </pre>
// \param   UINT					a_nIdx
// \return  BONJR_CAM_INFO* 
// 
BONJR_CAM_INFO* CBonjourMan::GetCameraInfo(
	_In_ UINT					a_nIdx)
{
	std::unique_lock<std::mutex> lock(m_hMutex);
	if( a_nIdx >= (UINT)m_vList.size() ) {
		return nullptr;
	}

	auto it = m_vList.begin();
	std::advance(it, a_nIdx);
	return NULL;
}


// 
// \brief <pre>
// 내용을 넣으세요.
// </pre>
// \param   CString&		a_strSrc
// \param   CString&		a_strToken
// \param   int				a_nListCnt
// \param   int				a_nGetIdx
// \return  CString 
// 
CString CBonjourMan::Split(
	_In_ const CString&			a_strSrc,
	_In_ const CString&			a_strToken,
	_In_ int					a_nListCnt,
	_In_ int					a_nGetIdx)
{
	CStringList strList;
	if( a_nListCnt == Split(a_strSrc, a_strToken, &strList) ) {
		POSITION pos = strList.FindIndex(a_nGetIdx);
		return strList.GetAt(pos);
	}
	return _T("");
}


// 
// \brief <pre>
// 내용을 넣으세요.
// </pre>
// \param   CString&		a_strSrc
// \param   CString&		a_strToken
// \param   CStringList*	a_pList
// \return  int 
// 
int CBonjourMan::Split(
	_In_ const CString&			a_strSrc,
	_In_ const CString&			a_strToken,
	_Out_ CStringList*			a_pList)
{
	int nCnt = 0;
	CString strData = a_strSrc;
	do {
		int nPos = strData.Find(a_strToken);
		if( nPos != -1 ) {
			++nCnt;
			a_pList->AddTail(strData.Mid(0, nPos));
			strData.Delete(0, nPos + 1);
		}
		else {
			++nCnt;
			a_pList->AddTail(strData);
			break;
		}
	} while( !strData.IsEmpty() );
	return nCnt;
}