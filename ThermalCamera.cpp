#include "stdafx.h"
#include "TraceLog.h"
#include "ThermalCamera.h"
#include "INIManager.h"
#include "PlaneHomography.h"
#include "MyHelperMan.h"
#include "TCamView.h"

//skpark in your area
#include "skpark/TraceLog.h"  
#include "Guardian/CoxConfig.h"  
void CThermalCamera::SetConfig(CoxConfig* config) { m_config = config; }
bool CThermalCamera::ThreadCondition() { return (m_config && m_config->m_is_normal) ? FALSE : TRUE; }
//skpark in your area end

CThermalCamera& theTCMan()
{
	static CThermalCamera man;
	return man;
}


CThermalCamera::CThermalCamera()
	: m_pTargetBuff(nullptr)
	, m_nWriteIdx(0)
	, m_hMainWnd(nullptr)
	, m_bConnect(FALSE)
	, m_nWidth(0)
	, m_nHeight(0)
	, m_hNetwork(INVALID_HANDLE_VALUE)
	, m_nKeepAliveID(0)
	, m_hThread(nullptr)
	, m_nThreadID(0)
	, m_dwWaitTime(INFINITE)
	, m_nBitPixel(32)
	, m_pTCamView(nullptr)
	, m_fMeasureMinTemp(0.f)
	, m_fMeasureMaxTemp(0.f)
	, m_pMirrorTempBuff(nullptr)
	, m_pMirrorRawBuff(nullptr)
	, m_config(NULL)  //skpark in your area
{
	for( int i = 0 ; i < DEF_MAX_PALETTE_COUNT ; ++i ) {
		m_pPalette[i][0].Init(DEF_MAX_PALETTE_SIZE, 1);
		m_pPalette[i][1].Init(DEF_MAX_PALETTE_SIZE, 1);
	}

	for( int i = 0 ; i < DEF_DATA_DEPTH ; ++i ) {
		m_pTFInfo[i] = new TF_INFO();
		m_pTFInfo[i]->idx = i;
	}

	ResetIRCamData();

	m_hEvtLoop[0] = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hEvtLoop[1] = CreateEvent(NULL, TRUE, FALSE, NULL);
}


CThermalCamera::~CThermalCamera()
{
	for( int i = 0 ; i < DEF_DATA_DEPTH ; ++i ) {
		DeleteObj(m_pTFInfo[i]);
	}

	if( m_stIRData.lpNextData != nullptr ) {
		DeleteAry(m_stIRData.lpNextData);
	}

	CloseHdl(m_hEvtLoop[0]);
	CloseHdl(m_hEvtLoop[1]);

	DeleteAry(m_pMirrorRawBuff);
	DeleteAry(m_pMirrorTempBuff);
}


// 
// \brief <pre>
// 내용을 넣으세요.
// </pre>
// 
void CThermalCamera::ResetIRCamData()
{
	if( m_pMirrorRawBuff == nullptr ) {
		m_pMirrorRawBuff = new uint16_t[DEF_MAX_THERMAL_CAM_W * DEF_MAX_THERMAL_CAM_H];
	}
	::memset(m_pMirrorRawBuff, 0x00, sizeof(uint16_t) * DEF_MAX_THERMAL_CAM_W * DEF_MAX_THERMAL_CAM_H);

	if( m_pMirrorTempBuff == nullptr ) {
		m_pMirrorTempBuff = new float[DEF_MAX_THERMAL_CAM_W * DEF_MAX_THERMAL_CAM_H];
	}
	::memset(m_pMirrorTempBuff, 0x00, sizeof(float) * DEF_MAX_THERMAL_CAM_W * DEF_MAX_THERMAL_CAM_H);

	if( m_stIRData.lpNextData == nullptr ) {
		m_stIRData.lpNextData = new uint8_t[IRF_BUFFER_SIZE];
	}
	::memset(m_stIRData.lpNextData, 0x00, IRF_BUFFER_SIZE);

	auto pNextData = m_stIRData.lpNextData;

	::memset(&m_stIRData, 0x00, sizeof(IRF_IR_CAM_DATA_T));
	m_stIRData.lpNextData			= pNextData;
	m_stIRData.image_buffer_size	= DEF_MAX_THERMAL_CAM_W * DEF_MAX_THERMAL_CAM_H;
	m_nWriteIdx						= 0;
	m_stIRData.ir_image				= m_pTFInfo[m_nWriteIdx]->raw.data;
}


