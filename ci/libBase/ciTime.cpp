/*! \file ciTime.C
 *
 *  Copyright ⓒ 2003 WINCC Inc.
 *  All Rights Reserved.
 *
 *  \brief Time Wrapper Class
 *  (Environment: OSF1 5.1A)
 *
 *  \author 	jhchoi
 *  \version
 *  \date 		2003년 4월 28일 14:00
 */

#define MAXLONG     0x7fffffff  

#include "ciTime.h"

void
ciTime::timeCheckLog(const char* pMsg)
{
#ifdef _COP_MSC_
#else
	ciString cmd = "echo \"";
	cmd += pMsg;
	cmd += "...at `date +'%r'`\" | tee -a ";
	cmd += getenv("PROJECT_HOME");
	cmd += "/log/timecheck.log";
	system(cmd.c_str());
#endif
}

ciTime::ciTime() {
	set();
}

ciTime::ciTime(const ciTime &p_time) {
	set(p_time);
}

ciTime::ciTime(time_t p_time) {
	set(p_time);
}

ciTime::ciTime(struct tm& p_tm) {
	set(p_tm);
}

ciTime::ciTime(const char* p_timeStr) {
	set(p_timeStr);
}

ciTime::ciTime(const ciString& p_timeStr) {
	set(p_timeStr);
}

void
ciTime::set() {
	//set current time
	 time(&m_time);
	_setTM();
}

void
ciTime::set(const ciTime& p_time) {
	m_time = p_time.m_time;
	_setTM();
}

void
ciTime::set(time_t p_time) {
	m_time = p_time;
	_setTM();
}

void
ciTime::set(struct tm& p_tm) {
	m_time = mktime(&p_tm);
	_setTM();
}

void
ciTime::set(const char *p_timeStr) {
	if (_setTimeString(p_timeStr) == 0) {
		m_time = 0;
		_setTM();
	}	
}

void
ciTime::set(const ciString& p_timeStr) {
	if (_setTimeString(p_timeStr.c_str()) == 0) {
		m_time = 0;
		_setTM();
	}	
}

