/*! \file ciBaseType.h
 *
 *  Copyright ⓒ 2003 WINCC Inc.
 *  All Rights Reserved.
 *
 *  \brief STL 의 string type 및 프로젝트에서 사용할 타입들을 typedef 한다.
 *  (Environment: OSF1 5.1A)
 *
 *  \author 	jhchoi
 *  \version
 *  \date 		2003년 4월 22일 18:00
 */

#ifndef _ciBaseType_h_
#define _ciBaseType_h_

/*
#define _COP_HPUX_  //or _COP_LINUX_ , _COP_SOLARIS_ , _COP_WIN32_ , _COP_HPUX_ , _COP_WINCE_ , _COP_TRU64_ //OS
#define _COP_ACC_ //or _COP_CXX_[Version]_ , _COP_GCC_<Version>_ , _COP_MSC_[Version]_ //COMPILER
*/


#include "ciWin32DLL.h"
//#include "ciAceType.h"

#include <cstdio>
#include <string>

#if defined(_COP_LINUX_)
#	include <string.h>
#	include <stdlib.h>
#endif

#if defined(_COP_ACC_) && defined(_COP_NOSTDLIB_) 
#	include <iostream.h>
#	include <fstream.h>
#else
#	include <iostream>
#	include <fstream>
using namespace std;
#endif

//using namespace _STLP_NEW_IO_NAMESPACE;


#define ciTrue		true
#define ciFalse	false

typedef string			ciString; //basic_string<char>

//typedef wstring		ciWString; //basic_string<wchar_t>
typedef basic_string<wchar_t>		ciWString; //wstring
typedef short			ciShort;
typedef unsigned short 	ciUShort;
typedef int 			ciInt;
typedef unsigned int 	ciUInt;

#if defined(_COP_TRU64_) || defined(_COP_HPUX_)
	typedef signed int ciLong;
	typedef unsigned int ciULong;
#else
	typedef signed long ciLong;
	typedef unsigned long ciULong;
#endif

#ifdef _COP_MSC_
	typedef __int64 ciLongLong;
	typedef unsigned __int64 ciULongLong;
    
#else
	typedef long long ciLongLong;
	typedef unsigned long long ciULongLong;
#endif

typedef float			ciFloat;
typedef double			ciDouble;
typedef char			ciByte;
typedef char 			ciChar;
typedef long			ciDate;
typedef bool 			ciBoolean;

#define null 0

#ifdef _COP_MSC_
	#define COP_INFINITE 0xFFFFFFFF
#else
	#define COP_INFINITE 0x00000000L
#endif

#define ciDoList(_list_, _itr_) \
for ((_itr_) = (_list_).begin(); (_itr_) != (_list_).end(); ++(_itr_))

#define ciDoListPtr(_list_, _itr_) \
for ((_itr_) = (_list_)->begin(); (_itr_) != (_list_)->end(); ++(_itr_))

#endif //_ciBaseType_h_