BOOL CThermalCamera::Connect(
	_In_ CTCamView*			a_pTCamView,
	_In_ HWND				a_hMainWnd,
	_In_ const CString&		a_strIP,
	_In_ IRF_PALETTE_TYPE_T	a_nPalette)
{
	ResetIRCamData();

	m_pTCamView	= a_pTCamView;
	m_hMainWnd	= a_hMainWnd;
	m_strIP		= a_strIP;
	m_nPalette	= a_nPalette;

	char szIP[32] = { 0, };
	sprintf_s(szIP, "%S", m_strIP.GetBuffer());

	int16_t nRet = OpenConnect(&m_hNetwork,
							   &m_nKeepAliveID,
							   szIP,
							   "15001",
							   AF_INET,
							   SOCK_STREAM);

	if( nRet != IRF_NO_ERROR ) {
		return FALSE;
	}

	SendCameraCommand(_IRF_REQ_CAM_DATA);
	nRet = GetIRImages(m_hNetwork, &m_nKeepAliveID, &m_stIRData);
	if( nRet != IRF_NO_ERROR ) {
		return FALSE;
	}

	if( m_stIRData.msg_type == _IRF_CAM_DATA && !InitCameraInfo() ) {
		DebugString(_T(">> [E] InitCameraInfo Fail."));
		return FALSE;
	}

	m_bConnect = TRUE;
	ResetEvent(m_hEvtLoop[0]);
	ResetEvent(m_hEvtLoop[1]);
	m_hThread = (HANDLE)_beginthreadex(NULL,
									   0,
									   &CThermalCamera::ThreadProc,
									   this,
									   0,
									   &m_nThreadID);
	return TRUE;
}


BOOL CThermalCamera::Disconnect()
{
	if( !m_bConnect ) {
		return TRUE;
	}

	if( m_hThread != nullptr ) {
		SetEvent(m_hEvtLoop[0]);
		WaitForSingleObject(m_hThread, INFINITE);
		m_hThread	= nullptr;
		m_nThreadID = 0;
	}

	if( m_hNetwork != nullptr ) {
		CloseConnect(&m_hNetwork,
					 m_nKeepAliveID);

		m_hNetwork		= INVALID_HANDLE_VALUE;
		m_nKeepAliveID	= 0;
	}

	m_bConnect = FALSE;
	return TRUE;
}


// 
// \brief <pre>
// 내용을 넣으세요.
// </pre>
// \return  BOOL 
// 
BOOL CThermalCamera::SendCameraCommand(
	_In_ IRF_MESSAGE_TYPE_T		a_nMsgType,
	_In_ uint16_t				a_nMessage,
	_In_ uint16_t				a_nRCode)
{
	uint16_t nRet = SendCameraMessage(m_hNetwork,
									  &m_nKeepAliveID,
									  a_nMsgType,
									  a_nMessage,
									  a_nRCode);
	if( nRet != IRF_NO_ERROR ) {
		return FALSE;
	}
	return TRUE;
}


