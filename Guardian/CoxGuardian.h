#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "afxmt.h"
#include "atlimage.h"
#include "define.h"
#include <list>

#include <IFaceMeRecognizer.h>


class CCOXFDSampleDlg;
class  CVCamView;
class CTCamView;
class CoxConfig;
class CImge;
class CStatistics;
class CFacePainterStatic;
//class IFaceMeRecognizer;

class Detected
{
public:
	Detected() : image(0)  {}
	~Detected() {
		if (image) {
			delete image;
		}
	}
	CString key;
	bool	isFever;
	bool		isNoMask;
	double		temperature;
	time_t			detectTime;
	CImage*		image;
	FACE_FEATURE_INFO		faceFeature;

	LPCTSTR		toString() {
			/*
			[ROOT]
			Temp=37.9
			X=0.472000
			Y=0.622000
			Width=0.031000
			Height=0.067000
			Level=1 or 0
			faceResult={"matches":[],"result_code":"F","result_msg":"face not found"}
			faceResult={"matches":[{"mask":false,"bounds":"-4,-36,303,404","name":"±è¿ø¿ë","score":0.4240404,"grdde":"3","room":"5"}],"result_code":"S","result_msg":null}
			*/
		m_strBuf.Format(
				_T("[ROOT]\r\nTemp=%.1f\r\nX=0\r\nY=0\r\nWidth=0\r\nHeight=0\r\nLevel=%d\r\nfaceResult={}\r\n"),
				temperature, isFever);

		return m_strBuf.GetBuffer();
	}

protected:
	CString m_strBuf;

};

typedef enum {
	NO_ALARM = 0,
	NO_MASK_ALARM = 1,
	FEVER_ALARM = 2,
} ALARM_STATE;

typedef  std::list<Detected*>		DETECTED_LIST;

class  CoxGuardian
{
public:
	CoxGuardian(CCOXFDSampleDlg* pWnd, CWnd* statPlate, CFacePainterStatic* faceArea);
	virtual ~CoxGuardian();

	float CheckBlackBodyTemp(float blackBodyTemp);

	void InitFont(CDC* pDc);
	void InitFont(CFont& targetFont, CDC* pDc, LPCTSTR fontName, int fontSize, float fontWidth = 1);

	void MaxWindow();
	void MinWindow();
	bool ToggleFullScreen();
	bool ToggleCamera();
	void KillEverything(bool killFirmwareView);
	
	void Invalidate(ALARM_STATE alarmState);

	int		DrawFaces(CDC* dc);
	bool	DrawFace(CDC* dc, Detected* detected, int index, int videoEndPosY);  
	void DrawTemperature(CDC* dc, Detected* detected, int videoEndPosY);  

	bool DrawNormal(CDC& dc);
	bool DrawFever(CDC& dc);
	bool DrawNoMask(CDC& dc);

	bool KeyboardSimulator(const char* command);

	bool WriteNormal(CDC& dc, int x, int y);
	bool WriteFever(CDC& dc, int x, int y);
	bool WriteNoMask(CDC& dc, int x, int y);
	void DrawNotice(CDC& dc, int posy);

	
	void WriteSingleLine(LPCTSTR text,
		CDC* pDc,
		CFont& targetFont,
		COLORREF color,
		int posY,
		int height,
		int align = DT_CENTER,
		int posX = 0,
		int width = WIN_WIDTH
		);
		
	void OnPaint();
	void OnTimer(UINT_PTR nIDEvent);
	HBRUSH OnCtlColor(int statisticsPlateId, CDC* pDC, CWnd* pWnd, UINT nCtlColor, HBRUSH hbr);
	void OnDestroy();
	void  PreTranslateMessage(MSG* pMsg);
	void OnLButtonUp(UINT nFlags, CPoint point);
	
	time_t CaseDetected(FACE_INFO* faceInfo, CRect& faceRect, uint32_t dataSize, BYTE* faceImage);  // for VCamView
	void NewFaceDetected(CAMF_INFO* faceInfo, int faceCount); // for VisibleCamera
	
	bool isCase(FACE_INFO* faceInfo);
	bool isFeverCase(FACE_INFO* faceInfo);
	bool isCaseOrAll(FACE_INFO* faceInfo);
	
	bool ImageProcess(BYTE* imageStream, int streamSize, CRect& cropArea);
	bool SaveImage(CImage* pImage, LPCTSTR filePrefix);
	CImage* CropGdiImage(BYTE * buffer, DWORD size, CRect& cropArea, LPCTSTR timeKey, LPCTSTR iniInfo );


	void SaveIni(LPCTSTR key, LPCTSTR iniInfo);
	int		GetDetectedList(DETECTED_LIST& list);
	
	ALARM_STATE GetAlarmLevel();
	int EraseOldDetected(time_t now);

	void DrawStat(CDC& dc, int posY, int height);
	void InitStat();
	void AddSample(float temper);
	
	void Clear();
	void EraseAll();

	BOOL IsFaceBoxInTheramlArea(FACE_INFO*	a_pFaceInfo, CTCamView*		a_pTCamView);
	CRect DrawValidAreaBox(CRect& rect, ID2D1BitmapRenderTarget*	 a_pBitmapTarget, ID2D1SolidColorBrush* a_pBrush);
	D2D1_COLOR_F GetD2DColor(COLORREF	 	a_clr, float	a_fAlpha = 1.f);

	bool CheckFever(FACE_INFO* faceInfo);

	void OpenAuthorizer();
	void OpenConfigurator();

	BOOL IsSameMan(FaceMeSDK::IFaceMeRecognizer*	 m_pFMRecognizerr, FACE_FEATURE_INFO* newFaceFeature);


public:
	CCOXFDSampleDlg* m_parent;
	CWnd* m_statPlate;
	
	CBrush		m_statBrush;

	CoxConfig*		m_config;
	CStatistics*		m_stat;

	bool m_bFullScreen;
	std::string m_currentCtrlKey;
	int m_alarmState;

	CString m_localeLang;

	CFont	m_titleFont;
	CFont	m_subTitleFont;
	CFont	m_noticeTitleFont;
	CFont	m_alarmTitleFont;
	CFont m_weatherFontTitle;
	CFont m_alarmFont2;
	CFont m_alarmFont3;
	CFont m_alarmFont4;
	CFont	m_alarmFont5;
	CFont m_alarmFont6;

	int m_cameraHeight;

	DETECTED_LIST	m_detectedList;

	CCriticalSection	m_cs;
	CFacePainterStatic* m_faceArea;


};