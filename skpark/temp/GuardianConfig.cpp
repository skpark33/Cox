/**********************************************************
FileName:    GuardianConfig.cpp
Description: set and remove guard
Date:        2008/11/19
Note:          <global>struct, define refer to GeneralDef.h, global variables and functions refer to ClientDemo.cpp
Modification History:
<version> <time>         <desc>
<1.0    > <2008/11/19>       <created>
***********************************************************/

#include "stdafx.h"
#include "GuardianConfig.h"
#ifdef _COP_UTV_
#define TraceLog(x)
#define UBCBRW_INI				_T("UBCBrowser.ini")
#define UBC_CONFIG_PATH	"C:\\SQISoft\\UTV1.0\\execute\\data\\"
#define		PROPERTY_DELI1		"###"
#define		PROPERTY_DELI2		"+++"
#else
#include "skpark/TraceLog.h" //skpark
#endif

#define ENTRY_NAME	"GUARDIAN"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGuardianConfig 

/*********************************************************
Function:    CGuardianConfig
Desc:        Constructor
Input:    none
Output:    none
Return:    none
**********************************************************/
CGuardianConfig::CGuardianConfig()
{
	m_iniPath = UBC_CONFIG_PATH;
	m_iniPath += UBCBRW_INI;

	m_not_use_encrypt = false;
	m_face_score_limit = 0.7;
	m_faceApiType = FACE_API_SQISOFT;
	m_alarmValidSec = 15;
	m_isWide = false;
	m_useQR = false;
	m_videoBelow = false;

	m_is_hiden_camera = false;

	Read();
}
CGuardianConfig::CGuardianConfig(LPCTSTR iniPath)
{
	m_iniPath = iniPath;

	m_not_use_encrypt = false;
	m_face_score_limit = 0.7;
	m_faceApiType = FACE_API_SQISOFT;
	m_alarmValidSec = 15;
	m_isWide = false;
	m_useQR = false;
	m_videoBelow = false;

	m_is_hiden_camera = false;

	Read();
}