BOOL CThermalCamera::GetMatchingPoint(
	_In_ int					a_nVisableWidth,
	_In_ int					a_nThermalWidth)
{
	DebugString(_T("[ FEVER_CAM ]  [ GetMatchingPoint ] EEPROM Get Matching Info Start."));
	int16_t nRet = 0;
	IRF_EEPROMINFO eeprom;
	EEPROM_MATCHING_INFO* pMatchingInfo = NULL;
	::memset(&eeprom, 0, sizeof(IRF_EEPROMINFO));
	///////////////////////////////////////////////////////////////////////////

	//Sleep(100);
	nRet = SendGetEEPROM(m_hNetwork, &m_nKeepAliveID);
	if( nRet != IRF_NO_ERROR ) {
		CString strErr = GetSDKErr2Msg(nRet);
		DebugString(GetMakeString(
			_T(">> [ E ] [ FEVER_CAM ] [ SendGetEEPROM ] Get EEPROM ")
			_T("Fail. Msg[ %s ]"), strErr));
		return FALSE;
	}

	//Sleep(100);
	nRet = RecvEEPROM(m_hNetwork, &m_nKeepAliveID, &eeprom);
	
	if( nRet != IRF_NO_ERROR ) {
		/*nRet = SendGetEEPROM(m_hNetwork, &m_nKeepAliveID);
		nRet = RecvEEPROM(m_hNetwork, &m_nKeepAliveID, &eeprom);*/

		if( nRet != IRF_NO_ERROR ) {
			CString strErr = GetSDKErr2Msg(nRet);
			DebugString(GetMakeString(
				_T(">> [ E ] [ FEVER_CAM ] [ RecvEEPROM ] Recv EEPROM Fail. ")
				_T("Msg[ %s ]"), strErr));
			return FALSE;
		}
	}

	pMatchingInfo = (EEPROM_MATCHING_INFO*)&eeprom.buf[DEF_USER_EEPROM_IDX];

	::memcpy_s(
		&m_stMatchingInfo, 
		sizeof(EEPROM_MATCHING_INFO),
		pMatchingInfo,
		sizeof(EEPROM_MATCHING_INFO));

	if( (pMatchingInfo->m_ptFever[0].x == -1 || pMatchingInfo->m_ptFever[0].x == 0)
		&& (pMatchingInfo->m_ptFever[0].y == -1 || pMatchingInfo->m_ptFever[0].y == 0)
		&& (pMatchingInfo->m_ptFever[1].x == -1 || pMatchingInfo->m_ptFever[1].x == 0)
		&& (pMatchingInfo->m_ptFever[1].y == -1 || pMatchingInfo->m_ptFever[1].y == 0)
		&& (pMatchingInfo->m_ptFever[2].x == -1 || pMatchingInfo->m_ptFever[2].x == 0)
		&& (pMatchingInfo->m_ptFever[2].y == -1 || pMatchingInfo->m_ptFever[2].y == 0)
		&& (pMatchingInfo->m_ptFever[3].x == -1 || pMatchingInfo->m_ptFever[3].x == 0)
		&& (pMatchingInfo->m_ptFever[3].y == -1 || pMatchingInfo->m_ptFever[3].y == 0)
		&& (pMatchingInfo->m_ptReal[0].x == -1 || pMatchingInfo->m_ptReal[0].x == 0)
		&& (pMatchingInfo->m_ptReal[0].y == -1 || pMatchingInfo->m_ptReal[0].y == 0)
		&& (pMatchingInfo->m_ptReal[1].x == -1 || pMatchingInfo->m_ptReal[1].x == 0)
		&& (pMatchingInfo->m_ptReal[1].y == -1 || pMatchingInfo->m_ptReal[1].y == 0)
		&& (pMatchingInfo->m_ptReal[2].x == -1 || pMatchingInfo->m_ptReal[2].x == 0)
		&& (pMatchingInfo->m_ptReal[2].y == -1 || pMatchingInfo->m_ptReal[2].y == 0)
		&& (pMatchingInfo->m_ptReal[3].x == -1 || pMatchingInfo->m_ptReal[3].x == 0)
		&& (pMatchingInfo->m_ptReal[3].y == -1 || pMatchingInfo->m_ptReal[3].y == 0) )
	{
		DebugString(_T("==================================="));
		DebugString(GetMakeString(_T(">> [ E ] [ FEVER_CAM ]  EEPROM_MATCHING_INFO is Empty!")));
		for( int i = 0 ; i < DEF_MAX_MATCHING_POINT ; ++i ) {
			DebugString(GetMakeString(_T("[ FEVER_CAM ]  Matching Info < Fever%d > %d, %d   < Real%d > %d, %d"),
				i,
				pMatchingInfo->m_ptFever[i].x,
				pMatchingInfo->m_ptFever[i].y,
				i,
				pMatchingInfo->m_ptReal[i].x,
				pMatchingInfo->m_ptReal[i].y));
		}
		DebugString(_T("==================================="));
		return FALSE;
	}

	if( !theINIMan().IsMirror() ) {
		theHelperMan().SetMatchingValue(pMatchingInfo);
	}
	else {
		usrPOINT ptFever[4];
		usrPOINT ptReal[4];

		int nTW = a_nThermalWidth;
		int nVW = a_nVisableWidth;

		ptFever[0].x = nTW - pMatchingInfo->m_ptFever[1].x;
		ptFever[0].y = pMatchingInfo->m_ptFever[1].y;
		ptFever[1].x = nTW - pMatchingInfo->m_ptFever[0].x;
		ptFever[1].y = pMatchingInfo->m_ptFever[0].y;
		ptFever[2].x = nTW - pMatchingInfo->m_ptFever[3].x;
		ptFever[2].y = pMatchingInfo->m_ptFever[3].y;
		ptFever[3].x = nTW - pMatchingInfo->m_ptFever[2].x;
		ptFever[3].y = pMatchingInfo->m_ptFever[2].y;

		ptReal[0].x = nVW - pMatchingInfo->m_ptReal[1].x;
		ptReal[0].y = pMatchingInfo->m_ptReal[1].y;
		ptReal[1].x = nVW - pMatchingInfo->m_ptReal[0].x;
		ptReal[1].y = pMatchingInfo->m_ptReal[0].y;
		ptReal[2].x = nVW - pMatchingInfo->m_ptReal[3].x;
		ptReal[2].y = pMatchingInfo->m_ptReal[3].y;
		ptReal[3].x = nVW - pMatchingInfo->m_ptReal[2].x;
		ptReal[3].y = pMatchingInfo->m_ptReal[2].y;

		theHelperMan().SetMatchingValue(ptFever, ptReal);
	}

	DebugString(_T("[ FEVER_CAM ]  [ GetMatchingPoint ] EEPROM Get Matching Info End."));
	return TRUE;
}