ciInt
ciTime::_setTimeString(const char* p_strTime) {

	if (p_strTime == NULL && *p_strTime == '\0') return 0;

	/*
		  0123456789012345678
		- YYYY/MM/DD HH:MI:SS	19 : default
		- YYYY/MM/DD-HH:MI:SS	19
		- YYYY-MM-DD HH:MI:SS	19
		- YYYY-MM-DD HH-MI-SS	19 add 2005.8.29
		- YYYY:MM:DD HH:MI:SS	19 add 2004.6.3
		- YYYY/MM/DD HH:MI		16
		- YYYY-MM-DD HH:MI		16
		- YYYY:MM:DD HH:MI		16 add 2004.8.2
		- YY-MM-DD HH:MI:SS		17
		- DD/MM/YY HH/MI/SS		17 add 2004.9.25
		- MM/DD/YY HH:MI		14
		- YY-MM-DD HH:MI        14 add 2004.6.3
		- YYYYMMDDHHMMSS        14 add 2004.10.6
		- YYYYMMDDHH			10
		- YYYY-MM-DD HH			13
		- YY-MM-DD HHMI         13 add 2004.6.3
		- YY-MM-DD HH           11 add 2004.6.3
		- YYYYMMDD				8  add 2004.10.18
		- DD HH                 5  add 2004.6.3
	*/

	struct tm	c_tm;
	memset(&c_tm, 0x00, sizeof(struct tm));

	switch (strlen(p_strTime)) 
	{
		case 19 : {
			if (*(p_strTime+4) == '/' && *(p_strTime+7) == '/' &&
				*(p_strTime+10) == ' ' && *(p_strTime+13) == ':' &&
				*(p_strTime+16) == ':') {

				//sscanf(p_strTime, ORA_DATE_FORMAT_C,
				sscanf(p_strTime, "%04d/%02d/%02d %02d:%02d:%02d",
						&c_tm.tm_year, &c_tm.tm_mon, &c_tm.tm_mday,
						&c_tm.tm_hour, &c_tm.tm_min, &c_tm.tm_sec);

			} else if (*(p_strTime+4) == '/' && *(p_strTime+7) == '/' &&
				*(p_strTime+10) == '-' && *(p_strTime+13) == ':' &&
				*(p_strTime+16) == ':') {

				//sscanf(p_strTime, UNIX_DATE_FORMAT_C,
				sscanf(p_strTime, "%04d/%02d/%02d-%02d:%02d:%02d",
						&c_tm.tm_year, &c_tm.tm_mon, &c_tm.tm_mday,
						&c_tm.tm_hour, &c_tm.tm_min, &c_tm.tm_sec);

			} else if (*(p_strTime+4) == '-' && *(p_strTime+7) == '-' &&
				*(p_strTime+10) == ' ' && *(p_strTime+13) == ':' &&
				*(p_strTime+16) == ':') {

				sscanf(p_strTime, "%04d-%02d-%02d %02d:%02d:%02d",
						&c_tm.tm_year, &c_tm.tm_mon, &c_tm.tm_mday,
						&c_tm.tm_hour, &c_tm.tm_min, &c_tm.tm_sec);
			} else if ( *(p_strTime+4) == '-' && *(p_strTime+7) == '-' &&
					*(p_strTime+10) == ' ' && *(p_strTime+13) == '-' &&
					*(p_strTime+16) == '-')  {
				sscanf(p_strTime, "%04d-%02d-%02d %02d-%02d-%02d",
						&c_tm.tm_year, &c_tm.tm_mon, &c_tm.tm_mday,
						&c_tm.tm_hour, &c_tm.tm_min, &c_tm.tm_sec);
			} else if(*(p_strTime+4) == ':' && *(p_strTime+7) == ':' &&
				*(p_strTime+10) == ' ' && *(p_strTime+13) == ':' &&
				*(p_strTime+16) == ':') {
				sscanf(p_strTime, "%04d:%02d:%02d %02d:%02d:%02d",
						&c_tm.tm_year, &c_tm.tm_mon, &c_tm.tm_mday,
						&c_tm.tm_hour, &c_tm.tm_min, &c_tm.tm_sec);
			} else {
				return 0;
			}
			break;
		}
		case 16 : {
			if (*(p_strTime+4) == '/' && *(p_strTime+7) == '/' &&
				*(p_strTime+10) == ' ' && *(p_strTime+13) == ':') {

				sscanf(p_strTime, "%04d/%02d/%02d %02d:%02d",
						&c_tm.tm_year, &c_tm.tm_mon, &c_tm.tm_mday,
						&c_tm.tm_hour, &c_tm.tm_min);

			} else if (*(p_strTime+4) == '-' && *(p_strTime+7) == '-' &&
				*(p_strTime+10) == ' ' && *(p_strTime+13) == ':') {

				sscanf(p_strTime, "%04d-%02d-%02d %02d:%02d",
						&c_tm.tm_year, &c_tm.tm_mon, &c_tm.tm_mday,
						&c_tm.tm_hour, &c_tm.tm_min);

			} else if (*(p_strTime+4) == ':' && *(p_strTime+7) == ':' &&
				*(p_strTime+10) == ' ' && *(p_strTime+13) == ':') {

				sscanf(p_strTime, "%04d:%02d:%02d %02d:%02d",
						&c_tm.tm_year, &c_tm.tm_mon, &c_tm.tm_mday,
						&c_tm.tm_hour, &c_tm.tm_min);
			} else {
				return 0;
			}
			break;
		}
		case 17 : {
			if (*(p_strTime+2) == '-' && *(p_strTime+5) == '-' &&
				*(p_strTime+8) == ' ' && *(p_strTime+11) == ':' &&
				*(p_strTime+14) == ':') {

				sscanf(p_strTime, "%02d-%02d-%02d %02d:%02d:%02d",
						&c_tm.tm_year, &c_tm.tm_mon, &c_tm.tm_mday,
						&c_tm.tm_hour, &c_tm.tm_min, &c_tm.tm_sec);
				c_tm.tm_year += 2000;

			} else if (*(p_strTime+2) == '/' && *(p_strTime+5) == '/' &&
				*(p_strTime+8) == ' ' && *(p_strTime+11) == '/' &&
				*(p_strTime+14) == '/') {

				sscanf(p_strTime, "%02d/%02d/%02d %02d/%02d/%02d",
						&c_tm.tm_mday, &c_tm.tm_mon, &c_tm.tm_year,
						&c_tm.tm_hour, &c_tm.tm_min, &c_tm.tm_sec);
				c_tm.tm_year += 2000;

			} else {
				return 0;
			}
			break;
		}
		case 14 : {
			if (*(p_strTime+2) == '/' && *(p_strTime+5) == '/' &&
				*(p_strTime+8) == ' ' && *(p_strTime+11) == ':') {

				sscanf(p_strTime, "%02d/%02d/%02d %02d:%02d",
						&c_tm.tm_mon,
						&c_tm.tm_mday,
						&c_tm.tm_year,
						&c_tm.tm_hour,
						&c_tm.tm_min);
				c_tm.tm_year += 2000;

			} else if(*(p_strTime+2) == '-' && *(p_strTime+5) == '-' &&
					   *(p_strTime+8) == ' ' && *(p_strTime+11) == ':') {
				sscanf(p_strTime, "%02d-%02d-%02d %02d:%02d",
						&c_tm.tm_year,
						&c_tm.tm_mon,
						&c_tm.tm_mday,
						&c_tm.tm_hour,
						&c_tm.tm_min);
				c_tm.tm_year += 2000;
			} else if(*(p_strTime+4) != ' ' && *(p_strTime+6) != ' ') {
				sscanf(p_strTime, "%04d%02d%02d%02d%02d%02d",
					&c_tm.tm_year, &c_tm.tm_mon, &c_tm.tm_mday,
					&c_tm.tm_hour, &c_tm.tm_min, &c_tm.tm_sec);
			} else {
				return 0;
			}
			break;
		}
		case 10 : {
			sscanf(p_strTime, "%04d%02d%02d%02d",
					&c_tm.tm_year,
					&c_tm.tm_mon,
					&c_tm.tm_mday,
					&c_tm.tm_hour);
			break;
		}

		case 13 : 
			{
				if(*(p_strTime+4) == '-' && *(p_strTime+7) == '-' &&
					*(p_strTime+10) == ' ') {
					sscanf(p_strTime, "%04d-%02d-%02d %02d",
						&c_tm.tm_year,
						&c_tm.tm_mon,
						&c_tm.tm_mday,
						&c_tm.tm_hour);
				} else if(*(p_strTime+2) == '-' && *(p_strTime+5) == '-' &&
						*(p_strTime+8) == ' ') {
					sscanf(p_strTime, "%02d-%02d-%02d %02d%02d",
						&c_tm.tm_year, &c_tm.tm_mon, &c_tm.tm_mday,
						&c_tm.tm_hour, &c_tm.tm_min);
						c_tm.tm_year += 2000;
				} else {
					return 0;
				}
					break;
			}
		case 11 :
			{
				if(*(p_strTime+2) == '-' && *(p_strTime+5) == '-' &&
					*(p_strTime+8) == ' ') {
					sscanf(p_strTime, "%02d-%02d-%02d %02d",
						&c_tm.tm_year,
						&c_tm.tm_mon,
						&c_tm.tm_mday,
						&c_tm.tm_hour);
					c_tm.tm_year += 2000;
				} else {
					sscanf(p_strTime, "%04d%02d%02d %02d",
						&c_tm.tm_year, &c_tm.tm_mon, &c_tm.tm_mday, &c_tm.tm_hour);
				}
			}
			break;


		case 8 : {
			sscanf(p_strTime, "%04d%02d%02d",
					&c_tm.tm_year,
					&c_tm.tm_mon,
					&c_tm.tm_mday);
			break;
		}

		case 5 :
			{
				if(*(p_strTime+2) == ' ') {
					time_t tmp_time;
					tmp_time = time(NULL);
					c_tm = *(localtime(&tmp_time));
					sscanf(p_strTime, "%02d %02d", &c_tm.tm_mday, &c_tm.tm_hour);
					c_tm.tm_year += 1900;
					c_tm.tm_mon +=1;
					c_tm.tm_min = 0 ;
					c_tm.tm_sec = 0;
				} else {
					return 0;
				}
			}
            break;
				
		default : {
			return 0;
		}
	}
	c_tm.tm_year -= 1900;
	c_tm.tm_mon -= 1;
	
	time_t now1;
	time(&now1);
	struct tm* localTime = localtime(&now1);
	c_tm.tm_isdst = localTime->tm_isdst;


	if((m_time = mktime(&c_tm)) < 0) {
		return 0;
	}
	_setTM();
	return 1;
}

