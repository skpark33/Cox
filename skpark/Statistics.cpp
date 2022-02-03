/**********************************************************
FileName:    Statistics.cpp
Description: set and remove guard      
Date:        2008/11/19
Note:          <global>struct, define refer to GeneralDef.h, global variables and functions refer to ClientDemo.cpp    
Modification History:      
    <version> <time>         <desc>
    <1.0    > <2008/11/19>       <created>
***********************************************************/

#include "stdafx.h"
#include "Statistics.h"
#include "common.h"
#ifdef _COP_UTV_
#define TraceLog(x)
#else
#include "TraceLog.h" //skpark
#endif


#define MIN_LARGE_NUMBER 30
#define MAX_LARGE_NUMBER 9999
#define ENTRY_NAME_T	 _T("GUARDIAN_STAT")

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifndef UM_REFRESH_GUARDIAN_SETTINGS
#define UM_REFRESH_GUARDIAN_SETTINGS		(WM_USER+301)
#endif

/////////////////////////////////////////////////////////////////////////////
// CStatistics 

/*********************************************************
  Function:    CStatistics
  Desc:        Constructor
  Input:    none
  Output:    none
  Return:    none
**********************************************************/
CStatistics::CStatistics()
{
	m_iniPath = UBC_CONFIG_PATH;
	m_iniPath += UBCBRW_INI;
	Init();
	Read();
}
CStatistics::CStatistics(LPCTSTR iniPath)
{
	TraceLog((_T("CGuardianConfig::Constructor()")));

	m_iniPath = iniPath;
	Init();
	Read();
}

void CStatistics::Init()
{
	m_use = FALSE;
	m_avg = m_prev_avg = 36.5f;
	m_threshold  =  1.0f;
	m_startTime = CTime::GetCurrentTime();
	m_min_large_number = MIN_LARGE_NUMBER;
	m_max_large_number = MAX_LARGE_NUMBER;
	m_sample_size = m_min_large_number;

	m_alarmTemperature = 37.5f;
	m_alertMinTemperature = 30.0f;

	m_manualCalibration = 0.4f;
	m_compensationType = "auto";

	m_lastApiInvokeTime = 0;
	m_apiChanged = false;
}

void CStatistics::InitCalc() 
{  // 평균을 내는 원점을 다시 시작한다. 이때 돗수는 30에서 시작한다.  
	m_sample_size = m_min_large_number;
	m_startTime = CTime::GetCurrentTime();
}
BOOL CStatistics::IsValid() 
{ 
	return (m_sample_size >= m_min_large_number ? TRUE : FALSE);
}
BOOL CStatistics::IsChanged() 
{
	TraceLog((_T("IsChanged(%f ? %f)"), m_prev_avg, m_avg))
	if (m_avg != m_prev_avg)
	{
		m_prev_avg = m_avg;
		Write();
		return TRUE;
	}
	return FALSE;
}
BOOL CStatistics::Read()
{
	TraceLog((_T("CStatistics::Read()")));

	TCHAR buf[4096];

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME_T, _T("USE_STAT"), _T("0"), buf, 4096, m_iniPath);
	m_use = _wtoi(buf);

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME_T, _T("AVG"), _T("36.5"), buf, 4096, m_iniPath);
	m_avg = _wtof(buf);

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME_T, _T("MIN_LARGE_NUMBER"), _T("30"), buf, 4096, m_iniPath);
	m_min_large_number = _wtoi(buf);

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME_T, _T("MAX_LARGE_NUMBER"), _T("9999"), buf, 4096, m_iniPath);
	m_max_large_number = _wtoi(buf);

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME_T, _T("SAMPLE_SIZE"), _T("1"), buf, 4096, m_iniPath);
	m_sample_size = _wtoi(buf);
	if (m_sample_size < m_min_large_number)
	{
		m_sample_size = m_min_large_number;
	}



	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME_T, _T("THRESHOLD"), _T("1.0"), buf, 4096, m_iniPath);
	m_threshold = _wtof(buf);

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME_T, _T("STARTDATE"), _T(""), buf, 4096, m_iniPath);
	if (wcslen(buf) > 0)
	{
		CString strDateTime = buf;     //  _T("2008/11/12 14:54:00");
		int nYear = _wtoi(strDateTime.Left(4));
		int nMon = _wtoi(strDateTime.Mid(5, 2));
		int nDay = _wtoi(strDateTime.Mid(8, 2));
		int nHour = _wtoi(strDateTime.Mid(11, 2));
		int nMin = _wtoi(strDateTime.Mid(14, 2));
		int nSec = _wtoi(strDateTime.Mid(17, 2));
		m_startTime = CTime(nYear, nMon, nDay, nHour, nMin, nSec);
	}

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME_T, _T("ALARM_TEMPERATURE"), _T("37.5"), buf, 4096, m_iniPath);
	m_alarmTemperature = _wtof(buf);

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME_T, _T("ALERT_MIN_TEMPERATURE"), _T("30.0"), buf, 4096, m_iniPath);
	m_alertMinTemperature = _wtof(buf);

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME_T, _T("COMPENSATION_TYPE"), _T("auto"), buf, 4096, m_iniPath);
	m_compensationType.Format(_T("%s"), buf);

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME_T, _T("MANUAL_CALIBRATION"), _T("0.4"), buf, 4096, m_iniPath);
	m_manualCalibration = _wtof(buf);

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME_T, _T("LAST_API_INVOKE_TIME"), _T("0"), buf, 4096, m_iniPath);
	m_lastApiInvokeTime = wcstoul(buf,NULL,10);
	return TRUE;
}

