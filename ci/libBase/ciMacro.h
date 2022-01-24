/*! \file ciMacro.h
 *
 *  Copyright ¨Ï 2003 WINCC Inc.
 *  All Rights Reserved.
 *
 *  \brief Special MACRO tips
 *  (Environment: OSF1 5.1A)
 *
 *  \author skpark
 *  \version
 *  \date 		2001³â 4¿ù 13ÀÏ
 */

#ifndef _ciMacro_h_
#define _ciMacro_h_

#define ciQuote(xxx)  # xxx
#define ciName2(a,b)   a##b
#define ciName3(a,b,c) a##b##c

#define ciDoList(_list_, _itr_) \
    for( (_itr_) = (_list_).begin() ; (_itr_) != (_list_).end() ; ++(_itr_))

#define ciDoListPtr(_list_, _itr_) \
    for( (_itr_) = (_list_)->begin() ; (_itr_) != (_list_)->end() ; ++(_itr_))

#define ORA_DATE_FORMAT		"YYYY/MM/DD HH24:MI:SS"
#define ORA_DATE_FORMAT_C	"%Y/%m/%d %H:%M:%S"
#define ORA_DATE_DATE_FORMAT_C   "%Y/%m/%d"
#define UNIX_DATE_FORMAT		"+%Y/%m/%d %H:%M:%S"
#define UNIX_DATE_FORMAT_C	"%04d/%02d/%02d %02d:%02d:%02d"

#define NMS_BUF_LEN 256
#define NMS_BUF_LEN_2 1024
#define NMS_BUF_LEN_3 2048

#define IDL_VERSION "1.0" // for gw/snmp/nbgen

#ifndef XMIR_REFRENCE_ID
#define XMIR_REFRENCE_ID    "ManagedElement"
#endif

#ifndef XMIR_REFRENCE_KIND
#define XMIR_REFRENCE_KIND  ""
#endif

#ifndef XSNMPMIR_REFRENCE_KIND
#define XSNMPMIR_REFRENCE_KIND  ""
#endif

#define MO_ROOT     "COPROOT"

#define MIR_ENTRY   0
#define MIR_GROUP   1
#define MIR_AO      2

#if defined (_COP_MSC_)
#	define SLEEP(x)		_sleep(x*1000)
#	define STRCASECMP	_stricmp
#else
#	define SLEEP(x)		sleep(x)
#	define STRCASECMP	strcasecmp
#endif

#ifdef _COP_MSC_
#   define COP_ISSPACE(x)   iswspace(x)
#else
#   define COP_ISSPACE(x)   isspace(x)
#endif

#endif //_ciMacro_h_