BOOL CGuardianConfig::Read()
{
	/*
	QUERYALL=IP+++192.168.11.65###title+++SQISOFT###subTitle+++발열 셀프체크 키오스크###noticeTitle+++공지사항###alarmTitle+++!WARNING!###alarmSec+++30
	IP=192.168.11.65
	title=SQISOFT
	subTitle=발열 셀프체크 키오스크
	noticeTitle=공지사항
	alarmTitle=!WARNING!
	alarmSec=30
	CHANGE_IP_TIME = 1584504250
	DEVICE_INDEX = 1
	LAST_DEVICE_INDEX_TIME = 1588401724
	FACEAPI = http://54.180.40.71:8080/face/REST/b49/identify
	FACEAPITYPE=1
	USE_FACE = 1
	SHOW_NAME = 1
	USE_WEATHER = 1
	measurStation=영등포구
	*/

	char buf[4096];

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME, "IP", "", buf, 4096, m_iniPath);
	m_ip = buf;

	memset(buf, 0x00, 4096);
	GetPrivateProfileString("UTV_brwClient2", "UsePosition", "1", buf, 4096, m_iniPath);
	m_use_pos = atoi(buf);


	if (m_ip.IsEmpty())
	{
		GetPrivateProfileString("GUARDIAN", "QUERYALL", "", buf, 4096, m_iniPath);
		CString property_value = buf;

		if (property_value.IsEmpty())
		{
			TraceLog(("CONFIG FILE ERROR"));
			return FALSE;
		}

		TraceLog(("QueryAll is %s", property_value));

		int pos = 0;
		CString nvPair = property_value.Tokenize(PROPERTY_DELI1, pos);
		while (nvPair != "")
		{
			TraceLog(("nvPair=%s", nvPair));
			int pos2 = 0;
			CString name = nvPair.Tokenize(PROPERTY_DELI2, pos2);
			TraceLog(("name=%s", name));
			CString val = nvPair.Tokenize(PROPERTY_DELI2, pos2);
			TraceLog(("val=%s", val));

			if (name == "IP")			m_ip = val;
			if (name == "title")		m_title = val;
			if (name == "subTitle")		m_subTitle = val;
			if (name == "noticeTitle")	m_noticeTitle = val;
			if (name == "alarmTitle")	m_alarmTitle = val;
			if (name == "alarmSec")		m_alarmValidSec = atoi(val);

			nvPair = property_value.Tokenize(PROPERTY_DELI1, pos);
		}
	}
	else
	{
		memset(buf, 0x00, 4096);
		GetPrivateProfileString(ENTRY_NAME, "title", "SQISOFT", buf, 4096, m_iniPath);
		m_title = buf;

		memset(buf, 0x00, 4096);
		GetPrivateProfileString(ENTRY_NAME, "subTitle", "발열 셀프체크 키오스크", buf, 4096, m_iniPath);
		m_subTitle = buf;

		memset(buf, 0x00, 4096);
		GetPrivateProfileString(ENTRY_NAME, "noticeTitle", "공지사항", buf, 4096, m_iniPath);
		m_noticeTitle = buf;

		memset(buf, 0x00, 4096);
		GetPrivateProfileString(ENTRY_NAME, "alarmTitle", "!WARNING!", buf, 4096, m_iniPath);
		m_alarmTitle = buf;

		memset(buf, 0x00, 4096);
		GetPrivateProfileString(ENTRY_NAME, "alarmSec", "15", buf, 4096, m_iniPath);
		m_alarmValidSec = atoi(buf);
	}
	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME, "CHANGE_IP_TIME", "0", buf, 4096, m_iniPath);
	m_change_ip_time = strtoul(buf, NULL, 10);

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME, "DEVICE_INDEX", "1", buf, 4096, m_iniPath);
	m_device_index = atoi(buf);

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME, "LAST_DEVICE_INDEX_TIME", "0", buf, 4096, m_iniPath);
	m_last_device_index_time = strtoul(buf, NULL, 10);

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME, "FACEAPI", "http://54.180.40.71:10049/face/REST/b49/identify", buf, 4096, m_iniPath);
	m_faceApi = buf;

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME, "FACEAPITYPE", "1", buf, 4096, m_iniPath);
	m_faceApiType = atoi(buf);

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME, "USE_FACE", "0", buf, 4096, m_iniPath);
	m_use_face = atoi(buf);

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME, "IS_WIDE", "0", buf, 4096, m_iniPath);
	m_isWide = atoi(buf);

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME, "USE_QR", "0", buf, 4096, m_iniPath);
	m_useQR = atoi(buf);

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME, "SHOW_NAME", "0", buf, 4096, m_iniPath);
	m_show_name = atoi(buf);

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME, "RECORD_ALL", "0", buf, 4096, m_iniPath);
	m_record_all = atoi(buf);

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME, "FACE_SCORE_LIMIT", "0.7", buf, 4096, m_iniPath);
	m_face_score_limit = atof(buf);
	if (m_face_score_limit > 1.0f || m_face_score_limit < 0.0f)
	{
		m_face_score_limit = 0.7;
	}

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME, "NOT_USE_ENCRYPT", "0", buf, 4096, m_iniPath);
	m_not_use_encrypt = atoi(buf);

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME, "USE_WEATHER", "0", buf, 4096, m_iniPath);
	m_use_weather = atoi(buf);

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME, "AIR_MEASUR_STATION", "영등포구", buf, 4096, m_iniPath);
	m_airMeasurStation = buf;

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME, "USE_MASK_CHECK", "0", buf, 4096, m_iniPath);
	m_use_mask_check = atoi(buf);

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME, "HIDEN_CAMERA", "0", buf, 4096, m_iniPath);
	m_is_hiden_camera = bool(atoi(buf));

	if (!m_is_hiden_camera) {
		memset(buf, 0x00, 4096);
		GetPrivateProfileString(ENTRY_NAME, "VIDEO_BELOW", "0", buf, 4096, m_iniPath);
		m_videoBelow = bool(atoi(buf));
	}
	else {
		m_videoBelow = false;
	}
	TraceLog(("USE_WEATHER[%d] AIR_MEASUR_STATION[%s] USE_MASK_CHECK[%d] VIDEO_BELOW[%d] ini[%s]", m_use_weather, m_airMeasurStation, m_use_mask_check, m_videoBelow, m_iniPath));
	//	TraceLog(("FACEAPI[%s]", m_faceApi));

	return TRUE;
}

