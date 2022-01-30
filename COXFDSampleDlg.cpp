
// COXFDSampleDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "COXFDSample.h"
#include "COXFDSampleDlg.h"
#include "afxdialogex.h"
#include "VisableCamera.h"
#include "ThermalCamera.h"
#include "INIManager.h"
#include "MyHelperMan.h"

//skpark in your area 
#include "skpark/common.h"
#include "skpark/TraceLog.h"
//#include "Guardian/Guardian.h"
#include "Guardian/CoxConfig.h"
#include "Guardian/CoxGuardian.h"
//skpark in your area end

#ifdef __USE_BONJOUR_SDK__
#include "BonjourMan.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



CCOXFDSampleDlg::CCOXFDSampleDlg(
	_In_ CWnd*				pParent /*=NULL*/)
	: CDialogEx(IDD_COXFDSAMPLE_DIALOG, pParent)
	, m_bTCamRefresh(FALSE)
	, m_bVCamRefresh(FALSE)
	, m_hVCamThread(nullptr)
	, m_nVCamThreadID(0)
	, m_hTCamThread(nullptr)
	, m_nTCamThreadID(0)
	, m_nVCamWidth(0)
	, m_nVCamHeight(0)
	, m_hConnWaitThread(nullptr)
	, m_nConnWaitThreadID(0)
#ifdef _DEBUG
	, m_nVisableSkipFrame(0)
#else
	, m_nVisableSkipFrame(2)
#endif
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	theINIMan().LoadINI(theHelperMan().GetAppendCurrDir(_T("Config.ini")));

	
	m_isMinimize = false;  //skpark add
	//m_guardian = 0; //skpark add
	m_cox_guardian = 0;
}


CCOXFDSampleDlg::~CCOXFDSampleDlg()
{
	if( m_hConnWaitThread != nullptr ) {
		SetEvent(m_hEvtConnWait[0]);
		WaitForSingleObject(m_hConnWaitThread, INFINITE);
	}

	CloseHdl(m_hEvtConnWait[0]);
	CloseHdl(m_hEvtConnWait[1]);

	//if (m_guardian) delete m_guardian;  //skpark add
	if (m_cox_guardian) delete m_cox_guardian;  //skpark add
}


void CCOXFDSampleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STVCAM_GRP, m_stVCamGroup);
	DDX_Control(pDX, IDC_STTCAM_GRP, m_stTCamGroup);
	DDX_Control(pDX, IDC_COMBO1, m_cbVCamList);
	DDX_Control(pDX, IDC_COMBO2, m_cbVCamSupportResolutionList);
	DDX_Control(pDX, IDC_COMBO3, m_cbTCamList);
	DDX_Control(pDX, IDC_COMBO4, m_cbTCamPaletteList);
	DDX_Control(pDX, IDC_BUTTON1, m_BtnConnect);
	DDX_Control(pDX, IDC_BUTTON2, m_BtnDisconnect);
	DDX_Control(pDX, IDC_BUTTON3, m_BtnVCamReload);
	DDX_Control(pDX, IDC_BUTTON4, m_BtnTCamReload);
	DDX_Control(pDX, IDC_VCAM_SCREEN, m_wndVCam);
	DDX_Control(pDX, IDC_TCAM_SCREEN, m_wndTCam);
	DDX_Control(pDX, IDC_EDIT1, m_edVisableSkipFrame);
	DDX_Control(pDX, IDC_BUTTON5, m_BtnSkipFrame);
	DDX_Control(pDX, IDC_STATIC_STAT, m_statArea);  //skpark in your area 평균갑 보여주기
	DDX_Control(pDX, IDC_STATIC_FACE_AREA, m_stFaceArea); //  skpark in your area 사진 보여주기
}


