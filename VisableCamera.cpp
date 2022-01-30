#include "stdafx.h"
#include "VisableCamera.h"
#include "INIManager.h"
#include "VCamView.h"
#include "TCamView.h"
#include "ThermalCamera.h"
#include "MyHelperMan.h"

//skpark in your area
#include "skpark/TraceLog.h"  
#include "Guardian/CoxConfig.h"  
#include "Guardian/CoxGuardian.h"  
#include "skpark/Statistics.h"  
void CVisableCamera::SetCallback(CoxGuardian* g) { m_cox_guardian = g; }
//skpark in your area end

CVisableCamera& theVCMan()
{
	static CVisableCamera man;
	return man;
}


CVisableCamera::CVisableCamera()
	: m_pFaceME(nullptr)
	, m_pFMLicense(nullptr)
	, m_pFMCameraCapture(nullptr)
	, m_pFMCameraManager(nullptr)
	, m_pFMRecognizer(nullptr)
	, m_nFaceMeError(FR_RETURN_OK)
	, m_hMainWnd(nullptr)
	, m_nWidth(0)
	, m_nHeight(0)
	, m_bConnect(FALSE)
	, m_nConnectIdx(-1)
	, m_nWriteIdx(0)
	, m_pTargetBuff(nullptr)
	, m_pVCamView(nullptr)
	, m_pTCamView(nullptr)
	, m_hThread(nullptr)
	, m_nThreadID(0)
	, m_nFrameWriteIdx(0)
	, m_pFrameTarget(nullptr)
	, m_nSkipFrame(0)
	, m_nCurrFrameSeq(0)
	, m_cox_guardian(NULL)  //skpark in your area
{
	for( int i = 0 ; i < DEF_DATA_DEPTH ; ++i ) {
		m_pVFInfo[i] = new VF_INFO();

		m_pFrameInfo[i] = new CAMF_INFO();
	}

	m_hEvtLoop[0] = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hEvtLoop[1] = CreateEvent(NULL, FALSE, FALSE, NULL);
}


CVisableCamera::~CVisableCamera()
{
	for( int i = 0 ; i < DEF_DATA_DEPTH ; ++i ) {
		DeleteObj(m_pVFInfo[i]);
		DeleteObj(m_pFrameInfo[i]);
	}

	CloseHdl(m_hEvtLoop[0]);
	CloseHdl(m_hEvtLoop[1]);
}


// 
// \brief <pre>
// �ʱ�ȭ
// </pre>
// \param   CVCamView*		a_pVCamView
// \param   CTCamView*		a_pTCamView
// \param   HWND			a_hMainWnd
// \param   CString&		a_strCamName
// \param   int				a_nWidth
// \param   int				a_nHeight
// \param   UINT			a_nSkipFrame
// \return  BOOL 
// 
BOOL CVisableCamera::Initialize(
	_In_ CVCamView*			a_pVCamView,
	_In_ CTCamView*			a_pTCamView,
	_In_ HWND				a_hMainWnd,
	_In_ const CString&		a_strCamName,
	_In_ int				a_nWidth,
	_In_ int				a_nHeight,
	_In_ UINT				a_nSkipFrame)
{
	m_pVCamView			= a_pVCamView;
	m_pTCamView			= a_pTCamView;
	m_hMainWnd			= a_hMainWnd;
	m_strTargetName		= a_strCamName;
	m_nWidth			= a_nWidth;
	m_nHeight			= a_nHeight;
	m_nSkipFrame		= a_nSkipFrame;

	if( m_bConnect ) {
		Disconnect();
	}

	CHAR szPath[BUFSIZ] = { 0, };
	GetModuleFileNameA(nullptr, szPath, BUFSIZ);
	PathRemoveFileSpecA(szPath);
	m_strModulePath = szPath;

	::memset(szPath, 0x00, BUFSIZ);
	SHGetSpecialFolderPathA(m_hMainWnd, szPath, CSIDL_COMMON_APPDATA, FALSE);
	m_strAppDataPath = szPath;

	::memset(szPath, 0x00, BUFSIZ);
	SHGetSpecialFolderPathA(m_hMainWnd, szPath, CSIDL_LOCAL_APPDATA, FALSE);
	m_strCachePath = szPath;

	FR_RETURN nResult = FR_RETURN_OK;
	CT2CA pszCvtLicense(theINIMan().GetFMLicenseKey());
	std::string strKeyText(pszCvtLicense);
	m_vLicenseList.assign(strKeyText.begin(), strKeyText.end());

	m_pFaceME = new FaceMeSDKLib();
	if( m_pFaceME == nullptr ) {
		return FALSE;
	}

	if( !FaceMeLicenseAuth() ) {
		// ����ó��
		__debugbreak();
	}
	if( !FaceMeRecognizerCfg() ) {
		// ����ó��
		__debugbreak();
	}
	if( !FaceMeCameraManagerCfg() ) {
		// ����ó��
		__debugbreak();
	}
	if( !FaceMeCameraCapturer() ) {
		// ����ó��
		__debugbreak();
	}

	ResetEvent(m_hEvtLoop[0]);
	ResetEvent(m_hEvtLoop[1]);
	m_hThread = (HANDLE)_beginthreadex(NULL,
									   0,
									   &CVisableCamera::ThreadProc,
									   this,
									   0,
									   &m_nThreadID);
	return TRUE;
}