/*
ciInt
ciTime::_setTimeString(const char* p_strTime) {

	if (p_strTime == NULL && *p_strTime == '\0') return 0;

	//	  0123456789012345678
	//	- YYYY/MM/DD HH:MI:SS	19 : default
	//	- YYYY/MM/DD-HH:MI:SS	19
	//	- YYYY-MM-DD HH:MI:SS	19
	//	- YYYY/MM/DD HH:MI		16
	//	- YYYY-MM-DD HH:MI		16
	//	- YY-MM-DD HH:MI:SS		17
	//	- MM/DD/YY HH:MI		14
	//	- YYYYMMDDHH			10
	//	- YYYY-MM-DD HH			13

	struct tm	c_tm;
	memset(&c_tm, 0x00, sizeof(struct tm));

	switch (strlen(p_strTime)) {
		case 19 : {
			if (*(p_strTime+4) == '/' && *(p_strTime+7) == '/' &&
				*(p_strTime+10) == ' ' && *(p_strTime+13) == ':' &&
				*(p_strTime+16) == ':') {

				sscanf(p_strTime, ORA_DATE_FORMAT_C,
						&c_tm.tm_year, &c_tm.tm_mon, &c_tm.tm_mday,
						&c_tm.tm_hour, &c_tm.tm_min, &c_tm.tm_sec);

			} else if (*(p_strTime+4) == '/' && *(p_strTime+7) == '/' &&
				*(p_strTime+10) == '-' && *(p_strTime+13) == ':' &&
				*(p_strTime+16) == ':') {

				sscanf(p_strTime, UNIX_DATE_FORMAT_C,
						&c_tm.tm_year, &c_tm.tm_mon, &c_tm.tm_mday,
						&c_tm.tm_hour, &c_tm.tm_min, &c_tm.tm_sec);

			} else if (*(p_strTime+4) == '-' && *(p_strTime+7) == '-' &&
				*(p_strTime+10) == ' ' && *(p_strTime+13) == ':' &&
				*(p_strTime+16) == ':') {

				sscanf(p_strTime, "%04d-%02d-%02d %02d:%02d:%02d",
						&c_tm.tm_year, &c_tm.tm_mon, &c_tm.tm_mday,
						&c_tm.tm_hour, &c_tm.tm_min, &c_tm.tm_sec);

			} else {
				return 0;
			}
			break;
		}
		case 16 : {
			if (*(p_strTime+4) == '/' && *(p_strTime+7) == '/' &&
				*(p_strTime+10) == ' ' && *(p_strTime+13) == ':') {

				sscanf(p_strTime, "%04d/%02d/%02d %02d:%02d",
						&c_tm.tm_year, &c_tm.tm_mon, &c_tm.tm_mday,
						&c_tm.tm_hour, &c_tm.tm_min);

			} else if (*(p_strTime+4) == '-' && *(p_strTime+7) == '-' &&
				*(p_strTime+10) == ' ' && *(p_strTime+13) == ':') {

				sscanf(p_strTime, "%04d-%02d-%02d %02d:%02d",
						&c_tm.tm_year, &c_tm.tm_mon, &c_tm.tm_mday,
						&c_tm.tm_hour, &c_tm.tm_min);

			} else {
				return 0;
			}
			break;
		}
		case 17 : {
			if (*(p_strTime+2) == '-' && *(p_strTime+5) == '-' &&
				*(p_strTime+8) == ' ' && *(p_strTime+11) == ':' &&
				*(p_strTime+14) == ':') {

				sscanf(p_strTime, "%02d-%02d-%02d %02d:%02d:%02d",
						&c_tm.tm_year, &c_tm.tm_mon, &c_tm.tm_mday,
						&c_tm.tm_hour, &c_tm.tm_min, &c_tm.tm_sec);
				c_tm.tm_year += 2000;

			} else {
				return 0;
			}
			break;
		}
		case 14 : {
			if (*(p_strTime+2) == '/' && *(p_strTime+5) == '/' &&
				*(p_strTime+8) == ' ' && *(p_strTime+11) == ':') {

				sscanf(p_strTime, "%02d/%02d/%02d %02d:%02d",
						&c_tm.tm_mon,
						&c_tm.tm_mday,
						&c_tm.tm_year,
						&c_tm.tm_hour,
						&c_tm.tm_min);
				c_tm.tm_year += 2000;

			} else {
				return 0;
			}
			break;
		}
		case 10 : {
			sscanf(p_strTime, "%04d%02d%02d%02d",
					&c_tm.tm_year,
					&c_tm.tm_mon,
					&c_tm.tm_mday,
					&c_tm.tm_hour);
			break;
		}

		case 13 : 
			{
				sscanf(p_strTime, "%04d-%02d-%02d %02d",
					&c_tm.tm_year,
                    &c_tm.tm_mon,
                    &c_tm.tm_mday,
                    &c_tm.tm_hour);
				break;
			}
		default : {
			return 0;
		}
	}
	c_tm.tm_year -= 1900;
	c_tm.tm_mon -= 1;
	m_time = mktime(&c_tm);
	_setTM();
	return 1;
}
*/

