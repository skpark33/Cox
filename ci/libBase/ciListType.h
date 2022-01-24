/*! \file ciListType.h
 *
 *  Copyright ⓒ 2003 WINCC Inc.
 *  All Rights Reserved.
 *
 *  \brief Base Type 에 대한 vector 과 list 의 typedef
 *  (Environment: OSF1 5.1A)
 *
 *  \author 	jhchoi
 *  \version
 *  \date 		2003년 4월 22일 18:00
 */

#ifndef _ciListType_h_
#define _ciListType_h_

#include "ciBaseType.h"

#include <vector>
#include <list>
#include <set>
#include <map>

typedef vector<ciString>    ciStringVector;
typedef vector<ciWString>   ciWStringVector;
typedef vector<ciShort>     ciShortVector;
typedef vector<ciInt>       ciIntVector;
typedef vector<ciLong>      ciLongVector;
typedef vector<ciULong>     ciULongVector;
typedef vector<ciFloat>     ciFloatVector;
typedef vector<ciDouble>    ciDoubleVector;
typedef vector<ciDate>      ciDateVector;
typedef vector<ciByte>      ciByteVector;

typedef vector<ciStringVector> ciStringVectorVector;

typedef list<ciString>  ciStringList;
typedef list<ciWString> ciWStringList;
typedef list<ciShort>   ciShortList;
typedef list<ciInt>     ciIntList;
typedef list<ciLong>    ciLongList;
typedef list<ciULong>	ciULongList;
typedef list<ciFloat>   ciFloatList;
typedef list<ciDouble>  ciDoubleList;
typedef list<ciDate>    ciDateList;
typedef list<ciByte>    ciByteList;

typedef set<ciString>  ciStringSet;
typedef set<ciWString> ciWStringSet;
typedef set<ciShort>   ciShortSet;
typedef set<ciInt>     ciIntSet;
typedef set<ciLong>    ciLongSet;
typedef set<ciULong>   ciULongSet;
typedef set<ciFloat>   ciFloatSet;
typedef set<ciDouble>  ciDoubleSet;
typedef set<ciDate>    ciDateSet;
typedef set<ciByte>    ciByteSet;

typedef map<ciString,ciString> ciStringMap;

#endif //_ciListType_h_