// 
// \brief <pre>
// ������ ��������.
// </pre>
// \return  BOOL 
// 
BOOL CVisableCamera::FaceMeLicenseAuth()
{
	if( m_pFaceME == nullptr ) {
		return FALSE;
	}

	FR_RETURN nResult = FR_RETURN_OK;
	FR_LicenseManagerConfig stLicenseCfg;
	FR_INIT_STRUCT(&stLicenseCfg, FR_LicenseManagerConfig);

	stLicenseCfg.appBundlePath		= m_strModulePath.c_str();
	stLicenseCfg.appDataPath		= m_strAppDataPath.c_str();
	stLicenseCfg.appCachePath		= m_strCachePath.c_str();

	bool bRet= m_pFaceME->CreateFaceMeLicenseManager(&m_pFMLicense);
	if( !bRet ) {
		DebugString(_T(">> [ E ] [ FACEME ] CreateFaceMeLicenseManager Fail..."));
		return FALSE;
	}

	nResult = m_pFMLicense->Initialize(&stLicenseCfg,
									   m_vLicenseList.data(),
									   (uint32_t)m_vLicenseList.size());
	if( FR_FAILED(nResult) ) {
		m_nFaceMeError = nResult;
		DebugString(GetMakeString(
			_T(">> [ E ] [ FACEME ] LicenseManager Initialize Fail. err[%d]"),
			m_nFaceMeError));
		return FALSE;
	}

	nResult = m_pFMLicense->RegisterLicense(nullptr, nullptr);
	if( FR_FAILED(nResult) ) {
		m_nFaceMeError = nResult;
		DebugString(GetMakeString(
			_T(">> [ E ] [ FACEME ] LicenseManager Register Fail. err[%d]"),
			m_nFaceMeError));
		return FALSE;
	}

	DebugString(_T("[ FACEME ] License Initialize OK."));
	return TRUE;
}


// 
// \brief <pre>
// ������ ��������.
// </pre>
// \return  BOOL 
// 
BOOL CVisableCamera::FaceMeRecognizerCfg()
{
	if( m_pFaceME == nullptr ) {
		return FALSE;
	}

	FR_RETURN nResult = FR_RETURN_OK;
	FR_RecognizerConfig stRecognizerCfg;
	FR_INIT_STRUCT(&stRecognizerCfg, FR_RecognizerConfig);

	stRecognizerCfg.appBundlePath					= m_strModulePath.c_str();
	stRecognizerCfg.appDataPath						= m_strAppDataPath.c_str();
	stRecognizerCfg.appCachePath					= m_strCachePath.c_str();
	stRecognizerCfg.preference						= PREFER_NONE;
	stRecognizerCfg.detectionModelSpeedLevel		= FR_DETECTION_MODEL_SPEED_LEVEL_DEFAULT;
	stRecognizerCfg.extractionModelSpeedLevel		= FR_EXTRACTION_MODEL_SPEED_LEVEL_VH5_M;
	stRecognizerCfg.maxDetectionThreads				= 0;
	stRecognizerCfg.maxExtractionThreads			= 0;
	stRecognizerCfg.maxFrameWidth					= m_nWidth;
	stRecognizerCfg.maxFrameHeight					= m_nHeight;
	stRecognizerCfg.minFaceWidth					= theINIMan().GetMinFaceWidth();
	stRecognizerCfg.detectOutputOrder				= FR_DETECTION_OUTPUT_ORDER_FACE_WIDTH;
	stRecognizerCfg.preferredDetectionBatchSize		= 1;
	stRecognizerCfg.preferredExtractionBatchSize	= 1;

	DebugString(_T("[ FACEME ] *******************************************"));
	DebugString(_T("[ FACEME ] Recognizer Config Information"));
	DebugString(GetMakeString(_T("[ FACEME ] - Preference : %d"), stRecognizerCfg.preference));
	DebugString(GetMakeString(_T("[ FACEME ] - DetectionModelSpeedLevel : %d"), stRecognizerCfg.detectionModelSpeedLevel));
	DebugString(GetMakeString(_T("[ FACEME ] - ExtractionModelSpeedLevel : %d"), stRecognizerCfg.extractionModelSpeedLevel));
	DebugString(GetMakeString(_T("[ FACEME ] - Mode : %d"), stRecognizerCfg.mode));
	DebugString(GetMakeString(_T("[ FACEME ] - maxDetectionThreads : %d"), stRecognizerCfg.maxDetectionThreads));
	DebugString(GetMakeString(_T("[ FACEME ] - maxExtractionThreads : %d"), stRecognizerCfg.maxExtractionThreads));
	DebugString(GetMakeString(_T("[ FACEME ] - maxFrameWidth : %d"), stRecognizerCfg.maxFrameWidth));
	DebugString(GetMakeString(_T("[ FACEME ] - maxFrameHeight : %d"), stRecognizerCfg.maxFrameHeight));
	DebugString(GetMakeString(_T("[ FACEME ] - minFaceWidth : %d"), stRecognizerCfg.minFaceWidth));
	DebugString(GetMakeString(_T("[ FACEME ] - detectionOutputOrder : %d"), stRecognizerCfg.detectOutputOrder));
	DebugString(GetMakeString(_T("[ FACEME ] - PreferredDetectionBatchSize : %d"), stRecognizerCfg.preferredDetectionBatchSize));
	DebugString(GetMakeString(_T("[ FACEME ] - PreferredExtractionBatchSize : %d"), stRecognizerCfg.preferredExtractionBatchSize));
	DebugString(_T("[ FACEME ] *******************************************"));

	bool bRet = m_pFaceME->CreateFaceMeRecognizer(&m_pFMRecognizer);
	if( !bRet ) {
		DebugString(_T(">> [ E ] [ FACEME ] CreateFaceMeRecognizer Fail..."));
		return FALSE;
	}

	nResult	= m_pFMRecognizer->Initialize(
									&stRecognizerCfg, 
									m_vLicenseList.data(),
									(uint32_t)m_vLicenseList.size());
	if( FR_FAILED(nResult) ) {
		m_nFaceMeError = nResult;
		DebugString(GetMakeString(
			_T(">> [ E ] [ FACEME ] Recognizer Initialize Fail. err[%d]"),
			m_nFaceMeError));
		return FALSE;
	}

	if( m_pVCamView != nullptr ) {
		m_pVCamView->SetImageSize(m_nWidth, m_nHeight);
	}

	DebugString(_T("[ FACEME ] Recognizer Initialize OK."));
	return TRUE;
}


