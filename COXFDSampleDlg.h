
// COXFDSampleDlg.h : 헤더 파일
//

#pragma once
#include "TCamView.h"
#include "VCamView.h"
#include <vector>
#include <memory>
#include "afxwin.h"


#include "Guardian/FacePainterStatic.h"  //skpark  in your area
class CGuardian;  //skpark add
class CoxGuardian;  //skpark add


// CCOXFDSampleDlg 대화 상자
class CCOXFDSampleDlg : public CDialogEx
{
// 생성입니다.
public:
	CCOXFDSampleDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

	virtual ~CCOXFDSampleDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_COXFDSAMPLE_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.
	HICON m_hIcon;
	DECLARE_MESSAGE_MAP()



private:
	void				InitMainUI();

	//void				SetControlPos();   //skpark comment out
	void				FindVCamList();

	void				GetSupportResolution(
							_In_ IMoniker*			a_pMoniker,
							_In_ VCAM_INFO*			a_pCam);

	void				RunVCamThread();

	void				RunTCamThread();

	void				RunWaitForConnThread();


private:
	static THREADFUNC	VCamThread(
							_In_ LPVOID				a_pVoid);

	static THREADFUNC	TCamThread(
							_In_ LPVOID				a_pVoid);

	static THREADFUNC	WaitForConnThread(
							_In_ LPVOID				a_pVoid);


private:
	LRESULT				OnUserNotify(
							_In_ WPARAM				a_wParam,
							_In_ LPARAM				a_lParam);

private:
	// UI
	// Visable Camera Group
	CStatic										m_stVCamGroup;
	CComboBox									m_cbVCamList;
	CComboBox									m_cbVCamSupportResolutionList;
	CButton										m_BtnVCamReload;
	CEdit										m_edVisableSkipFrame;
	UINT										m_nVisableSkipFrame;
	CButton										m_BtnSkipFrame;

	// Thermal Camera Group
	CStatic										m_stTCamGroup;
	CComboBox									m_cbTCamList;
	CComboBox									m_cbTCamPaletteList;
	CButton										m_BtnTCamReload;


	CButton										m_BtnConnect;
	CButton										m_BtnDisconnect;

	CVCamView									m_wndVCam;
	CTCamView									m_wndTCam;


	// Visable Camera
	std::vector<std::shared_ptr<VCAM_INFO> >	m_vVCamInfoList;
	BOOL										m_bVCamRefresh;
	HANDLE										m_hVCamThread;
	UINT										m_nVCamThreadID;
	CString										m_strVCamName;
	int											m_nVCamWidth;
	int											m_nVCamHeight;


	// Thermal Camera
	BOOL										m_bTCamRefresh;
	HANDLE										m_hTCamThread;
	UINT										m_nTCamThreadID;
	CString										m_strTCamIP;



	HANDLE										m_hConnWaitThread;
	UINT										m_nConnWaitThreadID;
	HANDLE										m_hEvtConnWait[2];
												// m_hEvtWaitClose[0] 실상용
												// m_hEvtWaitClose[1] 열상용


public:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnCbnSelchangeCombo1();
	afx_msg void OnBnClickedVCamReload();
	afx_msg void OnBnClickedTCamReload();
	afx_msg void OnBnClickedConnect();
	afx_msg void OnBnClickedDisconnect();
	afx_msg void OnCbnSelchangeCombo4();
	afx_msg void OnBnClickedVisableSkipFrameApply();

	//skpark  in your area start
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg LRESULT OnOldFileDelete(WPARAM, LPARAM);
	void CheckBlackBodyTemp();
public:
	bool m_isMinimize;
	CoxGuardian* m_cox_guardian;
	CStatic m_statArea;
	CFacePainterStatic m_stFaceArea;
	//skpark  in your area end

};