BOOL CGuardianConfig::FromString(LPCTSTR fromStr, bool createFile)
{
	CString a_iniPath;
	a_iniPath = UBC_CONFIG_PATH;
	a_iniPath += "temp_browser.ini";

	if (createFile)
	{
		CFileException ex;
		CFile clsFile;
		if (!clsFile.Open(a_iniPath, CFile::modeCreate | CFile::modeWrite | CFile::typeText | CFile::shareDenyNone, &ex))
		{
			TCHAR szError[1024] = { 0x00 };
			ex.GetErrorMessage(szError, 1024);
			TRACE(szError);
			return false;
		}//if


		clsFile.Write(fromStr, strlen(fromStr));
		clsFile.Close();
	}

	char buf[4096];

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME, "IP", "", buf, 4096, a_iniPath);
	m_ip = buf;

	memset(buf, 0x00, 4096);
	GetPrivateProfileString("UTV_brwClient2", "UsePosition", "1", buf, 4096, a_iniPath);
	m_use_pos = atoi(buf);


	if (m_ip.IsEmpty())
	{
		GetPrivateProfileString("GUARDIAN", "QUERYALL", "", buf, 4096, a_iniPath);
		CString property_value = buf;

		if (property_value.IsEmpty())
		{
			TraceLog(("CONFIG FILE ERROR"));
			return FALSE;
		}

		TraceLog(("QueryAll is %s", property_value));

		int pos = 0;
		CString nvPair = property_value.Tokenize(PROPERTY_DELI1, pos);
		while (nvPair != "")
		{
			TraceLog(("nvPair=%s", nvPair));
			int pos2 = 0;
			CString name = nvPair.Tokenize(PROPERTY_DELI2, pos2);
			TraceLog(("name=%s", name));
			CString val = nvPair.Tokenize(PROPERTY_DELI2, pos2);
			TraceLog(("val=%s", val));

			if (name == "IP")			m_ip = val;
			if (name == "title")		m_title = val;
			if (name == "subTitle")		m_subTitle = val;
			if (name == "noticeTitle")	m_noticeTitle = val;
			if (name == "alarmTitle")	m_alarmTitle = val;
			if (name == "alarmSec")		m_alarmValidSec = atoi(val);

			nvPair = property_value.Tokenize(PROPERTY_DELI1, pos);
		}
	}
	else
	{
		memset(buf, 0x00, 4096);
		GetPrivateProfileString(ENTRY_NAME, "title", "SQISOFT", buf, 4096, a_iniPath);
		m_title = buf;

		memset(buf, 0x00, 4096);
		GetPrivateProfileString(ENTRY_NAME, "subTitle", "발열 셀프체크 키오스크", buf, 4096, a_iniPath);
		m_subTitle = buf;

		memset(buf, 0x00, 4096);
		GetPrivateProfileString(ENTRY_NAME, "noticeTitle", "공지사항", buf, 4096, a_iniPath);
		m_noticeTitle = buf;

		memset(buf, 0x00, 4096);
		GetPrivateProfileString(ENTRY_NAME, "alarmTitle", "!WARNING!", buf, 4096, a_iniPath);
		m_alarmTitle = buf;

		memset(buf, 0x00, 4096);
		GetPrivateProfileString(ENTRY_NAME, "alarmSec", "15", buf, 4096, a_iniPath);
		m_alarmValidSec = atoi(buf);
	}
	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME, "CHANGE_IP_TIME", "0", buf, 4096, a_iniPath);
	m_change_ip_time = strtoul(buf, NULL, 10);

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME, "DEVICE_INDEX", "1", buf, 4096, a_iniPath);
	m_device_index = atoi(buf);

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME, "LAST_DEVICE_INDEX_TIME", "0", buf, 4096, a_iniPath);
	m_last_device_index_time = strtoul(buf, NULL, 10);

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME, "FACEAPI", "http://54.180.40.71:8080/face/REST/b49/identify", buf, 4096, a_iniPath);
	m_faceApi = buf;

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME, "FACEAPITYPE", "1", buf, 4096, a_iniPath);
	m_faceApiType = atoi(buf);

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME, "USE_FACE", "0", buf, 4096, a_iniPath);
	m_use_face = atoi(buf);

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME, "IS_WIDE", "0", buf, 4096, a_iniPath);
	m_isWide = atoi(buf);

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME, "USE_QR", "0", buf, 4096, a_iniPath);
	m_useQR = atoi(buf);
	
	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME, "SHOW_NAME", "0", buf, 4096, a_iniPath);
	m_show_name = atoi(buf);

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME, "RECORD_ALL", "0", buf, 4096, a_iniPath);
	m_record_all = atoi(buf);

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME, "FACE_SCORE_LIMIT", "0.7", buf, 4096, a_iniPath);
	m_face_score_limit = atof(buf);
	if (m_face_score_limit > 1.0f || m_face_score_limit < 0.0f)
	{
		m_face_score_limit = 0.75;
	}

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME, "USE_WEATHER", "0", buf, 4096, a_iniPath);
	m_use_weather = atoi(buf);

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME, "AIR_MEASUR_STATION", "영등포구", buf, 4096, a_iniPath);
	m_airMeasurStation = buf;

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME, "USE_MASK_CHECK", "0", buf, 4096, a_iniPath);
	m_use_mask_check = atoi(buf);

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME, "HIDEN_CAMERA", "0", buf, 4096, a_iniPath);
	m_is_hiden_camera = atoi(buf);

	if (!m_is_hiden_camera) {
		memset(buf, 0x00, 4096);
		GetPrivateProfileString(ENTRY_NAME, "VIDEO_BELOW", "0", buf, 4096, a_iniPath);
		m_videoBelow = bool(atoi(buf));
	}
	else {
		m_videoBelow = false;
	}

	return TRUE;
}



