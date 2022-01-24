// WindowCapture.h: interface for the CWindowCapture class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WINDOWCAPTURE_H__5B372A93_A94A_4770_9357_B387B2515E22__INCLUDED_)
#define AFX_WINDOWCAPTURE_H__5B372A93_A94A_4770_9357_B387B2515E22__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <atlbase.h>
#include <gdiplus.h>
using namespace Gdiplus;

#pragma comment(lib, "gdiplus.lib")

// Global Functions
HBITMAP CaptureDesktop();
HBITMAP CaptureDesktopRect();
HBITMAP CaptureForegroundWindow(BOOL bClientAreaOnly = FALSE);
HBITMAP CaptureWindow(HWND hwnd, BOOL bClientAreaOnly = FALSE);
HBITMAP CaptureWindowRect(HWND hwnd, RECT& rect, BOOL bClientAreaOnly = FALSE);

HBITMAP ScaleBitmap(HBITMAP hBmp, WORD wNewWidth, WORD wNewHeight);
BITMAPINFO* PrepareRGBBitmapInfo(WORD wWidth, WORD wHeight);
float* CreateCoeff(int nLen, int nNewLen, BOOL bShrink);
void ShrinkData(BYTE *pInBuff, WORD wWidth, WORD wHeight, 
				BYTE *pOutBuff, WORD wNewWidth, WORD wNewHeight);
void EnlargeData(BYTE *pInBuff, WORD wWidth, WORD wHeight, 
				 BYTE *pOutBuff, WORD wNewWidth, WORD wNewHeight);

class CWindowCapture  
{
public:
	typedef enum {
		CAP_PNG = 0,
		CAP_JPG = 1,
		CAP_GIF = 2
	} CAP_TYPE;

	void Start();
	static long OnCapture(
		CAP_TYPE type = CAP_JPG,
		const TCHAR* fileName = _T(""),
		const TCHAR* filePath = _T("")
		);
	static long OnCaptureRect(
		CAP_TYPE type,
		const TCHAR* fullpath,
		RECT& rect
		);
	static void CWindowCapture::MakeFolder(LPCTSTR pszPath);
	HBITMAP m_hBmp;

	CWindowCapture();
	virtual ~CWindowCapture();

	CRect		m_targetRect;
	void		SetRect(int left, int top, int w, int h) { m_targetRect.SetRect(left, top, w + left, h + top); }
protected:
	ULONG_PTR m_gpToken;
};

#endif // !defined(AFX_WINDOWCAPTURE_H__5B372A93_A94A_4770_9357_B387B2515E22__INCLUDED_)