// 
// \brief <pre>
// 내용을 넣으세요.
// </pre>
// \param   DWORD			a_nErrCode
// \return  CString 
// 
CString CThermalCamera::GetSDKErr2Msg(
	_In_ DWORD					a_nErrCode)
{
	CString strMsg;
	switch( a_nErrCode ) {
	case IRF_NO_ERROR:
	case IRF_NAK:
		break;

	case IRF_HANDLE_ERROR:
		strMsg.Format(_T("Invalid Handle."));
		break;

	case IRF_FILE_OPEN_ERROR:
		strMsg.Format(_T("File Open Error."));
		break;

	case IRF_FILE_CLOSE_ERROR:
		strMsg.Format(_T("File Close Error."));
		break;

	case IRF_IR_IMAGE_READ_ERROR:
		strMsg.Format(_T("CRD File Read Error."));
		break;

	case IRF_FILE_BUFFER_ALLOCATION_ERROR:
		strMsg.Format(_T("File Stream Buffer Allocation Error."));
		break;

	case IRF_END_OF_FILE:
		strMsg.Format(_T("End of CRD File."));
		break;

	case IRF_BEGIN_OF_FILE:
		strMsg.Format(_T("Begin of CRD File."));
		break;

	case IRF_IR_IMAGE_WRITE_ERROR:
		strMsg.Format(_T("CRD File Write Error."));
		break;

	case IRF_NOT_FOUND_WINSOCK_DLL:
		strMsg.Format(_T("Not Found WS2_32.dll."));
		break;

	case IRF_CAMERA_CONNECTION_ERROR:
		strMsg.Format(_T("Connection error from a Camera."));
		break;

	case IRF_CAMERA_DISCONNECTION:
		strMsg.Format(_T("Disconnected from a Camera."));
		break;

	case IRF_PACKET_ID_ERROR:
		strMsg.Format(_T("Unknown Network Packet ID."));
		break;

	case IRF_MESSAGE_SEND_ERROR:
		strMsg.Format(_T("Message Sending Error."));
		break;

	case IRF_FIRST_FRAME_POS_ERROR:
		strMsg.Format(_T("First Frame size Error."));
		break;

	case IRF_FILTER_SIZE_ERROR:
		strMsg.Format(_T("Image Filter Size Error."));
		break;

	case IRF_FILE_WRITE_COUNT_OVER:
		strMsg.Format(_T("Image Frame Count is Bigger than Limit."));
		break;

	case IRF_PALETTE_FILE_OPEN_ERROR:
		strMsg.Format(_T("Palette File open Error."));
		break;

	case IRF_CRD_SINATURE_ERROR:
		strMsg.Format(_T("CRD File Signature Error."));
		break;

	case IRF_FILTER_UNSUPPORT_ERROR:
		strMsg.Format(_T("Unsupported Filter Type."));
		break;

	case IRF_PACKET_UNSUPPORT_ERROR:
		strMsg.Format(_T("Unsupported Packet Type."));
		break;

	case IRF_BUFFER_ALLOCATION_ERROR:
		strMsg.Format(_T("Buffer Allocation Error."));
		break;

	case IRF_INVALID_PARAM:
		strMsg.Format(_T("Invalid API Parameter."));
		break;

	case IRF_INVALID_MODEL:
		strMsg.Format(_T("Invalid Model Type."));
		break;

	case IRF_UNSUPPORTED_MODEL:
		strMsg.Format(_T("Unsupported Model Type."));
		break;

	case IRF_NOT_ENOUGH_MEMORY:
		strMsg.Format(_T("Not Enough memory."));
		break;

	case IRF_ERROR_PACKET_SIZE:
		strMsg.Format(_T("Error Packet size."));
		break;

	default:
		strMsg.Format(_T("Unknown Error."));
		break;
	}

	return strMsg;
}


