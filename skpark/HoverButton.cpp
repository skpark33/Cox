// HoverButton.cpp : implementation file
//

#include "stdafx.h"
#include "HoverButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHoverButton

CHoverButton::CHoverButton()
{
	m_pParentWnd = NULL;

	m_bHover = FALSE;
	m_bTracking = FALSE;

	m_transparentColor = RGB(255,255,255);
}

CHoverButton::~CHoverButton()
{
}

IMPLEMENT_DYNAMIC(CHoverButton, CBitmapButton)

BEGIN_MESSAGE_MAP(CHoverButton, CBitmapButton)
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////
 //	CHoverButton message handlers
		
void CHoverButton::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (!m_bTracking)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE|TME_HOVER;
		tme.dwHoverTime = 1;
		m_bTracking = _TrackMouseEvent(&tme);
	}
	CBitmapButton::OnMouseMove(nFlags, point);
}

BOOL CHoverButton::PreTranslateMessage(MSG* pMsg) 
{
	InitToolTip();
	m_ToolTip.RelayEvent(pMsg);		
	return CButton::PreTranslateMessage(pMsg);
}

// Set the tooltip with a string resource
void CHoverButton::SetToolTipText(int nId, BOOL bActivate)
{
	CString sText;

	// load string resource
	sText.LoadString(nId);
	// If string resource is not empty
	if (sText.IsEmpty() == FALSE) SetToolTipText(sText, bActivate);

}

// Set the tooltip with a CString
void CHoverButton::SetToolTipText(LPCTSTR lpszTT, BOOL bActivate)
{
	// We cannot accept NULL pointer
	if (lpszTT == NULL) return;

	// Initialize ToolTip
	InitToolTip();

	// If there is no tooltip defined then add it
	if (m_ToolTip.GetToolCount() == 0)
	{
		CRect rectBtn; 
		GetClientRect(rectBtn);
		m_ToolTip.AddTool(this, lpszTT, rectBtn, 1);
	}

	// Set text for tooltip
	m_ToolTip.UpdateTipText(lpszTT, this, 1);
	m_ToolTip.Activate(bActivate);
}

void CHoverButton::InitToolTip()
{
	if (m_ToolTip.m_hWnd == NULL)
	{
		// Create ToolTip control
		m_ToolTip.Create(this);
		// Create inactive
		m_ToolTip.Activate(FALSE);
	}
}

// Activate the tooltip
void CHoverButton::ActivateTooltip(BOOL bActivate)
{
	// If there is no tooltip then do nothing
	if (m_ToolTip.GetToolCount() == 0) return;

	// Activate tooltip
	m_ToolTip.Activate(bActivate);
}

void CHoverButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	CDC *mydc=CDC::FromHandle(lpDrawItemStruct->hDC);

	CDC * pMemDC = new CDC;
	pMemDC -> CreateCompatibleDC(mydc);

	CBitmap * pOldBitmap;
	pOldBitmap = pMemDC -> SelectObject(&mybitmap);
	
	CPoint point(0,0);	

	// Modified by 정운형 2009-01-23 오전 10:07
	// 변경내역 :  Disable 상태 삭제, Up/Down/Hover 순서를 Up/Hover/Down 로 변경
	/*
	// [-- modified by socket7
	if(GetStyle() & WS_DISABLED)
	{
		mydc->TransparentBlt(0,0,m_ButtonSize.cx,m_ButtonSize.cy, pMemDC, m_ButtonSize.cx*3,0,m_ButtonSize.cx,m_ButtonSize.cy, m_transparentColor);
	}
	else
	// --]
	*/

	if(lpDrawItemStruct->itemState & ODS_SELECTED)
	{
//		mydc->BitBlt(0,0,m_ButtonSize.cx,m_ButtonSize.cy,pMemDC,m_ButtonSize.cx,0,SRCCOPY); // modified by socket7
		//mydc->TransparentBlt(0,0,m_ButtonSize.cx,m_ButtonSize.cy, pMemDC, m_ButtonSize.cx, 0, m_ButtonSize.cx,m_ButtonSize.cy, m_transparentColor);
		mydc->TransparentBlt(0,0,m_ButtonSize.cx,m_ButtonSize.cy, pMemDC, m_ButtonSize.cx*2, 0, m_ButtonSize.cx,m_ButtonSize.cy, m_transparentColor);
	} 
	else
	{
		if(m_bHover)
		{
//			mydc->BitBlt(0,0,m_ButtonSize.cx,m_ButtonSize.cy,pMemDC,m_ButtonSize.cx*2,0,SRCCOPY); // modified by socket7
			//mydc->TransparentBlt(0,0,m_ButtonSize.cx,m_ButtonSize.cy, pMemDC, m_ButtonSize.cx*2, 0, m_ButtonSize.cx,m_ButtonSize.cy, m_transparentColor);
			mydc->TransparentBlt(0,0,m_ButtonSize.cx,m_ButtonSize.cy, pMemDC, m_ButtonSize.cx, 0, m_ButtonSize.cx,m_ButtonSize.cy, m_transparentColor);
		}else
		{
//			mydc->BitBlt(0,0,m_ButtonSize.cx,m_ButtonSize.cy,pMemDC,0,0,SRCCOPY); // modified by socket7
			mydc->TransparentBlt(0,0,m_ButtonSize.cx,m_ButtonSize.cy, pMemDC, 0, 0, m_ButtonSize.cx,m_ButtonSize.cy, m_transparentColor);
		}	
	}
	// Modified by 정운형 2009-01-23 오전 10:07
	// 변경내역 :  Disable 상태 삭제, Up/Down/Hover 순서를 Up/Hover/Down 로 변경

	// clean up
	pMemDC -> SelectObject(pOldBitmap);
	delete pMemDC;
}

// Load a bitmap from the resources in the button, the bitmap has to have 3 buttonsstates next to each other: Up/Down/Hover
BOOL CHoverButton::LoadBitmap(UINT bitmapid, COLORREF transparentColor)
{
	ModifyStyle(0, BS_OWNERDRAW);

	m_transparentColor = transparentColor;

	mybitmap.DeleteObject();
	mybitmap.LoadBitmap(bitmapid);

	BITMAP bitmapbits;
	mybitmap.GetBitmap(&bitmapbits);

	m_ButtonSize.cy=bitmapbits.bmHeight;
	// Modified by 정운형 2009-01-21 오후 8:56
	// 변경내역 :  이미지 종류를 3개로 변경
	//m_ButtonSize.cx=bitmapbits.bmWidth/4;
	m_ButtonSize.cx=bitmapbits.bmWidth/3;
	// Modified by 정운형 2009-01-21 오후 8:56
	// 변경내역 :  이미지 종류를 3개로 변경

	SetWindowPos(
		NULL,
		0,
		0,
		m_ButtonSize.cx,
		m_ButtonSize.cy,
		SWP_NOMOVE | SWP_NOOWNERZORDER
	);

	return TRUE;
}

LRESULT CHoverButton::OnMouseHover(WPARAM wparam, LPARAM lparam) 
{
	m_bHover=TRUE;
	Invalidate();
	return 0;
}

LRESULT CHoverButton::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
	m_bTracking = FALSE;
	m_bHover=FALSE;
	Invalidate();
	return 0;
}

void CHoverButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	CBitmapButton::OnLButtonUp(nFlags, point);

	if(m_pParentWnd)
		m_pParentWnd->SendMessage(WM_COMMAND, m_command);
}
