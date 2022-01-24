#pragma once

#include "afxcmn.h"
#include "afxwin.h"
//#include "libCxImage/ximage.h"	//skpark

class AlarmInfoEle
{
public:
	AlarmInfoEle(LPCTSTR id) : m_cxImage(0), m_currentTempTime(0), m_alarmLevel(-1) , m_score(0.0f), m_eventId(id),
		m_maskLevel(1) {}
	virtual ~AlarmInfoEle()
	{
		Clear();
	}
	void Init(LPCTSTR szInfoBuf, int alarmLevel)
	{
		this->m_humanName = _T("");
		this->m_grade = _T("");
		this->m_room = _T("");
		this->m_currentTemp = szInfoBuf;
		time_t now = time(NULL);
		this->m_currentTempTime = now;
		this->m_alarmLevel = alarmLevel;
		this->m_isFever = false;
		this->m_maskLevel = 1;
		m_imgUrl = _T("");
		//m_eventId = _T("");

	}
	void Clear()
	{
		this->m_humanName = _T("");
		this->m_grade = _T("");
		this->m_room = _T("");
		this->m_currentTemp = _T("");
		this->m_currentTempTime = 0;
		if (m_cxImage) { delete m_cxImage; m_cxImage = 0; }
		m_alarmLevel = -1;
		this->m_isFever = false;
		this->m_maskLevel = 1;
		m_imgUrl = _T("");
		//m_eventId = _T("");
	}

	void ToJson(CString& outJson)
	{
		/*
		[ROOT]
		Temp=37.9
		X=0.472000
		Y=0.622000
		Width=0.031000
		Height=0.067000
		Level=1 or 0
		faceResult={"matches":[],"result_code":"F","result_msg":"face not found"}
		faceResult={"matches":[{"mask":false,"bounds":"-4,-36,303,404","name":"�����","score":0.4240404,"grdde":"3","room":"5"}],"result_code":"S","result_msg":null}
		*/
		outJson.Format(
			_T("[ROOT]\r\nTemp=%s\r\nX=0\r\nY=0\r\nWidth=0\r\nHeight=0\r\nLevel=%d\r\nfaceResult={\"matches\":[{\"mask\":%s,\"bounds\":\"0,0,0,0\",\"name\":\"%s\",\"score\":%f,\"grade\":\"%s\",\"room\":\"%s\"}],\"result_code\":\"S\",\"result_msg\":null}\r\n"),
			m_currentTemp, m_alarmLevel, (m_maskLevel ? "true" : "false"), m_humanName, m_score, m_grade, m_room);
	}

	bool SameAs(AlarmInfoEle* t)
	{
		if (m_eventId == t->m_eventId			&&
			m_humanName == t->m_humanName	&&
			m_grade == t->m_grade			&&
			m_room == t->m_room				&&
			m_alarmLevel == t->m_alarmLevel		&&
			m_isFever == t->m_isFever			&&
			m_maskLevel == t->m_maskLevel)
			return true;
		return false;
	}
	CString m_eventId; 
	CString m_currentTemp;		// ü��
	time_t	m_currentTempTime;	// ü�½ð�
	CString m_humanName;		// �̸�
	CString m_grade;			// �г�
	CString m_room;				// ��
	CImage* m_cxImage;			// ����
	CString m_imgUrl;			// �����н�
	int		m_alarmLevel;
	bool	m_isFever;			// �߿�����
	int		m_maskLevel;		// ����ũ(�̰���:-1, ������:0, ����:1)
	float  m_score;				// �µ� ��Ȯ��
};
typedef std::map<long, AlarmInfoEle*>  HUMAN_INFOMAP;
typedef std::list<AlarmInfoEle*>  HUMAN_INFOLIST;
typedef std::map<CString, AlarmInfoEle*>  DRAW_FACE_MAP;