BEGIN_MESSAGE_MAP(CCOXFDSampleDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_CBN_SELCHANGE(IDC_COMBO1, &CCOXFDSampleDlg::OnCbnSelchangeCombo1)
	ON_MESSAGE(UMSG_NOTIFY, &CCOXFDSampleDlg::OnUserNotify)
	ON_BN_CLICKED(IDC_BUTTON3, &CCOXFDSampleDlg::OnBnClickedVCamReload)
	ON_BN_CLICKED(IDC_BUTTON4, &CCOXFDSampleDlg::OnBnClickedTCamReload)
	ON_BN_CLICKED(IDC_BUTTON1, &CCOXFDSampleDlg::OnBnClickedConnect)
	ON_BN_CLICKED(IDC_BUTTON2, &CCOXFDSampleDlg::OnBnClickedDisconnect)
	ON_CBN_SELCHANGE(IDC_COMBO4, &CCOXFDSampleDlg::OnCbnSelchangeCombo4)
	ON_BN_CLICKED(IDC_BUTTON5, &CCOXFDSampleDlg::OnBnClickedVisableSkipFrameApply)

	ON_WM_CTLCOLOR() //skpark add
	ON_MESSAGE(WM_OLD_FILE_DELETE, OnOldFileDelete)   //skpark add
	ON_WM_COPYDATA() //skpark add
	ON_WM_LBUTTONUP() //skpark add
	ON_WM_TIMER() //skpark add
END_MESSAGE_MAP()

//skpark in your area start
LRESULT CCOXFDSampleDlg::OnOldFileDelete(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	//if (m_guardian)
	//{
	//	TraceLog((_T("OnOldFileDelete")));
	//	deleteOldFile(FACE_PATH, 0, 0, 5, _T("*.jpg"));  // 최근 5분 분량만 놔두고 다 지운다.
	//}
	return 0;
}

void CCOXFDSampleDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (m_cox_guardian)
	{
		m_cox_guardian->OnTimer(nIDEvent);  //skpark in your arer
		//m_guardian->OnTimer(nIDEvent);
	}
	CDialog::OnTimer(nIDEvent);
}

BOOL CCOXFDSampleDlg::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
	//if (m_guardian)
	//{
	//	m_guardian->OnCopyData(pWnd, pCopyDataStruct);
	//}
	return CDialog::OnCopyData(pWnd, pCopyDataStruct);
}

void CCOXFDSampleDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_cox_guardian)
	{
		m_cox_guardian->OnLButtonUp(nFlags, point);  //skpark in your area
	}
	CDialog::OnLButtonUp(nFlags, point);
	return;

}

HBRUSH CCOXFDSampleDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	if (m_cox_guardian)
	{
		return m_cox_guardian->OnCtlColor(IDC_STATIC_STAT, pDC, pWnd, nCtlColor, hbr);  // skpark in your area
	}
	return hbr;
}

//skpark in your area end

LRESULT CCOXFDSampleDlg::OnUserNotify(
	_In_ WPARAM					a_wParam,
	_In_ LPARAM					a_lParam)
{
	USER_MSG msg = static_cast<USER_MSG>(a_wParam);

	switch( msg ) {
		case NOTI_MSG_BONJR_ADD_CAM:
		{
			BONJR_CAM_INFO* pCam = reinterpret_cast<BONJR_CAM_INFO*>(a_lParam);
			if( pCam != nullptr ) {
				m_cbTCamList.AddString(pCam->m_strIPAddress);
			}
			break;
		}

		case NOTI_MSG_BONJR_FINISH:
		{
			m_bTCamRefresh = FALSE;
			break;
		}
	}
	return S_OK;
}


BOOL CCOXFDSampleDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
	SetIcon(m_hIcon, TRUE);			
	SetIcon(m_hIcon, FALSE);		


	//
	// OnInitDialog start
	// 
	//skpark in your area
	m_cox_guardian = new CoxGuardian(this, &m_wndVCam, &m_wndTCam, &m_statArea, &m_stFaceArea);  //skpark in your area
	//m_guardian = new CGuardian(this, &m_wndVCam, &m_wndTCam, GetDlgItem(IDC_STATIC_STAT)); //skpark in your area
	m_wndVCam.SetCallback(m_cox_guardian);
	theTCMan().SetConfig(m_cox_guardian->m_config);
	theVCMan().SetCallback(m_cox_guardian);
	TraceLog((_T("waitTime=%d"), m_cox_guardian->m_config->m_wait_time));
	// skpark in your area end

	InitMainUI();

	m_hEvtConnWait[0] = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hEvtConnWait[1] = CreateEvent(NULL, TRUE, FALSE, NULL);

	m_edVisableSkipFrame.SetWindowText(GetMakeString(_T("%d"), 
													 m_nVisableSkipFrame));

	OnBnClickedConnect(); // skpark in your area 자동으로 셋팅
	SetTimer(ALARM_CHECK_TIMER, 1000, NULL);  // skpark in your area

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}


void CCOXFDSampleDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		if (m_cox_guardian)  m_cox_guardian->OnPaint();  //skpark in your area
		CDialogEx::OnPaint();
	}
}


void CCOXFDSampleDlg::OnClose()
{
	OnBnClickedDisconnect();
	CDialogEx::OnClose();
}


void CCOXFDSampleDlg::OnDestroy()
{
	if (m_cox_guardian)  //skpark add
	{
		m_cox_guardian->OnDestroy();  //skpark in your area
	}
	CDialogEx::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}


void CCOXFDSampleDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	if (m_cox_guardian && m_cox_guardian->m_bFullScreen)
	{
		m_cox_guardian->MaxWindow();  //skpark in your area
	}
}


BOOL CCOXFDSampleDlg::PreTranslateMessage(MSG* pMsg)
{
	if (m_cox_guardian) 
	{
		m_cox_guardian->PreTranslateMessage(pMsg);  //skpark in your area
	}

	if( pMsg->message == WM_KEYDOWN ) {
		if( pMsg->wParam == VK_RETURN  ) { //|| pMsg->wParam == VK_ESCAPE ) {
			return TRUE;
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}


// 
// \brief <pre>
// UI 초기화
// </pre>
// 
void CCOXFDSampleDlg::InitMainUI()
{
	m_wndVCam.InitializeEx(this);
	m_wndTCam.InitializeEx(this);

	//SetControlPos();
	m_cox_guardian->MaxWindow();

	// Visable Camera Load
	FindVCamList();

	// Thermal Camera Load
#ifdef __USE_BONJOUR_SDK__
	theBonjrMan().InitBonjour(this);
#endif
	m_cbTCamPaletteList.SetCurSel(1);		// Default Grey Palette

	// INI파일에 TCamIP가 있다면 콤보박스에 초기화 해준다.
	if( !theINIMan().GetTCamIP().IsEmpty() ) {
		m_cbTCamList.SetWindowText(theINIMan().GetTCamIP());
	}
}


// 
// \brief <pre>
// UI 위치 조정
// </pre>
// 
//void CCOXFDSampleDlg::SetControlPos()
//{
//	CRect rc;
//	GetClientRect(&rc);
//
//	/* skpark comment out area 
//	// Visable Camera Group
//	if( m_stVCamGroup.GetSafeHwnd() ) {
//		m_stVCamGroup.SetWindowPos(NULL,
//								   rc.left + 10,
//								   rc.top + 10,
//								   500,
//								   70,
//								   SWP_SHOWWINDOW);
//	}
//
//	if( m_cbVCamList.GetSafeHwnd() ) {
//		m_cbVCamList.SetWindowPos(NULL,
//								  rc.left + 20,
//								  rc.top + 40,
//								  130,
//								  30,
//								  SWP_SHOWWINDOW);
//	}
//
//	if( m_cbVCamSupportResolutionList.GetSafeHwnd() ) {
//		m_cbVCamSupportResolutionList.SetWindowPos(NULL,
//												   rc.left + 160,
//												   rc.top + 40,
//												   130,
//												   30,
//												   SWP_SHOWWINDOW);
//	}
//
//	if( m_BtnVCamReload.GetSafeHwnd() ) {
//		m_BtnVCamReload.SetWindowPos(NULL,
//									 rc.left + 300,
//									 rc.top + 40,
//									 70,
//									 23,
//									 SWP_SHOWWINDOW);
//	}
//
//	if( m_edVisableSkipFrame.GetSafeHwnd() ) {
//		m_edVisableSkipFrame.SetWindowPos(NULL,
//										  rc.left + 380,
//										  rc.top + 40,
//										  40,
//										  23,
//										  SWP_SHOWWINDOW);
//	}
//
//	if( m_BtnSkipFrame.GetSafeHwnd() ) {
//		m_BtnSkipFrame.SetWindowPos(NULL,
//									rc.left + 430,
//									rc.top + 40,
//									70,
//									23,
//									SWP_SHOWWINDOW);
//	}
//
//	// Thermal Camera Group
//	if( m_stTCamGroup.GetSafeHwnd() ) {
//		m_stTCamGroup.SetWindowPos(NULL,
//								   rc.left + 520,
//								   rc.top + 10,
//								   400,
//								   70,
//								   SWP_SHOWWINDOW);
//	}
//
//	if( m_cbTCamList.GetSafeHwnd() ) {
//		m_cbTCamList.SetWindowPos(NULL,
//								  rc.left + 530,
//								  rc.top + 40,
//								  160,
//								  30,
//								  SWP_SHOWWINDOW);
//	}
//
//	if( m_cbTCamPaletteList.GetSafeHwnd() ) {
//		m_cbTCamPaletteList.SetWindowPos(NULL,
//										 rc.left + 700,
//										 rc.top + 40,
//										 100,
//										 30,
//										 SWP_SHOWWINDOW);
//	}
//
//	if( m_BtnTCamReload.GetSafeHwnd() ) {
//		m_BtnTCamReload.SetWindowPos(NULL,
//									 rc.left + 810,
//									 rc.top + 40,
//									 100,
//									 23,
//									 SWP_SHOWWINDOW);
//	}
//
//	if( m_BtnConnect.GetSafeHwnd() ) {
//		m_BtnConnect.SetWindowPos(NULL,
//								  rc.right - 220,
//								  rc.top + 10,
//								  100,
//								  70,
//								  SWP_SHOWWINDOW);
//		m_BtnConnect.RedrawWindow();
//	}
//
//	if( m_BtnDisconnect.GetSafeHwnd() ) {
//		m_BtnDisconnect.SetWindowPos(NULL,
//									 rc.right - 110,
//									 rc.top + 10,
//									 100,
//									 70,
//									 SWP_SHOWWINDOW);
//		m_BtnDisconnect.RedrawWindow();
//	}
//	skpark comment out area */
//
//	//skpark modify
//	ShowWindow(SW_MAXIMIZE);
//	
//	double  ratio = 1280.0 / 720.0;
//	int width = rc.Width();
//	double height = static_cast<double>(width) * ratio;
//
//	if (m_wndTCam.GetSafeHwnd()) {
//		m_wndTCam.SetWindowPos(NULL,
//			rc.left,
//			rc.top + TITLE_HEIGHT,
//			width,
//			static_cast<int>(height),
//			SWP_HIDEWINDOW);  //skpark  <-- SWP_SHOWWINDOW); 
//	}
//	if( m_wndVCam.GetSafeHwnd() ) {
//		m_wndVCam.SetWindowPos(NULL,
//								  rc.left,
//								  rc.top + TITLE_HEIGHT,
//								  width,
//								  static_cast<int>(height),
//								  SWP_SHOWWINDOW);
//	}
//
//	
//	m_statArea.MoveWindow(rc.left, rc.bottom - STAT_HEIGHT, width, STAT_HEIGHT);
//
//
//
//}


// 
// \brief <pre>
// 실상 카메라 목록을 얻는다.
// </pre>
// 
void CCOXFDSampleDlg::FindVCamList()
{
	HRESULT hr = S_OK;
	CComPtr<ICreateDevEnum> pDevEnum	= nullptr;
	CComPtr<IEnumMoniker>	pEnum		= nullptr;
	CComPtr<IMoniker>		pMoniker	= nullptr;

	hr = CoCreateInstance(CLSID_SystemDeviceEnum,
						  NULL,
						  CLSCTX_INPROC_SERVER,
						  IID_PPV_ARGS(&pDevEnum));

	if( SUCCEEDED(hr) ) {
		hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory,
											 &pEnum,
											 0);

		if( hr == S_FALSE ) {
			hr = VFW_E_NOT_FOUND;
			return;
		}
	}

	int nDeviceNum = 0;

	while( pEnum->Next(1, &pMoniker, NULL) == S_OK ) {
		CComPtr<IPropertyBag> pPropBag = nullptr;
		hr = pMoniker->BindToStorage(0, 0, IID_PPV_ARGS(&pPropBag));
		if( FAILED(hr) ) {
			pMoniker.Release();
			pMoniker = nullptr;
			continue;
		}

		VARIANT var;
		VariantInit(&var);

		hr = pPropBag->Read(_T("Friendlyname"), &var, 0);
		if( FAILED(hr) ) {
			VariantClear(&var);
			pMoniker.Release();
			pMoniker = nullptr;
			continue;
		}

		CString strDeviceName = CString(var.bstrVal);
		strDeviceName.TrimRight();
		VariantClear(&var);

		if( strDeviceName.Compare(_T("eBUS DirectShow Source")) == 0 ) {
			pMoniker.Release();
			pMoniker = nullptr;
			continue;
		}

		hr = pPropBag->Read(_T("Description"), &var, 0);
		CString strDesc = CString(var.bstrVal);
		VariantClear(&var);

		hr = pPropBag->Read(_T("DevicePath"), &var, 0);
		CString strPath = CString(var.bstrVal);
		VariantClear(&var);

		std::shared_ptr<VCAM_INFO> pInfo =
									std::make_shared<VCAM_INFO>(
														VCAM_INFO(nDeviceNum,
																  strDeviceName,
																  strDesc,
																  strPath));
		GetSupportResolution(pMoniker, pInfo.get());
		m_vVCamInfoList.push_back(pInfo);

		pMoniker.Release();
		pMoniker = nullptr;
	}

	for( auto pCam : m_vVCamInfoList ) {
		m_cbVCamList.AddString(pCam->m_strDeviceName);
	}
	
	m_cbVCamList.SetCurSel(0);   //skpark in your area  3번 USB
	OnCbnSelchangeCombo1();   //skpark in your area

	m_bVCamRefresh = FALSE;
}


// 
// \brief <pre>
// 실상카메라의 지원되는 해상도 및 데이터 포멧을 얻는다.
// </pre>
// \param   IMoniker*			a_pMoniker
// \param   VCAM_INFO*			a_pCam
// 
void CCOXFDSampleDlg::GetSupportResolution(
	_In_ IMoniker*			a_pMoniker,
	_In_ VCAM_INFO*			a_pCam)
{
	HRESULT hr = S_OK;
	CComPtr<IBaseFilter>	pFilter = nullptr;
	hr = a_pMoniker->BindToObject(nullptr,
								  nullptr,
								  IID_IBaseFilter,
								  (void**)&pFilter);

	if( FAILED(hr) ) {
		return;
	}

	CComPtr<IEnumPins>	pEnumPin	= nullptr;
	CComPtr<IPin>		pPin		= nullptr;

	hr = pFilter->EnumPins(&pEnumPin);
	if( FAILED(hr) ) {
		goto FINISH;
	}

	BOOL bFind = FALSE;
	while( pEnumPin->Next(1, &pPin, nullptr) == S_OK ) {
		PIN_DIRECTION pinDirection;
		hr = pPin->QueryDirection(&pinDirection);

		if( pinDirection == PINDIR_INPUT ) {
			pPin.Release();
			pPin = nullptr;
			continue;
		}
		else if( bFind ) {
			pPin.Release();
			pPin = nullptr;
			continue;
		}
		else if( pinDirection == PINDIR_OUTPUT ) {
			bFind = TRUE;
		}

		if( SUCCEEDED(hr) ) {
			AM_MEDIA_TYPE*				pMediaType	= nullptr;
			VIDEOINFOHEADER*			pVDOInfoHD	= nullptr;
			CComPtr<IEnumMediaTypes>	pEnumMedia	= nullptr;

			hr = pPin->EnumMediaTypes(&pEnumMedia);
			if( FAILED(hr) ) {
				pPin.Release();
				pPin = nullptr;
				continue;
			}

			int nCount = 0;
			while( hr = pEnumMedia->Next(1, &pMediaType, nullptr), hr == S_OK ) {
				if( (pMediaType->formattype == FORMAT_VideoInfo)
				   && (pMediaType->cbFormat >= sizeof(VIDEOINFOHEADER))
				   && (pMediaType->pbFormat != nullptr) )
				{
					pVDOInfoHD = (VIDEOINFOHEADER*)pMediaType->pbFormat;
					a_pCam->m_vVideoInfoHDList.push_back(
						std::make_shared<VCAM_RES>(VCAM_RES(pVDOInfoHD)));
				}

				if( pMediaType->cbFormat != 0 ) {
					CoTaskMemFree((PVOID)pMediaType->pbFormat);
					pMediaType->cbFormat	= 0;
					pMediaType->pbFormat	= nullptr;
				}
				if( pMediaType->pUnk != nullptr ) {
					pMediaType->pUnk->Release();
					pMediaType->pUnk = nullptr;
				}

				CoTaskMemFree(pMediaType);
			}

			pEnumMedia.Release();
			pEnumMedia = nullptr;
		}

		pPin.Release();
		pPin = nullptr;
	}

	FINISH:
	if( pEnumPin != nullptr ) {
		pEnumPin.Release();
		pEnumPin = nullptr;
	}

	if( pFilter != nullptr ) {
		pFilter->Stop();
		pFilter.Release();
		pFilter = nullptr;
	}
}


// 
// \brief <pre>
// 실상카메라 선택 시 지원되는 해상도 및 DataFormat을 
// 콤보박스에 보여준다.
// </pre>
// 
void CCOXFDSampleDlg::OnCbnSelchangeCombo1()
{
	int nSel = m_cbVCamList.GetCurSel();
	if( nSel == -1 ) {
		return;
	}

	VCAM_INFO* pCam = m_vVCamInfoList.at(nSel).get();
	
	m_cbVCamSupportResolutionList.ResetContent();
	
	for( auto pVideo : pCam->m_vVideoInfoHDList ) {
		CString strRes;
		strRes.Format(_T("%dx%d %s"),
					  pVideo->m_stHeader.bmiHeader.biWidth,
					  pVideo->m_stHeader.bmiHeader.biHeight,
					  pVideo->getCompression2String());
		m_cbVCamSupportResolutionList.AddString(strRes);
		TraceLog((_T("strRes:%s"), strRes));
	}
	m_cbVCamSupportResolutionList.SetCurSel(COX_RESOLTION);  //skpark in our area 640x480
}


// 
// \brief <pre>
// 실상 카메라 Reload
// </pre>
// 
void CCOXFDSampleDlg::OnBnClickedVCamReload()
{
	if( m_bVCamRefresh )		{ return; }

	m_bVCamRefresh = TRUE;
	m_cbVCamList.ResetContent();
	m_cbVCamSupportResolutionList.ResetContent();
	m_vVCamInfoList.clear();
	FindVCamList();
}


// 
// \brief <pre>
// 열상 카메라 Reload
// </pre>
// 
void CCOXFDSampleDlg::OnBnClickedTCamReload()
{
	if( m_bTCamRefresh )		{ return; }

	m_bTCamRefresh = TRUE;
	m_cbTCamList.ResetContent();
	theBonjrMan().RefreshBonjour(this);
}


// 
// \brief <pre>
// 실상&열상 카메라 접속
// </pre>
// 
void CCOXFDSampleDlg::OnBnClickedConnect()
{
	ResetEvent(m_hEvtConnWait[0]);
	ResetEvent(m_hEvtConnWait[1]);

	int nVCamSel = m_cbVCamList.GetCurSel();
	int nVCamRes = m_cbVCamSupportResolutionList.GetCurSel();

	// Visable Camera
	if( nVCamSel == -1 || nVCamRes == -1 ) {
		// 실상카메라 or 실상카메라해상도 선택 안됨
		return;
	}

	m_cbVCamList.GetLBText(nVCamSel, m_strVCamName);

	auto pHeader = &m_vVCamInfoList[nVCamSel]->m_vVideoInfoHDList[nVCamRes]->m_stHeader.bmiHeader;
	if( !&m_vVCamInfoList[nVCamSel]->m_vVideoInfoHDList[nVCamRes]->m_bSupport ) {
		// FaceMe에서 지원하지 않는 DataFormat (YUY2만 지원)
		MessageBox(_T("지원하지 않는 Compression 입니다."),
				   _T("COXFDSample"),
				   MB_ICONWARNING);
		return;
	}

	m_nVCamWidth	= pHeader->biWidth;
	m_nVCamHeight	= pHeader->biHeight;

	// 실상카메라 접속스레드
	m_hVCamThread = (HANDLE)_beginthreadex(NULL,
										   0,
										   &CCOXFDSampleDlg::VCamThread,
										   this,
										   0,
										   &m_nVCamThreadID);

	// Thermal Camera
	CString strTCam;
	m_cbTCamList.GetWindowText(strTCam);
	if( strTCam.IsEmpty() ) {
		return;
	}
	m_strTCamIP = strTCam;
	
	// 열상카메라 접속스레드
	m_hTCamThread = (HANDLE)_beginthreadex(NULL,
										   0,
										   &CCOXFDSampleDlg::TCamThread,
										   this,
										   0,
										   &m_nTCamThreadID);


	// 실상 & 열상 접속 결과 대기 스레드
	// 둘다 접속 성공 시 스트리밍 시작
	m_hConnWaitThread = (HANDLE)_beginthreadex(NULL,
											   0,
											   &CCOXFDSampleDlg::WaitForConnThread,
											   this,
											   0,
											   &m_nConnWaitThreadID);
}


// 
// \brief <pre>
// 내용을 넣으세요.
// </pre>
// 
void CCOXFDSampleDlg::OnBnClickedDisconnect()
{
	if( theVCMan().IsConnected() && theTCMan().IsConnected() ) {
		theTCMan().Disconnect();
		theVCMan().Disconnect();
		m_wndTCam.SetCamStatus(CAM_DISCONNECT);
		m_wndVCam.SetCamStatus(CAM_DISCONNECT);
	}
}


// 
// \brief <pre>
// 실상 스트리밍 쓰레드
// </pre>
// \param   LPVOID					a_pVoid
// \return  THREADFUNC 
// 
THREADFUNC CCOXFDSampleDlg::VCamThread(
	_In_ LPVOID					a_pVoid)
{
	CCOXFDSampleDlg* pThis = (CCOXFDSampleDlg*)a_pVoid;
	if( pThis != nullptr ) {
		pThis->RunVCamThread();
	}
	return 0;
}


// 
// \brief <pre>
// 내용을 넣으세요.
// </pre>
// 
void CCOXFDSampleDlg::RunVCamThread()
{
	m_wndVCam.SetCamStatus(CAM_CONNECTING);
	BOOL bConnect = theVCMan().Initialize(&m_wndVCam,
										  &m_wndTCam,
										  GetSafeHwnd(),
										  m_strVCamName,
										  m_nVCamWidth,
										  m_nVCamHeight,
										  m_nVisableSkipFrame);

	SetEvent(m_hEvtConnWait[0]);
	if( bConnect ) {
		// ...
	}
	else {
		m_wndVCam.SetCamStatus(CAM_DISCONNECT);
		// 예외처리
	}
}


// 
// \brief <pre>
// 열상 스트리밍 쓰레드
// </pre>
// \param   LPVOID					a_pVoid
// \return  THREADFUNC 
// 
THREADFUNC CCOXFDSampleDlg::TCamThread(
	_In_ LPVOID					a_pVoid)
{
	CCOXFDSampleDlg* pThis = (CCOXFDSampleDlg*)a_pVoid;
	if( pThis != nullptr ) {
		pThis->RunTCamThread();
	}
	return 0;
}


// 
// \brief <pre>
// 내용을 넣으세요.
// </pre>
// 
void CCOXFDSampleDlg::RunTCamThread()
{
	int nPalSel = m_cbTCamPaletteList.GetCurSel();
	
	m_wndTCam.SetCamStatus(CAM_CONNECTING);
	BOOL bConnect = theTCMan().Connect(&m_wndTCam,
									   GetSafeHwnd(),
									   m_strTCamIP,
									   (IRF_PALETTE_TYPE_T)nPalSel);

	SetEvent(m_hEvtConnWait[1]);
	if( bConnect ) {
		// ...
	}
	else {
		m_wndTCam.SetCamStatus(CAM_DISCONNECT);
		// 예외처리
	}
}


THREADFUNC CCOXFDSampleDlg::WaitForConnThread(
	_In_ LPVOID					a_pVoid)
{
	CCOXFDSampleDlg* pThis = (CCOXFDSampleDlg*)a_pVoid;
	if( pThis != nullptr ) {
		pThis->RunWaitForConnThread();
	}
	return 0;
}


// 
// \brief <pre>
// 실상&열상 접속 결과 대기 스레드
//
// 접속 성공 시
//		-> 열상에서 좌표매칭 정보를 불러와 셋팅한다.
//		-> 실상&열상 스트리밍 시작
//
// 접속 실패 시
//		-> 실패 처리
//
// </pre>
// 
void CCOXFDSampleDlg::RunWaitForConnThread()
{
	DWORD dwRet = WaitForMultipleObjects(2,
										 m_hEvtConnWait,
										 TRUE,
										 INFINITE);

	if( dwRet == WAIT_OBJECT_0 ) {
		if( theVCMan().IsConnected()
		   && theTCMan().IsConnected() )
		{
			// 실상좌표 -> 열상좌표 변환을 위한
			// 매칭좌표 로드
			if( !theTCMan().LoadV2TMatcingPoint(theVCMan().GetVisableWidth(),
											   theTCMan().GetThermalWidth()) )
			{
				// 매칭좌표가 없음
				// 예외처리
				theTCMan().Disconnect();
				theVCMan().Disconnect();
				m_wndTCam.SetCamStatus(CAM_DISCONNECT);
				m_wndVCam.SetCamStatus(CAM_DISCONNECT);
				return;
			}

			// 접속 성공 및 매칭좌표 셋팅이 되면
			// 스트리밍 시작
			theVCMan().StreamStart();
			theTCMan().StreamStart();

			m_wndTCam.SetCamStatus(CAM_STREAMING);
			m_wndVCam.SetCamStatus(CAM_STREAMING);
		}
		else {
			// 연결 실패
			// 예외 처리
			m_wndTCam.SetCamStatus(CAM_DISCONNECT);
			m_wndVCam.SetCamStatus(CAM_DISCONNECT);
		}
	}
}


// 
// \brief <pre>
// 열상 팔레트 변경
// </pre>
// 
void CCOXFDSampleDlg::OnCbnSelchangeCombo4()
{
	if( !theTCMan().IsConnected() ) {
		return;
	}

	theTCMan().SetPalette(m_cbTCamPaletteList.GetCurSel());
}


// 
// \brief <pre>
// 실상 FPS조절을 위한 SkipFrame 수 조정
// 값이 n이라면 n장 버리고 1장 디스플레이
// </pre>
// 
void CCOXFDSampleDlg::OnBnClickedVisableSkipFrameApply()
{
	CString strTxt;
	m_edVisableSkipFrame.GetWindowText(strTxt);
	m_nVisableSkipFrame = _ttoi(strTxt);
	theVCMan().SetVisableCamSkipFrame(m_nVisableSkipFrame);
}
