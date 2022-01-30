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
#include "common.h"
#include "CoxConfig.h"
#include <io.h>                // skpark

#ifdef _COP_UTV_
#define TraceLog(x)
#else
#include "TraceLog.h" //skpark
#endif



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CoxConfig 

/*********************************************************
Function:    CoxConfig
Desc:        Constructor
Input:    none
Output:    none
Return:    none
**********************************************************/
void CoxConfig::_Init()
{
	m_iniPath = UBC_CONFIG_PATH;
	m_iniPath += UBCBRW_INI;

	m_use_pos = true;
	m_record_all = false;
	m_use_mask_check = true;
	m_not_use_encrypt = false;
	m_is_normal = true;
	m_alarmValidSec = 15;

	m_wait_time = 10;
	m_valid_event_limit = VALID_EVENT_LIMIT;
	m_tipping_point = TIPPING_POINT;

	m_same_face_confidence = SAME_FACE_CONFIDENCE;

	Read(m_iniPath);
}

CoxConfig::CoxConfig()
{
	TraceLog((_T("CoxConfig::Constructor()")));
	_Init();
	Read(m_iniPath);
}
CoxConfig::CoxConfig(LPCTSTR iniPath)
{
	TraceLog((_T("CoxConfig::Constructor(%s)"), iniPath));
	_Init();

	Read(m_iniPath);
}

BOOL CoxConfig::Read(LPCTSTR a_iniPath)
{
	TraceLog((_T("CoxConfig::Read()")));
	return _Read(a_iniPath);
	/*
	FILE* pFile = _wfopen(a_iniPath, _T("rb"));
	if (!pFile)
	{
		TraceLog((_T("(%s) file read error "), a_iniPath));
		return false;
	}

	DWORD dwFileLen = _filelength(_fileno(pFile));
	BYTE* bBuff = new BYTE[dwFileLen];

	// 파일 읽어서 버퍼에 저장
	fread(bBuff, 1, dwFileLen, pFile);
	fclose(pFile);

	// 파일은 Ansi 로 저장되므로, WCHAR 로 다시 전환함.
	wchar_t*  unicodeStr = ConvertCtoWC((const char*)bBuff);
	bool retval = this->FromString(unicodeStr, true);

	delete[] bBuff;
	delete[] unicodeStr;

	return retval;
	*/
}

