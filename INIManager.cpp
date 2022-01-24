#include "stdafx.h"
#include "INIManager.h"
#include "MyHelperMan.h"


CINIManager& theINIMan()
{
	static CINIManager man;
	return man;
}


CINIManager::CINIManager()
	: m_bMirror(TRUE)
	, m_nMinFaceWidth(64)
	, m_nVCamCutLeft(0)
	, m_nVCamCutTop(0)
	, m_nVCamCutRight(0)
	, m_nVCamCutBottom(0)
	, m_nTCamCutLeft(0)
	, m_nTCamCutTop(0)
	, m_nTCamCutRight(0)
	, m_nTCamCutBottom(0)
	, m_bUseBlackbody(FALSE)
	, m_fBlackbodyTemp(40.f)
	, m_fOverlapRatio(0.7f)
{

}


CINIManager::~CINIManager()
{
}


// 
// \brief <pre>
// INI정보를 로드한다.
// </pre>
// \param   CString&		a_strPath
// 
void CINIManager::LoadINI(
	_In_ const CString&		a_strPath)
{
	m_strConfigPath = a_strPath;

	// General Section ////////////////////////////////////////////////////////
	// Mirror
	m_bMirror			= (BOOL)_ttoi(ReadINI(_T("GENERAL"),
											  _T("MIRROR"),
											  GetMakeString(_T("%d"), 
															m_bMirror)));
	///////////////////////////////////////////////////////////////////////////

	// Visable Camera Section /////////////////////////////////////////////////
	// License Key
	m_strFaceMeLicense	= ReadINI(_T("VISABLE_CAM"),
								  _T("LICENSE_KEY"),
								  _T(""));

	// Min Face Width
	m_nMinFaceWidth		= (UINT)_ttoi(ReadINI(_T("VISABLE_CAM"),
											  _T("MIN_FACE_WIDTH"),
											  GetMakeString(_T("%d"),
															m_nMinFaceWidth)));

	// 실상이미지에서 좌측 컷팅 영역
	m_nVCamCutLeft		= (UINT)_ttoi(ReadINI(_T("VISABLE_CAM"),
											  _T("VC_CUT_LEFT"),
											  GetMakeString(_T("%d"),
															m_nVCamCutLeft)));

	// 실상이미지에서 상단 컷팅 영역
	m_nVCamCutTop		= (UINT)_ttoi(ReadINI(_T("VISABLE_CAM"),
											  _T("VC_CUT_TOP"),
											  GetMakeString(_T("%d"),
															m_nVCamCutTop)));

	// 실상이미지에서 우측 컷팅 영역
	m_nVCamCutRight		= (UINT)_ttoi(ReadINI(_T("VISABLE_CAM"),
											  _T("VC_CUT_RIGHT"),
											  GetMakeString(_T("%d"),
															m_nVCamCutRight)));

	// 실상이미지에서 하단 컷팅 영역
	m_nVCamCutBottom	= (UINT)_ttoi(ReadINI(_T("VISABLE_CAM"),
											  _T("VC_CUT_BOTTOM"),
											  GetMakeString(_T("%d"),
															m_nVCamCutBottom)));

	// 얼굴감지 박스가 겹쳤다고 판단하는 조건값
	// FaceMeSDK에서 한얼굴에 2개의 박스를 리턴해줄 때가 있음.
	m_fOverlapRatio		= (float)_ttof(ReadINI(_T("VISABLE_CAM"),
											   _T("PASS_OVERLAP_BOX_RATIO"),
											   GetMakeString(_T("%d"),
															 (int)(m_fOverlapRatio * 100))));
	m_fOverlapRatio /= 100;

	///////////////////////////////////////////////////////////////////////////


	// Thermal Camera Section /////////////////////////////////////////////////
	// ThermalCam IP
	m_strTCamIP			= ReadINI(_T("THERMAL_CAM"),
								  _T("IP"),
								  _T(""));

	// 열상이미지에서 좌측 컷팅 영역
	m_nTCamCutLeft		= (UINT)_ttoi(ReadINI(_T("THERMAL_CAM"),
											  _T("TC_CUT_LEFT"),
											  GetMakeString(_T("%d"),
															m_nTCamCutLeft)));

	// 열상이미지에서 상단 컷팅 영역
	m_nTCamCutTop		= (UINT)_ttoi(ReadINI(_T("THERMAL_CAM"),
											  _T("TC_CUT_TOP"),
											  GetMakeString(_T("%d"),
															m_nTCamCutTop)));

	// 열상이미지에서 우측 컷팅 영역
	m_nTCamCutRight		= (UINT)_ttoi(ReadINI(_T("THERMAL_CAM"),
											  _T("TC_CUT_RIGHT"),
											  GetMakeString(_T("%d"),
															m_nTCamCutRight)));

	// 열상이미지에서 하단 컷팅 영역
	m_nTCamCutBottom	= (UINT)_ttoi(ReadINI(_T("THERMAL_CAM"),
											  _T("TC_CUT_BOTTOM"),
											  GetMakeString(_T("%d"),
															m_nTCamCutBottom)));

	// 블랙바디 사용유무
	m_bUseBlackbody		= (BOOL)_ttoi(ReadINI(_T("THERMAL_CAM"),
											  _T("TC_USE_BLACKBODY"),
											  GetMakeString(_T("%d"),
															m_bUseBlackbody)));

	// 블랙바디 좌표(열상 좌표계)
	// 구분자 ,
	CString strPos		= ReadINI(_T("THERMAL_CAM"),
								  _T("TC_BLACKBODY_POS"),
								  _T("0,0"));
	CStringList strList;
	theHelperMan().Split(strPos, _T(","), &strList);
	if( strList.GetCount() == 2 ) {
		m_ptBlackbody.SetPoint(_ttoi(strList.GetAt(strList.FindIndex(0))),
							   _ttoi(strList.GetAt(strList.FindIndex(1))));
	}

	// 블랙바디 온도
	m_fBlackbodyTemp	= (float)_ttof(ReadINI(_T("THERMAL_CAM"),
											   _T("TC_BLACKBODY_TEMP"),
											   GetMakeString(_T("%.1f"),
															 m_fBlackbodyTemp)));
	///////////////////////////////////////////////////////////////////////////
}