void
ciTime::add(ciInt sec) {
	set(m_time + sec);
}

void
ciTime::_setTM() {
#ifdef _COP_MSC_
	//struct tm *temp = localtime(&m_time);
	//m_tm = *temp;
	//m_tm = *localtime(&m_time);

	// m_time 이 Invalid 값이 들어올것을 대비한다.
	tm* local_time = localtime(&m_time);
	if(local_time == 0){
		m_time = MAXLONG;
		local_time = localtime(&m_time);
	}
	m_tm = *local_time;
#else
    localtime_r(&m_time, &m_tm);
#endif

}

time_t
ciTime::getTime() const {
	return m_time;
}

struct tm
ciTime::getTM() {
	return m_tm;
}

ciString
ciTime::getTimeString() {
	char	buf[20];
	sprintf(buf, UNIX_DATE_FORMAT_C,
			m_tm.tm_year + 1900, m_tm.tm_mon + 1, m_tm.tm_mday,
			m_tm.tm_hour, m_tm.tm_min, m_tm.tm_sec);
	return ciString(buf);
}

ciString
ciTime::getTimeString(const ciString& p_strForm) {
	char	buf[32];   
	strftime(buf, sizeof(buf), p_strForm.c_str(), &m_tm);
	return ciString(buf);
}