// 
// \brief <pre>
// 내용을 넣으세요.
// </pre>
// \return  BOOL 
// 
BOOL CThermalCamera::InitCameraInfo()
{
	auto pSaveData = &m_stIRData.save_data;
	ASSERT(pSaveData);

	if( QVGA_ID == pSaveData->sensor ) {
		m_nWidth	= 384;
		m_nHeight	= 288;
	}
	else if( VGA_ID == pSaveData->sensor ) {
		m_nWidth	= 640;
		m_nHeight	= 480;
	}
	else if( XGA_ID == pSaveData->sensor ) {
		m_nWidth	= 1024;
		m_nHeight	= 768;
	}
	else {
		// Unknown Sensor
		return FALSE;
	}
	TraceLog((_T("Thermal Res=%dx%d"), m_nWidth, m_nHeight));

	int nRes = 0;
	for( int i = 0 ; i < DEF_MAX_PALETTE_COUNT ; ++i ) {
		nRes = GetImageLUT(m_pPalette[i][0].data,
						   (IRF_PALETTE_TYPE_T)i,
						   FALSE);
		if( nRes != IRF_NO_ERROR ) {
			return FALSE;
		}
		nRes = GetImageLUT(m_pPalette[i][1].data,
						   (IRF_PALETTE_TYPE_T)i,
						   TRUE);
		if( nRes != IRF_NO_ERROR ) {
			return FALSE;
		}
	}

	int16_t min = 0;
	int16_t max = 0;
	GetTempRangeValueCG(&m_stIRData,
						(IRF_DYNAMIC_RANGE_T)m_stIRData.save_data.temp_mode,
						&min,
						&max);
	m_fMeasureMinTemp	= (float)min;
	m_fMeasureMaxTemp	= (float)max;

	::memset(&m_stAGCCtrl, 0x00, sizeof(IRF_AUTO_RANGE_METHOD_T));
	m_stAGCCtrl.autoScale		= _IRF_AUTO;
	m_stAGCCtrl.inputMethod		= _IRF_SD_RATE;
	m_stAGCCtrl.SD_Rate			= 5.0f;
	m_stAGCCtrl.B_Rate			= 0.01f;
	m_stAGCCtrl.outputMethod	= _IRF_LINEAR;
	m_stAGCCtrl.intercept		= 0;
	m_stAGCCtrl.gamma			= 1.0f;
	m_stAGCCtrl.plateau			= 100;
	m_stAGCCtrl.epsilon			= 0.5f;
	m_stAGCCtrl.psi				= 0.3f;
	m_stAGCCtrl.prevPalteau		= 0;

	::memset(&m_stCorr, 0x00, sizeof(IRF_TEMP_CORRECTION_PAR_T));
	m_stCorr.emissivity		= m_stIRData.save_data.emissivity / 100.f;
	m_stCorr.atmTemp		= m_stIRData.save_data.atmosphere / 10.f;
	m_stCorr.atmTrans		= m_stIRData.save_data.transmission / 100.f;
	m_stCorr.zero_offset	= 0;

	::memset(&m_stImage, 0x00, sizeof(IRF_IMAGE_INFO_T));
	m_stImage.xSize			= m_nWidth;
	m_stImage.ySize			= m_nHeight;


	HDC hDC = ::GetDC(m_hMainWnd);
	m_nBitPixel = ::GetDeviceCaps(hDC, BITSPIXEL);
	if( hDC != NULL ) {
		::ReleaseDC(m_hMainWnd, hDC);
		hDC = NULL;
	}

	if( m_pTCamView != nullptr ) {
		m_pTCamView->SetImageSize(m_nWidth, m_nHeight);
	}
	return TRUE;
}


