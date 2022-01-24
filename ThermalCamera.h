#pragma once
#include "CBBTempCorrection.h"
#include "Lock.h"


class CTCamView;
class CoxConfig;  //skpark in your area

// \brief
// 모듈의 기능 및 설명 사용방법 기술.
//
// \code
// - 샘플 코드를 넣어주세요.
// \encode
// \warning
// \sa
// \author	이현석
// \date		2021-12-27 17:35:29		최초작성
class CThermalCamera
{
public:
	friend CThermalCamera& theTCMan();

	~CThermalCamera();

	BOOL				IsConnected();

	BOOL				Connect(
							_In_ CTCamView*			a_pTCamView,
							_In_ HWND				a_hMainWnd,
							_In_ const CString&		a_strIP,
							_In_ IRF_PALETTE_TYPE_T	a_nPalette);

	BOOL				Disconnect();

	BOOL				SendCameraCommand(
							_In_ IRF_MESSAGE_TYPE_T	a_nMsgType,
							_In_ uint16_t			a_nMessage = 0,
							_In_ uint16_t			a_nRCode = 0);

	void				RunThreadProc();

	TF_INFO*			GetThermalBuff();

	void				StreamStart();

	int					GetThermalWidth();

	int					GetThermalHeight();

	BOOL				GetFaceInfoTemp(
							_In_ FACE_INFO*			a_pFaceInfo,
							_In_ float*				a_pTempBuff);

	BOOL				LoadV2TMatcingPoint(
							_In_ int				a_nVisableWidth,
							_In_ int				a_nThermalWidth);

	int					GetStreamFPS();

	void				SetPalette(
							_In_ int				a_nPalSel);

	float				GetMeasureMinTemp();

	float				GetMeasureMaxTemp();

private:
	CThermalCamera();

	void				ResetIRCamData();

	CString				GetSDKErr2Msg(
							_In_ DWORD				a_nErrCode);

	BOOL				InitCameraInfo();

	static THREADFUNC	ThreadProc(
							_In_ LPVOID				a_pVoid);

	int16_t				GetIRImage();

	BOOL				GetMatchingPoint(
							_In_ int				a_nVisableWidth,
							_In_ int				a_nThermalWidth);

	int					CorrBlackbody(
							_In_ uint16_t*			a_pRawData);

	template<typename T>
	void				MirrorBuffer(
							_In_ T*					src,
							_Out_ T*				dst,
							_In_ int				w,
							_In_ int				h);



private:
	CTCamView*					m_pTCamView;
	HWND						m_hMainWnd;
	BOOL						m_bConnect;
	int							m_nWidth;
	int							m_nHeight;
	CString						m_strIP;

	CPBUFF<uint8_t>				m_pPalette[DEF_MAX_PALETTE_COUNT][2];

	TF_INFO*					m_pTFInfo[DEF_DATA_DEPTH];
	TF_INFO*					m_pTargetBuff;
	UINT						m_nWriteIdx;


	IRF_IR_CAM_DATA_T			m_stIRData;
	HANDLE						m_hNetwork;
	HANDLE						m_nKeepAliveID;
	IRF_AUTO_RANGE_METHOD_T		m_stAGCCtrl;
	IRF_TEMP_CORRECTION_PAR_T	m_stCorr;
	IRF_IMAGE_INFO_T			m_stImage;
	IRF_PALETTE_TYPE_T			m_nPalette;
	int							m_nBitPixel;
	float						m_fMeasureMinTemp;
	float						m_fMeasureMaxTemp;

	EEPROM_MATCHING_INFO		m_stMatchingInfo;

	HANDLE						m_hThread;
	UINT						m_nThreadID;
	HANDLE						m_hEvtLoop[2];
	DWORD						m_dwWaitTime;

	CFrameRate					m_StreamFPS;

	CBBTempCorrection			m_BBTempCorr;


	CSection					m_hLock;
	float*						m_pMirrorTempBuff;
	uint16_t*					m_pMirrorRawBuff;

public:  //skpark in your area
	void SetConfig(CoxConfig* config); 
	bool ThreadCondition();
	CoxConfig* m_config;
};


template<typename T>
inline void CThermalCamera::MirrorBuffer(T * src, T * dst, int w, int h)
{
	int idx = w - 1;
	for( int y = 0 ; y < h ; ++y ) {
		idx = w - 1;
		for( int x = 0 ; x < w ; ++x ) {
			int src_idx = (y * w) + x;
			int dst_idx = (y * w) + idx;

			*(dst + dst_idx) = *(src + src_idx);
			idx--;
		}
	}
}
