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
// 초기화
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
		// 예외처리
		__debugbreak();
	}
	if( !FaceMeRecognizerCfg() ) {
		// 예외처리
		__debugbreak();
	}
	if( !FaceMeCameraManagerCfg() ) {
		// 예외처리
		__debugbreak();
	}
	if( !FaceMeCameraCapturer() ) {
		// 예외처리
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
// 내용을 넣으세요.
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
// 내용을 넣으세요.
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
// 내용을 넣으세요.
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
// 내용을 넣으세요.
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
// 내용을 넣으세요.
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
// 내용을 넣으세요.
// </pre>
// \return  BOOL 
// 
BOOL CVisableCamera::IsConnected()
{
	return m_bConnect;
}


// 
// \brief <pre>
// 내용을 넣으세요.
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
// Skip Frame 값 셋팅
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
// 상속받은 IFaceMeVideoStreamHandler의 가상함수
// 프레임이 넘어오면 해당Function이 호출된다.
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
	// 실상 프레임을 Skip한다.
	// m_nSkipFrame 만큼 Skip
	// 실상 카메라가 30 or 60 FPS로 보내주면
	// 부하가 심해져 전체적으로 성능이 하락함을 방지
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

	// 원본이미지를 자른다면
	if( theINIMan().IsVisableZoom() ) {
		// 검은바탕의 동일한 사이즈 이미지에
		// 원본 이미지에서 디스플레이될 영역(Zoom영역)만 붙여넣는다.
		// 얼굴감지 알고리즘의 부하를 줄이기 위함
		m_cvBlack = cv::Scalar(0);
		CRect rcZoom = m_pVCamView->GetZoomRect();

		// 원본 이미지에서 Zoom영역
		cv::Mat roi_src = m_cvFrame(cv::Rect(rcZoom.left,
											 rcZoom.top,
											 rcZoom.Width(),
											 rcZoom.Height()));

		// 검은바탕 이미지에서 Zoom영역
		cv::Mat roi_dst = m_cvBlack(cv::Rect(rcZoom.left,
											 rcZoom.top,
											 rcZoom.Width(),
											 rcZoom.Height()));

		// 원본Zoom영역을 검은바탕Zoom영역으로 복사
		roi_src.copyTo(roi_dst);

		// Direct2D 사용을 위해 BGR(24bit)에서 BGRA(32bit)로 변환
		cv::cvtColor(m_cvBlack,
					 cv::Mat(m_nHeight,
							 m_nWidth,
							 CV_8UC4,
							 pVisable->image.data),
					 cv::COLOR_BGR2BGRA);
	}
	else {
		// Direct2D 사용을 위해 BGR(24bit)에서 BGRA(32bit)로 변환
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

		// 실상이미지 복사
		::memcpy_s(pDst->visable_image.data,
				   m_nWidth * m_nHeight * 4,
				   pVisable->image.data,
				   m_nWidth * m_nHeight *4);

		// 열상 Gray이미지 복사
		::memcpy_s(pDst->thermal_gray.data,
				   tw * th,
				   pThermal->gray.data,
				   tw * th);

		// 열상 온도데이터 복사
		::memcpy_s(pDst->thermal_temp.data,
				   tw * th * sizeof(float),
				   pThermal->temp.data,
				   tw * th * sizeof(float));

		// 열상 팔레트이미지 복사
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
							if (feverCount > m_cox_guardian->m_config->m_valid_event_limit) // 튀는 값을 막기 위해, 6번이상 연속으로 fever 로 체크된 경우에 한해서 Fever 로 간주한다.
							{
								if (m_cox_guardian->IsSameMan(m_pFMRecognizer, &(pFace->facefeature)) == false) //동일인물 체크
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
		if (m_cox_guardian) { ::Sleep(m_cox_guardian->m_config->m_wait_time); }  		//skpark in your area  부하를 줄이기 위해
	} // while
}


// 
// \brief <pre>
// 얼굴감지 Function
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
		FR_OPT(FR_FEATURE_OPTION_BOUNDING_BOX		// 얼굴 박스
		| FR_FEATURE_OPTION_OCCLUSION				// 마스크 착용 유무
		| FR_FEATURE_OPTION_FEATURE_LANDMARK		// 눈코입 랜드마크
		| FR_FEATURE_OPTION_POSE					// 얼굴 각도
		| FR_FEATURE_OPTION_FULL_FEATURE);			// 얼굴 특징

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
		// 얼굴 좌표 & 마스크
		if (!GetFaceInfo(i, &vFaceInfo.at(i), &stFace)) {
			continue;
		}

		// 얼굴감지 박스 겹침 방지
		if (!IsBoxOverlapCheck(nCount, &vFaceInfo.at(i), nCount + 1, a_pFrame->face_detect)) {
			continue;
		}

		// 실상 열상 스크린 좌표계로 변환
		ConvertScreenRect(&stFace);

		// 얼굴박스 열상 벗어남 체크
		
		// skpark in your area  start //  유효영역을 축소해서, 얼굴이 반반 걸칠때는 인식하지 않도록 한다.
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

		// 얼굴 각도, 나이, 성별, 감정 추정
		if( !GetFaceAttribute(i, &vFaceAttribute.at(i), &stFace) ) {
			continue;
		}

		// 눈코입 위치
		if( !GetFaceLandmark(i, &vFaceLandmark.at(i), &stFace) ) {
			continue;
		}

		// 얼굴 특징
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

	int prevFaceCount = m_nFaceCount;  // skpark in your area,  이전 faceCount 를 보관한다.
	{
		CLock hLock(m_hLock);
		m_FaceDetectFPS.CalcFrameRate();
		::memcpy_s(m_arrFaceInfo,
				   sizeof(FACE_INFO) * 30,
				   a_pFrame->face_detect,
				   sizeof(FACE_INFO) * 30);
		m_nFaceCount = nCount;
	}
	return  (prevFaceCount != m_nFaceCount);  //skpark in your area,  count 가 변한 경우,
	//return TRUE;
}


