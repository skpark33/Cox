// FacePainterStatic.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "CoxGuardian.h"
#include "FacePainterStatic.h"


// CFacePainterStatic

IMPLEMENT_DYNAMIC(CFacePainterStatic, CStatic)

CFacePainterStatic::CFacePainterStatic()
: m_cox_guardian(0)
{
}

CFacePainterStatic::~CFacePainterStatic()
{
}


BEGIN_MESSAGE_MAP(CFacePainterStatic, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CFacePainterStatic 메시지 처리기입니다.



void CFacePainterStatic::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect client_rect;
	GetClientRect(client_rect);

	// 깜빡임 방지 코드 start
	CRect rect;
	this->GetClientRect(&rect);
	CDC* pDC = (CDC*)&dc;
	CDC dcMem;
	CBitmap bitmap;
	dcMem.CreateCompatibleDC(pDC);
	bitmap.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
	CBitmap* pOldBitmap = dcMem.SelectObject(&bitmap);
	// 깜박임 방지 코드 end

	COLORREF  bg = NORMAL_BG_COLOR;

	switch (m_cox_guardian->m_alarmState)
	{
		case NO_MASK_ALARM:  bg = NO_MASK_BG_COLOR; break;
		case FEVER_ALARM:  bg = FEVER_BG_COLOR; break;
	}
	dcMem.FillSolidRect(&client_rect, bg);
	if (m_cox_guardian->m_alarmState == NO_ALARM)
	{
		m_cox_guardian->DrawNotice(dcMem, NOTICE_AREA_HEIGHT / 2 + 5);
	}
	else
	{
		m_cox_guardian->DrawFaces(&dcMem);
	}
	//깜박임 방지 코드 start
	pDC->BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), &dcMem, 0, 0, SRCCOPY);
	dcMem.SelectObject(pOldBitmap);
	//깜박임 방지 코드 end
}

