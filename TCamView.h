#pragma once
#include "afxwin.h"




class CTCamView : public CStatic
{
public:
	CTCamView();

	~CTCamView();

	void			InitializeEx(
						_In_ CWnd*			a_pParent);

	void			SetData(
						_In_ CAMF_INFO*		a_pCamFrameInfo);

	void			SetImageSize(
						_In_ int			a_nWidth,
						_In_ int			a_nHeight);

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
						_In_ FACE_INFO*		a_pFaceInfo);

	void			RecalcLayout();

	CPoint			GetZoom2Img2ScalePoint(
						_In_ CPoint			a_pt);

	CPoint			GetScr2Img2ScalePoint(
						_In_ CPoint			a_pt);

	CPoint			GetImg2ZoomScalePoint(
						_In_ CPoint			a_pt);

	CPoint			GetImg2ScreenScalePoint(
						_In_ CPoint			a_pt);

	CRect			GetImg2ZoomScaleRect(
						_In_ CRect			a_rc);

	CRect			GetImg2ScreenScaleRect(
						_In_ CRect			a_rc);


private:
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
	

	//TF_INFO*								m_pThermalBuff;
	CAMF_INFO*								m_pCamFrameInfo;

	FACE_INFO								m_arrFace[DEF_MAX_FACE_COUNT];
	UINT									m_nFaceCount;

	CAMERA_STATUS							m_nStatus;

public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
};