// 
// \brief <pre>
// 내용을 넣으세요.
// </pre>
// \param   CString&		a_strSection
// \param   CString&		a_strKeyValue
// \param   CString&		a_strValue
// 
void CINIManager::WriteINI(
	_In_ const CString&		a_strSection,
	_In_ const CString&		a_strKeyValue,
	_In_ const CString&		a_strValue)
{
	WritePrivateProfileString(
		a_strSection,
		a_strKeyValue,
		a_strValue,
		m_strConfigPath);
}


// 
// \brief <pre>
// 내용을 넣으세요.
// </pre>
// \param   CString&		a_strSection
// \param   CString&		a_strKeyValue
// \param   CString&		a_strDefaultValue
// \param   BOOL			a_bWrite
// \return  CString 
// 
CString CINIManager::ReadINI(
	_In_ const CString&		a_strSection,
	_In_ const CString&		a_strKeyValue,
	_In_ const CString&		a_strDefaultValue,
	_In_ BOOL				a_bWrite)
{
	CString strValue;
	TCHAR szValue[BUFFER_SIZE] = { 0, };

	GetPrivateProfileString(
		a_strSection,
		a_strKeyValue,
		USTR_INI_NO_EXIST,
		szValue,
		sizeof(szValue),
		m_strConfigPath);
	strValue = szValue;
	::memset(szValue, 0, sizeof(szValue));

	// INI 파일에 해당 Key가 존재하지 않는다면 INI에 write한다.
	if( strValue.Compare(USTR_INI_NO_EXIST) == 0 && a_bWrite ) {
		WriteINI(a_strSection,
			a_strKeyValue,
			a_strDefaultValue);

		// write 후에 다시 읽어온다.
		GetPrivateProfileString(
			a_strSection,
			a_strKeyValue,
			USTR_INI_NO_EXIST,
			szValue,
			sizeof(szValue),
			m_strConfigPath);
		strValue = szValue;
		::memset(szValue, 0, sizeof(szValue));
	}
	return strValue;
}


// 
// \brief <pre>
// 좌우 반전
// </pre>
// \return  BOOL 
// 
BOOL CINIManager::IsMirror()
{
	return m_bMirror;
}


// 
// \brief <pre>
// FaceMeSDK License
// </pre>
// \return  CString 
// 
CString CINIManager::GetFMLicenseKey()
{
	return m_strFaceMeLicense;
}


