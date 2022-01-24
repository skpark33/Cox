/*
*
*  Copyright ⓒ 2002 WINCC Inc.
*  All Rights Reserved.
*
*  This source code is confidential and proprietary and may not be used
*  or distributed without the written permission of WINCC Inc.
*
*  Created by :  nms
*  Modified by :
*  Last update : 2002.6.24 14:00
*  Comment :
*/

#include "ciStringUtil.h"
#include "ciStringTokenizer.h"
#include "ciMacro.h"


#ifndef _COP_MSC_
#include <regex.h>
#endif

#include <cstdlib>
#include <ctype.h>

#ifdef _COP_MSC_
#include <time.h>

#endif

ciStringUtil::ciStringUtil() {
}

ciStringUtil::~ciStringUtil() {
}


ciBoolean
ciStringUtil::contain(const char* target, const char pDeli) {
    const char *ptr = target;

    while(*ptr) {
        if ( *ptr == pDeli ){
            return ciTrue;
        }
        ptr++;
    }
    return ciFalse;
}

void
ciStringUtil::divide(const ciString& pTarget, const char pDeli,
                     ciString* pFirst, ciString* pOther)
{
    const char *ptr = pTarget.c_str();
    ciString* aResPtr = pFirst;
    ciBoolean   matched = ciFalse;
    while(*ptr){
        if ( !matched && *ptr == pDeli ){
            aResPtr = pOther;
            ptr++;
            matched = ciTrue;
            continue;
        }
        *aResPtr += *ptr;
        ptr++;
    }
}
void 
ciStringUtil::getBraceValue(ciString& target) 
{ getBraceValue(target,"(",")"); }
ciBoolean  
ciStringUtil::hasBrace(ciString& target) 
{ return hasBrace(target,"(",")"); }
ciBoolean  
ciStringUtil::hasBrace(const char* target) 
{ return hasBrace(target,"(",")"); }
int  
ciStringUtil::getBraceValues(ciString& target, const char* deli,ciStringList* result)
{ return getBraceValues(target,deli,result,"(",")"); }

int  
ciStringUtil::getBraceValues(ciString& target, ciString& before, ciString& after, const char* deli, ciStringList* result)
{ return getBraceValues(target,before,after,deli,result,"(",")"); }

void
ciStringUtil::getBraceValue(ciString& target,const char* lb, const char* rb) {
    ciString a_buf = target;
    int start = target.find(lb);
    int end = target.rfind(rb);
    if(start >= 0 && end >=0 && start+1 <= end ){
        a_buf = target.substr(start+1, end - start - 1);
    }
    target = a_buf;
}

ciBoolean
ciStringUtil::hasBrace(ciString& target,const char* lb, const char* rb)
{
    int start = target.find(lb);
    if(start < 0) {
		return ciFalse;
	}
    int end = target.rfind(rb);
    if(end < 0 ){
		return ciFalse;
	}
    if(start+1 > end ){
		return ciFalse;
	}
	return ciTrue;
}

ciBoolean
ciStringUtil::hasBrace(const char* target,const char* lb, const char* rb)
{   
	ciString aTar =  target;
	return ciStringUtil::hasBrace(aTar,lb,rb);
}   

int
ciStringUtil::getBraceValues(ciString& target,ciStringList* result,const char* lb, const char* rb) 
{
    int start = target.find(lb);

	if(start < 0) {
		return 0;
	}
    int end = target.find(rb);
    if(end < 0){
		return 0;
	}
	int l_len = strlen(lb);
	start += l_len;

	if(start > end){
		return 0;
	}
    ciString a_buf = target.substr(start, end - start);
	result->push_back(a_buf);
	return ciStringUtil::getBraceValues(target.substr(end+strlen(rb)), result, lb,rb);
}


int
ciStringUtil::getBraceValues(ciString& target, const char* deli, ciStringList* result,const char* lb, const char* rb) 
{
    int start = target.find(lb);
    if(start < 0) {
		return 0;
	}
    int end = target.rfind(rb);
    if(end < 0){
		return 0;
	}
    if(start+1 > end){
		return 0;
	}
    ciString a_buf = target.substr(start+1, end - start - 1);
	return ciStringUtil::Tokenizer(a_buf.c_str(), deli, result);
}
int
ciStringUtil::getBraceValues(ciString& target, const char* deli, ciStringVector* result,const char* lb, const char* rb) 
{
    int start = target.find(lb);
    if(start < 0) {
		return 0;
	}
    int end = target.rfind(rb);
    if(end < 0){
		return 0;
	}
    if(start+1 > end){
		return 0;
	}
    ciString a_buf = target.substr(start+1, end - start - 1);
	return ciStringUtil::Tokenizer(a_buf.c_str(), deli, result);
}
int
ciStringUtil::getBraceValues(ciString& target, ciString& before, ciString& after, const char* deli, ciStringList* result,const char* lb, const char* rb)
{
    int start = target.find(lb);
    if(start < 0) {
        return 0;
    }
    int end = target.rfind(rb);
    if(end < 0){
        return 0;
    }
    if(start+1 > end){
        return 0;
    }
	int size = target.length();
	before = target.substr(0,start);
	after = target.substr(end+1,size-end);
    ciString a_buf = target.substr(start+1, end - start - 1);
    return ciStringUtil::Tokenizer(a_buf.c_str(), deli, result);
}


// ciStringUtil::toUpper(ciString& str)
/*!
str 문자열의 uppercase 변환문자열을 리턴한다.
*/
void
ciStringUtil::toUpper(const ciString& str, ciString& retStr) {
    const char* ptr = str.c_str();
    retStr = "";
    while(*ptr) {
        char a_buf[2];
        sprintf(a_buf, "%c", toupper(*ptr));
        retStr += a_buf;
        ptr++;
    }
}

// ciStringUtil::toLower(ciString& str)
/*!
str 문자열의 lowercase 변환문자열을 리턴한다.
*/
void
ciStringUtil::toLower(const ciString& str, ciString& retStr) {
    const char *ptr = str.c_str();
    retStr = "";
    while(*ptr){
        char a_buf[2];
        sprintf(a_buf, "%c", tolower(*ptr));
        retStr += a_buf;
        ptr++;
    }
}

// ciStringUtil::toUpper(ciString& str)
/*!
str 문자열을 uppercase 로 변환한다.
*/
void
ciStringUtil::toUpper(ciString& str) {
    ciString to;
    const char *ptr = str.c_str();
    while(*ptr){
        char a_buf[2];
        sprintf(a_buf, "%c", toupper(*ptr));
        to += a_buf;
        ptr++;
    }
    str = to;
}

// ciStringUtil::toLower(ciString& str)
/*!
str 문자열을 lowercase 로 변환한다.
*/
void
ciStringUtil::toLower(ciString& str) {
    ciString to;
    const char *ptr = str.c_str();
    while(*ptr){
        char a_buf[2];
        sprintf(a_buf, "%c", tolower(*ptr));
        to += a_buf;
        ptr++;
    }
    str = to;
}

void
ciStringUtil::safeToUpper(ciString& src) {
    char pre_char = ' ';
    bool isDoubleLiteralZone = false;
    bool isSingleLiteralZone = false;
    ciString buf= "";

    int len = src.size();
    for( int i= 0;i<len;i++){

        if(pre_char != '\\' && src[i] == '\"' ){
            isDoubleLiteralZone = ( isDoubleLiteralZone ? false : true );
        }
        if(pre_char != '\\' && src[i] == '\'' ){
            isSingleLiteralZone = ( isSingleLiteralZone ? false : true );
        }
        pre_char = src[i];
        if(isDoubleLiteralZone == false && isSingleLiteralZone == false ){
			//char a_buf[2];
			//sprintf(a_buf, "%c", toupper(src[i]));
			//buf += a_buf;
			buf += toupper(src[i]);
        }else{
			buf += src[i];
		}
    }
    src = buf;
}
void
ciStringUtil::safeToLower(ciString& src) {
    char pre_char = ' ';
    bool isDoubleLiteralZone = false;
    bool isSingleLiteralZone = false;
    ciString buf= "";

    int len = src.size();
    for( int i= 0;i<len;i++){

        if(pre_char != '\\' && src[i] == '\"' ){
            isDoubleLiteralZone = ( isDoubleLiteralZone ? false : true );
        }
        if(pre_char != '\\' && src[i] == '\'' ){
            isSingleLiteralZone = ( isSingleLiteralZone ? false : true );
        }
        pre_char = src[i];
        if(isDoubleLiteralZone == false && isSingleLiteralZone == false ){
			//char a_buf[2];
			//sprintf(a_buf, "%c", tolower(src[i]));
			//buf += a_buf;
			buf += toupper(src[i]);
        }else{
			buf += src[i];
		}
    }
    src = buf;
}


