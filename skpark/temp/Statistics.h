#pragma once
#include "afxwin.h"
/////////////////////////////////////////////////////////////////////////////
// CStatistics 

class CStatistics 
{
// Construction
public:
    CStatistics();   // standard constructor
    CStatistics(LPCTSTR iniPath);   // standard constructor
	virtual ~CStatistics() {};   // standard constructor

	void Init();
	
	void InitCalc();
	BOOL IsValid();

	BOOL IsUsed() { return m_use; }
	BOOL IsChanged();

	BOOL IsAlarmCase(float temper)
	{
		return  (temper >= m_avg + m_threshold);
	}

	BOOL FromString(LPCTSTR fromStr, bool createFile);

	LPCTSTR ToString()
	{
		m_msgBuf.Format("평균:%.1f, 비정상온도:%.1f, 샘플:%lu", this->m_avg, this->m_avg + this->m_threshold, this->m_sample_size);
		return m_msgBuf;
	}
	LPCTSTR ToString(CString localeLang);

	LPCTSTR ToIniString();

	BOOL Read();
	BOOL Write();
	CString WriteISAPIInfo(float alarmTemperature, float alertMinTemperature, LPCTSTR compensationType, float manualCalibration);

	float AddSample(float newSample);

	float GetAvg() { return m_avg; }
	float GetThreshold() { return m_threshold; }
	unsigned long GetSampleSize() { return m_sample_size;  }
	CString GetStartTime() {  return m_startTime.Format(_T("%04Y/%02m/%02d %02H:%02M:%02S"));	}
	unsigned long GetMinLargetNumber() { return m_min_large_number; }
	unsigned long GetMaxLargetNumber() { return m_max_large_number; }

	bool SendEvent();

protected:
	CString m_msgBuf;
	CString m_iniPath;

public:
	bool	m_use;
	float	m_avg;
	float	m_prev_avg;
	float	m_threshold;
	unsigned long		m_sample_size;
	CTime	m_startTime;
	unsigned long	m_min_large_number;
	unsigned long	m_max_large_number;

	// http://192.168.11.96/ISAPI/Thermal/channels/1/faceThermometry => <alarmTemperature>37.50</alarmTemperature>, <alert>30.00</alert>
	float	m_alarmTemperature;		// 발열알람온도:37.50
	float	m_alertMinTemperature;	// 사전알람온도:30.00

	// http://192.168.11.96/ISAPI/Thermal/channels/2/bodyTemperatureCompensation
	float	m_manualCalibration;	// default:0.4				<smartCorrection>
	CString	m_compensationType;		// default:"true", "manual"/"auto" <temperatureCompensation>

	time_t	m_lastApiInvokeTime;

};