BOOL CGuardianConfig::Write()
{
	TraceLog(("Write()"));
	CString buf;

	buf.Format("%d", m_use_pos);
	WritePrivateProfileString("UTV_brwClient2", "UsePosition", buf, m_iniPath);

	buf = m_ip;
	WritePrivateProfileString(ENTRY_NAME, "IP", buf, m_iniPath);

	buf = m_title;
	WritePrivateProfileString(ENTRY_NAME, "title", buf, m_iniPath);

	buf = m_subTitle;
	WritePrivateProfileString(ENTRY_NAME, "subTitle", buf, m_iniPath);

	buf = m_noticeTitle;
	WritePrivateProfileString(ENTRY_NAME, "noticeTitle", buf, m_iniPath);

	buf = m_alarmTitle;
	WritePrivateProfileString(ENTRY_NAME, "alarmTitle", buf, m_iniPath);

	buf.Format("%d", m_alarmValidSec);
	WritePrivateProfileString(ENTRY_NAME, "alarmSec", buf, m_iniPath);

	buf.Format("%lu", m_change_ip_time);
	WritePrivateProfileString(ENTRY_NAME, "CHANGE_IP_TIME", buf, m_iniPath);

	buf.Format("%d", m_device_index);
	WritePrivateProfileString(ENTRY_NAME, "DEVICE_INDEX", buf, m_iniPath);

	buf.Format("%lu", m_last_device_index_time);
	WritePrivateProfileString(ENTRY_NAME, "LAST_DEVICE_INDEX_TIME", buf, m_iniPath);

	buf = m_faceApi;
	WritePrivateProfileString(ENTRY_NAME, "FACEAPI", buf, m_iniPath);

	buf.Format("%d", m_faceApiType);
	WritePrivateProfileString(ENTRY_NAME, "FACEAPITYPE", buf, m_iniPath);

	buf.Format("%d", m_use_face);
	WritePrivateProfileString(ENTRY_NAME, "USE_FACE", buf, m_iniPath);

	buf.Format("%d", m_isWide);
	WritePrivateProfileString(ENTRY_NAME, "IS_WIDE", buf, m_iniPath);

	buf.Format("%d", m_useQR);
	WritePrivateProfileString(ENTRY_NAME, "USE_QR", buf, m_iniPath);

	buf.Format("%d", m_show_name);
	WritePrivateProfileStringA(ENTRY_NAME, "SHOW_NAME", buf, m_iniPath);

	buf.Format("%d", m_record_all);
	WritePrivateProfileStringA(ENTRY_NAME, "RECORD_ALL", buf, m_iniPath);

	buf.Format("%f", m_face_score_limit);
	WritePrivateProfileStringA(ENTRY_NAME, "FACE_SCORE_LIMIT", buf, m_iniPath);

	buf.Format("%d", m_use_weather);
	WritePrivateProfileString(ENTRY_NAME, "USE_WEATHER", buf, m_iniPath);

	buf = m_airMeasurStation;
	WritePrivateProfileString(ENTRY_NAME, "AIR_MEASUR_STATION", buf, m_iniPath);

	buf.Format("%d", m_use_mask_check);
	WritePrivateProfileString(ENTRY_NAME, "USE_MASK_CHECK", buf, m_iniPath);

	buf.Format("%d", m_is_hiden_camera);
	WritePrivateProfileString(ENTRY_NAME, "HIDEN_CAMERA", buf, m_iniPath);

	if (!m_is_hiden_camera) {
		buf.Format("%d", m_videoBelow);
	}
	else {
		buf.Format("%d", 0);
	}
	WritePrivateProfileString(ENTRY_NAME, "VIDEO_BELOW", buf, m_iniPath);

	TraceLog(("Write() end"));
	return TRUE;  // return TRUE unless you set the focus to a control
}