BOOL CStatistics::FromString(LPCTSTR fromStr, bool createFile)
{

	CString a_iniPath;
	a_iniPath = UBC_CONFIG_PATH;
	a_iniPath += _T("temp_browser.ini");

	if(createFile)
	{
		CFileException ex;
		CFile clsFile;
		if(!clsFile.Open(a_iniPath, CFile::modeCreate | CFile::modeWrite | CFile::typeText | CFile::shareDenyNone, &ex))
		{
			TCHAR szError[1024] = { 0x00 };
			ex.GetErrorMessage(szError, 1024);
			TRACE(szError);
			return false;
		}//if


		clsFile.Write(fromStr, wcslen(fromStr));
		clsFile.Close();
	}


	TCHAR buf[4096];

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME_T, _T("USE_STAT"), _T("0"), buf, 4096, a_iniPath);
	m_use = _wtoi(buf);

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME_T, _T("AVG"), _T("36.5"), buf, 4096, a_iniPath);
	m_avg = _wtof(buf);

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME_T, _T("MIN_LARGE_NUMBER"), _T("30"), buf, 4096, a_iniPath);
	m_min_large_number = _wtoi(buf);

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME_T, _T("MAX_LARGE_NUMBER"), _T("9999"), buf, 4096, a_iniPath);
	m_max_large_number = _wtoi(buf);

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME_T, _T("SAMPLE_SIZE"), _T("1"), buf, 4096, a_iniPath);
	m_sample_size = _wtoi(buf);
	if (m_sample_size < m_min_large_number)
	{
		m_sample_size = m_min_large_number;
	}

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME_T, _T("THRESHOLD"), _T("1.0"), buf, 4096, a_iniPath);
	m_threshold = _wtof(buf);

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME_T, _T("STARTDATE"), _T(""), buf, 4096, a_iniPath);
	if (wcslen(buf) > 0)
	{
		CString strDateTime = buf;     //  _T("2008/11/12 14:54:00");
		int nYear = _wtoi(strDateTime.Left(4));
		int nMon = _wtoi(strDateTime.Mid(5, 2));
		int nDay = _wtoi(strDateTime.Mid(8, 2));
		int nHour = _wtoi(strDateTime.Mid(11, 2));
		int nMin = _wtoi(strDateTime.Mid(14, 2));
		int nSec = _wtoi(strDateTime.Mid(17, 2));
		m_startTime = CTime(nYear, nMon, nDay, nHour, nMin, nSec);
	}

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME_T, _T("ALARM_TEMPERATURE"), _T("37.5"), buf, 4096, a_iniPath);
	m_alarmTemperature = _wtof(buf);

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME_T, _T("ALERT_MIN_TEMPERATURE"), _T("30.0"), buf, 4096, a_iniPath);
	m_alertMinTemperature = _wtof(buf);

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME_T, _T("COMPENSATION_TYPE"), _T("auto"), buf, 4096, a_iniPath);
	m_compensationType.Format(_T("%s"), buf);

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME_T, _T("MANUAL_CALIBRATION"), _T("0.4"), buf, 4096, a_iniPath);
	m_manualCalibration = _wtof(buf);

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME_T, _T("LAST_API_INVOKE_TIME"), _T("0"), buf, 4096, a_iniPath);
	m_lastApiInvokeTime = wcstoul(buf, NULL, 10);
	
	return TRUE;
}