// 
// \brief <pre>
// ������ ��������.
// </pre>
// \return  BOOL 
// 
BOOL CVisableCamera::FaceMeCameraManagerCfg()
{
	if( m_pFaceME == nullptr ) {
		return FALSE;
	}

	FR_RETURN nResult = FR_RETURN_OK;
	FR_CameraManagerConfig stCameraCfg;
	FR_INIT_STRUCT(&stCameraCfg, FR_CameraManagerConfig);

	stCameraCfg.appBundlePath	= m_strModulePath.c_str();
	stCameraCfg.appDataPath		= m_strAppDataPath.c_str();
	stCameraCfg.appCachePath	= m_strCachePath.c_str();
	stCameraCfg.options			= FR_CAPTURE_ORIENTATIONE_NONE;

	m_pFMCameraManager = nullptr;
	bool bRet = m_pFaceME->CreateInstance(
									FR_INSTANCE_CAMERA_MANAGER,
									(void**)&m_pFMCameraManager);
	if( !bRet ) {
		DebugString(_T(">> [ E ] [ FACEME ] CreateInstance CameraManager Fail."));
		return FALSE;
	}

	nResult = m_pFMCameraManager->Initialize(
										&stCameraCfg,
										m_vLicenseList.data(),
										(uint32_t)m_vLicenseList.size());
	if( FR_FAILED(nResult) ) {
		m_nFaceMeError = nResult;
		DebugString(GetMakeString(
			_T(">> [ E ] [ FACEME ] CameraManager Initialize Fail. err[%d]"),
			m_nFaceMeError));
		return FALSE;
	}
	
	FR_CameraEnumeratorConfig stEnumeratorCfg;
	FR_INIT_STRUCT(&stEnumeratorCfg, FR_CameraEnumeratorConfig);
	stEnumeratorCfg.cameraType = FR_CAMERA_TYPE_LOCAL_CAMERA;

	IFaceMeCameraEnumerator* pCameraEnumerator = nullptr;
	nResult = m_pFMCameraManager->CreateCameraEnumerator(
												&stEnumeratorCfg,
												&pCameraEnumerator);
	if( FR_FAILED(nResult) ) {
		m_nFaceMeError = nResult;
		DebugString(GetMakeString(
			_T(">> [ E ] [ FACEME ] CameraManager CreateCameraEnumerator Create Fail. err[%d]"),
			m_nFaceMeError));
		return FALSE;
	}

	DebugString(_T("[ FACEME ] Get USBCamera Device Information List - [START]"));
	std::vector<int32_t> vDeviceIdx;
	std::vector<std::string> vDeviceName;
	std::vector<std::string> vDevicePath;

	int nSeq = 0;
	while( FR_SUCCEEDED(pCameraEnumerator->Next()) ) {
		IFaceMeCameraInfo* pCameraInfo = nullptr;
		nResult = pCameraEnumerator->GetCurrent(&pCameraInfo);

		int32_t nDeviceId = 0;
		if( FR_SUCCEEDED(pCameraInfo->GetUInt32(
									FR_CAMERA_INFO_PROP_UINT32_DEVICE_ID,
									(uint32_t*)&nDeviceId)) )
		{
			vDeviceIdx.emplace_back(nDeviceId);
		}

		BaseObject::ICharString* pDeviceName = nullptr;
		if( FR_SUCCEEDED(pCameraInfo->GetString(
									FR_CAMERA_INFO_PROP_STRING_DEVICE_NAME,
									&pDeviceName)) )
		{
			vDeviceName.emplace_back(pDeviceName->GetString());
		}

		BaseObject::ICharString* pDevicePath = nullptr;
		if( FR_SUCCEEDED(pCameraInfo->GetString(
									FR_CAMERA_INFO_PROP_STRING_DEVICE_PATH,
									&pDevicePath)) )
		{
			vDevicePath.emplace_back(pDevicePath->GetString());
		}

		if( pDevicePath != nullptr ) {
			DebugString(GetMakeString(_T("[ FACEME ] [%d] USBCamera Device Info : %d, %s, %s"),
												(UINT)vDeviceName.size() - 1,
												nDeviceId,
												CString(pDeviceName->GetString()),
												CString(pDevicePath->GetString())));
		}
		else {
			DebugString(GetMakeString(_T("[ FACEME ] [%d] USBCamera Device Info : %d, %s"),
												(UINT)vDeviceName.size() - 1,
												nDeviceId,
												CString(pDeviceName->GetString())));
		}

		if( pDeviceName != nullptr ) {
			pDeviceName->Release();
			pDeviceName = nullptr;
		}
		if( pDevicePath != nullptr ) {
			pDevicePath->Release();
			pDevicePath = nullptr;
		}

		if( pCameraInfo != nullptr ) {
			pCameraInfo->Release();
			pCameraInfo = nullptr;
		}
	}
	DebugString(_T("[ FACEME ] Get USBCamera Device Information List - [END]"));
	DebugString(_T("[ FACEME ] FaceMeCameraManagerCfg Initialize OK."));

	nSeq = 0;
	for( auto name : vDeviceName ) {
		CString strName(CA2CT(name.c_str()));
		strName.TrimRight();
		if( strName.Compare(m_strTargetName) == 0 ) {
			m_nConnectIdx = vDeviceIdx[nSeq];
			break;
		}
		nSeq++;
	}
	return m_nConnectIdx != -1 ? TRUE : FALSE;
}


