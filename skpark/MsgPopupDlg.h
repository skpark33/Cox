#pragma once


// CMsgPopupDlg ��ȭ ����

class CMsgPopupDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMsgPopupDlg)

public:
	CMsgPopupDlg(CWnd* pParent = nullptr);   // ǥ�� �������Դϴ�.
	virtual ~CMsgPopupDlg();

	// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_POP_MSG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
	virtual void PostNcDestroy();
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnNcPaint();
};