BOOL CoxConfig::_Read(LPCTSTR a_iniPath)
{
	TCHAR buf[4096];

	memset((void*)buf, 0x00, sizeof(buf));
	GetPrivateProfileString(_T("UTV_brwClient2"), _T("UsePosition"), _T("1"), buf, 4096, a_iniPath);
	m_use_pos = bool(_wtoi(buf));

	// title
	memset(buf, 0x00, sizeof(buf));
	GetPrivateProfileString(ENTRY_NAME, _T("title"), _T("SQISOFT"), buf, 4096, a_iniPath);
	m_title.text = buf;

	memset(buf, 0x00, sizeof(buf));
	GetPrivateProfileString(ENTRY_NAME, _T("title.font"), _T(""), buf, 4096, a_iniPath);
	if (wcslen(buf) > 0) m_title.font = buf;

	memset(buf, 0x00, sizeof(buf));
	GetPrivateProfileString(ENTRY_NAME, _T("title.fontSize"), _T(""), buf, 4096, a_iniPath);
	if (wcslen(buf) > 0) m_title.fontSize = _wtoi(buf);

	memset(buf, 0x00, sizeof(buf));
	GetPrivateProfileString(ENTRY_NAME, _T("title.fgColor"), _T(""), buf, 4096, a_iniPath);
	if (wcslen(buf) > 0) m_title.fgColor = hex2rgb(buf);

	// subTitle
	memset(buf, 0x00, sizeof(buf));
	GetPrivateProfileString(ENTRY_NAME, _T("subTitle"), _T("발열 셀프체크 키오스크"), buf, 4096, a_iniPath);
	m_subTitle.text = buf;

	memset(buf, 0x00, sizeof(buf));
	GetPrivateProfileString(ENTRY_NAME, _T("subTitle.font"), _T(""), buf, 4096, a_iniPath);
	if (wcslen(buf) > 0) m_subTitle.font = buf;

	memset(buf, 0x00, sizeof(buf));
	GetPrivateProfileString(ENTRY_NAME, _T("subTitle.fontSize"), _T(""), buf, 4096, a_iniPath);
	if (wcslen(buf) > 0) m_subTitle.fontSize = _wtoi(buf);

	memset(buf, 0x00, sizeof(buf));
	GetPrivateProfileString(ENTRY_NAME, _T("subTitle.fgColor"), _T(""), buf, 4096, a_iniPath);
	if (wcslen(buf) > 0) m_subTitle.fgColor = hex2rgb(buf);

	// noticeTitle
	memset(buf, 0x00, sizeof(buf));
	GetPrivateProfileString(ENTRY_NAME, _T("noticeTitle"), _T("공지사항"), buf, 4096, a_iniPath);
	m_noticeTitle.text = buf;

	memset(buf, 0x00, sizeof(buf));
	GetPrivateProfileString(ENTRY_NAME, _T("noticeTitle.font"), _T(""), buf, 4096, a_iniPath);
	if (wcslen(buf) > 0) m_noticeTitle.font = buf;

	memset(buf, 0x00, sizeof(buf));
	GetPrivateProfileString(ENTRY_NAME, _T("noticeTitle.fontSize"), _T(""), buf, 4096, a_iniPath);
	if (wcslen(buf) > 0) m_noticeTitle.fontSize = _wtoi(buf);

	memset(buf, 0x00, sizeof(buf));
	GetPrivateProfileString(ENTRY_NAME, _T("noticeTitle.fgColor"), _T(""), buf, 4096, a_iniPath);
	if (wcslen(buf) > 0) m_noticeTitle.fgColor = hex2rgb(buf);

	// alarmTitle
	memset(buf, 0x00, sizeof(buf));
	GetPrivateProfileString(ENTRY_NAME, _T("alarmTitle"), _T("!WARNING!"), buf, 4096, a_iniPath);
	m_alarmTitle.text = buf;

	memset(buf, 0x00, sizeof(buf));
	GetPrivateProfileString(ENTRY_NAME, _T("alarmTitle.font"), _T(""), buf, 4096, a_iniPath);
	if (wcslen(buf) > 0) m_alarmTitle.font = buf;

	memset(buf, 0x00, sizeof(buf));
	GetPrivateProfileString(ENTRY_NAME, _T("alarmTitle.fontSize"), _T(""), buf, 4096, a_iniPath);
	if (wcslen(buf) > 0) m_alarmTitle.fontSize = _wtoi(buf);

	memset(buf, 0x00, sizeof(buf));
	GetPrivateProfileString(ENTRY_NAME, _T("alarmTitle.fgColor"), _T(""), buf, 4096, a_iniPath);
	if (wcslen(buf) > 0) m_alarmTitle.fgColor = hex2rgb(buf);

	memset(buf, 0x00, sizeof(buf));
	GetPrivateProfileString(ENTRY_NAME, _T("alarmSec"), _T("15"), buf, 4096, a_iniPath);
	m_alarmValidSec = _wtoi(buf);

	memset(buf, 0x00, sizeof(buf));
	GetPrivateProfileString(ENTRY_NAME, _T("RECORD_ALL"), _T("0"), buf, 4096, a_iniPath);
	m_record_all = bool(_wtoi(buf));

	memset(buf, 0x00, sizeof(buf));
	GetPrivateProfileString(ENTRY_NAME, _T("USE_MASK_CHECK"), _T("1"), buf, 4096, a_iniPath);
	m_use_mask_check = bool(_wtoi(buf));

	memset(buf, 0x00, sizeof(buf));
	GetPrivateProfileString(ENTRY_NAME, _T("NOT_USE_ENCRYPT"), _T("0"), buf, 4096, a_iniPath);
	m_not_use_encrypt = bool(_wtoi(buf));

	memset(buf, 0x00, sizeof(buf));
	GetPrivateProfileString(ENTRY_NAME, _T("IS_NORMAL"), _T("1"), buf, 4096, a_iniPath);
	m_is_normal = bool(_wtoi(buf));

	memset(buf, 0x00, sizeof(buf));
	GetPrivateProfileString(ENTRY_NAME, _T("TIPPING_POINT"), _T("37.5"), buf, 4096, a_iniPath);
	m_tipping_point = _wtof(buf);

	memset(buf, 0x00, sizeof(buf));
	GetPrivateProfileString(ENTRY_NAME, _T("SAME_FACE_CONFIDENCE"), _T("0.9"), buf, 4096, a_iniPath);
	m_same_face_confidence = _wtof(buf);

	memset(buf, 0x00, sizeof(buf));
	GetPrivateProfileString(ENTRY_NAME, _T("WAIT_TIME"), _T("10"), buf, 4096, a_iniPath);
	m_wait_time = _wtoi(buf);

	memset(buf, 0x00, sizeof(buf));
	GetPrivateProfileString(ENTRY_NAME, _T("VALID_EVENT_LIMIT"), _T("10"), buf, 4096, a_iniPath);
	m_valid_event_limit = _wtoi(buf);
	
	return TRUE;
}

BOOL CoxConfig::FromString(LPCTSTR fromStr, bool createFile)
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


		clsFile.Write(fromStr, wcslen(fromStr));
		clsFile.Close();
	}
	_Read(a_iniPath);
	return TRUE;
}