BOOL CGuardianConfig::WriteIniDeviceInfo(int index)
{
	CString indexStr;
	indexStr.Format("%d", index);
	WritePrivateProfileStringA(ENTRY_NAME, "DEVICE_INDEX", indexStr, m_iniPath);

	time_t now = time(NULL);
	CString nowStr;
	nowStr.Format("%lu", now);
	WritePrivateProfileStringA(ENTRY_NAME, "LAST_DEVICE_INDEX_TIME", nowStr, m_iniPath);

	return true;
}

LPCTSTR CGuardianConfig::ToIniString()
{
	CString buf;

	m_msgBuf = "[UTV_brwClient2]";
	m_msgBuf += "\r\n";
	buf.Format("UsePosition=%d\r\n", m_use_pos);
	m_msgBuf += buf;

	m_msgBuf += "[";
	m_msgBuf += ENTRY_NAME;
	m_msgBuf += "]\r\n";

	buf.Format("IP=%s\r\n", m_ip);
	m_msgBuf += buf;

	buf.Format("title=%s\r\n", m_title);
	m_msgBuf += buf;

	buf.Format("subTitle=%s\r\n", m_subTitle);
	m_msgBuf += buf;

	buf.Format("noticeTitle=%s\r\n", m_noticeTitle);
	m_msgBuf += buf;

	buf.Format("alarmTitle=%s\r\n", m_alarmTitle);
	m_msgBuf += buf;

	buf.Format("alarmSec=%d\r\n", m_alarmValidSec);
	m_msgBuf += buf;

	buf.Format("CHANGE_IP_TIME=%lu\r\n", m_change_ip_time);
	m_msgBuf += buf;

	buf.Format("DEVICE_INDEX=%d\r\n", m_device_index);
	m_msgBuf += buf;

	buf.Format("LAST_DEVICE_INDEX_TIME=%lu\r\n", m_last_device_index_time);
	m_msgBuf += buf;

	buf.Format("FACEAPI=%s\r\n", m_faceApi);
	m_msgBuf += buf;

	buf.Format("FACEAPITYPE=%d\r\n", m_faceApiType);
	m_msgBuf += buf;

	buf.Format("USE_FACE=%d\r\n", m_use_face);
	m_msgBuf += buf;

	buf.Format("IS_WIDE=%d\r\n", m_isWide);
	m_msgBuf += buf;

	buf.Format("USE_QR=%d\r\n", m_useQR);
	m_msgBuf += buf;

	buf.Format("SHOW_NAME=%d\r\n", m_show_name);
	m_msgBuf += buf;

	buf.Format("RECORD_ALL=%d\r\n", m_record_all);
	m_msgBuf += buf;

	buf.Format("FACE_SCORE_LIMIT=%f\r\n", m_face_score_limit);
	m_msgBuf += buf;

	buf.Format("USE_WEATHER=%d\r\n", m_use_weather);
	m_msgBuf += buf;

	buf.Format("AIR_MEASUR_STATION=%s\r\n", m_airMeasurStation);
	m_msgBuf += buf;

	buf.Format("USE_MASK_CHECK=%d\r\n", m_use_mask_check);
	m_msgBuf += buf;

	buf.Format("HIDEN_CAMERA=%d\r\n", m_is_hiden_camera);
	m_msgBuf += buf;

	if (!m_is_hiden_camera) {
		buf.Format("VIDEO_BELOW=%d\r\n", m_videoBelow);
	}
	else {
		buf.Format("VIDEO_BELOW=%d\r\n", 0);
	}
	m_msgBuf += buf;

	return m_msgBuf;
}

