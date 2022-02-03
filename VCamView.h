#pragma once
#include "afxwin.h"

class CoxGuardian;  //skpark in your area


class CVCamView : public CStatic
{
public:
	CVCamView();

	~CVCamView();

	void			InitializeEx(
						_In_ CWnd*			a_pParent);

	void			SetImageSize(
						_In_ int			a_nWidth,
						_In_ int			a_nHeight);

	void			SetData(
						_In_ CAMF_INFO*		a_pCamFrameInfo);

	void			SetCamStatus(
						_In_ CAMERA_STATUS	a_nStatus);

	CRect			GetZoomRect();

protected:
	DECLARE_MESSAGE_MAP()


private:
	void			DrawScreen();

	void			DrawEmpty();

	void			DrawConnecting();

	void			DrawStreaming();

	void			DrawDisconnect();

	void			DrawFaceBox();

	CRect			DrawBox(
						_In_ FACE_INFO*		a_pFaceInfo, COLORREF color);  //skpark in your area,  color 값을 fever 일때 다르게 주기 추가하였다.

	CRect			DrawValidAreaBox(CRect& 	prect); //skpark in your area  유효영역을 화면에 그려준다.

	void			RecalcLayout();

	CRect			GetImg2ZoomScale(
						_In_ CRect			a_rc);

	CRect			GetImg2ScreenScale(
						_In_ CRect			a_rc);



public: // skpark modify ,<-- private: 
	ID2D1Bitmap*							m_pLogoBmp;
	ID2D1Factory*							m_pFactory;
	ID2D1HwndRenderTarget*					m_pTarget;
	ID2D1BitmapRenderTarget*				m_pBitmapTarget;
	IDWriteFactory*							m_pWriteFactory;
	ID2D1SolidColorBrush*					m_pBrush;
	IDWriteTextFormat*						m_pTextFmt;
	ID2D1Bitmap*							m_pBitmap;
	IWICImagingFactory*						m_pWICFactory;

	CFrameRate								m_FrameRate;
	CWnd*									m_pParentWnd;
	int										m_nWidth;
	int										m_nHeight;
	CRect									m_rcZoom;
	float									m_fScaleX;
	float									m_fScaleY;
	float									m_fZoomScaleX;
	float									m_fZoomScaleY;
	int										m_nCutLeft;
	int										m_nCutTop;
	int										m_nCutRight;
	int										m_nCutBottom;

	//VF_INFO*								m_pVisableBuff;
	CAMF_INFO*								m_pCamFrameInfo;


	FACE_INFO								m_arrFace[DEF_MAX_FACE_COUNT];
	UINT									m_nFaceCount;

	CAMERA_STATUS							m_nStatus;

public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();

//skpark in your area
public:
	void SetCallback(CoxGuardian* g);
	CString  GetTimeStr();
	CoxGuardian	*	m_cox_guardian;
//skpark in your area end
};