BOOL CoxConfig::Write()
{
	TraceLog((_T("Write()")));
	/*
	// Unicode 를 Ansi로 변경해서 저장한다.
	CString buf = ToIniString();
	char* ansiStr = ConvertWCtoC(buf);

	FILE* pFile = _wfopen(m_iniPath, _T("wb"));
	fwrite(ansiStr, 1, strlen(ansiStr), pFile);
	fclose(pFile);
	delete[] ansiStr;
	*/
	CString buf;
	buf.Format(_T("%d"), m_use_pos);
	WritePrivateProfileString(_T("UTV_brwClient2"), _T("UsePosition"), buf, m_iniPath);

	buf = m_title.text;
	WritePrivateProfileString(ENTRY_NAME, _T("title"), buf, m_iniPath);
	buf = m_title.font;
	WritePrivateProfileString(ENTRY_NAME, _T("title.font"), buf, m_iniPath);
	buf.Format(_T("%d"), m_title.fontSize);
	WritePrivateProfileString(ENTRY_NAME, _T("title.fontSize"), buf, m_iniPath);
	buf = rgb2hex(m_title.fgColor).c_str();
	WritePrivateProfileString(ENTRY_NAME, _T("title.fgColor"), buf, m_iniPath);

	buf = m_subTitle.text;
	WritePrivateProfileString(ENTRY_NAME, _T("subTitle"), buf, m_iniPath);
	buf = m_subTitle.font;
	WritePrivateProfileString(ENTRY_NAME, _T("subTitle.font"), buf, m_iniPath);
	buf.Format(_T("%d"), m_subTitle.fontSize);
	WritePrivateProfileString(ENTRY_NAME, _T("subTitle.fontSize"), buf, m_iniPath);
	buf = rgb2hex(m_subTitle.fgColor).c_str();
	WritePrivateProfileString(ENTRY_NAME, _T("subTitle.fgColor"), buf, m_iniPath);

	buf = m_noticeTitle.text;
	WritePrivateProfileString(ENTRY_NAME, _T("noticeTitle"), buf, m_iniPath);
	buf = m_noticeTitle.font;
	WritePrivateProfileString(ENTRY_NAME, _T("noticeTitle.font"), buf, m_iniPath);
	buf.Format(_T("%d"), m_noticeTitle.fontSize);
	WritePrivateProfileString(ENTRY_NAME, _T("noticeTitle.fontSize"), buf, m_iniPath);
	buf = rgb2hex(m_noticeTitle.fgColor).c_str();
	WritePrivateProfileString(ENTRY_NAME, _T("noticeTitle.fgColor"), buf, m_iniPath);

	buf = m_alarmTitle.text;
	WritePrivateProfileString(ENTRY_NAME, _T("alarmTitle"), buf, m_iniPath);
	buf = m_alarmTitle.font;
	WritePrivateProfileString(ENTRY_NAME, _T("alarmTitle.font"), buf, m_iniPath);
	buf.Format(_T("%d"), m_alarmTitle.fontSize);
	WritePrivateProfileString(ENTRY_NAME, _T("alarmTitle.fontSize"), buf, m_iniPath);
	buf = rgb2hex(m_alarmTitle.fgColor).c_str();
	WritePrivateProfileString(ENTRY_NAME, _T("alarmTitle.fgColor"), buf, m_iniPath);

	buf.Format(_T("%d"), m_alarmValidSec);
	WritePrivateProfileString(ENTRY_NAME, _T("alarmSec"), buf, m_iniPath);
	
	buf.Format(_T("%d"), m_record_all);
	WritePrivateProfileString(ENTRY_NAME, _T("RECORD_ALL"), buf, m_iniPath);

	buf.Format(_T("%d"), m_use_mask_check);
	WritePrivateProfileString(ENTRY_NAME, _T("USE_MASK_CHECK"), buf, m_iniPath);

	buf.Format(_T("%d"), m_not_use_encrypt);
	WritePrivateProfileString(ENTRY_NAME, _T("NOT_USE_ENCRYPT"), buf, m_iniPath);

	buf.Format(_T("%d"), m_is_normal);
	WritePrivateProfileString(ENTRY_NAME, _T("IS_NORMAL"), buf, m_iniPath);

	buf.Format(_T("%2.1f"), m_tipping_point);
	WritePrivateProfileString(ENTRY_NAME, _T("TIPPING_POINT"), buf, m_iniPath);

	buf.Format(_T("%2.1f"), m_same_face_confidence);
	WritePrivateProfileString(ENTRY_NAME, _T("SAME_FACE_CONFIDENCE"), buf, m_iniPath);

	buf.Format(_T("%d"), m_wait_time);
	WritePrivateProfileString(ENTRY_NAME, _T("WAIT_TIME"), buf, m_iniPath);

	buf.Format(_T("%d"), m_valid_event_limit);
	WritePrivateProfileString(ENTRY_NAME, _T("VALID_EVENT_LIMIT"), buf, m_iniPath);

	TraceLog((_T("Write() end")));
	return TRUE;  // return TRUE unless you set the focus to a control
}