// ciStringUtil::trim(char* str)
/*!
str 문자열에서 첫번째 space이후의 문자열을 잘라낸다.
*/
void
ciStringUtil::trimPreSpace(ciString& str)
{
    int size = str.size();
    if (size < 1) return;
    int start = 0, i;
    for( i=0 ; i < size ; i++){
        if(str[i] != ' ' && str[i] != '\t'){
            start = i;
            break;
        }
    }
    str = str.substr(start);
}

void
ciStringUtil::trim(char *str) {
    if(!str) return;
    int len = strlen(str);
    if(len <= 0) return;

    char *ptr = str;
    while(*ptr){
        if(COP_ISSPACE(*ptr)){
            ptr++;
            continue;
        }else{
            break;
        }
    }
    if(*ptr) strcpy(str,ptr);
    else {
        str = (char *)"";
        return;
    }

    len = strlen(str);
    if(len <= 0) return;

    for(int i = len-1; i >= 0 ;i--){
        if(COP_ISSPACE(str[i])){
            str[i] = 0;
            break;
        }else{
            break;
        }
    }
}

// ciStringUtil::trim(ciString& str)
/*!
str 문자열에서 첫번째 space이후의 문자열을 잘라내고
첫번째 space 까지의 문자열을 return 한다.
*/
ciString&
ciStringUtil::trim(ciString& str) {
    int size = str.size();
    if (size < 1) return str;
    int start = 0, i;
    for( i=0 ; i < size ; i++){
        if(str[i] != ' ' && str[i] != '\t'){
            start = i;
            break;
        }
    }
    str = str.substr(start);
    size = str.size();
    if(size < 1) return str;

    int end = size ;

    for(i=0 ; i < size ; i++){
        if(str[i] == ' ' || str[i] == '\t'){
            end = i;
            break;
        }
    }
    str = str.substr(0, end);
    return str;
}

// ciStringUtil::safeRemoveSpace(const char *src, char *dest)
/*!
src 문자열에서 space 를 삭제하여 dest 로 return 한다.
*/
void
ciStringUtil::safeRemoveSpace(const char *src, char *dest) {
    char pre_char = ' ';
    bool isStringLiteralZone = false;

    while(*src) {
        if(pre_char != '\\' && *src == '\"'){
            isStringLiteralZone = ( isStringLiteralZone ? false : true );
        }
        pre_char = *src;
        if(isStringLiteralZone == false && COP_ISSPACE(*src)){
            src++;
            continue;
        }
        *dest = *src;
        dest++;
        src++;
    }
    *dest = '\0';
}

void
ciStringUtil::safeRemoveSpace(ciString& src) {
    char pre_char = ' ';
    bool isStringLiteralZone = false;
    ciString buf= "";

    int len = src.size();
    for( int i= 0;i<len;i++){

        if(pre_char != '\\' && src[i] == '\"'){
            isStringLiteralZone = ( isStringLiteralZone ? false : true );
        }
        pre_char = src[i];
        if(isStringLiteralZone == false && COP_ISSPACE(src[i])){
            continue;
        }
        buf += src[i];
    }
    src = buf;
}



// ciStringUtil::safeRemoveSpaceAndQuote(const char *src, char *dest)
/*!
src 문자열에서 space, ', " 를 삭제하여 dest 로 return 한다.
*/
void
ciStringUtil::safeRemoveSpaceAndQuote(const char *src, char *dest) {
    char pre_char = ' ';
    bool isStringLiteralZone = false;

    while (*src) {
        if (pre_char != '\\' && *src == '\"') {
            isStringLiteralZone = ( isStringLiteralZone ? false : true );
            pre_char = *src;
            src++;
            continue;
        }
        pre_char = *src;
        if(isStringLiteralZone == false && COP_ISSPACE(*src)){
            src++;
            continue;
        }
        *dest++ = *src++;
    }
    *dest = 0;
}

// ciStringUtil::removeSpace(ciString& str)
/*!
str 문자열에서 space 를 삭제한다.
*/
void
ciStringUtil::removeSpace(ciString& str) {
    //cout << "-----------------------------------removeSpace(" << str.c_str() << ")-------" << endl;
    ciString buf= "";
    int len = str.size();
    for ( int i= 0;i<len;i++) {
        if(COP_ISSPACE(str[i])){
            continue;
        }
        buf += str[i];
    }
    str = buf;
}

// ciStringUtil::removeSpace(const char *src, char *dest)
/*!
src 문자열에서 space 를 삭제하여 dest 로 return 한다.
*/
void
ciStringUtil::removeSpace(const char *src, char *dest) {
    //cout << "-----------------------------------removeSpace(" << src << ")-------" << endl;
    while(*src) {
        if(COP_ISSPACE(*src)){
            src++;
            continue;
        }
        *dest++ = *src++;
    }
    *dest = 0;
}

void
ciStringUtil::removeSpace(const char *src, ciString& dest) {
    //cout << "-----------------------------------removeSpace(" << src << ")-------" << endl;
    dest = "";
    while(*src) {
        if(COP_ISSPACE(*src)){
            src++;
            continue;
        }
        dest += *src++;
    }
}

// ciStringUtil::removeQuote(ciString& str)
/*!
str 문자열에서 양쪽 끝의 " 를 삭제한다.
*/
ciString&
ciStringUtil::removeQuote(ciString& str) {
    int size = str.size();
    if (size < 2) return str;
    if (str[0] != '"') return str;
    if (str[size-1] != '"') return str;
    if (size == 2) {
        str = "";
        return str;
    }

    ciString dest = str.substr(1, size-2);
    str = dest;
    return str;
}

// ciStringUtil::removeAllQuote(ciString& str)
/*!
str 문자열에서 " 를 삭제한다.
*/
ciString&
ciStringUtil::removeAllQuote(ciString& str) {
    int a_len = str.size();
    if (a_len == 0) return str;

    ciString a_buf = "";
    for (int i=0; i < a_len ; i++) {
        if(str[i] == '"'){
            continue;
        }
        a_buf += str[i];
    }
    str = a_buf;
    return str;
}

// ciStringUtil::addSingleQuote(const char* p_str, ciString &p_buf)
/*!
p_str 문자열의 ' 문자앞에 '을 하나 추가한다.
*/
ciInt
ciStringUtil::addSingleQuote(const char* p_str, ciString &p_buf) {
    if(p_str == 0) return 0;

    p_buf = "";
    const char *ptr = p_str;
    while (*ptr){
        if (*ptr == '\'') {
            p_buf += '\'';
        }
        p_buf += *ptr;
        ptr++;
    }

    return p_buf.size();
}

// ciStringUtil::ciCharReplace(ciString &target, const char from, const char to)
/*!
target 문자열에서 from 문자를  찾아 to 문자로 바꾼다.
*/
void
ciStringUtil::charReplace(ciString& target, const char from, const char to) {
	if (from == to) return;
	for (int i=0; i<target.size(); ++i) {
		if (target[i] == from) target[i]=to;
	}
}

