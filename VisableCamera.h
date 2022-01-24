#pragma once


#include <IFaceMeLicenseManager.h>
#include <IFaceMeRecognizer.h>
#include <IFaceMeCameraManager.h>
#include <IBaseObj.h>
#include <FaceMeBaseDef.h>
#include <FaceMeSDKLib.h>
#include <BaseObjUtils.h>
//#include <gdiplus.h>
#include <vector>
#include <list>
#include "Lock.h"


#define FR_INIT_STRUCT(ptr, class_name) \
{ \
	std::memset((uint8_t*)ptr, 0, sizeof(class_name)); \
	(*ptr).sizeOfStructure = sizeof(class_name); \
}
#define HAS_OPTION(name, option) (((name) & (int32_t)(option)) == (int32_t)(option))


using namespace FaceMeSDK;


class CVCamView;
class CTCamView;
class CoxGuardian;  //skpark in your area

// \brief
// 모듈의 기능 및 설명 사용방법 기술.
//
// \code
// - 샘플 코드를 넣어주세요.
// \encode
// \warning
// \sa
// \author	이현석
// \date		2021-12-27 14:54:44		최초작성
class CVisableCamera : public IFaceMeVideoStreamHandler
{
public:
	friend CVisableCamera& theVCMan();

	~CVisableCamera();

	BOOL				Initialize(
							_In_ CVCamView*			a_pVCamView,
							_In_ CTCamView*			a_pTCamView,
							_In_ HWND				a_hMainWnd,
							_In_ const CString&		a_strCamName,
							_In_ int				a_nWidth,
							_In_ int				a_nHeight,
							_In_ UINT				a_nSkipFrame);

	BOOL				IsConnected();

	BOOL				Disconnect();

	BOOL				StreamStart();

	void				RunThreadProc();

	int					GetVisableWidth();

	int					GetVisableHeight();

	int					GetFaceDetectInfo(
							_Inout_ FACE_INFO*		a_pArrayFace);

	int					GetStreamFPS();

	int					GetFaceDetectFPS();

	void				SetVisableCamSkipFrame(
							_In_ UINT				a_nSkipFrame);


private:
	CVisableCamera();

	BOOL				FaceMeLicenseAuth();

	BOOL				FaceMeRecognizerCfg();

	BOOL				FaceMeCameraManagerCfg();

	BOOL				FaceMeCameraCapturer();

	static THREADFUNC	ThreadProc(
							_In_ LPVOID				a_pVoid);

	BOOL				FaceDetection(
							_In_ CAMF_INFO*			a_pFrame);

	BOOL				GetFaceInfo(
							_In_ UINT				a_nSeq,
							_In_ FR_FaceInfo*		a_pFRFaceInfo,
							_Out_ FACE_INFO*		a_pFaceInfo);

	BOOL				GetFaceAttribute(
							_In_ UINT				a_nSeq,
							_In_ FR_FaceAttribute*	a_pFRFaceAttribute,
							_Out_ FACE_INFO*		a_pFaceInfo);

	BOOL				GetFaceLandmark(
							_In_ UINT				a_nSeq,
							_In_ FR_FaceLandmark*	a_pFRFaceLandmark,
							_Out_ FACE_INFO*		a_pFaceInfo);

	BOOL				GetFaceFeature(
							_In_ UINT				a_nSeq,
							_In_ FR_FaceFeature*	a_pFRFaceFeature,
							_Out_ FACE_INFO*		a_pFaceInfo);

	void				ConvertScreenRect(
							_Inout_ FACE_INFO*		a_pFaceInfo);

	BOOL				IsFaceBoxInTheramlArea(
							_In_ FACE_INFO*			a_pFaceInfo);

	BOOL				IsBoxOverlapCheck(
							_In_ UINT				a_nCurrIdx,
							_In_ FR_FaceInfo*		a_pFRFaceInfo,
							_In_ int				a_nFaceCount,
							_In_ FACE_INFO*			a_pArrayFaceInfo);


// IFaceMeVideoStreamHandler
private:
	virtual void		OnVideoFrame(
							_In_ const FR_VideoFrameInfo*	a_pFrameInfo,
							_In_ const FR_Image*			a_pFrame,
							_In_ const FR_DepthMap*			a_pDepthMap);

	virtual void		OnEndOfStream();

	virtual void		OnError(
							_In_ FR_RETURN					a_nResult);


private:
	std::string							m_strModulePath;
	std::string							m_strAppDataPath;
	std::string							m_strCachePath;
	std::vector<uint8_t>				m_vLicenseList;

	FaceMeSDKLib*						m_pFaceME;
	IFaceMeLicenseManager*				m_pFMLicense;
	IFaceMeCameraCapturer*				m_pFMCameraCapture;
	IFaceMeCameraManager*				m_pFMCameraManager;
	IFaceMeRecognizer*					m_pFMRecognizer;
	FR_RETURN							m_nFaceMeError;

	UINT								m_nCurrFrameSeq;
	UINT								m_nSkipFrame;
	CVCamView*							m_pVCamView;
	CTCamView*							m_pTCamView;
	HWND								m_hMainWnd;
	int									m_nConnectIdx;
	CString								m_strTargetName;
	int									m_nWidth;
	int									m_nHeight;
	BOOL								m_bConnect;


	VF_INFO*							m_pVFInfo[DEF_DATA_DEPTH];
	VF_INFO*							m_pTargetBuff;
	UINT								m_nWriteIdx;


	HANDLE								m_hThread;
	UINT								m_nThreadID;
	HANDLE								m_hEvtLoop[2];


	CAMF_INFO*							m_pFrameInfo[DEF_DATA_DEPTH];
	CAMF_INFO*							m_pFrameTarget;
	UINT								m_nFrameWriteIdx;


	FACE_INFO							m_arrFaceInfo[DEF_MAX_FACE_COUNT];
	UINT								m_nFaceCount;

	CSection							m_hLock;


	CFrameRate							m_StreamFPS;
	CFrameRate							m_FaceDetectFPS;


	CRect								m_rcVisableZoom;
	cv::Mat								m_cvFrame;
	cv::Mat								m_cvBlack;

public:  //skpark in your area
	CTCamView*	 GetTCamView()	 { return	m_pTCamView; }
	IFaceMeRecognizer* GetFaceRecognizer() { return m_pFMRecognizer; }
	void SetCallback(CoxGuardian* g);
	CoxGuardian* m_cox_guardian;
};


