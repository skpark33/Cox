#include "afxwin.h"

#pragma once
/////////////////////////////////////////////////////////////////////////////
// CGuardianConfig 

class CGuardianConfig
{
	// Construction
public:
	CGuardianConfig();   // standard constructor
	CGuardianConfig(LPCTSTR iniPath);   // standard constructor
	virtual ~CGuardianConfig() {};   // standard constructor

	LPCTSTR ToIniString();
	BOOL FromString(LPCTSTR fromStr, bool createFile);

	BOOL Read();
	BOOL Write();
	BOOL WriteIniDeviceInfo(int index);

protected:
	CString m_msgBuf;
	CString m_iniPath;

public:
	enum { FACE_API_NONE, FACE_API_SQISOFT, FACE_API_REINFORCE, FACE_API_WATCH };

	int		m_alarmValidSec;
	CString	m_ip;
	CString m_title;
	CString m_subTitle;
	CString m_noticeTitle;
	CString m_alarmTitle;
	time_t	m_change_ip_time;
	time_t	m_last_device_index_time;
	int		m_device_index;
	bool	m_use_face;		//skpark
	bool	m_show_name;	//skpark
	CString m_faceApi;		//skpark
	int		m_faceApiType;	//skpark

	bool	m_use_pos;
	bool	m_is_hiden_camera;
	bool	m_record_all;
	float	m_face_score_limit;

	bool	m_not_use_encrypt;

	bool	m_use_weather;		// �������, ������� ��뿩��
	CString	m_airMeasurStation;	// ��������Ҹ� 
	bool	m_use_mask_check;	// ����ũ���� ��뿩��

	bool	m_isWide;
	bool	m_useQR;
	bool	m_videoBelow;		// ī�޶󿵻� ȭ�� �ϴܿ� ��ġ ����(0:���, 1:�ϴ�)
};