BOOL CStatistics::Write()
{
	TraceLog((_T("Write()")));
	CString buf;

	buf.Format(_T("%f"), m_avg);
	WritePrivateProfileString(ENTRY_NAME_T, _T("AVG"), buf, m_iniPath);

	buf.Format(_T("%lu"), m_sample_size);
	WritePrivateProfileString(ENTRY_NAME_T, _T("SAMPLE_SIZE"), buf, m_iniPath);

	buf.Format(_T("%lu"), m_min_large_number);
	WritePrivateProfileString(ENTRY_NAME_T, _T("MIN_LARGE_NUMBER"), buf, m_iniPath);

	buf.Format(_T("%lu"), m_max_large_number);
	WritePrivateProfileString(ENTRY_NAME_T, _T("MAX_LARGE_NUMBER"), buf, m_iniPath);

	buf.Format(_T("%f"), m_threshold);
	WritePrivateProfileString(ENTRY_NAME_T, _T("THRESHOLD"), buf, m_iniPath);

	buf.Format(_T("%d"), m_use);
	WritePrivateProfileString(ENTRY_NAME_T, _T("USE_STAT"), buf, m_iniPath);


	buf.Format(_T("%04d/%02d/%02d %02d:%02d:%02d"),
		m_startTime.GetYear(), m_startTime.GetMonth(), m_startTime.GetDay(),
		m_startTime.GetHour(), m_startTime.GetMinute(), m_startTime.GetSecond());
	//buf = m_startTime.Format(_T("%04Y/%02m/%02d %02H:%02M:%02S")));
	WritePrivateProfileString(ENTRY_NAME_T, _T("STARTDATE"), buf, m_iniPath);

	buf.Format(_T("%.1f"), m_alarmTemperature);
	WritePrivateProfileString(ENTRY_NAME_T, _T("ALARM_TEMPERATURE"), buf, m_iniPath);

	//buf.Format("%.1f", m_alertMinTemperature);
	//WritePrivateProfileString(ENTRY_NAME_T, "ALERT_MIN_TEMPERATURE", buf, m_iniPath);

	//buf.Format("%s", m_compensationType);
	//WritePrivateProfileString(ENTRY_NAME_T, "COMPENSATION_TYPE", buf, m_iniPath);

	buf.Format(_T("%.1f"), m_manualCalibration);
	WritePrivateProfileString(ENTRY_NAME_T, _T("MANUAL_CALIBRATION"), buf, m_iniPath);

	//buf.Format("%lu", m_lastApiInvokeTime);
	//WritePrivateProfileString(ENTRY_NAME_T, "LAST_API_INVOKE_TIME", buf, m_iniPath);
	TraceLog((_T("Write() end")));

	return TRUE;
}

CString CStatistics::WriteISAPIInfo(float alarmTemperature, float alertMinTemperature, LPCTSTR compensationType, float manualCalibration)
{
	CString buf;
	CString test = (m_iniPath + _T(" : "));

	m_alarmTemperature = alarmTemperature;
	buf.Format(_T("%.1f"), m_alarmTemperature);
	WritePrivateProfileString(ENTRY_NAME_T, _T("ALARM_TEMPERATURE"), buf, m_iniPath);
	test += (buf + _T(", "));

	m_alertMinTemperature = alertMinTemperature;
	buf.Format(_T("%.1f"), m_alertMinTemperature);
	WritePrivateProfileString(ENTRY_NAME_T, _T("ALERT_MIN_TEMPERATURE"), buf, m_iniPath);
	test += (buf + _T(", "));

	m_compensationType = compensationType;
	buf.Format(_T("%s"), m_compensationType);
	WritePrivateProfileString(ENTRY_NAME_T, _T("COMPENSATION_TYPE"), buf, m_iniPath);
	test += (buf + _T(", "));

	m_manualCalibration = manualCalibration;
	buf.Format(_T("%.1f"), m_manualCalibration);
	WritePrivateProfileString(ENTRY_NAME_T, _T("MANUAL_CALIBRATION"), buf, m_iniPath);
	test += buf;

	time_t now = time(NULL);
	this->m_lastApiInvokeTime = now;
	buf.Format(_T("%lu"), m_lastApiInvokeTime);
	WritePrivateProfileString(ENTRY_NAME_T, _T("LAST_API_INVOKE_TIME"), buf, m_iniPath);
	test += buf;

	return test;
}

float CStatistics::AddSample(float newSample)
{
	TraceLog((_T("AddSample(%f,%d)"), newSample, m_sample_size));
	float f_size = static_cast<float>(m_sample_size);
	m_avg = ((m_avg*f_size) + newSample) / (f_size + 1.0f);
	m_sample_size++;
	if (m_sample_size > m_max_large_number)
	{
		m_sample_size = m_min_large_number;
	}
	return m_avg;
}