//
// \brief <pre>
// 얼굴 좌표, 확률, 마스크 정보를 가져온다.
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

	// 얼굴 박스좌표
	FR_Rectangle box = a_pFRFaceInfo->boundingBox;
	CRect facebox(box.topLeft.x,
				  box.topLeft.y,
				  box.bottomRight.x,
				  box.bottomRight.y);
	a_pFaceInfo->facebox.CopyRect(facebox);

	// 얼굴일 확률
	a_pFaceInfo->confidence	= a_pFRFaceInfo->confidence;

	// 마스크 정보
	if( HAS_OPTION(a_pFRFaceInfo->occlusion,
				   FR_OCCLUSION_STATUS_NOSE
				   | FR_OCCLUSION_STATUS_MOUTH
				   | FR_OCCLUSION_STATUS_MASK) )
	{
		// 정상 착용
		a_pFaceInfo->mask_state = MASK_WEARING_WELL;
	}
	else if( HAS_OPTION(a_pFRFaceInfo->occlusion,
						FR_OCCLUSION_STATUS_MASK) )
	{
		// 마스크 제대로 안씀
		a_pFaceInfo->mask_state = MASK_NOT_RIGHT_WAY_WEARING;
	}
	else {
		// 마스크 미착용
		a_pFaceInfo->mask_state = MASK_NOT_WEARING;
	}
	
	return TRUE;
}


