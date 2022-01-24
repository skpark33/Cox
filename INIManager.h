#pragma once


#define		BUFFER_SIZE				1024
#define		USTR_INI_NO_EXIST		_T("NO_EXIST")


// \brief
// ����� ��� �� ���� ����� ���.
//
// \code
// - ���� �ڵ带 �־��ּ���.
// \encode
// \warning
// \sa
// \author	������
// \date		2021-12-27 15:09:16		�����ۼ�
class CINIManager
{
public:
	friend CINIManager& theINIMan();

	~CINIManager();

	void				LoadINI(
							_In_ const CString&		a_strPath);

	BOOL				IsMirror();

	CString				GetFMLicenseKey();

	UINT				GetMinFaceWidth();

	CString				GetTCamIP();

	CRect				GetVCamCutRect();

	UINT				GetVCamCutLeft();

	UINT				GetVCamCutTop();

	UINT				GetVCamCutRight();

	UINT				GetVCamCutBottom();

	CRect				GetTCamCutRect();

	UINT				GetTCamCutLeft();

	UINT				GetTCamCutTop();

	UINT				GetTCamCutRight();

	UINT				GetTCamCutBottom();

	BOOL				IsUseBlackbody();

	CPoint				GetBlackbodyPos();

	float				GetBlackbodyTemp();

	BOOL				IsVisableZoom();

	BOOL				IsThermalZoom();

	float				GetPassOverlapBoxRatio();




private:
	CINIManager();

	void				WriteINI(
							_In_ const CString&		a_strSection,
							_In_ const CString&		a_strKeyValue,
							_In_ const CString&		a_strValue);

	CString				ReadINI(
							_In_ const CString&		a_strSection,
							_In_ const CString&		a_strKeyValue,
							_In_ const CString&		a_strDefaultValue,
							_In_ BOOL				a_bWrite = TRUE);


private:
	// INI���� �н�
	CString						m_strConfigPath;

	// Mirror ���� �¿� ����
	BOOL						m_bMirror;

	// FaceMeSDK LicenseKey
	CString						m_strFaceMeLicense;

	// FaeMeSDK MinFaceWidth	(���� �������� �����Ÿ���, �����ս���)
	//							(���� Ŭ���� �����Ÿ���, �����ս���)
	// 8����� ���
	UINT						m_nMinFaceWidth;


	// ���� IP
	CString						m_strTCamIP;

	// �ǻ��̹������� �����¿� ���� ����
	UINT						m_nVCamCutLeft;
	UINT						m_nVCamCutTop;
	UINT						m_nVCamCutRight;
	UINT						m_nVCamCutBottom;

	// �����̹������� �����¿� ���� ����
	UINT						m_nTCamCutLeft;
	UINT						m_nTCamCutTop;
	UINT						m_nTCamCutRight;
	UINT						m_nTCamCutBottom;

	// ���ٵ� �������
	BOOL						m_bUseBlackbody;

	// ���ٵ� ��ǥ(���� ��ǥ��)
	CPoint						m_ptBlackbody;

	// ���ٵ� �µ�
	float						m_fBlackbodyTemp;

	// �󱼰��� �ڽ��� ���ƴٰ� �Ǵ��ϴ� ���ǰ�
	float						m_fOverlapRatio;
};

