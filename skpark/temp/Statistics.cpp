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
#ifdef _COP_UTV_
#define TraceLog(x)
#define UBCBRW_INI				_T("UBCBrowser.ini")
#define UBC_CONFIG_PATH	"C:\\SQISoft\\UTV1.0\\execute\\data\\"
#define		PROPERTY_DELI1		"###"
#define		PROPERTY_DELI2		"+++"
#else
#include "TraceLog.h" //skpark
#endif

#define MIN_LARGE_NUMBER 30
#define MAX_LARGE_NUMBER 99
#define ENTRY_NAME	"GUARDIAN_STAT"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
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
	m_iniPath = iniPath;
	Init();
	Read();
}

void CStatistics::Init()
{
	m_use = FALSE;
	m_avg = m_prev_avg = 36.5f;
	m_threshold  =  0.8f;
	m_startTime = CTime::GetCurrentTime();
	m_min_large_number = MIN_LARGE_NUMBER;
	m_max_large_number = MAX_LARGE_NUMBER;
	m_sample_size = m_min_large_number;

	m_alarmTemperature = 37.5f;
	m_alertMinTemperature = 30.0f;

	m_manualCalibration = 0.4f;
	m_compensationType = "auto";

	m_lastApiInvokeTime = 0;
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
	TraceLog(("IsChanged(%f ? %f)", m_prev_avg, m_avg))
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
	char buf[4096];

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME, "USE_STAT", "0", buf, 4096, m_iniPath);
	m_use = atoi(buf);

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME, "AVG", "36.5", buf, 4096, m_iniPath);
	m_avg = atof(buf);

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME, "MIN_LARGE_NUMBER", "30", buf, 4096, m_iniPath);
	m_min_large_number = atoi(buf);

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME, "MAX_LARGE_NUMBER", "99", buf, 4096, m_iniPath);
	m_max_large_number = atoi(buf);

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME, "SAMPLE_SIZE", "1", buf, 4096, m_iniPath);
	m_sample_size = atoi(buf);
	if (m_sample_size < m_min_large_number)
	{
		m_sample_size = m_min_large_number;
	}



	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME, "THRESHOLD", "0.8", buf, 4096, m_iniPath);
	m_threshold = atof(buf);

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME, "STARTDATE", "", buf, 4096, m_iniPath);
	if (strlen(buf) > 0)
	{
		CString strDateTime = buf;     //  _T("2008/11/12 14:54:00");
		int nYear = atoi(strDateTime.Left(4));
		int nMon = atoi(strDateTime.Mid(5, 2));
		int nDay = atoi(strDateTime.Mid(8, 2));
		int nHour = atoi(strDateTime.Mid(11, 2));
		int nMin = atoi(strDateTime.Mid(14, 2));
		int nSec = atoi(strDateTime.Mid(17, 2));
		m_startTime = CTime(nYear, nMon, nDay, nHour, nMin, nSec);
	}

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME, "ALARM_TEMPERATURE", "37.5", buf, 4096, m_iniPath);
	m_alarmTemperature = atof(buf);

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME, "ALERT_MIN_TEMPERATURE", "30.0", buf, 4096, m_iniPath);
	m_alertMinTemperature = atof(buf);

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME, "COMPENSATION_TYPE", "auto", buf, 4096, m_iniPath);
	m_compensationType.Format(_T("%s"), buf);

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME, "MANUAL_CALIBRATION", "0.4", buf, 4096, m_iniPath);
	m_manualCalibration = atof(buf);

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME, "LAST_API_INVOKE_TIME", "0", buf, 4096, m_iniPath);
	m_lastApiInvokeTime = strtoul(buf,NULL,10);
	return TRUE;
}