// 
// \brief <pre>
// 얼굴박스가 디스플레이되는 영역 내부에 있는지 체크
// TRUE : 내부
// FALSE : 외부 or 걸침
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

	// rcFilter와 rcBox의 교집합
	// 결과는 rcDst
	rcDst.IntersectRect(rcFilter, rcBox);

	// rcDst와 rcBox와 일치하지 않는다면
	// 열상영역 외부 or 걸침
	if( !rcDst.EqualRect(rcBox) ) {
		return FALSE;

		/*
		// 특정값 이상 걸쳐있을 경우 TRUE로 간주 할때에는
		// 위 return FALSE 주석
		// 아래 코드를 주석 해제
		// 하지만, 경험상 권장하지는 않음.
		int nOriginalArea	= rcBox.Width() * rcBox.Height();
		int nRemainArea		= rcDst.Width() * rcBox.Height();

		float val = (float)nRemainArea / nOriginalArea;
		// 조건값이 필요
		// ex) 70% 이상일 경우 통과
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
// 이미 감지된 얼굴에 다시 또 얼굴로 감지되는 경우가 있음.
// 이를 방지하기 위해 이미 감지된 얼굴과 새롭게 감지된 얼굴을
// 비교했을 때 조건값만큼 겹쳤다면 패스한다.
// 조건값의 Default값은 0.7(70%)
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

		// 기존 감지된 박스와 겹쳐 있다면
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
// 얼굴의 각도, 나이, 성별, 감정을 추정한다.
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

	// 좌우각 (양수: 왼쪽, 음수: 오른쪽)
	a_pFRFaceAttribute->pose.yaw;

	// 위아래각 (양수: 위, 음수: 아래)
	a_pFRFaceAttribute->pose.pitch;

	// 좌우기울기
	a_pFRFaceAttribute->pose.roll;

	// 추정 나이
	a_pFRFaceAttribute->age;

	// 추정 감정
	a_pFRFaceAttribute->emotion;

	// 추정한 감정 확률
	a_pFRFaceAttribute->emotionConfidence;

	// 추정 성별
	a_pFRFaceAttribute->gender;

	// 추정한 성별 확률
	a_pFRFaceAttribute->genderConfidence;

	return TRUE;
}


// 
// \brief <pre>
// 얼굴의 눈코입의 위치를 얻는다.
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
// 얼굴의 특징을 얻는다.
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

	//m_pFMRecognizer->CompareFaceFeature   동일인 판단....

	::memcpy_s(&a_pFaceInfo->facefeature,
			   sizeof(FR_FaceFeature),
			   a_pFRFaceFeature,
			   sizeof(FR_FaceFeature));

	return TRUE;
}


// 
// \brief <pre>
// 실상의 얼굴박스좌표를 열상좌표로 변환
// </pre>
// \param   FACE_INFO*		a_pFaceInfo
// 
void  CVisableCamera::ConvertScreenRect(
	_Inout_ FACE_INFO*		a_pFaceInfo)
{
	CRect box(a_pFaceInfo->facebox);

	// 실상박스 좌표
	a_pFaceInfo->visable_facebox.CopyRect(box);
	
	// 살상박스 좌표 -> 열상박스 좌표 변환
	a_pFaceInfo->thermal_facebox.SetRect(
		theHelperMan().GetVisable2ThermalPos(box.TopLeft()),
		theHelperMan().GetVisable2ThermalPos(box.BottomRight()));

}


// 
// \brief <pre>
// 최신 얼굴감지 정보를 얻는다.
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
// 실상 가로 사이즈
// </pre>
// \return  int 
// 
int CVisableCamera::GetVisableWidth()
{
	return m_nWidth;
}


// 
// \brief <pre>
// 실상 세로 사이즈
// </pre>
// \return  int 
// 
int CVisableCamera::GetVisableHeight()
{
	return m_nHeight;
}


// 
// \brief <pre>
// 스트리밍 FPS
// </pre>
// \return  int 
// 
int CVisableCamera::GetStreamFPS()
{
	return m_StreamFPS.GetFrameRate();
}


// 
// \brief <pre>
// 얼굴감지 FPS
// </pre>
// \return  int 
// 
int CVisableCamera::GetFaceDetectFPS()
{
	return m_FaceDetectFPS.GetFrameRate();
}