// 
// \brief <pre>
// ������ ��������.
// </pre>
// \return  BOOL 
// 
BOOL CVisableCamera::FaceMeCameraCapturer()
{
	FR_RETURN nResult = FR_RETURN_OK;
	FR_CameraCapturerConfig stCameraCaptureCfg;
	FR_INIT_STRUCT(&stCameraCaptureCfg, FR_CameraCapturerConfig);
	stCameraCaptureCfg.cameraType = FR_CAMERA_TYPE_LOCAL_CAMERA;

	nResult = m_pFMCameraManager->CreateCameraCapturer(m_nConnectIdx,
													   &stCameraCaptureCfg,
													   &m_pFMCameraCapture);
	if( FR_FAILED(nResult) ) {
		m_nFaceMeError = nResult;
		DebugString(_T(">> [ E ] [ FACEME ] CreateCameraCapturer Fail. err[%d]"),
					m_nFaceMeError);
		return FALSE;
	}
	DebugString(_T("[ FACEME ] CameraCapture Create OK. [%d]"),
				m_nConnectIdx);
	
	m_bConnect = TRUE;
	return TRUE;
}


// 
// \brief <pre>
// ������ ��������.
// </pre>
// \return  BOOL 
// 
BOOL CVisableCamera::StreamStart()
{
	FR_RETURN nResult = FR_RETURN_OK;
	FR_CaptureConfig stCaptureCfg;
	FR_INIT_STRUCT(&stCaptureCfg, FR_CaptureConfig);
	stCaptureCfg.preferredWidth		= m_nWidth;
	stCaptureCfg.preferredHeight	= m_nHeight;
	stCaptureCfg.preferredFrameRate	= 30.f;
	stCaptureCfg.orientation		= theINIMan().IsMirror()
										? FR_CAPTURE_ORIENTATION_FLIP_HORIZONTAL
										: FR_CAPTURE_ORIENTATIONE_NONE;

	nResult = m_pFMCameraCapture->Start(&stCaptureCfg, this);
	if( FR_FAILED(nResult) ) {
		m_nFaceMeError = nResult;
		DebugString(GetMakeString(
			_T(">> [ E ] [ FACEME ] CameraCapture Stream Start Fail. err[%d]"),
			m_nFaceMeError));
		return FALSE;
	}

	m_nWriteIdx = 0;

	m_cvFrame.create(m_nHeight, m_nWidth, CV_8UC3);
	if( theINIMan().IsVisableZoom() ) {
		m_cvBlack.create(m_nHeight, m_nWidth, CV_8UC3);
	}
	return TRUE;
}


// 
// \brief <pre>
// ������ ��������.
// </pre>
// \return  BOOL 
// 
BOOL CVisableCamera::IsConnected()
{
	return m_bConnect;
}


// 
// \brief <pre>
// ������ ��������.
// </pre>
// \return  BOOL 
// 
BOOL CVisableCamera::Disconnect()
{
	if( m_hThread != nullptr ) {
		SetEvent(m_hEvtLoop[0]);
		WaitForSingleObject(m_hThread, INFINITE);
		m_hThread	= nullptr;
		m_nThreadID	= 0;
	}

	if( m_pFMCameraCapture != nullptr ) {
		m_pFMCameraCapture->Stop();
		m_pFMCameraCapture->Release();
		m_pFMCameraCapture = nullptr;
	}

	if( m_pFMCameraManager != nullptr ) {
		m_pFMCameraManager->Finalize();
		m_pFMCameraManager->Release();
		m_pFMCameraManager = nullptr;
	}

	if( m_pFMRecognizer != nullptr ) {
		m_pFMRecognizer->Finalize();
		m_pFMRecognizer->Release();
		m_pFMRecognizer = nullptr;
	}

	if( m_pFMLicense != nullptr ) {
		m_pFMLicense->Finalize();
		m_pFMLicense->Release();
		m_pFMLicense = nullptr;
	}

	if( m_pFaceME != nullptr ) {
		delete m_pFaceME;
		m_pFaceME = nullptr;
	}

	m_bConnect = FALSE;

	for( int i = 0 ; i < DEF_DATA_DEPTH ; ++i ) {
		m_pFrameInfo[i]->resetData();
	}

	::memset(m_arrFaceInfo, 0x00, sizeof(FACE_INFO) * DEF_MAX_FACE_COUNT);
	m_nFaceCount = 0;

	m_cvBlack.release();
	m_cvFrame.release();
	return TRUE;
}


// 
// \brief <pre>
// Skip Frame �� ����
// </pre>
// \param   UINT				a_nSkipFrame
// 
void CVisableCamera::SetVisableCamSkipFrame(
	_In_ UINT				a_nSkipFrame)
{
	m_nSkipFrame = a_nSkipFrame;
}


