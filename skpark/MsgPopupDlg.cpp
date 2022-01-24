// CMsgPopupDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "resource.h"
#include "MsgPopupDlg.h"
#include "afxdialogex.h"
#include <TraceLog.h>  

#define POP_MSG_TIMER	123

// CMsgPopupDlg 대화 상자

IMPLEMENT_DYNAMIC(CMsgPopupDlg, CDialogEx)

CMsgPopupDlg::CMsgPopupDlg(CWnd* pParent /*=nullptr*/)
: CDialogEx(IDD_POP_MSG, pParent)
{

}

CMsgPopupDlg::~CMsgPopupDlg()
{
}

void CMsgPopupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMsgPopupDlg, CDialogEx)
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_WM_NCPAINT()
END_MESSAGE_MAP()


// CMsgPopupDlg 메시지 처리기
void CMsgPopupDlg::PostNcDestroy()
{
	delete this;
}


BOOL CMsgPopupDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetTimer(POP_MSG_TIMER, 2000, 0);

//	CenterWindow();

	HWND desktop = ::GetDesktopWindow();
	CRect rcDesk;
	::GetWindowRect(desktop, &rcDesk);

	CRect rc;
	GetClientRect(&rc);
	SetWindowPos(NULL, rcDesk.Width() / 2 - (rc.Width() / 2), 500, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	CRgn rgn;
	rgn.CreateRoundRectRgn(0, 0, rc.Width(), rc.Height(), 30, 30);
	SetWindowRgn(static_cast<HRGN>(rgn.GetSafeHandle()), TRUE);
	rgn.Detach();

	return TRUE;
}


void CMsgPopupDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == POP_MSG_TIMER) {
		KillTimer(POP_MSG_TIMER);
		DestroyWindow();
	}

	CDialogEx::OnTimer(nIDEvent);
}

BOOL CMsgPopupDlg::OnEraseBkgnd(CDC* pDC)
{
	CRect rect;
	GetClientRect(&rect);
	CBrush myBrush(RGB(255, 255, 255));
	CBrush* pOld = pDC->SelectObject(&myBrush);
	BOOL bRes = pDC->PatBlt(0, 0, rect.Width(), rect.Height(), PATCOPY);
	pDC->SelectObject(pOld);
	return bRes;
}

HBRUSH CMsgPopupDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	if (nCtlColor == CTLCOLOR_STATIC) {
		pDC->SetTextColor(RGB(0, 0, 0));
		pDC->SetBkColor(RGB(255, 255, 255));
		hbr = ::CreateSolidBrush(RGB(255, 255, 255));
	}

	return hbr;
}


void CMsgPopupDlg::OnNcPaint()
{
	CDC* pDC = GetWindowDC();
	
	CBrush brush;
	brush.CreateSolidBrush(RGB(255, 0, 0));

	/*
	CRect rect;
	GetWindowRect(&rect);
	rect.OffsetRect(-rect.left, -rect.top);
	pDC->FrameRect(&rect, &brush);
	*/
	CRect rect;
	GetWindowRect(&rect);

	CRgn rgn;
	GetWindowRgn(static_cast<HRGN>(rgn.GetSafeHandle()));
	pDC->FrameRgn(&rgn, &brush, rect.Width()-2, rect.Height()-2);
	ReleaseDC(pDC);
}