// ciStringUtil::ciStringReplace(ciString &target, const char* from, const char* to)
/*!
target 문자열에서 from 문자열을 찾아 to 문자열로 바꾼다.
*/
void
ciStringUtil::stringReplace(ciString& target, const char* from, const char* to) {
    int end_pos = strlen(from);
	int begin_pos = 0;

	int tmpLen = strlen(to);
	begin_pos -= tmpLen;

    while (1) {
        begin_pos = int(target.find(from, begin_pos+tmpLen));
        if (begin_pos < 0) {
            break;
        }
        target.replace(begin_pos, end_pos, to);
    }
	
}

// ciStringUtil::ciStringReplace(ciWString &target, const wchar_t* from, const wchar_t* to)
/*!
target 문자열에서 from 문자열을 찾아 to 문자열로 바꾼다.
*/
void
ciStringUtil::stringReplace(ciWString& target, const wchar_t* from, const wchar_t* to) {
    int end_pos = wcslen(from);
	int begin_pos = 0;

	int tmpLen = wcslen(to);
	begin_pos -= tmpLen;

    while (1) {
        begin_pos = int(target.find(from, begin_pos+tmpLen));
        if (begin_pos < 0) {
            break;
        }
        target.replace(begin_pos, end_pos, to);
    }
}

void
ciStringUtil::stringReplaceWithToken(ciString& target, const char* from, const char* to, ciStringList& deli) {

	ciString buf="";
	ciStringTokenizer aTokenizer(target,deli,ciFalse,ciTrue);
	while(aTokenizer.hasMoreTokens()) {
		ciString aTok = aTokenizer.nextToken();
		if(aTok.empty()) {
			continue;
		}
		if(aTok==from) {
			buf += to;
		}else{
			buf += aTok;
		}
	}	
	target=buf;
	return;
}
	
// ciStringUtil::startsWith(const ciString& p_source, const ciString& p_dest)
/*!
Comemnt   : 두번 인자가 , 첫번째 인자로 로 시작하면 TRUE
ex) p_source=SUM , p_dest=SUM(s1+s2+s3)
-->startsWith( p_source , p_dest ) == TRUE
*/
ciBoolean
ciStringUtil::startsWith(const ciString& p_source, const ciString& p_dest) {
    if ( p_source=="" || p_dest == "" )
        return ciTrue;

    int a_len = strlen( (char*)p_source.c_str() );

    if ( strncmp( (char*)p_dest.c_str() , (char*)p_source.c_str() , a_len ) == 0 )
        return ciTrue;

    return ciFalse;
}


ciBoolean
ciStringUtil::endsWith(const ciString& p_source, const ciString& p_dest) {
    if( p_source=="" || p_dest == "" )
        return ciTrue;

    int a_len = p_source.size();
    int b_len = p_dest.size();

    if (a_len > b_len) return ciFalse;

    if (a_len == b_len) {
        if ( p_source == p_dest ) return ciTrue;
        return ciFalse;
    }

    int offset = b_len - a_len;
    if ( p_source == p_dest.substr(offset) ) {
        return ciTrue;
    }
    return ciFalse;
}


// ciStringUtil::getFirstInteger(const char* p_str, const char* deli)
/*!
Comment   : get first integer after deliminator
Parameter : target ciString,  deliminator
Return    : first integer after deliminator
example   : getFirstInteger("this is sample (3)", "(");
=> 3 will be return
IF NO MATCH , -9999 will be return
*/
ciInt
ciStringUtil::getFirstInteger(const char* p_str, const char* deli) {
    if(p_str == 0 || deli == 0) {
        //cout << "ciStringUtil::getFirstInteger(): error : xxxxx" << endl;
        cout << "ciStringUtil::getFirstInteger(): error : xxxxx";
        return -9999;
    }

    const char* a_ptr = p_str;
    int len = strlen(deli);

    while (*a_ptr) {
        if(strncmp(a_ptr,deli,len) == 0){
            a_ptr += len;
            return atoi(a_ptr);
        }
        a_ptr++;
    }
    return atoi(p_str);
}

// ciStringUtil::btoa(ciBoolean tf, char* pStr)
/*!
Comment   : convert type from Boolean to ascii
Parameter : ciBoolean, char*
Return    : void
*/
void
ciStringUtil::btoa(ciBoolean tf, char* pStr) {
    if (tf == ciTrue) {
        sprintf(pStr, "true");
    } else {
        sprintf(pStr, "flase");
    }
}

// ciStringUtil::itoa(int p_int, char* pStr)
/*!
Comment   : convert type from integer to ascii
Parameter : int, char*
Return    : void
*/
void
ciStringUtil::itoa(int p_int, char* pStr) {
    sprintf(pStr, "%d", p_int);
}

// ciStringUtil::uitoa(unsigned int p_int, char* pStr)
/*!
Comment   : convert type from unsigned integer to ascii
Parameter : unsigned int, char*
Return    : void
*/
void
ciStringUtil::uitoa(unsigned int p_uint, char* pStr) {
    sprintf(pStr, "%u", p_uint);
}

// ciStringUtil::shtoa(short p_short, char* pStr)
/*!
Comment   : convert type from short to ascii
Parameter : short, char*
Return    : void
*/
void
ciStringUtil::shtoa(short p_short, char* pStr) {
    sprintf(pStr, "%d", p_short);
}

// ciStringUtil::ushtoa(unsigned short p_ushort, char* pStr)
/*!
Comment   : convert type from Unsigned short to ascii
Parameter : unsigned short, char*
Return    : void
*/
void
ciStringUtil::ushtoa(unsigned short p_ushort, char* pStr) {
    sprintf(pStr, "%d", p_ushort);
}

// ciStringUtil::ltoa(long p_long, char* pStr)
/*!
Comment   : convert type from long to ascii
Parameter : long, char*
Return    : void
*/
void
ciStringUtil::ltoa(long p_long, char* pStr) {
    sprintf(pStr, "%ld", p_long);
}

// ciStringUtil::ultoa(unsigned long p_long, char* pStr)
/*!
Comment   : convert type from unsigned long to ascii
Parameter : unsigned long, char*
Return    : void
*/
void
ciStringUtil::ultoa(unsigned long p_ulong, char* pStr) {
    sprintf(pStr, "%lu", p_ulong);
}

// ciStringUtil::lltoa(long long p_longlong, char* pStr)
/*!
Comment   : convert type from long long to ascii
Parameter : long long, char*
Return    : void
*/
void
ciStringUtil::lltoa(ciLongLong p_longlong, char* pStr) {
    sprintf(pStr, "%lld", p_longlong);
}

// ciStringUtil::ulltoa(unsigned long long p_ulonglong, char* pStr)
/*!
Comment   : convert type from unsigned long long to ascii
Parameter : unsigned long long, char*
Return    : void
*/
void
ciStringUtil::ulltoa(ciULongLong p_ulonglong, char* pStr) {
    sprintf(pStr, "%llu", p_ulonglong);
}

unsigned long long 
ciStringUtil::atoull(const char* str)
{
	unsigned long long retval = 0ll;
	const char* ptr = str;
	while(*ptr != 0)
	{
		retval = retval*10 + (*ptr-'0');
		ptr++;
	}
	return retval;
}


// ciStringUtil::dtoa(double p_double, char* pStr)
/*!
Comment   : convert type from double to ascii
Parameter : double, char*
Return    : void
*/
void
ciStringUtil::dtoa(double p_double, char* pStr) {
    sprintf(pStr, "%f", p_double);
}


// ciStringUtil::ftoa(float p_float, char* pStr)
/*!
Comment   : convert type from float to ascii
Parameter : float, char*
Return    : void
*/
void
ciStringUtil::ftoa(float p_float, char* pStr) {
    sprintf(pStr, "%.5f", p_float);
}

void
ciStringUtil::btoa(ciBoolean tf, ciString& str) {
    if (tf == ciTrue) {
        str = "true";
    } else {
        str = "false";;
    }
}

void
ciStringUtil::itoa(int p_int, ciString& str) {
    char a_str[256];
    memset(a_str, 0x00, sizeof(a_str));
    sprintf(a_str, "%d", p_int);
    str = a_str;
}