// 
// \brief <pre>
// 내용을 넣으세요.
// </pre>
// \param   LPVOID				a_pVoid
// \return  THREADFUNC 
// 
THREADFUNC CThermalCamera::ThreadProc(
	_In_ LPVOID				a_pVoid)
{
	CThermalCamera* pThis = (CThermalCamera*)a_pVoid;
	if( pThis != nullptr ) {
		pThis->RunThreadProc();
	}
	return 0;
}


// 
// \brief <pre>
// 내용을 넣으세요.
// </pre>
// 
void CThermalCamera::RunThreadProc()
{
	m_dwWaitTime = INFINITE;
	while (TRUE) {
		DWORD dwRet = WaitForMultipleObjects(2, m_hEvtLoop, FALSE, m_dwWaitTime);
		if( dwRet == WAIT_OBJECT_0 ) {
			break;
		}
		if( dwRet == WAIT_OBJECT_0 + 1 ) {
			m_dwWaitTime = 33;   
			ResetEvent(m_hEvtLoop[1]);
			TraceLog((_T("Reset Event")));
		}

		int16_t nRet = IRF_NO_ERROR;
		if( nRet = GetIRImage(), nRet != IRF_NO_ERROR ) {
			// 예외처리
			TraceLog((_T("GetIRImage() failed")));
			break;
		}
		if (m_config) {::Sleep(m_config->m_wait_time);	}  		//skpark in your area  부하를 줄이기 위해
	}
}


// 
// \brief <pre>
// 블랙바디를 이용해 보정값을 얻는다.
// </pre>
// \param   uint16_t*			a_pRawData
// \return  int 
// 
int CThermalCamera::CorrBlackbody(
	_In_ uint16_t*			a_pRawData)
{
	CPoint ptBB = theINIMan().GetBlackbodyPos();
	CRect rcThermal(0, 0, m_nWidth, m_nHeight);

	rcThermal.DeflateRect(2, 2);

	// 열상 영역 밖이라면 리턴
	if( !rcThermal.PtInRect(ptBB) ) {
		return 0;
	}

	CRect rcBB(ptBB, ptBB);
	rcBB.InflateRect(2, 2);

	::memcpy_s(m_pMirrorRawBuff,
			   sizeof(uint16_t) * m_nWidth * m_nHeight,
			   a_pRawData,
			   sizeof(uint16_t) * m_nWidth * m_nHeight);

	CRect rcResult;
	int nOffset = 0;
	bool bResult = m_BBTempCorr.CompensateTempError(m_pMirrorRawBuff,
													m_nWidth,
													m_nHeight,
													rcBB,
													m_fMeasureMinTemp,
													m_fMeasureMaxTemp,
													theINIMan().GetBlackbodyTemp(),
													rcResult,
													&nOffset,
													true);

	return nOffset;
}


