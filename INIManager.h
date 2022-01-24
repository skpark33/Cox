#pragma once


#define		BUFFER_SIZE				1024
#define		USTR_INI_NO_EXIST		_T("NO_EXIST")


// \brief
// 모듈의 기능 및 설명 사용방법 기술.
//
// \code
// - 샘플 코드를 넣어주세요.
// \encode
// \warning
// \sa
// \author	이현석
// \date		2021-12-27 15:09:16		최초작성
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
	// INI파일 패스
	CString						m_strConfigPath;

	// Mirror 영상 좌우 반전
	BOOL						m_bMirror;

	// FaceMeSDK LicenseKey
	CString						m_strFaceMeLicense;

	// FaeMeSDK MinFaceWidth	(값이 작을수록 감지거리↑, 퍼포먼스↓)
	//							(값이 클수록 감지거리↓, 퍼포먼스↑)
	// 8배수로 사용
	UINT						m_nMinFaceWidth;


	// 열상 IP
	CString						m_strTCamIP;

	// 실상이미지에서 상하좌우 컷팅 영역
	UINT						m_nVCamCutLeft;
	UINT						m_nVCamCutTop;
	UINT						m_nVCamCutRight;
	UINT						m_nVCamCutBottom;

	// 열상이미지에서 상하좌우 컷팅 영역
	UINT						m_nTCamCutLeft;
	UINT						m_nTCamCutTop;
	UINT						m_nTCamCutRight;
	UINT						m_nTCamCutBottom;

	// 블랙바디 사용유무
	BOOL						m_bUseBlackbody;

	// 블랙바디 좌표(열상 좌표계)
	CPoint						m_ptBlackbody;

	// 블랙바디 온도
	float						m_fBlackbodyTemp;

	// 얼굴감지 박스가 겹쳤다고 판단하는 조건값
	float						m_fOverlapRatio;
};