LPCTSTR CStatistics::ToIniString()
{
	m_msgBuf = _T("[");
	m_msgBuf += ENTRY_NAME_T;
	m_msgBuf += _T("]\r\n");
	CString buf;
	buf.Format(_T("AVG=%f\r\n"), this->m_avg);
	m_msgBuf += buf;
	buf.Format(_T("SAMPLE_SIZE=%lu\r\n"), this->m_sample_size);
	m_msgBuf += buf;
	buf.Format(_T("THRESHOLD=%f\r\n"), this->m_threshold);
	m_msgBuf += buf;
	buf.Format(_T("STARTDATE=%04d/%02d/%02d %02d:%02d:%02d\r\n"),
		m_startTime.GetYear(), m_startTime.GetMonth(), m_startTime.GetDay(),
		m_startTime.GetHour(), m_startTime.GetMinute(), m_startTime.GetSecond());
	m_msgBuf += buf;
	buf.Format(_T("USE_STAT=%d\r\n"), this->m_use);
	m_msgBuf += buf;
	buf.Format(_T("MIN_LARGE_NUMBER=%lu\r\n"), this->m_min_large_number);
	m_msgBuf += buf;
	buf.Format(_T("MAX_LARGE_NUMBER=%lu\r\n"), this->m_max_large_number);
	m_msgBuf += buf;
	buf.Format(_T("ALARM_TEMPERATURE=%.1f\r\n"), this->m_alarmTemperature);
	m_msgBuf += buf;
	buf.Format(_T("ALERT_MIN_TEMPERATURE=%.1f\r\n"), this->m_alertMinTemperature);
	m_msgBuf += buf;

	buf.Format(_T("COMPENSATION_TYPE=%s\r\n"), this->m_compensationType);
	m_msgBuf += buf;
	buf.Format(_T("MANUAL_CALIBRATION=%.1f\r\n"), this->m_manualCalibration);
	m_msgBuf += buf;
	buf.Format(_T("LAST_API_INVOKE_TIME=%lu\r\n"), this->m_lastApiInvokeTime);
	m_msgBuf += buf;

	return m_msgBuf;
}
LPCTSTR CStatistics::ToString(CString localeLang)
{
	if ("ko" == localeLang) {
		m_msgBuf.Format(_T("평균:%.1f , 비정상온도:%.1f , 샘플:%lu"), 
			this->m_avg, this->m_avg + this->m_threshold, this->m_sample_size);
	}
	else {
		m_msgBuf.Format(_T("Average:%.1f  Abnormal:%.1f   Sample Count:%lu"),
			this->m_avg, this->m_avg + this->m_threshold, this->m_sample_size);
	}
	return m_msgBuf;
}

bool CStatistics::SendEvent()
{

	HWND hwnd = ::FindWindow(NULL, _T("Guardian2.0"));
	if(hwnd != NULL ) 
	{
		//MY_DEBUG("PostMessage(Guardian2.0) >>> [UM_REFRESH_GUARDIAN_SETTINGS]");
		//::PostMessage(hwnd, UM_REFRESH_GUARDIAN_SETTINGS, 0, 0);

		CString sendData;
		sendData.Format(_T("%.1f,%.1f,%s,%.1f"), 
			this->m_alarmTemperature,
			this->m_alertMinTemperature,
			this->m_compensationType,
			this->m_manualCalibration);

		//MY_DEBUG("COPYDATASTRUCT(Guardian2.0) >>> %s", sendData);
		COPYDATASTRUCT appInfo;
		appInfo.dwData = UM_REFRESH_GUARDIAN_SETTINGS; // UM_REFRESH_GUARDIAN_SETTINGS;
		appInfo.lpData = (char*)(LPCTSTR)sendData;
		appInfo.cbData =sendData.GetLength();
		::SendMessage(hwnd, WM_COPYDATA, NULL, (LPARAM)&appInfo);

		return true;

	}
	return false;
}

void CStatistics::Copy(CStatistics* that)
{
	TraceLog((_T("Copy()-1")));
	m_use					= that->m_use;
	TraceLog((_T("Copy()-2")));
	m_avg					= that->m_avg;
	m_prev_avg				= that->m_prev_avg;
	m_threshold				= that->m_threshold;
	m_sample_size			= that->m_sample_size;
	m_startTime				= that->m_startTime;
	m_min_large_number		= that->m_min_large_number;
	m_max_large_number		= that->m_max_large_number;
	m_alarmTemperature		= that->m_alarmTemperature;		
	m_alertMinTemperature	= that->m_alertMinTemperature;	
	m_manualCalibration		= that->m_manualCalibration;
	m_compensationType		= that->m_compensationType;
	m_lastApiInvokeTime		= that->m_lastApiInvokeTime;
	m_apiChanged			= that->m_apiChanged;

	TraceLog((_T("Copy()-10")));
}