void
ciStringUtil::uitoa(unsigned int p_uint, ciString& str) {
    char a_str[256];
    memset(a_str, 0x00, sizeof(a_str));
    sprintf(a_str, "%u", p_uint);
    str = a_str;
}

void
ciStringUtil::shtoa(short p_short, ciString& str) {
    char a_str[256];
    memset(a_str, 0x00, sizeof(a_str));
    sprintf(a_str, "%d", p_short);
    str = a_str;
}

void
ciStringUtil::ushtoa(unsigned short p_ushort, ciString& str) {
    char a_str[256];
    memset(a_str, 0x00, sizeof(a_str));
    sprintf(a_str, "%d", p_ushort);
    str = a_str;
}

void
ciStringUtil::ltoa(long p_long, ciString& str) {
    char a_str[256];
    memset(a_str, 0x00, sizeof(a_str));
    sprintf(a_str, "%ld", p_long);
    str = a_str;
}

void
ciStringUtil::ultoa(unsigned long p_ulong, ciString& str) {
    char a_str[256];
    memset(a_str, 0x00, sizeof(a_str));
    sprintf(a_str, "%lu", p_ulong);
    str = a_str;
}

void
ciStringUtil::lltoa(ciLongLong p_longlong, ciString& str) {
    char a_str[256];
    memset(a_str, 0x00, sizeof(a_str));
    sprintf(a_str, "%ld", p_longlong);
    str = a_str;
}

void
ciStringUtil::ulltoa(ciULongLong p_ulonglong, ciString& str) {
    char a_str[256];
    memset(a_str, 0x00, sizeof(a_str));
    sprintf(a_str, "%lu", p_ulonglong);
    str = a_str;
}

void
ciStringUtil::dtoa(double p_double, ciString& str) {
    char a_str[256];
    memset(a_str, 0x00, sizeof(a_str));
    sprintf(a_str, "%f", p_double);
    str = a_str;
}

void
ciStringUtil::ftoa(float p_float, ciString& str) {
    char a_str[256];
    memset(a_str, 0x00, sizeof(a_str));
    sprintf(a_str, "%.5f", p_float);
    str = a_str;
}

// ciStringUtil::makeDigit(ciString& str)
/*!
name : ciMakeDigit
desc : make ciString only consist of digit characters
*/
ciString&
ciStringUtil::makeDigit(ciString& str) {
    ciInt size = str.size();
    if (size == 0) return str;

    ciString a_buf = "";
    const char *p_str = str.c_str();

    for (ciInt i = 0; i < size; i ++) {
        if (isdigit(p_str[i])) {
            a_buf += p_str[i];
        }
    }
    str = a_buf;
    return str;
}

// ciStringUtil::isOnlyDigit(const char *str)
/*!
name : isOnlyDigit
desc : if ciString only consist of digit characters, return True.
otherwise return False.
*/
ciBoolean
ciStringUtil::isOnlyDigit(const char *str) {
    if (!str) return ciFalse;
    ciInt size = strlen(str);
    if (size == 0) return ciFalse;
    for (ciInt i = 0; i < size; i ++) {
        if (!isdigit(str[i])) return ciFalse;
    }
    return ciTrue;
}

// ciStringUtil::isOnlyLetter(const char* str)
/*!
name : isOnlyLetter
desc : if ciString only consist of letter characters, return True.
otherwise return False.
*/
ciBoolean
ciStringUtil::isOnlyLetter(const char* str) {
    if (!str) return ciFalse;
    ciInt size = strlen(str);
    if (size == 0) return ciFalse;
    for (ciInt i = 0; i < size; i ++) {
        if (!isalpha(str[i])) return ciFalse;
    }
    return ciTrue;
}

// ciStringUtil::int2String(ciInt p_src, ciString& p_dst)
/*!
정수를 문자열로 변환 한다.
*/
const char*
ciStringUtil::int2String(ciInt p_src, ciString& p_dst) {
    char a_buf[256];
    sprintf(a_buf,"%ld",p_src);
    p_dst = a_buf;
    return p_dst.c_str();
}

void
ciStringUtil::cutPrefix(const ciString& target, ciString& retval, const char *prefix) {
    ciString a_buf = target;
    if(startsWith(prefix, target)) {
        int start = target.find(prefix);
        if(start >= 0){
            a_buf = target.substr(strlen(prefix));
        }
    }
    retval = a_buf;
}

void
ciStringUtil::cutPostfix2(const ciString& target, ciString& retval, const char *postfix) {

    ciString a_buf = target;
    int start = target.rfind(postfix);
	int len = strlen(postfix);
	if(start == (target.length()-len)) {
		if (start > 0) {
			a_buf = target.substr(0,start);
		} else if (start == 0){
			a_buf = "";
		}
	}
    retval = a_buf;
}

void
ciStringUtil::cutPostfix(const ciString& target, ciString& retval, const char *postfix) {
    ciString a_buf = target;
    int start = target.rfind(postfix);
    if (start > 0) {
        a_buf = target.substr(0,start);
    } else if (start == 0){
        a_buf = "";
    }
    retval = a_buf;
}

void
ciStringUtil::getPrefix(const ciString& target, ciString& retval, const char *prefix) {
	int start = target.find(prefix);
	if (start > 0) {
		retval = target.substr(0, start);
	}else if (start == 0) {
		retval = "";
	}else{
		retval = target; 
	}
}

void
ciStringUtil::cutPrefix(ciString& target, const char *prefix) {
    ciString a_buf = target;
    if (startsWith(prefix, target)) {
        int start = target.find(prefix);
        if (start >= 0) {
            a_buf = target.substr(strlen(prefix));
        }
    }
    target = a_buf;
}

void
ciStringUtil::cutPostfix(ciString& target, const char *postfix) {
    ciString a_buf = target;
    int start = target.rfind(postfix);
    if (start > 0) {
        a_buf = target.substr(0,start);
    } else if (start == 0) {
        a_buf = "";
    }
    target = a_buf;
}

void
ciStringUtil::cutPostfix2(ciString& target, const char *postfix) {

    ciString a_buf = target;
    int start = target.rfind(postfix);
	int len = strlen(postfix);

	if(start == (target.length()-len)) {
		int start = target.rfind(postfix);
		if (start > 0) {
			a_buf = target.substr(0,start);
		} else if (start == 0) {
			a_buf = "";
		}
	}
    target = a_buf;
}

int
ciStringUtil::Tokenizer(const char *object, const char *deli, list<ciString> *resultList) {
    if(!object && !resultList) return 0;

    //char a_token[NMS_BUF_LEN_2*10];
    //memset(a_token,0x00,NMS_BUF_LEN_2*10);

    ciString a_token = "";

    int deli_len = strlen(deli);

    while (*object) {
        if (strncmp(object,deli,deli_len) == 0) {
            //a_token[i] = 0;                       // null termination
            //i = 0;
            resultList->push_back(a_token);     // push token
            //memset(a_token,0x00,NMS_BUF_LEN_2*10); // rewind token
            a_token = "";
            object = object + deli_len;  // forward pointer
            continue;
        }
        //a_token[i++] = *object++;
        a_token += *object++;
    }
    //a_token[i] = 0;
    resultList->push_back(a_token);

    return resultList->size();
}
int
ciStringUtil::Tokenizer(const char *object, const char *deli, vector<ciString> *resultVector) {
    if(!object && !resultVector) return 0;

    //char a_token[NMS_BUF_LEN_2*10];
    //memset(a_token,0x00,NMS_BUF_LEN_2*10);

    ciString a_token = "";

    int deli_len = strlen(deli);

    while (*object) {
        if (strncmp(object,deli,deli_len) == 0) {
            //a_token[i] = 0;                       // null termination
            //i = 0;
            resultVector->push_back(a_token);     // push token
            //memset(a_token,0x00,NMS_BUF_LEN_2*10); // rewind token
            a_token = "";
            object = object + deli_len;  // forward pointer
            continue;
        }
        //a_token[i++] = *object++;
        a_token += *object++;
    }
    //a_token[i] = 0;
    resultVector->push_back(a_token);

    return resultVector->size();
}