// 
// \brief <pre>
// ��ӹ��� IFaceMeVideoStreamHandler�� �����Լ�
// �������� �Ѿ���� �ش�Function�� ȣ��ȴ�.
// </pre>
// \param   FR_VideoFrameInfo*	a_pFrameInfo
// \param   FR_Image*			a_pFrame
// \param   FR_DepthMap*		a_pDepthMap
// 
void CVisableCamera::OnVideoFrame(
	_In_ const FR_VideoFrameInfo*	a_pFrameInfo,
	_In_ const FR_Image*			a_pFrame,
	_In_ const FR_DepthMap*			a_pDepthMap)
{
	// �ǻ� �������� Skip�Ѵ�.
	// m_nSkipFrame ��ŭ Skip
	// �ǻ� ī�޶� 30 or 60 FPS�� �����ָ�
	// ���ϰ� ������ ��ü������ ������ �϶����� ����
	if( m_nCurrFrameSeq < m_nSkipFrame ) {
		m_nCurrFrameSeq++;
		return;
	}

	auto pVisable = m_pVFInfo[m_nWriteIdx];
	if( pVisable == nullptr ) {
		return;
	}

	pVisable->reset();

	m_cvFrame.data = (LPBYTE)a_pFrame->data;

	// �����̹����� �ڸ��ٸ�
	if( theINIMan().IsVisableZoom() ) {
		// ���������� ������ ������ �̹�����
		// ���� �̹������� ���÷��̵� ����(Zoom����)�� �ٿ��ִ´�.
		// �󱼰��� �˰����� ���ϸ� ���̱� ����
		m_cvBlack = cv::Scalar(0);
		CRect rcZoom = m_pVCamView->GetZoomRect();

		// ���� �̹������� Zoom����
		cv::Mat roi_src = m_cvFrame(cv::Rect(rcZoom.left,
											 rcZoom.top,
											 rcZoom.Width(),
											 rcZoom.Height()));

		// �������� �̹������� Zoom����
		cv::Mat roi_dst = m_cvBlack(cv::Rect(rcZoom.left,
											 rcZoom.top,
											 rcZoom.Width(),
											 rcZoom.Height()));

		// ����Zoom������ ��������Zoom�������� ����
		roi_src.copyTo(roi_dst);

		// Direct2D ����� ���� BGR(24bit)���� BGRA(32bit)�� ��ȯ
		cv::cvtColor(m_cvBlack,
					 cv::Mat(m_nHeight,
							 m_nWidth,
							 CV_8UC4,
							 pVisable->image.data),
					 cv::COLOR_BGR2BGRA);
	}
	else {
		// Direct2D ����� ���� BGR(24bit)���� BGRA(32bit)�� ��ȯ
		cv::cvtColor(m_cvFrame,
					 cv::Mat(m_nHeight,
							 m_nWidth,
							 CV_8UC4,
							 pVisable->image.data),
					 cv::COLOR_BGR2BGRA);
	}
	
	m_pTargetBuff = pVisable;
	m_nWriteIdx++;
	if( m_nWriteIdx >= DEF_DATA_DEPTH ) {
		m_nWriteIdx = 0;
	}

	auto pThermal	= theTCMan().GetThermalBuff();
	if( pThermal != nullptr ) {
		int tw			= theTCMan().GetThermalWidth();
		int th			= theTCMan().GetThermalHeight();
		auto pDst		= m_pFrameInfo[m_nFrameWriteIdx];

		// �ǻ��̹��� ����
		::memcpy_s(pDst->visable_image.data,
				   m_nWidth * m_nHeight * 4,
				   pVisable->image.data,
				   m_nWidth * m_nHeight *4);

		// ���� Gray�̹��� ����
		::memcpy_s(pDst->thermal_gray.data,
				   tw * th,
				   pThermal->gray.data,
				   tw * th);

		// ���� �µ������� ����
		::memcpy_s(pDst->thermal_temp.data,
				   tw * th * sizeof(float),
				   pThermal->temp.data,
				   tw * th * sizeof(float));

		// ���� �ȷ�Ʈ�̹��� ����
		::memcpy_s(pDst->thermal_image.data,
				   tw * th * 4,
				   pThermal->image.data,
				   tw * th * 4);


		m_nFrameWriteIdx++;
		if( m_nFrameWriteIdx >= 5 ) {
			m_nFrameWriteIdx = 0;
		}
		m_pFrameTarget = pDst;
		SetEvent(m_hEvtLoop[1]);

		m_StreamFPS.CalcFrameRate();
		if( m_pVCamView != nullptr && m_pTCamView != nullptr ) {
			m_pVCamView->SetData(pDst);
			m_pTCamView->SetData(pDst);
			::InvalidateRect(m_pVCamView->GetSafeHwnd(),
							 nullptr,
							 FALSE);
			::InvalidateRect(m_pTCamView->GetSafeHwnd(),
							 nullptr,
							 FALSE);
		}
	}

	if( m_nCurrFrameSeq >= m_nSkipFrame ) {
		m_nCurrFrameSeq = 0;
	}
}


void CVisableCamera::OnEndOfStream()
{

}


void CVisableCamera::OnError(
	_In_ FR_RETURN				a_nResult)
{

}


THREADFUNC CVisableCamera::ThreadProc(
	_In_ LPVOID					a_pVoid)
{
	CVisableCamera* pThis = (CVisableCamera*)a_pVoid;
	if( pThis != nullptr ) {
		pThis->RunThreadProc();
	}
	return 0;
}


void CVisableCamera::RunThreadProc()
{
	DWORD dwWaitTime = INFINITE;
	while( TRUE ) {
		DWORD dwRet = WaitForMultipleObjects(2, m_hEvtLoop, FALSE, dwWaitTime);
		if( dwRet == WAIT_OBJECT_0 ) {
			break;
		}

		auto pDst = m_pFrameTarget;
		if( IsConnected() && theTCMan().IsConnected() ) {
			if (FaceDetection(pDst))
			{
				//skpark in your area
				if (m_cox_guardian && m_nFaceCount  > 0)
				{
					//skpark in your area
					TraceLog((_T("New Face Detected (%d) ========================================================"), m_nFaceCount));
					m_cox_guardian->NewFaceDetected(pDst, m_nFaceCount);
				}
				//skpark in your area end
			}
			else  // It seems it's not new face
			{
				if (m_cox_guardian)
				{
					//skpark in your area start
					static int feverCount = 0;   
					for (UINT i = 0; i < m_nFaceCount; ++i) {
						FACE_INFO* pFace = &pDst->face_detect[i];
						if (m_cox_guardian->isCase(pFace))
						{
							feverCount++;
							if (feverCount > m_cox_guardian->m_config->m_valid_event_limit) // Ƣ�� ���� ���� ����, 6���̻� �������� fever �� üũ�� ��쿡 ���ؼ� Fever �� �����Ѵ�.
							{
								if (m_cox_guardian->IsSameMan(m_pFMRecognizer, &(pFace->facefeature)) == false) //�����ι� üũ
								{
									m_cox_guardian->CaseDetected(pFace,
										pFace->visable_facebox,
										pDst->visable_image.size,
										pDst->visable_image.data
										);
								}
								feverCount = 0;
							}
						}
						else{
							feverCount = 0;
						}
					}
				} // for
			} // else
			// skpark in your area end
		} // IsConnected() && theTCMan().IsConnected()
		if (m_cox_guardian) { ::Sleep(m_cox_guardian->m_config->m_wait_time); }  		//skpark in your area  ���ϸ� ���̱� ����
	} // while
}