BOOL CStatistics::FromString(LPCTSTR fromStr, bool createFile)
{

	CString a_iniPath;
	a_iniPath = UBC_CONFIG_PATH;
	a_iniPath += "temp_browser.ini";

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


		clsFile.Write(fromStr, strlen(fromStr));
		clsFile.Close();
	}


	char buf[4096];

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME, "USE_STAT", "0", buf, 4096, a_iniPath);
	m_use = atoi(buf);

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME, "AVG", "36.5", buf, 4096, a_iniPath);
	m_avg = atof(buf);

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME, "MIN_LARGE_NUMBER", "30", buf, 4096, a_iniPath);
	m_min_large_number = atoi(buf);

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME, "MAX_LARGE_NUMBER", "99", buf, 4096, a_iniPath);
	m_max_large_number = atoi(buf);

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME, "SAMPLE_SIZE", "1", buf, 4096, a_iniPath);
	m_sample_size = atoi(buf);
	if (m_sample_size < m_min_large_number)
	{
		m_sample_size = m_min_large_number;
	}

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME, "THRESHOLD", "0.8", buf, 4096, a_iniPath);
	m_threshold = atof(buf);

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME, "STARTDATE", "", buf, 4096, a_iniPath);
	if (strlen(buf) > 0)
	{
		CString strDateTime = buf;     //  _T("2008/11/12 14:54:00");
		int nYear = atoi(strDateTime.Left(4));
		int nMon = atoi(strDateTime.Mid(5, 2));
		int nDay = atoi(strDateTime.Mid(8, 2));
		int nHour = atoi(strDateTime.Mid(11, 2));
		int nMin = atoi(strDateTime.Mid(14, 2));
		int nSec = atoi(strDateTime.Mid(17, 2));
		m_startTime = CTime(nYear, nMon, nDay, nHour, nMin, nSec);
	}

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME, "ALARM_TEMPERATURE", "37.5", buf, 4096, a_iniPath);
	m_alarmTemperature = atof(buf);

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME, "ALERT_MIN_TEMPERATURE", "30.0", buf, 4096, a_iniPath);
	m_alertMinTemperature = atof(buf);

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME, "COMPENSATION_TYPE", "auto", buf, 4096, a_iniPath);
	m_compensationType.Format(_T("%s"), buf);

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME, "MANUAL_CALIBRATION", "0.4", buf, 4096, a_iniPath);
	m_manualCalibration = atof(buf);

	memset(buf, 0x00, 4096);
	GetPrivateProfileString(ENTRY_NAME, "LAST_API_INVOKE_TIME", "0", buf, 4096, a_iniPath);
	m_lastApiInvokeTime = strtoul(buf, NULL, 10);
	
	return TRUE;
}

BOOL CStatistics::Write()
{
	TraceLog(("Write()"));
	CString buf;

	buf.Format("%f", m_avg);
	WritePrivateProfileStringA(ENTRY_NAME, "AVG", buf, m_iniPath);

	buf.Format("%lu", m_sample_size);
	WritePrivateProfileString(ENTRY_NAME, "SAMPLE_SIZE", buf, m_iniPath);

	buf.Format("%lu", m_min_large_number);
	WritePrivateProfileString(ENTRY_NAME, "MIN_LARGE_NUMBER", buf, m_iniPath);

	buf.Format("%lu", m_max_large_number);
	WritePrivateProfileString(ENTRY_NAME, "MAX_LARGE_NUMBER", buf, m_iniPath);

	buf.Format("%f", m_threshold);
	WritePrivateProfileString(ENTRY_NAME, "THRESHOLD",  buf, m_iniPath);

	buf.Format("%d", m_use);
	WritePrivateProfileString(ENTRY_NAME, "USE_STAT", buf, m_iniPath);


	buf.Format("%04d/%02d/%02d %02d:%02d:%02d",
		m_startTime.GetYear(), m_startTime.GetMonth(), m_startTime.GetDay(),
		m_startTime.GetHour(), m_startTime.GetMinute(), m_startTime.GetSecond());
	//buf = m_startTime.Format(_T("%04Y/%02m/%02d %02H:%02M:%02S"));
	WritePrivateProfileString(ENTRY_NAME, "STARTDATE", buf, m_iniPath);

	//buf.Format("%.1f", m_alarmTemperature);
	//WritePrivateProfileString(ENTRY_NAME, "ALARM_TEMPERATURE", buf, m_iniPath);

	//buf.Format("%.1f", m_alertMinTemperature);
	//WritePrivateProfileString(ENTRY_NAME, "ALERT_MIN_TEMPERATURE", buf, m_iniPath);

	//buf.Format("%s", m_compensationType);
	//WritePrivateProfileString(ENTRY_NAME, "COMPENSATION_TYPE", buf, m_iniPath);

	//buf.Format("%.1f", m_manualCalibration);
	//WritePrivateProfileString(ENTRY_NAME, "MANUAL_CALIBRATION", buf, m_iniPath);

	//buf.Format("%lu", m_lastApiInvokeTime);
	//WritePrivateProfileString(ENTRY_NAME, "LAST_API_INVOKE_TIME", buf, m_iniPath);
	TraceLog(("Write() end"));

	return TRUE;
}