// 
// \brief <pre>
// 얼굴감지 최소 값
// </pre>
// \return  UINT 
// 
UINT CINIManager::GetMinFaceWidth()
{
	return m_nMinFaceWidth;
}


// 
// \brief <pre>
// 열상IP를 얻는다.
// </pre>
// \return  CString 
// 
CString CINIManager::GetTCamIP()
{
	return m_strTCamIP;
}


// 
// \brief <pre>
// 실상 컷팅 영역
// </pre>
// \return  CRect 
// 
CRect CINIManager::GetVCamCutRect()
{
	return CRect(m_nVCamCutLeft,
				 m_nVCamCutTop,
				 m_nVCamCutRight,
				 m_nVCamCutBottom);
}


// 
// \brief <pre>
//  실상 컷팅 영역 (좌)
// </pre>
// \return  UINT 
// 
UINT CINIManager::GetVCamCutLeft()
{
	return m_nVCamCutLeft;
}


// 
// \brief <pre>
//  실상 컷팅 영역 (상)
// </pre>
// \return  UINT 
// 
UINT CINIManager::GetVCamCutTop()
{
	return m_nVCamCutTop;
}


// 
// \brief <pre>
//  실상 컷팅 영역 (우)
// </pre>
// \return  UINT 
// 
UINT CINIManager::GetVCamCutRight()
{
	return m_nVCamCutRight;
}


// 
// \brief <pre>
//  실상 컷팅 영역 (하)
// </pre>
// \return  UINT 
// 
UINT CINIManager::GetVCamCutBottom()
{
	return m_nVCamCutBottom;
}


// 
// \brief <pre>
//  열상 컷팅 영역
// </pre>
// \return  CRect 
// 
CRect CINIManager::GetTCamCutRect()
{
	return CRect(m_nTCamCutLeft,
				 m_nTCamCutTop,
				 m_nTCamCutRight,
				 m_nTCamCutBottom);
}


// 
// \brief <pre>
//  열상 컷팅 영역 (좌)
// </pre>
// \return  UINT 
// 
UINT CINIManager::GetTCamCutLeft()
{
	return m_nTCamCutLeft;
}


// 
// \brief <pre>
//  열상 컷팅 영역 (상)
// </pre>
// \return  UINT 
// 
UINT CINIManager::GetTCamCutTop()
{
	return m_nTCamCutTop;
}


// 
// \brief <pre>
//  열상 컷팅 영역 (우)
// </pre>
// \return  UINT 
// 
UINT CINIManager::GetTCamCutRight()
{
	return m_nTCamCutRight;
}


// 
// \brief <pre>
//  열상 컷팅 영역 (하)
// </pre>
// \return  UINT 
// 
UINT CINIManager::GetTCamCutBottom()
{
	return m_nTCamCutBottom;
}


// 
// \brief <pre>
// 블랙바디 사용 유무
// </pre>
// \return  BOOL 
// 
BOOL CINIManager::IsUseBlackbody()
{
	return m_bUseBlackbody;
}


// 
// \brief <pre>
// 블랙바디 좌표 (열상 좌표계)
// </pre>
// \return  CPoint 
// 
CPoint CINIManager::GetBlackbodyPos()
{
	return m_ptBlackbody;
}


// 
// \brief <pre>
// 블랙바디 온도
// </pre>
// \return  float 
// 
float CINIManager::GetBlackbodyTemp()
{
	return m_fBlackbodyTemp;
}


// 
// \brief <pre>
// 실상에서 컷팅 영역 존재 유무
// </pre>
// \return  BOOL 
// 
BOOL CINIManager::IsVisableZoom()
{
	return (m_nVCamCutLeft != 0)
		|| (m_nVCamCutTop != 0)
		|| (m_nVCamCutRight != 0)
		|| (m_nVCamCutBottom != 0);
}


// 
// \brief <pre>
// 열상에서 컷팅 영역 존재 유무
// </pre>
// \return  BOOL 
// 
BOOL CINIManager::IsThermalZoom()
{
	return (m_nTCamCutLeft != 0)
		|| (m_nTCamCutTop != 0)
		|| (m_nTCamCutRight !=0 )
		|| (m_nTCamCutBottom != 0);
}


// 
// \brief <pre>
// 박스 오버랩 조건값을 리턴
// </pre>
// \return  float 
// 
float CINIManager::GetPassOverlapBoxRatio()
{
	return m_fOverlapRatio;
}