ciString
ciStringUtil::getLastToken(const ciString& p_string, const ciString& p_token) {
    ciStringList    a_stringList;
    int     a_size = Tokenizer((char*)p_string.c_str(), p_token.c_str(), &a_stringList);
    if (a_size > 0) {
        return a_stringList.back();
    } else {
        return p_string;
    }
}

ciString
ciStringUtil::getFirstToken(const ciString& p_string, const ciString& p_token) {
    ciStringList    a_stringList;
    int     a_size = Tokenizer((char*)p_string.c_str(), p_token.c_str(), &a_stringList);
    if(a_size > 0){
        return a_stringList.front();
    } else {
        return p_string;
    }
}

ciString
ciStringUtil::getFirstScopedName(const char* pScopedName) {
    ciStringTokenizer strTokenizer(pScopedName, "::");
    ciString aToken;
    for (int i=0; strTokenizer.hasMoreTokens(); i++) {
        aToken = strTokenizer.nextToken();
        if (!aToken.empty()) {
            return aToken;
        }
    }
    aToken = "";
    return aToken;
}

ciString
ciStringUtil::getParentScopedName(const char* pScopedName) {
    ciStringTokenizer aStrTok(pScopedName, "::");
    int aCnt = aStrTok.countTokens();
    ciString aParentScopedName = "";
    for (int i=0; i<aCnt-1; i++) {
        aParentScopedName += "::";
        aParentScopedName += aStrTok.nextToken();
    }
    return aParentScopedName;
}

void
ciStringUtil::getLastToken(const ciString& p_string, const char p_token, ciString& retval)
{
	int len = p_string.size();
	const char* ptr = p_string.c_str();
	int i=len-1;
	for(;i<=0;i--){
		if(ptr[i]==p_token){
			break;
		}
	}
	if(i==0){
		retval = p_string;
		return;
	}
	retval = p_string.substr(i+1);
	return;
}

void
ciStringUtil::getLastToken(const ciString& p_string, const ciString& p_token, ciString& retString) {
    ciStringList    a_stringList;
    int     a_size = Tokenizer((char*)p_string.c_str(), p_token.c_str(), &a_stringList);
    if (a_size > 0) {
        retString = a_stringList.back();
    } else {
        retString = p_string;
    }
}

void
ciStringUtil::getFirstToken(const ciString& p_string, const ciString& p_token, ciString& retString) {
    ciStringList    a_stringList;
    int     a_size = Tokenizer((char*)p_string.c_str(), p_token.c_str(), &a_stringList);
    if (a_size > 0) {
        retString = a_stringList.front();
    } else {
        retString = p_string;
    }
}

void
ciStringUtil::getFirstScopedName(const char* pScopedName, ciString& retString) {
    ciStringTokenizer strTokenizer(pScopedName, "::");
    retString = "";
    for (int i=0; strTokenizer.hasMoreTokens(); i++) {
        retString = strTokenizer.nextToken();
        if (!retString.empty()) break;
    }
}

void
ciStringUtil::getParentScopedName(const char* pScopedName, ciString& retString) {
    ciStringTokenizer aStrTok(pScopedName, "::");
    int aCnt = aStrTok.countTokens();
    retString = "";
    for (int i=0; i<aCnt-1; i++) {
        retString += "::";
        retString += aStrTok.nextToken();
    }
}

ciString
ciStringUtil::curTimeStr() {
    time_t the_time;

    time(&the_time);

    return time2Str(the_time);
}

ciString
ciStringUtil::time2Str(time_t tm) {
    char* str;
    struct tm aTM;
#ifdef _COP_MSC_
    //struct tm *temp = localtime(&m_time);
    //m_tm = *temp;
    //aTM = *localtime(&tm);
    struct tm *temp = localtime(&tm);
    if (temp) {
        aTM = *temp;
    } else {
        memset((void*)&aTM, 0x00, sizeof(aTM));
    }
#else
    localtime_r(&tm, &aTM);
#endif

    str = (char*)malloc(sizeof(char)*20);
    sprintf(str,"%4d-%02d-%02d %02d:%02d:%02d",
        aTM.tm_year+1900,aTM.tm_mon+1,aTM.tm_mday,
        aTM.tm_hour,aTM.tm_min,aTM.tm_sec);
    ciString strTime = str;
    free(str);
    return strTime;
}

ciString
ciStringUtil::time2StrD(time_t tm) {
    char* str;
    struct tm aTM;

#ifdef _COP_MSC_
    //struct tm *temp = localtime(&m_time);
    //m_tm = *temp;
    //aTM = *localtime(&tm);
    struct tm *temp = localtime(&tm);
    if (temp) {
        aTM = *temp;
    } else {
        memset((void*)&aTM, 0x00, sizeof(aTM));
    }

#else
    localtime_r(&tm, &aTM);
#endif

    str = (char*)malloc(sizeof(char)*24);
    sprintf(str,"%4d %02d %02d %02d %02d %02d %02d",
        aTM.tm_year+1900,aTM.tm_mon+1,aTM.tm_mday,
        aTM.tm_wday,aTM.tm_hour,aTM.tm_min,aTM.tm_sec);
    ciString strTime = str;
    free(str);
    return strTime;
}

ciString
ciStringUtil::time2Str4CLI(time_t tm) {
    char* str;
    struct tm aTM;

#ifdef _COP_MSC_
    //struct tm *temp = localtime(&m_time);
    //m_tm = *temp;
    //aTM = *localtime(&tm);
    struct tm *temp = localtime(&tm);
    if (temp) {
        aTM = *temp;
    } else {
        memset((void*)&aTM, 0x00, sizeof(aTM));
    }
#else
    localtime_r(&tm, &aTM);
#endif


    str = (char*)malloc(sizeof(char)*20);
    sprintf(str,"%4d-%02d-%02d-%02d:%02d:%02d",
        aTM.tm_year+1900,aTM.tm_mon+1,aTM.tm_mday,
        aTM.tm_hour,aTM.tm_min,aTM.tm_sec);
    ciString strTime = str;
    free(str);
    return strTime;
}

void
ciStringUtil::curTimeStr(ciString& pOutTimeStr) {
    time2Str(time(NULL), pOutTimeStr);
}

/*! 출력형식: 2002-08-21 11:51:42 */
void
ciStringUtil::time2Str(time_t tm, ciString& pOutTimeStr) {
    struct tm aTM;
#ifdef _COP_MSC_
    //struct tm *temp = localtime(&m_time);
    //m_tm = *temp;
    //aTM = *localtime(&tm);
    struct tm *temp = localtime(&tm);
    if (temp) {
        aTM = *temp;
    } else {
        memset((void*)&aTM, 0x00, sizeof(aTM));
    }

#else
    localtime_r(&tm, &aTM);
#endif


    char aBuf[20];
    memset(aBuf, 0x00, sizeof(aBuf));

    sprintf(aBuf,
        "%4d-%02d-%02d %02d:%02d:%02d",
        aTM.tm_year+1900, aTM.tm_mon+1, aTM.tm_mday,
        aTM.tm_hour, aTM.tm_min, aTM.tm_sec);

    pOutTimeStr = aBuf;
}

/*! 출력형식: 2002 10 03 04 11 51 42 */
/* means : 2002년 10월 03일 목요일 11시 51분 41초 */
/* 파싱의 편의를 위한 함수입니다. */
void
ciStringUtil::time2StrD(time_t tm, ciString& pOutTimeStr) {
    struct tm aTM;

#ifdef _COP_MSC_
    //struct tm *temp = localtime(&m_time);
    //m_tm = *temp;
    //aTM = *localtime(&tm);
    struct tm *temp = localtime(&tm);
    if (temp) {
        aTM = *temp;
    } else {
        memset((void*)&aTM, 0x00, sizeof(aTM));
    }
#else
    localtime_r(&tm, &aTM);
#endif

    char aBuf[24];
    memset(aBuf, 0x00, sizeof(aBuf));

    sprintf(aBuf,
        "%4d %02d %02d %02d %02d %02d %02d",
        aTM.tm_year+1900, aTM.tm_mon+1, aTM.tm_mday,
        aTM.tm_wday, aTM.tm_hour, aTM.tm_min, aTM.tm_sec);

    pOutTimeStr = aBuf;
}