// 
// \brief <pre>
// �󱼰��� Function
// </pre>
// \param   CAMF_INFO*			a_pFrame
// \return  BOOL 
// 
BOOL CVisableCamera::FaceDetection(
	_In_ CAMF_INFO*			a_pFrame)
{
	ASSERT(a_pFrame);

	FR_Image stImage;
	FR_INIT_STRUCT(&stImage, FR_Image);

	stImage.channel		= 4;
	stImage.width		= m_nWidth;
	stImage.height		= m_nHeight;
	stImage.stride		= m_nWidth * stImage.channel;
	stImage.pixelFormat	= FR_PIXEL_FORMAT_BGRA;
	stImage.data		= a_pFrame->visable_image.data;

	FR_ExtractConfig stExtractCfg;
	FR_INIT_STRUCT(&stExtractCfg, FR_ExtractConfig);

	stExtractCfg.extractOptions = 
		FR_OPT(FR_FEATURE_OPTION_BOUNDING_BOX		// �� �ڽ�
		| FR_FEATURE_OPTION_OCCLUSION				// ����ũ ���� ����
		| FR_FEATURE_OPTION_FEATURE_LANDMARK		// ������ ���帶ũ
		| FR_FEATURE_OPTION_POSE					// �� ����
		| FR_FEATURE_OPTION_FULL_FEATURE);			// �� Ư¡

	uint32_t nImgNum = 1;
	std::vector<uint32_t> vFaceNum(nImgNum);

	FR_RETURN nResult = m_pFMRecognizer->ExtractFace(&stExtractCfg,
													 &stImage,
													 nImgNum,
													 vFaceNum.data());

	std::vector<FR_FaceInfo>		vFaceInfo(vFaceNum.at(0));
	std::vector<FR_FaceAttribute>	vFaceAttribute(vFaceNum.at(0));
	std::vector<FR_FaceLandmark>	vFaceLandmark(vFaceNum.at(0));
	std::vector<FR_FaceFeature>		vFaceFeature(vFaceNum.at(0));

	if( FR_FAILED(nResult) ) {
		return FALSE;
	}

	a_pFrame->resetFace();

	//m_pFMRecognizer->CompareFaceFeature();

	int nCount = 0;
	for (UINT i = 0; i < (UINT)vFaceInfo.size(); ++i) {
		FR_INIT_STRUCT(&vFaceInfo.at(nCount), FR_FaceInfo);
		FR_INIT_STRUCT(&vFaceAttribute.at(nCount), FR_FaceAttribute);
		FR_INIT_STRUCT(&vFaceLandmark.at(nCount), FR_FaceLandmark);
		FR_INIT_STRUCT(&vFaceFeature.at(nCount), FR_FaceFeature);

		FACE_INFO stFace;
		// �� ��ǥ & ����ũ
		if (!GetFaceInfo(i, &vFaceInfo.at(i), &stFace)) {
			continue;
		}

		// �󱼰��� �ڽ� ��ħ ����
		if (!IsBoxOverlapCheck(nCount, &vFaceInfo.at(i), nCount + 1, a_pFrame->face_detect)) {
			continue;
		}

		// �ǻ� ���� ��ũ�� ��ǥ��� ��ȯ
		ConvertScreenRect(&stFace);

		// �󱼹ڽ� ���� ��� üũ
		
		// skpark in your area  start //  ��ȿ������ ����ؼ�, ���� �ݹ� ��ĥ���� �ν����� �ʵ��� �Ѵ�.
		if (m_cox_guardian) {
			if (!m_cox_guardian->IsFaceBoxInTheramlArea(&stFace, m_pTCamView)) {
				continue;
			}
		}
		// skpark in your area end
		else {
			if (!IsFaceBoxInTheramlArea(&stFace)) {
				continue;
			}
		}

		// �� ����, ����, ����, ���� ����
		if( !GetFaceAttribute(i, &vFaceAttribute.at(i), &stFace) ) {
			continue;
		}

		// ������ ��ġ
		if( !GetFaceLandmark(i, &vFaceLandmark.at(i), &stFace) ) {
			continue;
		}

		// �� Ư¡
		if( !GetFaceFeature(i, &vFaceFeature.at(i), &stFace) ) {
			continue;
		}
		
		::memcpy_s(a_pFrame->face_detect + nCount,
				   sizeof(FACE_INFO),
				   &stFace,
				   sizeof(FACE_INFO));
		nCount++;
	}
	a_pFrame->face_count = nCount;

	for( int i = 0 ; i < nCount ; ++i ) {
		theTCMan().GetFaceInfoTemp(&a_pFrame->face_detect[i],
								   a_pFrame->thermal_temp.data);
	}

	int prevFaceCount = m_nFaceCount;  // skpark in your area,  ���� faceCount �� �����Ѵ�.
	{
		CLock hLock(m_hLock);
		m_FaceDetectFPS.CalcFrameRate();
		::memcpy_s(m_arrFaceInfo,
				   sizeof(FACE_INFO) * 30,
				   a_pFrame->face_detect,
				   sizeof(FACE_INFO) * 30);
		m_nFaceCount = nCount;
	}
	return  (prevFaceCount != m_nFaceCount);  //skpark in your area,  count �� ���� ���,
	//return TRUE;
}