void
ciTime::getTimeString_r(ciString& p_timeStr) {
	char	buf[20];
	sprintf(buf, UNIX_DATE_FORMAT_C,
			m_tm.tm_year + 1900, m_tm.tm_mon + 1, m_tm.tm_mday,
			m_tm.tm_hour, m_tm.tm_min, m_tm.tm_sec);
	p_timeStr = buf;
}

void
ciTime::getTimeString_r(const ciString& p_strForm, ciString& p_timeStr) {
	char	buf[32];   
	strftime(buf, sizeof(buf), p_strForm.c_str(), &m_tm);
	p_timeStr = buf;
}

ciInt
ciTime::operator - (const ciTime &p_time) {
	return m_time - p_time.m_time;
}

ciInt
ciTime::operator + (ciInt p_time) {
	return m_time + p_time;
}

ciInt
ciTime::operator - (ciInt p_time) {
	return m_time - p_time;
}

ciTime&
ciTime::operator = (const ciTime& p_time) {
	set(p_time);
	return *this;
}

ciTime&
ciTime::operator = (const char* p_timeStr) {
	set(p_timeStr);
	return *this;
}

bool
ciTime::operator > (const ciTime& p_time) {
	return m_time > p_time.m_time;
}

bool
ciTime::operator >= (const ciTime& p_time) {
	return m_time >= p_time.m_time;
}

bool
ciTime::operator < (const ciTime& p_time) {
	return m_time < p_time.m_time;
}

bool
ciTime::operator <= (const ciTime& p_time) {
	return m_time <= p_time.m_time;
}

bool
ciTime::operator == (const ciTime& p_time) {
	return m_time == p_time.m_time;
}

ciInt
ciTime::getYear() {
	return m_tm.tm_year + 1900;
}

ciInt
ciTime::getMonth() {
	return m_tm.tm_mon + 1;
}

ciInt
ciTime::getDay() {
	return m_tm.tm_mday;
}

ciInt
ciTime::getHour() {
	return m_tm.tm_hour;
}

ciInt
ciTime::getMinute() {
	return m_tm.tm_min;
}

ciInt
ciTime::getSecond() {
	return m_tm.tm_sec;
}

ciInt
ciTime::getWday() {
	return m_tm.tm_wday;
}

ciInt ciTime::getRemainDaySec()
{
	return (60*60*24 - (getHour()*3600 + getMinute()*60 + getSecond()));
}

ciInt ciTime::getRemainHourSec()
{
	return (3600 - (getMinute()*60 + getSecond()));
}