CString CStatistics::WriteISAPIInfo(float alarmTemperature, float alertMinTemperature, LPCTSTR compensationType, float manualCalibration)
{
	CString buf;
	CString test = (m_iniPath + " : ");

	m_alarmTemperature = alarmTemperature;
	buf.Format("%.1f", m_alarmTemperature);
	WritePrivateProfileString(ENTRY_NAME, "ALARM_TEMPERATURE", buf, m_iniPath);
	test += (buf + ", ");

	m_alertMinTemperature = alertMinTemperature;
	buf.Format("%.1f", m_alertMinTemperature);
	WritePrivateProfileString(ENTRY_NAME, "ALERT_MIN_TEMPERATURE", buf, m_iniPath);
	test += (buf + ", ");

	m_compensationType = compensationType;
	buf.Format("%s", m_compensationType);
	WritePrivateProfileString(ENTRY_NAME, "COMPENSATION_TYPE", buf, m_iniPath);
	test += (buf + ", ");

	m_manualCalibration = manualCalibration;
	buf.Format("%.1f", m_manualCalibration);
	WritePrivateProfileString(ENTRY_NAME, "MANUAL_CALIBRATION", buf, m_iniPath);
	test += buf;

	time_t now = time(NULL);
	this->m_lastApiInvokeTime = now;
	buf.Format("%lu", m_lastApiInvokeTime);
	WritePrivateProfileString(ENTRY_NAME, "LAST_API_INVOKE_TIME", buf, m_iniPath);
	test += buf;

	return test;
}

float CStatistics::AddSample(float newSample)
{
	TraceLog(("AddSample(%f,%d)", newSample, m_sample_size));
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
	m_msgBuf = "[";
	m_msgBuf += ENTRY_NAME;
	m_msgBuf += "]\r\n";
	CString buf;
	buf.Format("AVG=%f\r\n", this->m_avg);
	m_msgBuf += buf;
	buf.Format("SAMPLE_SIZE=%lu\r\n", this->m_sample_size);
	m_msgBuf += buf;
	buf.Format("THRESHOLD=%f\r\n", this->m_threshold);
	m_msgBuf += buf;
	buf.Format("STARTDATE=%04d/%02d/%02d %02d:%02d:%02d\r\n",
		m_startTime.GetYear(), m_startTime.GetMonth(), m_startTime.GetDay(),
		m_startTime.GetHour(), m_startTime.GetMinute(), m_startTime.GetSecond());
	m_msgBuf += buf;
	buf.Format("USE_STAT=%d\r\n", this->m_use);
	m_msgBuf += buf;
	buf.Format("MIN_LARGE_NUMBER=%lu\r\n", this->m_min_large_number);
	m_msgBuf += buf;
	buf.Format("MAX_LARGE_NUMBER=%lu\r\n", this->m_max_large_number);
	m_msgBuf += buf;
	buf.Format("ALARM_TEMPERATURE=%.1f\r\n", this->m_alarmTemperature);
	m_msgBuf += buf;
	buf.Format("ALERT_MIN_TEMPERATURE=%.1f\r\n", this->m_alertMinTemperature);
	m_msgBuf += buf;

	buf.Format("COMPENSATION_TYPE=%s\r\n", this->m_compensationType);
	m_msgBuf += buf;
	buf.Format("MANUAL_CALIBRATION=%.1f\r\n", this->m_manualCalibration);
	m_msgBuf += buf;
	buf.Format("LAST_API_INVOKE_TIME=%lu\r\n", this->m_lastApiInvokeTime);
	m_msgBuf += buf;

	return m_msgBuf;
}
LPCTSTR CStatistics::ToString(CString localeLang)
{
	if ("ko" == localeLang) {
		m_msgBuf.Format("평균:%.1f , 비정상온도:%.1f , 샘플:%lu", 
			this->m_avg, this->m_avg + this->m_threshold, this->m_sample_size);
	}
	else {
		m_msgBuf.Format("Average:%.1f  Abnormal:%.1f   Sample Count:%lu", 
			this->m_avg, this->m_avg + this->m_threshold, this->m_sample_size);
	}
	return m_msgBuf;
}

bool CStatistics::SendEvent()
{

	HWND hwnd = ::FindWindow(NULL, "Guardian2.0");
	if(hwnd != NULL ) 
	{
		//MY_DEBUG("PostMessage(Guardian2.0) >>> [UM_REFRESH_GUARDIAN_SETTINGS]");
		//::PostMessage(hwnd, UM_REFRESH_GUARDIAN_SETTINGS, 0, 0);

		CString sendData;
		sendData.Format("%.1f,%.1f,%s,%.1f", 
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