//
// \brief <pre>
// �� ��ǥ, Ȯ��, ����ũ ������ �����´�.
// </pre>
// \param   UINT				a_nSeq
// \param   FR_FaceInfo*		a_pFRFaceInfo
// \param   FACE_INFO*			a_pFaceInfo
// \return  BOOL 
// 
BOOL CVisableCamera::GetFaceInfo(
	_In_ UINT				a_nSeq,
	_In_ FR_FaceInfo*		a_pFRFaceInfo,
	_Out_ FACE_INFO*		a_pFaceInfo)
{
	FR_RETURN nResult = FR_RETURN_OK;
	nResult = m_pFMRecognizer->GetFaceInfo(0,
										   (uint32_t)a_nSeq,
										   a_pFRFaceInfo);

	if( FR_FAILED(nResult) ) {
		return FALSE;
	}

	// �� �ڽ���ǥ
	FR_Rectangle box = a_pFRFaceInfo->boundingBox;
	CRect facebox(box.topLeft.x,
				  box.topLeft.y,
				  box.bottomRight.x,
				  box.bottomRight.y);
	a_pFaceInfo->facebox.CopyRect(facebox);

	// ���� Ȯ��
	a_pFaceInfo->confidence	= a_pFRFaceInfo->confidence;

	// ����ũ ����
	if( HAS_OPTION(a_pFRFaceInfo->occlusion,
				   FR_OCCLUSION_STATUS_NOSE
				   | FR_OCCLUSION_STATUS_MOUTH
				   | FR_OCCLUSION_STATUS_MASK) )
	{
		// ���� ����
		a_pFaceInfo->mask_state = MASK_WEARING_WELL;
	}
	else if( HAS_OPTION(a_pFRFaceInfo->occlusion,
						FR_OCCLUSION_STATUS_MASK) )
	{
		// ����ũ ����� �Ⱦ�
		a_pFaceInfo->mask_state = MASK_NOT_RIGHT_WAY_WEARING;
	}
	else {
		// ����ũ ������
		a_pFaceInfo->mask_state = MASK_NOT_WEARING;
	}
	
	return TRUE;
}


// 
// \brief <pre>
// �󱼹ڽ��� ���÷��̵Ǵ� ���� ���ο� �ִ��� üũ
// TRUE : ����
// FALSE : �ܺ� or ��ħ
// </pre>
// \param   FACE_INFO*			a_pFaceInfo
// \return  BOOL 
// 
BOOL CVisableCamera::IsFaceBoxInTheramlArea(
	_In_ FACE_INFO*			a_pFaceInfo)
{
	CRect rcBox(a_pFaceInfo->thermal_facebox);
	CRect rcFilter(m_pTCamView->GetZoomRect());

	CRect rcDst;

	// rcFilter�� rcBox�� ������
	// ����� rcDst
	rcDst.IntersectRect(rcFilter, rcBox);

	// rcDst�� rcBox�� ��ġ���� �ʴ´ٸ�
	// ���󿵿� �ܺ� or ��ħ
	if( !rcDst.EqualRect(rcBox) ) {
		return FALSE;

		/*
		// Ư���� �̻� �������� ��� TRUE�� ���� �Ҷ�����
		// �� return FALSE �ּ�
		// �Ʒ� �ڵ带 �ּ� ����
		// ������, ����� ���������� ����.
		int nOriginalArea	= rcBox.Width() * rcBox.Height();
		int nRemainArea		= rcDst.Width() * rcBox.Height();

		float val = (float)nRemainArea / nOriginalArea;
		// ���ǰ��� �ʿ�
		// ex) 70% �̻��� ��� ���
		if( 0.7f <= val ) {
			a_pFaceInfo->thermal_facebox.CopyRect(rcDst);
			return TRUE;
		}
		else {
			return FALSE;
		}
		*/
	}

	return TRUE;
}


// 
// \brief <pre>
// �̹� ������ �󱼿� �ٽ� �� �󱼷� �����Ǵ� ��찡 ����.
// �̸� �����ϱ� ���� �̹� ������ �󱼰� ���Ӱ� ������ ����
// ������ �� ���ǰ���ŭ ���ƴٸ� �н��Ѵ�.
// ���ǰ��� Default���� 0.7(70%)
// </pre>
// \param   UINT			a_nCurrIdx
// \param   FR_FaceInfo*	a_pFRFaceInfo
// \param   int				a_nFaceCount
// \param   FACE_INFO*		a_pArrayFaceInfo
// \return  BOOL 
// 
BOOL CVisableCamera::IsBoxOverlapCheck(
	_In_ UINT				a_nCurrIdx,
	_In_ FR_FaceInfo*		a_pFRFaceInfo,
	_In_ int				a_nFaceCount,
	_In_ FACE_INFO*			a_pArrayFaceInfo)
{
	BOOL bResult = TRUE;

	CRect rcBox(a_pFRFaceInfo->boundingBox.topLeft.x,
				a_pFRFaceInfo->boundingBox.topLeft.y,
				a_pFRFaceInfo->boundingBox.bottomRight.x,
				a_pFRFaceInfo->boundingBox.bottomRight.y);

	for( int i = 0 ; i < a_nFaceCount ; ++i ) {
		if( a_nCurrIdx == (UINT)i ) {
			continue;
		}
		auto pInfo = &a_pArrayFaceInfo[i];

		// ���� ������ �ڽ��� ���� �ִٸ�
		if( theHelperMan().GetOverlapRectScore(pInfo->facebox, rcBox)
		   >= theINIMan().GetPassOverlapBoxRatio() )
		{
			bResult = FALSE;
			break;
		}
	}

	return bResult;
}