/*! 출력형식: 2002-08-21-11:51:42 */
void
ciStringUtil::time2Str4CLI(time_t tm, ciString& pOutTimeStr) {
    struct tm aTM;
#ifdef _COP_MSC_
    //struct tm *temp = localtime(&m_time);
    //m_tm = *temp;
    //aTM = *localtime(&tm);
    struct tm *temp = localtime(&tm);
    if (temp) {
        aTM = *temp;
    } else {
        memset((void*)&aTM, 0x00, sizeof(aTM));
    }

#else
    localtime_r(&tm, &aTM);
#endif


    char aBuf[20];
    memset(aBuf, 0x00, sizeof(aBuf));

    sprintf(aBuf,
        "%4d-%02d-%02d-%02d:%02d:%02d",
        aTM.tm_year+1900, aTM.tm_mon+1, aTM.tm_mday,
        aTM.tm_hour, aTM.tm_min, aTM.tm_sec);

    pOutTimeStr = aBuf;
}

/*! 입력형식: 2002-08-21 11:51:42 */
time_t
ciStringUtil::str2Time(const ciString& str) {
    int year, month, day, hour, minute, second;

    if(sscanf(str.c_str(), "%4d-%2d-%2d %2d:%2d:%2d",&year,&month,&day,&hour,&minute,&second) != 6) {
        cout << "ciStringUtil::str2Time(): Error, Failed To Convert Six Fields" << endl;
    } else {
        struct tm tms;
        time_t date;

        tms.tm_year = year - 1900;
        tms.tm_mon = month - 1;
        tms.tm_mday = day;
        tms.tm_hour = hour;
        tms.tm_min = minute;
        tms.tm_sec = second;
        tms.tm_isdst = -1;

        date = mktime(&tms);
        return date;
    }
    return -1;
}

/*! 입력형식: 2002-08-21-11:51:42 */
time_t
ciStringUtil::str2Time4CLI(const ciString& str) {
    int year, month, day, hour, minute, second;

    if(sscanf(str.c_str(), "%4d-%2d-%2d-%2d:%2d:%2d",&year,&month,&day,&hour,&minute,&second) != 6) {
        cout<<"ciStringUtil::str2Time(): Error, Failed To Convert Six Fields"<<endl;
    } else {
        struct tm tms;
        time_t date;

        tms.tm_year = year - 1900;
        tms.tm_mon = month - 1;
        tms.tm_mday = day;
        tms.tm_hour = hour;
        tms.tm_min = minute;
        tms.tm_sec = second;
        tms.tm_isdst = -1;

        date = mktime(&tms);
        return date;
    }
    return -1;
}

/*! 입력형식: 20020821115142 */
time_t
ciStringUtil::str2Time4GW(const ciString& str) {
    int year, month, day, hour, minute, second;

    if(sscanf(str.c_str(), "%4d%2d%2d%2d%2d%2d",&year,&month,&day,&hour,&minute,&second) != 6) {
        cout<<"ciStringUtil::str2Time(): Error, Failed To Convert Six Fields"<<endl;
    } else {
        struct tm tms;
        time_t date;

        tms.tm_year = year - 1900;
        tms.tm_mon = month - 1;
        tms.tm_mday = day;
        tms.tm_hour = hour;
        tms.tm_min = minute;
        tms.tm_sec = second;
        tms.tm_isdst = -1;

        date = mktime(&tms);
        return date;
    }
    return -1;
}

/*
* Author    : SEON K. PARK
* Date      : 010416
* Comment   : token the ciString and push it to list
* Parameter : char * object ==> target ciString
list<ciString> *resultList ==> vasel list for tokened ciString (out)
* Return    : size of list
*
* for example,
*              targetString = "(aaa >= ccc) && ( aaa > bbb)"
*              deliList = "&&", "||", "==", "!=", "<", ">","<=",">=","(",")"
*              if includeDeli == wcFalse  (default)
*                   resultVector has    aaa, bbb, ccc
*              otherwise
*                   resultVector has   (,aaa,>=,ccc,),&&,(,aaa,>,bbb,)"
*
* Caution : Order of delimenator is very important !!!!
*           for example , In case there is two delimetors such as  '<=' , '<' ,
*           has a relationship which one contains another one, ( '<=' contains '<' )
*           Container should be pushed first to deliList !!!!
*           in other words,  long delimenator should be pushed first than short one.
*
*/
int
ciStringUtil::multiTokenizer(const char *object,
                             ciStringList& deliList,
                             ciStringList& resultList,
                             ciBoolean includeDeli)
{
    if(!object && resultList.size() == 0) return 0;

    ciString a_token = "";

    while (*object) {
        ciBoolean matched_flag = ciFalse;
        ciStringList::iterator itr;
        ciDoList(deliList,itr) {
            ciString aDeli = (*itr);
            int deli_len = aDeli.size();
            if(strncmp(object, aDeli.c_str(), deli_len) == 0) {
                if(a_token.size() > 0){
                    resultList.push_back(a_token);   // push token
                }
                if(includeDeli == ciTrue){
                    resultList.push_back(aDeli); // push token
                }
                matched_flag = ciTrue;
                a_token = "";
                object = object + deli_len;  // forward pointer
                break;
            }
        }
        if(matched_flag == ciTrue){
            continue;
        }
        a_token += *object++;
    }
    if(a_token.size() > 0){
        resultList.push_back(a_token);
    }
    return resultList.size();
}

ciBoolean
ciStringUtil::string2Bool(const char* str)
{
    ciString aStr = str;
    toLower(aStr);
    if (aStr == "true")
        return ciTrue;
    return ciFalse;
}

void
ciStringUtil::rightTrim(char* str)
{
    if (str != NULL) {
        int len = strlen(str);
        for (int i = (len-1); i >= 0; i--) {
            if (COP_ISSPACE(str[i])) {
                str[i] = 0x00;
            } else {
                break;
            }
        }
    }
}

void
ciStringUtil::rightTrim(ciString& str)
{
    if(!str.empty()) {
        ciString aStr = str;
        int len = aStr.size();
        int i = (len -1);
        for (; i >= 0; i--) {
            if (COP_ISSPACE(aStr[i])) {
            }else{
                break;
            }
        }
        if(i<len-1) {
            str = aStr.substr(0,i+1);
        }
    }
    /*
    if(!str.empty()) {
    int len = str.size();
    for (int i = (len-1); i >= 0; i--) {
    if (COP_ISSPACE(str[i])) {
    str[i] = 0x00;
    } else {
    break;
    }
    }
    }
    */

}

void
ciStringUtil::leftTrim(char* str)
{
    if (str == NULL) return;

    int len = strlen(str);
    int count = 0, i;
    for(i=0;i<len;i++) {
        if (COP_ISSPACE(str[i])) {
            count++;
        } else {
            break;
        }
    }
	if(count==0) return;

    for (i=0; i<len-count; i++) {
        str[i] = str[i+count];
    }
    if (count > 0) str[len-count] = 0x00;
    return;
}
void
ciStringUtil::leftTrim(ciString& str)
{
    if (str == "") return;

    ciString aStr = str;
    int len = aStr.size();
    int count = 0;
    for( int i=0;i<len;i++){
        if(COP_ISSPACE(aStr[i])){
            count++;
        } else {
            break;
        }
    }
    if(count>0) {
        str = aStr.substr(count,len-count);
    }
    //for (int i=0; i<len-count; i++) {
    //  str[i] = str[i+count];
    //}
    //if (count > 0) str[len-count] = 0x00;
    return;
}

