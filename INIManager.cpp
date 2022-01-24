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
// INI������ �ε��Ѵ�.
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

	// �ǻ��̹������� ���� ���� ����
	m_nVCamCutLeft		= (UINT)_ttoi(ReadINI(_T("VISABLE_CAM"),
											  _T("VC_CUT_LEFT"),
											  GetMakeString(_T("%d"),
															m_nVCamCutLeft)));

	// �ǻ��̹������� ��� ���� ����
	m_nVCamCutTop		= (UINT)_ttoi(ReadINI(_T("VISABLE_CAM"),
											  _T("VC_CUT_TOP"),
											  GetMakeString(_T("%d"),
															m_nVCamCutTop)));

	// �ǻ��̹������� ���� ���� ����
	m_nVCamCutRight		= (UINT)_ttoi(ReadINI(_T("VISABLE_CAM"),
											  _T("VC_CUT_RIGHT"),
											  GetMakeString(_T("%d"),
															m_nVCamCutRight)));

	// �ǻ��̹������� �ϴ� ���� ����
	m_nVCamCutBottom	= (UINT)_ttoi(ReadINI(_T("VISABLE_CAM"),
											  _T("VC_CUT_BOTTOM"),
											  GetMakeString(_T("%d"),
															m_nVCamCutBottom)));

	// �󱼰��� �ڽ��� ���ƴٰ� �Ǵ��ϴ� ���ǰ�
	// FaceMeSDK���� �Ѿ󱼿� 2���� �ڽ��� �������� ���� ����.
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

	// �����̹������� ���� ���� ����
	m_nTCamCutLeft		= (UINT)_ttoi(ReadINI(_T("THERMAL_CAM"),
											  _T("TC_CUT_LEFT"),
											  GetMakeString(_T("%d"),
															m_nTCamCutLeft)));

	// �����̹������� ��� ���� ����
	m_nTCamCutTop		= (UINT)_ttoi(ReadINI(_T("THERMAL_CAM"),
											  _T("TC_CUT_TOP"),
											  GetMakeString(_T("%d"),
															m_nTCamCutTop)));

	// �����̹������� ���� ���� ����
	m_nTCamCutRight		= (UINT)_ttoi(ReadINI(_T("THERMAL_CAM"),
											  _T("TC_CUT_RIGHT"),
											  GetMakeString(_T("%d"),
															m_nTCamCutRight)));

	// �����̹������� �ϴ� ���� ����
	m_nTCamCutBottom	= (UINT)_ttoi(ReadINI(_T("THERMAL_CAM"),
											  _T("TC_CUT_BOTTOM"),
											  GetMakeString(_T("%d"),
															m_nTCamCutBottom)));

	// ���ٵ� �������
	m_bUseBlackbody		= (BOOL)_ttoi(ReadINI(_T("THERMAL_CAM"),
											  _T("TC_USE_BLACKBODY"),
											  GetMakeString(_T("%d"),
															m_bUseBlackbody)));

	// ���ٵ� ��ǥ(���� ��ǥ��)
	// ������ ,
	CString strPos		= ReadINI(_T("THERMAL_CAM"),
								  _T("TC_BLACKBODY_POS"),
								  _T("0,0"));
	CStringList strList;
	theHelperMan().Split(strPos, _T(","), &strList);
	if( strList.GetCount() == 2 ) {
		m_ptBlackbody.SetPoint(_ttoi(strList.GetAt(strList.FindIndex(0))),
							   _ttoi(strList.GetAt(strList.FindIndex(1))));
	}

	// ���ٵ� �µ�
	m_fBlackbodyTemp	= (float)_ttof(ReadINI(_T("THERMAL_CAM"),
											   _T("TC_BLACKBODY_TEMP"),
											   GetMakeString(_T("%.1f"),
															 m_fBlackbodyTemp)));
	///////////////////////////////////////////////////////////////////////////
}


// 
// \brief <pre>
// ������ ��������.
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
// ������ ��������.
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

	// INI ���Ͽ� �ش� Key�� �������� �ʴ´ٸ� INI�� write�Ѵ�.
	if( strValue.Compare(USTR_INI_NO_EXIST) == 0 && a_bWrite ) {
		WriteINI(a_strSection,
			a_strKeyValue,
			a_strDefaultValue);

		// write �Ŀ� �ٽ� �о�´�.
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
// �¿� ����
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
// �󱼰��� �ּ� ��
// </pre>
// \return  UINT 
// 
UINT CINIManager::GetMinFaceWidth()
{
	return m_nMinFaceWidth;
}


// 
// \brief <pre>
// ����IP�� ��´�.
// </pre>
// \return  CString 
// 
CString CINIManager::GetTCamIP()
{
	return m_strTCamIP;
}


// 
// \brief <pre>
// �ǻ� ���� ����
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
//  �ǻ� ���� ���� (��)
// </pre>
// \return  UINT 
// 
UINT CINIManager::GetVCamCutLeft()
{
	return m_nVCamCutLeft;
}


// 
// \brief <pre>
//  �ǻ� ���� ���� (��)
// </pre>
// \return  UINT 
// 
UINT CINIManager::GetVCamCutTop()
{
	return m_nVCamCutTop;
}


// 
// \brief <pre>
//  �ǻ� ���� ���� (��)
// </pre>
// \return  UINT 
// 
UINT CINIManager::GetVCamCutRight()
{
	return m_nVCamCutRight;
}


// 
// \brief <pre>
//  �ǻ� ���� ���� (��)
// </pre>
// \return  UINT 
// 
UINT CINIManager::GetVCamCutBottom()
{
	return m_nVCamCutBottom;
}


// 
// \brief <pre>
//  ���� ���� ����
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
//  ���� ���� ���� (��)
// </pre>
// \return  UINT 
// 
UINT CINIManager::GetTCamCutLeft()
{
	return m_nTCamCutLeft;
}


// 
// \brief <pre>
//  ���� ���� ���� (��)
// </pre>
// \return  UINT 
// 
UINT CINIManager::GetTCamCutTop()
{
	return m_nTCamCutTop;
}


// 
// \brief <pre>
//  ���� ���� ���� (��)
// </pre>
// \return  UINT 
// 
UINT CINIManager::GetTCamCutRight()
{
	return m_nTCamCutRight;
}


// 
// \brief <pre>
//  ���� ���� ���� (��)
// </pre>
// \return  UINT 
// 
UINT CINIManager::GetTCamCutBottom()
{
	return m_nTCamCutBottom;
}


// 
// \brief <pre>
// ���ٵ� ��� ����
// </pre>
// \return  BOOL 
// 
BOOL CINIManager::IsUseBlackbody()
{
	return m_bUseBlackbody;
}


// 
// \brief <pre>
// ���ٵ� ��ǥ (���� ��ǥ��)
// </pre>
// \return  CPoint 
// 
CPoint CINIManager::GetBlackbodyPos()
{
	return m_ptBlackbody;
}


// 
// \brief <pre>
// ���ٵ� �µ�
// </pre>
// \return  float 
// 
float CINIManager::GetBlackbodyTemp()
{
	return m_fBlackbodyTemp;
}


// 
// \brief <pre>
// �ǻ󿡼� ���� ���� ���� ����
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
// ���󿡼� ���� ���� ���� ����
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
// �ڽ� ������ ���ǰ��� ����
// </pre>
// \return  float 
// 
float CINIManager::GetPassOverlapBoxRatio()
{
	return m_fOverlapRatio;
}

