#pragma once

// HoverButton.h : header file

/////////////////////////////////////////////////////////////////////////////
// CHoverButton by Niek Albers
// Thanks to some people for the tooltip.
// A cool CBitmapButton derived class with 3 states,
// Up/Down/Hover/Disable.

class CHoverButton : public CBitmapButton
{
	DECLARE_DYNAMIC(CHoverButton);

public:
	CHoverButton();
	virtual ~CHoverButton();

protected:
	virtual BOOL	PreTranslateMessage(MSG* pMsg);
	virtual void	DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

	afx_msg void	OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT	OnMouseLeave(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT	OnMouseHover(WPARAM wparam, LPARAM lparam) ;
	afx_msg void	OnLButtonUp(UINT nFlags, CPoint point);

	BOOL	m_bHover;						// indicates if mouse is over the button
	CSize	m_ButtonSize;					// width and height of the button
	CBitmap	mybitmap;
	BOOL	m_bTracking;

	CWnd*	m_pParentWnd;
	UINT	m_command;

	COLORREF	m_transparentColor;		// 투명색 처리하기위한 색깔

	CToolTipCtrl	m_ToolTip;
	void			InitToolTip();
	void			ActivateTooltip(BOOL bActivate = TRUE);

	DECLARE_MESSAGE_MAP()

public:
	void	SetToolTipText(LPCTSTR lpszTT, BOOL bActivate = TRUE);
	void	SetToolTipText(int nId, BOOL bActivate = TRUE);

	BOOL	LoadBitmap(UINT bitmapid, COLORREF transparentColor);
	void	SetParent(CWnd* wnd, UINT command) { m_pParentWnd=wnd; m_command = command;};
};