//
// 문자열의 양쪽 끝의 스페이스를 제거한다.
//
/*
void
ciStringUtil::bothTrim(ciString& str)
{
    if (str == "") return;

    ciString aStr = str;
    int len = aStr.size();
    int start = 0;
    for(;start<len;start++){
        if(COP_ISSPACE(aStr[start])){
        } else {
            break;
        }
    }
	int end = len-1;
	for(;end>=0;end--){
        if(COP_ISSPACE(aStr[end])){
        } else {
            break;
        }
    }
	if(start > end) {
		str = "";
	}else{
		if(start>0 || end<len-1 ) {
			str = aStr.substr(start,end+1-start);
		}
	}
    return;
}
*/
void
ciStringUtil::bothTrim(ciString& str)
{
    if (str == "") return;

	const char* buf = str.c_str();
    int len = str.size();
    int start = 0;
    for(;start<len;start++){
        if(COP_ISSPACE(buf[start])){
        } else {
            break;
        }
    }
	int end = len-1;
	for(;end>=0;end--){
        if(COP_ISSPACE(buf[end])){
        } else {
            break;
        }
    }
	if(start > end) {
		str = "";
	}else{
		if(start>0 || end<len-1 ) {
			str = str.substr(start,end+1-start);
		}
	}
    return;
}

void
ciStringUtil::getDisplayable(unsigned char* msgStream,
                             int tokenCounter,
                             const char* deliminator,
                             ciString& displayable)
{
	char* buf = (char*)msgStream;	
	for(int i = 0 ; i < tokenCounter ; i++) {
		if(buf == NULL) {
			buf++;
			displayable += deliminator;
		} else {
			displayable += buf;
			displayable += deliminator;
			buf += (strlen(buf) + 1);
		}
	}
/*
    for (int i = 0,idx  = 0; i < tokenCounter ; i++) {
        while(1) {
            if (msgStream[idx] == 0) {
                displayable += deliminator;
                idx++;
                break;
            }
            displayable += msgStream[idx];
            idx++;
        }
    }
*/
}

void
ciStringUtil::getDisplayableList(unsigned char* msgStream,
                                 int tokenCounter,
                                 ciStringList& displayableList)
{
	char* buf = (char*)msgStream;
	for(int i = 0 ; i < tokenCounter ; i++) {
		if(buf == NULL) {
			buf++;
			displayableList.push_back("");
		} else {
			displayableList.push_back(buf);
			buf += (strlen(buf) + 1);
		}
	}

/*
    for(int i = 0,idx  = 0; i < tokenCounter ; i++) {
        ciString buf = "";
        while(1) {
            if (msgStream[idx] == 0) {
                displayableList.push_back(buf);
                idx++;
                break;
            }
            buf += msgStream[idx];
            idx++;
        }
    }
*/
}

void
ciStringUtil::getDisplayableList(unsigned char* msgStream,
                                 int tokenCounter,
                                 const char* deliminator,
                                 ciString& displayable,
                                 ciStringList& displayableList)
{
    for(int i = 0,idx  = 0; i < tokenCounter ; i++) {
        ciString buf = "";
        while(1) {
            if (msgStream[idx] == 0) {
                displayableList.push_back(buf);
                displayable +=  (buf + deliminator);
                idx++;
                break;
            }
            buf += msgStream[idx];
            idx++;
        }
    }
}

#ifndef _COP_MSC_
ciBoolean
ciStringUtil::regexpMatch(const char* string, const char* pattern)
{
    int i;
    regex_t re;
    char buf[1024];

    ciString aStr = string;

    i=regcomp(&re, pattern, REG_EXTENDED|REG_NOSUB);
    if (i != 0) {
        (void)regerror(i,&re,buf,sizeof buf);
        //ciDEBUG(9,("%s\n",buf));
        return ciFalse;
    }

    i = regexec(&re, aStr.c_str(), (size_t) 0, NULL, 0);
    regfree(&re);
    if (i != 0) {
        (void)regerror(i,&re,buf,sizeof buf);
        //ciDEBUG(9,("%s\n",buf));
        return ciFalse;
    }

    return ciTrue;
}
#endif

ciBoolean
ciStringUtil::simpleMatch(const char* target, const char* pattern)
{
    if(!pattern  || !target ) {
        //ciDEBUG(9,("null pattern or target"));
        return ciFalse;
    }

    int t_len = strlen(target);
    int p_len = strlen(pattern) - 1;

    if( p_len < 0 || t_len < 0 ) {
        //ciDEBUG(9,("invalid pattern or target"));
        return ciFalse;
    }

    if(t_len < p_len) {
        //ciDEBUG(9,("does not match"));
        return ciFalse;
    }

    string str1 = "";
    string str2 = "";

    if(pattern[0] == '*') {
        str1 = pattern+1;
        str2 = target + (t_len-p_len);
    }else if(pattern[p_len] == '*') {
        str1 = string(pattern).substr(0,p_len);
        str2 = string(target).substr(0,p_len);
    }else{
        str1 = pattern;
        str2 = target;
    }

    if(str1 == str2 ) {
        return ciTrue;
    }

    //ciDEBUG(9,("does not match"));
    return ciFalse;
}

ciBoolean
ciStringUtil::atoB(ciString& str, ciBoolean& pBool)
{
    if(str == "true")   { pBool = true;     return true; }
    if(str == "TRUE")   { pBool = true;     return true; }
    if(str == "false")  { pBool = false;    return true; }
    if(str == "FALSE")  { pBool = false;    return true; }
    return false;
}
/*

sprintf(eventDir, "%s/idl/event", ciEnv::newEnv("CONFIGROOT"));
DIR *_dp;
if ((_dp = opendir(eventDir)) == NULL) {
ciERROR(("Cannot open directory %s", eventDir));
exit(1);
}

struct dirent *dirp;

while( (dirp = readdir(_dp))!= NULL) {
if(ciStringUtil::endsWith(".idl", dirp->d_name) == ciFalse)
{
continue;
}else{
fullPath = eventDir;
fullPath += "/";
fullPath += dirp->d_name;
getEvent((ciChar *) fullPath.c_str());
}
}
return ciTrue;
*/


void
ciStringUtil::splitString(const char* src, int size, ciStringList* resultList)
{
    if(!src || strlen(src)==0) {
        resultList->push_back("");
        return;
    }
    int i = 0;
    ciString token = "";
    while(*src) {
        if(i >= size) {
            resultList->push_back(token);
            i = 0;
            token = "";
        }
        i++;
        token += *src;
        src++;
    }
    if(!token.empty()) {
        resultList->push_back(token);
    }

}

void
ciStringUtil::uniqPush(ciStringList& src, const char* str)
{
    ciBoolean alreadyExist=ciFalse;
    ciStringList::iterator jtr;
    for(jtr=src.begin();jtr!=src.end();jtr++) {
		if((*jtr) == str) {
			alreadyExist = ciTrue;
			break;
		}
    }
	if(!alreadyExist){
		src.push_back(str);
	}
}

void
ciStringUtil::StringListToString(ciStringList& src, ciString& target)
{
    int i=0;
    ciStringList::iterator jtr;
    for(jtr=src.begin();jtr!=src.end();jtr++,i++) {
        if(i) target += ",";
        target += (*jtr);
    }
}

int
ciStringUtil::wildcmp(const char *pwild, const char *pstr)
{
    char wildArr[1024], strArr[1024];
    memset(wildArr,0,256); strcpy(wildArr,pwild);
    memset(strArr,0,256); strcpy(strArr,pstr);

    char *cp, *mp;
    char *wild = wildArr;
    char *str = strArr;

    while ((*str) && (*wild != '*')) {
        if ((*wild != *str) && (*wild != '?')) {
            return 0;
        }
        wild++;
        str++;
    }

    while (*str) {
        if (*wild == '*') {
            if (!*++wild) {
                return 1;
            }
            mp = wild;
            cp = str+1;
        } else if ((*wild == *str) || (*wild == '?')) {
            wild++;
            str++;
        } else {
            wild = mp;
            str = cp++;
        }
    }

    while (*wild == '*') {
        wild++;
    }
    return !*wild;
}