// 
// \brief <pre>
// ���� ����, ����, ����, ������ �����Ѵ�.
// </pre>
// \param   UINT				a_nSeq
// \param   FR_FaceAttribute*	a_pFRFaceAttribute
// \param   FACE_INFO*			a_pFaceInfo
// \return  BOOL 
// 
BOOL CVisableCamera::GetFaceAttribute(
	_In_ UINT				a_nSeq,
	_In_ FR_FaceAttribute*	a_pFRFaceAttribute,
	_Out_ FACE_INFO*		a_pFaceInfo)
{
	FR_RETURN nResult = FR_RETURN_OK;
	nResult = m_pFMRecognizer->GetFaceAttribute(0,
												(uint32_t)a_nSeq,
												a_pFRFaceAttribute);

	if( FR_FAILED(nResult) ) {
		return FALSE;
	}

	// �¿찢 (���: ����, ����: ������)
	a_pFRFaceAttribute->pose.yaw;

	// ���Ʒ��� (���: ��, ����: �Ʒ�)
	a_pFRFaceAttribute->pose.pitch;

	// �¿����
	a_pFRFaceAttribute->pose.roll;

	// ���� ����
	a_pFRFaceAttribute->age;

	// ���� ����
	a_pFRFaceAttribute->emotion;

	// ������ ���� Ȯ��
	a_pFRFaceAttribute->emotionConfidence;

	// ���� ����
	a_pFRFaceAttribute->gender;

	// ������ ���� Ȯ��
	a_pFRFaceAttribute->genderConfidence;

	return TRUE;
}


// 
// \brief <pre>
// ���� �������� ��ġ�� ��´�.
// </pre>
// \param   UINT				a_nSeq
// \param   FR_FaceLandmark*	a_pFRFaceLandmark
// \param   FACE_INFO*			a_pFaceInfo
// \return  BOOL 
// 
BOOL CVisableCamera::GetFaceLandmark(
	_In_ UINT				a_nSeq,
	_In_ FR_FaceLandmark*	a_pFRFaceLandmark,
	_Out_ FACE_INFO*		a_pFaceInfo)
{
	FR_RETURN nResult = FR_RETURN_OK;
	nResult = m_pFMRecognizer->GetFaceLandmark(0,
											   (uint32_t)a_nSeq,
											   a_pFRFaceLandmark);
	if( FR_FAILED(nResult) ) {
		return FALSE;
	}

	for( uint32_t i = 0 ; i < a_pFRFaceLandmark->featurePointSize ; ++i ) {
		a_pFaceInfo->facelandmark[i].SetPoint(
										a_pFRFaceLandmark->featurePoints[i].x,
										a_pFRFaceLandmark->featurePoints[i].y);
	}

	return TRUE;
}


// 
// \brief <pre>
// ���� Ư¡�� ��´�.
// </pre>
// \param   UINT				a_nSeq
// \param   FR_FaceFeature*		a_pFRFaceFeature
// \param   FACE_INFO*			a_pFaceInfo
// \return  BOOL 
// 
BOOL CVisableCamera::GetFaceFeature(
	_In_ UINT				a_nSeq,
	_In_ FR_FaceFeature*	a_pFRFaceFeature,
	_Out_ FACE_INFO*		a_pFaceInfo)
{
	FR_RETURN nResult = FR_RETURN_OK;
	nResult = m_pFMRecognizer->GetFaceFeature(0,
											  (uint32_t)a_nSeq,
											  a_pFRFaceFeature);
	if( FR_FAILED(nResult) ) {
		return FALSE;
	}

	//m_pFMRecognizer->CompareFaceFeature   ������ �Ǵ�....

	::memcpy_s(&a_pFaceInfo->facefeature,
			   sizeof(FR_FaceFeature),
			   a_pFRFaceFeature,
			   sizeof(FR_FaceFeature));

	return TRUE;
}


// 
// \brief <pre>
// �ǻ��� �󱼹ڽ���ǥ�� ������ǥ�� ��ȯ
// </pre>
// \param   FACE_INFO*		a_pFaceInfo
// 
void  CVisableCamera::ConvertScreenRect(
	_Inout_ FACE_INFO*		a_pFaceInfo)
{
	CRect box(a_pFaceInfo->facebox);

	// �ǻ�ڽ� ��ǥ
	a_pFaceInfo->visable_facebox.CopyRect(box);
	
	// ���ڽ� ��ǥ -> ����ڽ� ��ǥ ��ȯ
	a_pFaceInfo->thermal_facebox.SetRect(
		theHelperMan().GetVisable2ThermalPos(box.TopLeft()),
		theHelperMan().GetVisable2ThermalPos(box.BottomRight()));

}


// 
// \brief <pre>
// �ֽ� �󱼰��� ������ ��´�.
// </pre>
// \param   FACE_INFO*		a_pArrayFace
// \return  int 
// 
int CVisableCamera::GetFaceDetectInfo(
	_Inout_ FACE_INFO*		a_pArrayFace)
{
	CLock hLock(m_hLock);
	::memcpy_s(a_pArrayFace,
			   sizeof(FACE_INFO) * 30,
			   m_arrFaceInfo,
			   sizeof(FACE_INFO) * 30);

	return m_nFaceCount;
}


// 
// \brief <pre>
// �ǻ� ���� ������
// </pre>
// \return  int 
// 
int CVisableCamera::GetVisableWidth()
{
	return m_nWidth;
}


// 
// \brief <pre>
// �ǻ� ���� ������
// </pre>
// \return  int 
// 
int CVisableCamera::GetVisableHeight()
{
	return m_nHeight;
}


// 
// \brief <pre>
// ��Ʈ���� FPS
// </pre>
// \return  int 
// 
int CVisableCamera::GetStreamFPS()
{
	return m_StreamFPS.GetFrameRate();
}


// 
// \brief <pre>
// �󱼰��� FPS
// </pre>
// \return  int 
// 
int CVisableCamera::GetFaceDetectFPS()
{
	return m_FaceDetectFPS.GetFrameRate();
}