// 
// \brief <pre>
// 열상 스트리밍
// </pre>
// \return  int16_t 
// 
int16_t CThermalCamera::GetIRImage()
{
	int16_t nRet	= IRF_NO_ERROR;
	float fCorrTemp = 0.f;

	auto pData = m_pTFInfo[m_nWriteIdx];

	m_stIRData.ir_image = pData->raw.data;
	nRet = GetIRImages(m_hNetwork, &m_nKeepAliveID, &m_stIRData);


	// 블랙바디를 사용한다면
	if( theINIMan().IsUseBlackbody() ) {
		// 보정값을 구한다.
		int nCorrVal = CorrBlackbody(pData->raw.data);

		// 한틱당 온도 계산
		float fOneTick = (m_fMeasureMaxTemp - m_fMeasureMinTemp) / (14000 - 2000);

		// 실제 보정될 온도값
		fCorrTemp = fOneTick * nCorrVal;

		//TraceLog((_T("Org Temp=%f, last Temp=%f"), fOneTick, fCorrTemp));

		//DebugString(_T("corrVal : %d, corrTemp : %.2f"),			nCorrVal,					fCorrTemp);
	}

	if( theINIMan().IsMirror() ) {
		MirrorBuffer<uint16_t>(m_stIRData.ir_image,
							   m_pMirrorRawBuff,
							   m_nWidth,
							   m_nHeight);
		::memcpy_s(m_stIRData.ir_image,
				   sizeof(uint16_t) * m_nWidth * m_nHeight,
				   m_pMirrorRawBuff,
				   sizeof(uint16_t) * m_nWidth * m_nHeight);
	}

	if( nRet != IRF_NO_ERROR ) {
		return nRet;
	}

	if( m_stIRData.msg_type == _IRF_STREAM_DATA ) {
		float fLevel	= 0.f;
		float fSpan		= 0.f;

		// Gray 이미지 얻기
		nRet = GetImageCG(pData->gray.data,
						  &m_stIRData,
						  m_nWidth * m_nHeight,
						  &fLevel,
						  &fSpan,
						  &m_stAGCCtrl);

		if( nRet != IRF_NO_ERROR ) {
			return nRet;
		}

		// 온도 얻기
		nRet = GetRawToTempCG(&m_stIRData,
							  m_stImage,
							  m_stCorr,
							  pData->temp.data);


		if( nRet != IRF_NO_ERROR ) {
			return nRet;
		}
		if( theINIMan().IsMirror() ) {
			MirrorBuffer<float>(pData->temp.data,
								m_pMirrorTempBuff,
								m_nWidth,
								m_nHeight);
			::memcpy_s(pData->temp.data,
					   sizeof(float) * m_nWidth * m_nHeight,
					   m_pMirrorTempBuff,
					   sizeof(float) * m_nWidth * m_nHeight);

			cv::Mat temp(m_nHeight, m_nWidth, CV_32FC1, pData->temp.data);
			temp += fCorrTemp;
		}

		// 팔레드 이미지 얻기
		nRet = GetGrayToPaletteImage(pData->gray.data,
									 pData->image.data,
									 (uint16_t)m_nWidth,
									 (uint16_t)m_nHeight,
									 m_pPalette[12 - m_nPalette][0].data,
									 m_nBitPixel,
									 FALSE,
									 FALSE);

		if( nRet != IRF_NO_ERROR ) {
			return nRet;
		}

		m_StreamFPS.CalcFrameRate();

		m_pTargetBuff = pData;
		m_nWriteIdx++;
		if( m_nWriteIdx >= 5 ) {
			m_nWriteIdx = 0;
		}
	}
	return nRet;
}


TF_INFO* CThermalCamera::GetThermalBuff()
{
	return m_pTargetBuff;
}


void CThermalCamera::StreamStart()
{
	SendCameraCommand(_IRF_STREAM_ON);
	SetEvent(m_hEvtLoop[1]);
}


int CThermalCamera::GetThermalWidth()
{
	return m_nWidth;
}


int CThermalCamera::GetThermalHeight()
{
	return m_nHeight;
}


BOOL CThermalCamera::GetFaceInfoTemp(
	_In_ FACE_INFO*			a_pFaceInfo,
	_In_ float*				a_pTempBuff)
{
	int16_t nRet = GetFaceTemp(a_pTempBuff,
							   a_pFaceInfo->thermal_facebox,
							   &a_pFaceInfo->face_temp,
							   m_nWidth,
							   m_nHeight);

	//TraceLog((_T("GetFaceTemp(%f), %d"), a_pFaceInfo->face_temp, nRet));

	return nRet == IRF_NO_ERROR ? TRUE : FALSE;
}


BOOL CThermalCamera::IsConnected()
{
	return m_bConnect;
}


BOOL CThermalCamera::LoadV2TMatcingPoint(
	_In_ int				a_nVisableWidth,
	_In_ int				a_nThermalWidth)
{
	// 실상 열상 좌표 매칭 정보 검사
	if( !GetMatchingPoint(a_nVisableWidth, a_nThermalWidth) ) {
		DebugString(_T(">> [E] Not found matcing information."));
		return FALSE;
	}
	return TRUE;
}


int CThermalCamera::GetStreamFPS()
{
	return m_StreamFPS.GetFrameRate();
}


void CThermalCamera::SetPalette(
	_In_ int				a_nPalSel)
{
	m_nPalette = (IRF_PALETTE_TYPE_T)a_nPalSel;
}


float CThermalCamera::GetMeasureMinTemp()
{
	return m_fMeasureMinTemp;
}


float CThermalCamera::GetMeasureMaxTemp()
{
	return m_fMeasureMaxTemp;
}