LPCTSTR CoxConfig::ToIniString()
{
	CString buf;

	m_msgBuf = _T("[UTV_brwClient2]");
	m_msgBuf += _T("\r\n");
	buf.Format(_T("UsePosition=%d\r\n"), m_use_pos);
	m_msgBuf += buf;

	buf.Format(_T("title=%s\r\n"), m_title.text);
	m_msgBuf += buf;
	buf.Format(_T("title.font=%s\r\n"), m_title.font);
	m_msgBuf += buf;
	buf.Format(_T("title.fontSize=%d\r\n"), m_title.fontSize);
	m_msgBuf += buf;
	buf.Format(_T("title.fgColor=%s\r\n"), rgb2hex(m_title.fgColor).c_str());
	m_msgBuf += buf;

	buf.Format(_T("subTitle=%s\r\n"), m_subTitle.text);
	m_msgBuf += buf;
	buf.Format(_T("subTitle.font=%s\r\n"), m_subTitle.font);
	m_msgBuf += buf;
	buf.Format(_T("subTitle.fontSize=%d\r\n"), m_subTitle.fontSize);
	m_msgBuf += buf;
	buf.Format(_T("subTitle.fgColor=%s\r\n"), rgb2hex(m_subTitle.fgColor).c_str());
	m_msgBuf += buf;

	buf.Format(_T("noticeTitle=%s\r\n"), m_noticeTitle.text);
	m_msgBuf += buf;
	buf.Format(_T("noticeTitle.font=%s\r\n"), m_noticeTitle.font);
	m_msgBuf += buf;
	buf.Format(_T("noticeTitle.fontSize=%d\r\n"), m_noticeTitle.fontSize);
	m_msgBuf += buf;
	buf.Format(_T("noticeTitle.fgColor=%s\r\n"), rgb2hex(m_noticeTitle.fgColor).c_str());
	m_msgBuf += buf;

	buf.Format(_T("alarmTitle=%s\r\n"), m_alarmTitle.text);
	m_msgBuf += buf;
	buf.Format(_T("alarmTitle.font=%s\r\n"), m_alarmTitle.font);
	m_msgBuf += buf;
	buf.Format(_T("alarmTitle.fontSize=%d\r\n"), m_alarmTitle.fontSize);
	m_msgBuf += buf;
	buf.Format(_T("alarmTitle.fgColor=%s\r\n"), rgb2hex(m_alarmTitle.fgColor).c_str());
	m_msgBuf += buf;


	m_msgBuf += _T("[");
	m_msgBuf += ENTRY_NAME;
	m_msgBuf += _T("]\r\n");

	buf.Format(_T("alarmSec=%d\r\n"), m_alarmValidSec);
	m_msgBuf += buf;

	buf.Format(_T("RECORD_ALL=%d\r\n"), m_record_all);
	m_msgBuf += buf;

	buf.Format(_T("USE_MASK_CHECK=%d\r\n"), m_use_mask_check);
	m_msgBuf += buf;

	buf.Format(_T("NOT_USE_ENCRYPT=%d\r\n"), m_not_use_encrypt);
	m_msgBuf += buf;

	buf.Format(_T("IS_NORMAL=%d\r\n"), m_is_normal);
	m_msgBuf += buf;

	buf.Format(_T("TIPPING_POINT=%2.1f\r\n"), m_tipping_point);
	m_msgBuf += buf;

	buf.Format(_T("SAME_FACE_CONFIDENCE=%2.1f\r\n"), m_same_face_confidence);
	m_msgBuf += buf;

	buf.Format(_T("WAIT_TIME=%d\r\n"), m_wait_time);
	m_msgBuf += buf;

	buf.Format(_T("VALID_EVENT_LIMIT=%d\r\n"), m_valid_event_limit);
	m_msgBuf += buf;

	return m_msgBuf;
}

void CoxConfig::Copy(CoxConfig* that)
{
	m_alarmValidSec			= that->m_alarmValidSec;
	m_title						= that->m_title;
	m_subTitle					= that->m_subTitle;
	m_noticeTitle				= that->m_noticeTitle;
	m_alarmTitle					= that->m_alarmTitle;
	m_use_pos					= that->m_use_pos;
	m_record_all					= that->m_record_all;
	m_use_mask_check = that->m_use_mask_check;
	m_not_use_encrypt = that->m_not_use_encrypt;
	m_is_normal					= that->m_is_normal;
	m_tipping_point = that->m_tipping_point;
	m_wait_time = that->m_wait_time;
	m_valid_event_limit = that->m_valid_event_limit;
	m_same_face_confidence = that->m_same_face_confidence;
}