int
ciStringUtil::fnmatch (const char *pattern, const char *string, int flags) {
    register const char *p = pattern, *n = string;
    register unsigned char c;

#define FOLD(c) ((flags & CI_CASEFOLD) ? tolower (c) : (c))
#ifdef _COP_MSC_
#   define DIR_DELIMITER '\\'
#else
#   define DIR_DELIMITER '/'
#endif

    while ((c = *p++) != '\0')
    {
        c = FOLD (c);

        switch (c)
        {
        case '?':
            if (*n == '\0')
                return -1;
            else if ((flags & CI_FILE_NAME) && *n == DIR_DELIMITER)
                return -1;
            else if ((flags & CI_PERIOD) && *n == '.' &&
                (n == string || ((flags & CI_FILE_NAME) && n[-1] == DIR_DELIMITER)))
                return -1;
            break;

        case '\\':
            if (!(flags & CI_NOESCAPE))
            {
                c = *p++;
                c = FOLD (c);
            }
            if (FOLD ((unsigned char)*n) != c)
                return -1;
            break;

        case '*':
            if ((flags & CI_PERIOD) && *n == '.' &&
                (n == string || ((flags & CI_FILE_NAME) && n[-1] == DIR_DELIMITER)))
                return -1;
            for (c = *p++; c == '?' || c == '*'; c = *p++, ++n)
                if (((flags & CI_FILE_NAME) && *n == DIR_DELIMITER) ||
                    (c == '?' && *n == '\0'))
                    return -1;

            if (c == '\0')
                return 0;

            {
                unsigned char c1 = (!(flags & CI_NOESCAPE) && c == '\\') ? *p : c;
                c1 = FOLD (c1);
                for (--p; *n != '\0'; ++n)
                    if ((c == '[' || FOLD ((unsigned char)*n) == c1) &&
                        fnmatch (p, n, flags & ~CI_PERIOD) == 0)
                        return 0;
                return -1;
            }

        case '[':
            {
                /* Nonzero if the sense of the character class is inverted.  */
                register int rNot;

                if (*n == '\0')
                    return -1;

                if ((flags & CI_PERIOD) && *n == '.' &&
                    (n == string || ((flags & CI_FILE_NAME) && n[-1] == DIR_DELIMITER)))
                    return -1;

                rNot = (*p == '!' || *p == '^');
                if (rNot)
                    ++p;

                c = *p++;
                for (;;)
                {
                    register unsigned char cstart = c, cend = c;

                    if (!(flags & CI_NOESCAPE) && c == '\\')
                        cstart = cend = *p++;
                    cstart = cend = FOLD (cstart);

                    if (c == '\0')
                        /* [ (unterminated) loses.  */
                        return -1;

                    c = *p++;
                    c = FOLD (c);

                    if ((flags & CI_FILE_NAME) && c == DIR_DELIMITER)
                        /* [/] can never match.  */
                        return -1;

                    if (c == '-' && *p != ']')
                    {
                        cend = *p++;
                        if (!(flags & CI_NOESCAPE) && cend == '\\')
                            cend = *p++;
                        if (cend == '\0')
                            return -1;
                        cend = FOLD (cend);

                        c = *p++;
                    }

                    if (FOLD ((unsigned char)*n) >= cstart
                        && FOLD ((unsigned char)*n) <= cend)
                        goto matched;

                    if (c == ']')
                        break;
                }
                if (!rNot)
                    return -1;
                break;

matched:;
                /* Skip the rest of the [...] that already matched.  */
                while (c != ']')
                {
                    if (c == '\0')
                        /* [... (unterminated) loses.  */
                        return -1;
                    c = *p++;
                    if (!(flags & CI_NOESCAPE) && c == '\\')
                        /* XXX 1003.2d11 is unclear if this is right.  */
                        ++p;
                }
                if (rNot)
                    return -1;
            }
            break;

        default:
            if (c != FOLD ((unsigned char)*n))
                return -1;
        }

        ++n;
    }

    if (*n == '\0')
        return 0;

    if ((flags & CI_LEADING_DIR) && *n == DIR_DELIMITER)
        /* The CI_LEADING_DIR flag says that "foo*" matches "foobar/frobozz".  */
        return 0;

    return -1;
}


ciBoolean
ciStringUtil::getWildPart(const char* pattern, ciString& value)
{
	/* value 와 pattren 을 비교하여 patern 의 '*'에 매칭되는 문자열 부분만 분리해 준다 */

	if(!pattern || value.empty()) {
		return ciFalse;
	}

	ciString p = pattern;
	int len = p.size();

	if(p[0] == '*') {
		if(len>1) {
			cutPostfix(value,p.substr(1).c_str());
		}
		return ciTrue;
	}

	if(p[len-1]=='*') {
		cutPrefix(value,p.substr(0,len-1).c_str());
		return ciTrue;
	}

	return ciTrue;
}

static const string base64_chars = 
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
             "0123456789+/";


bool ciStringUtil::is_base64(unsigned char c) 
{
  return (isalnum(c) || (c == '+') || (c == '/'));
}

string ciStringUtil::base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len) 
{
  string ret;
  int i = 0, j = 0;
  unsigned char char_array_3[3], char_array_4[4];

  while (in_len--)
	{
    char_array_3[i++] = *(bytes_to_encode++);
    if (i == 3) 
		{
      char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
      char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
      char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
      char_array_4[3] = char_array_3[2] & 0x3f;

      for(i = 0; (i <4) ; i++)
        ret += base64_chars[char_array_4[i]];
      i = 0;
    }
  }

  if (i)
  {
    for(j = i; j < 3; j++)
      char_array_3[j] = '\0';

    char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
    char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
    char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
    char_array_4[3] = char_array_3[2] & 0x3f;

    for (j = 0; (j < i + 1); j++)
      ret += base64_chars[char_array_4[j]];

    while((i++ < 3))
      ret += '=';

  }

  return ret;

}

string ciStringUtil::base64_decode(string const& encoded_string) 
{
  int in_len = encoded_string.size();
  int i = 0, j = 0, in_ = 0;
  unsigned char char_array_4[4], char_array_3[3];
  string ret;

  while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_])) 
	{
    char_array_4[i++] = encoded_string[in_]; in_++;
    if (i ==4) {
      for (i = 0; i <4; i++)
        char_array_4[i] = base64_chars.find(char_array_4[i]);

      char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
      char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
      char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

      for (i = 0; (i < 3); i++)
        ret += char_array_3[i];
      i = 0;
    }
  }

  if (i) 
	{
    for (j = i; j <4; j++)
      char_array_4[j] = 0;

    for (j = 0; j <4; j++)
      char_array_4[j] = base64_chars.find(char_array_4[j]);

    char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
    char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
    char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

    for (j = 0; (j < i - 1); j++) 
			ret += char_array_3[j];
  }

  return ret;
}

unsigned char* ciStringUtil::base64_decode(string const& encoded_string, size_t& outlen)
{
	int in_len = encoded_string.size();
	int i = 0, j = 0, in_ = 0;
	unsigned char char_array_4[4], char_array_3[3];
	unsigned char* ret = new unsigned char[in_len*2];
	memset(ret, 0x00, in_len * 2);

	while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_]))
	{
		char_array_4[i++] = encoded_string[in_]; in_++;
		if (i == 4) {
			for (i = 0; i <4; i++)
				char_array_4[i] = base64_chars.find(char_array_4[i]);

			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
			char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

			for (i = 0; (i < 3); i++)
				ret += char_array_3[i];
			i = 0;
		}
	}

	if (i)
	{
		for (j = i; j <4; j++)
			char_array_4[j] = 0;

		for (j = 0; j <4; j++)
			char_array_4[j] = base64_chars.find(char_array_4[j]);

		char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
		char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
		char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

		for (j = 0; (j < i - 1); j++)
			ret += char_array_3[j];
	}

	return ret;
}
