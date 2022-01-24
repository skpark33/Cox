/*! \file ciTime.h
 *
 *  Copyright ¨Ï 2003 WINCC Inc.
 *  All Rights Reserved.
 *
 *  \brief Time Wrapper Class
 *  (Environment: OSF1 5.1A)
 *
 *  \author 	jhchoi
 *  \version
 *  \date 		2003³â 4¿ù 28ÀÏ 14:00
 */

#ifndef _ciTime_h_
#define _ciTime_h_

#include "ciBaseType.h"
#include "ciMacro.h"
#include <ctime>

class  ciTime {
public:

	static void timeCheckLog(const char* pMsg);

	ciTime();
	ciTime(const ciTime& p_time);
	ciTime(time_t p_time);
	ciTime(struct tm& p_tm);
	ciTime(const char* p_strTime);
	ciTime(const ciString& p_strTime);

	void	set(); //set current time
	void	set(const ciTime& p_time);
	void	set(time_t p_time);
	void	set(struct tm& p_tm);
	void	set(const char* p_strTime);
	void	set(const ciString& p_strTime);

	void	add(ciInt sec);

	time_t	getTime() const;
	struct tm	getTM();
	ciString	getTimeString();
	ciString	getTimeString(const ciString& p_strForm);
	void		getTimeString_r(ciString& p_strTime);
	void		getTimeString_r(const ciString& p_strForm, ciString& p_strTime);
	
	ciInt		getYear();
	ciInt		getMonth();
	ciInt		getDay();
	ciInt		getHour();
	ciInt		getMinute();
	ciInt		getSecond();
	ciInt		getWday();
	ciInt		getRemainDaySec();
	ciInt		getRemainHourSec();

	ciInt operator - (const ciTime &p_time);
	ciInt operator - (ciInt p_time);
	ciInt operator + (ciInt p_time);
	ciTime& operator = (const ciTime& p_time);
	ciTime& operator = (const char* p_strTime);
	bool operator > (const ciTime& p_time);
	bool operator >= (const ciTime& p_time);
	bool operator == (const ciTime& p_time);
	bool operator < (const ciTime& p_time);
	bool operator <= (const ciTime& p_time);

protected:
	void		_setTM();
	ciInt		_setTimeString(const char* p_strTime);

	time_t	m_time;
	struct tm	m_tm;
};

#endif //_ciTime_h_
