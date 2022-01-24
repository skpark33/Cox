#pragma once


// CMsgPopupDlg 대화 상자

class CMsgPopupDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMsgPopupDlg)

public:
	CMsgPopupDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CMsgPopupDlg();

	// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_POP_MSG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	virtual void PostNcDestroy();
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnNcPaint();
};
