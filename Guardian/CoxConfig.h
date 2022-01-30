#pragma once

#include "afxwin.h"
#include "define.h"


class MyText
{
public:
	MyText()
	{
		text = "";
		font = ALARM_FONT_1;
		fontSize = ALARM_FONT_SIZE_1;
		fgColor = ALARM_FG_COLOR_1;
	}
	MyText(LPCTSTR myText) : text(myText)
	{
		font = ALARM_FONT_1;
		fontSize = ALARM_FONT_SIZE_1;
		fgColor = ALARM_FG_COLOR_1;
	}
	MyText(LPCTSTR myText, int mySize, COLORREF myColor) : text(myText), fontSize(mySize), fgColor(myColor)
	{
		font = ALARM_FONT_1;
	}
	MyText(LPCTSTR myText, LPCTSTR myFont, int mySize, COLORREF myColor) : text(myText), fontSize(mySize), fgColor(myColor), font(myFont)
	{
	}

	CString		text;
	CString		font;
	int				fontSize;
	COLORREF  fgColor;
};

/////////////////////////////////////////////////////////////////////////////
// CoxConfig 

class CoxConfig
{
	// Construction
public:
	CoxConfig();   // standard constructor
	CoxConfig(LPCTSTR iniPath);   // standard constructor
	virtual ~CoxConfig() {};   // standard constructor

	LPCTSTR ToIniString();
	BOOL FromString(LPCTSTR fromStr, bool createFile);

	BOOL Read(LPCTSTR a_iniPath);
	BOOL Write();
	//BOOL WriteIniDeviceInfo(int index);

	void Copy(CoxConfig* that);

protected:
	void _Init();
	BOOL _Read(LPCTSTR a_iniPath);

protected:
	CString m_msgBuf;
	CString m_iniPath;

public:
	MyText m_title;
	MyText m_subTitle;
	MyText m_noticeTitle;
	MyText m_alarmTitle;

	bool	m_use_pos;
	int		m_alarmValidSec;
	bool	m_is_normal;
	bool	m_record_all;
	bool	m_use_mask_check;	// 마스크착용 사용여부

	double m_tipping_point;   // 온도 경보점
	int		 m_wait_time;	// 온도를 가져오는 간격 micro second, default 33
	bool	m_not_use_encrypt;

	double m_same_face_confidence;
	int		m_valid_event_limit;

};


